module Nokogiri
  module XML
    class FragmentHandler < Nokogiri::XML::SAX::Document # :nodoc:
      def initialize node, original_html
        @doc_started    = false
        @original_html  = original_html
        @document       = node.document
        @stack          = [node]
      end

      def start_element name, attrs = []
        @doc_started = true if @original_html =~ %r{^<#{Regexp.escape(name)}}
        return unless @doc_started

        node = Node.new(name, @document)
        attrs << "" unless (attrs.length % 2) == 0
        Hash[*attrs].each do |k,v|
          node[k] = v
        end
        @stack.last << node
        @stack << node
      end

      def characters string
        @doc_started = true if @original_html.strip =~ %r{^\s*#{Regexp.escape(string)}}
        @stack.last << Nokogiri::XML::Text.new(string, @document)
      end

      def end_element name
        return unless @stack.last.name == name
        @stack.pop
      end
    end
  end
end
