module Nokogiri
  module HTML
    class Document < XML::Document

      attr_accessor :cstruct # :nodoc:

      def self.new(*args) # :nodoc:
        uri         = args[0]
        external_id = args[1]
        doc = Document.wrap(LibXML.htmlNewDoc(uri, external_id), self)
        doc.send :initialize, *args
        doc
      end

      def self.read_io(io, url, encoding, options) # :nodoc:
        wrap_with_error_handling(HTML_DOCUMENT_NODE) do
          LibXML.htmlReadIO(IoCallbacks.reader(io), nil, nil, url, encoding, options)
        end
      end

      def self.read_memory(string, url, encoding, options) # :nodoc:
        wrap_with_error_handling(HTML_DOCUMENT_NODE) do
          LibXML.htmlReadMemory(string, string.length, url, encoding, options)
        end
      end

      def meta_encoding=(encoding) # :nodoc:
        LibXML.htmlSetMetaEncoding(cstruct, encoding)
        encoding
      end

      def meta_encoding # :nodoc:
        LibXML.htmlGetMetaEncoding(cstruct)
      end
    end
  end
end
