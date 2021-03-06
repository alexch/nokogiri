#include <xml_relax_ng.h>

static void dealloc(xmlRelaxNGPtr schema)
{
  NOKOGIRI_DEBUG_START(doc);
  xmlRelaxNGFree(schema);
  NOKOGIRI_DEBUG_END(doc);
}

/*
 * call-seq:
 *  validate_document(document)
 *
 * Validate a Nokogiri::XML::Document against this RelaxNG schema.
 */
static VALUE validate_document(VALUE self, VALUE document)
{
  xmlDocPtr doc;
  xmlRelaxNGPtr schema;

  Data_Get_Struct(self, xmlRelaxNG, schema);
  Data_Get_Struct(document, xmlDoc, doc);

  VALUE errors = rb_ary_new();

  xmlRelaxNGValidCtxtPtr valid_ctxt = xmlRelaxNGNewValidCtxt(schema);

  if(NULL == valid_ctxt) {
    // we have a problem
    rb_raise(rb_eRuntimeError, "Could not create a validation context");
  }

  if (! is_2_6_16()) {
    xmlRelaxNGSetValidStructuredErrors(
      valid_ctxt,
      Nokogiri_error_array_pusher,
      (void *)errors
    );
  }

  xmlRelaxNGValidateDoc(valid_ctxt, doc);

  xmlRelaxNGFreeValidCtxt(valid_ctxt);

  return errors;
}

/*
 * call-seq:
 *  read_memory(string)
 *
 * Create a new RelaxNG from the contents of +string+
 */
static VALUE read_memory(VALUE klass, VALUE content)
{
  xmlRelaxNGParserCtxtPtr ctx = xmlRelaxNGNewMemParserCtxt(
      (const char *)StringValuePtr(content),
      RSTRING_LEN(content)
  );

  VALUE errors = rb_ary_new();
  xmlSetStructuredErrorFunc((void *)errors, Nokogiri_error_array_pusher);

  if (! is_2_6_16()) {
    xmlRelaxNGSetParserStructuredErrors(
      ctx,
      Nokogiri_error_array_pusher,
      (void *)errors
    );
  }

  xmlRelaxNGPtr schema = xmlRelaxNGParse(ctx);

  xmlSetStructuredErrorFunc(NULL, NULL);
  xmlRelaxNGFreeParserCtxt(ctx);

  if(NULL == schema) {
    xmlErrorPtr error = xmlGetLastError();
    if(error)
      rb_funcall(rb_mKernel, rb_intern("raise"), 1,
          Nokogiri_wrap_xml_syntax_error((VALUE)NULL, error)
      );
    else
      rb_raise(rb_eRuntimeError, "Could not parse document");

    return Qnil;
  }

  VALUE rb_schema = Data_Wrap_Struct(klass, 0, dealloc, schema);
  rb_iv_set(rb_schema, "@errors", errors);

  return rb_schema;
}

VALUE cNokogiriXmlRelaxNG;
void init_xml_relax_ng()
{
  VALUE nokogiri = rb_define_module("Nokogiri");
  VALUE xml = rb_define_module_under(nokogiri, "XML");
  VALUE klass = rb_define_class_under(xml, "RelaxNG", cNokogiriXmlSchema);

  cNokogiriXmlRelaxNG = klass;

  rb_define_singleton_method(klass, "read_memory", read_memory, 1);
  rb_define_private_method(klass, "validate_document", validate_document, 1);
}
