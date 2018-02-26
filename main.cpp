#include <iostream>
#include "warc.hh"
#include "http_parser.h"
#include <string>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <memory>
#include "html_parser.h"
#include "stats.h"

Stats gstats;

struct HTTPParserState {
  bool isContentTypeHeader;
  bool isHtml;
  std::string uri;

  HTTPParserState(const std::string& uri): 
    isContentTypeHeader(false), 
    isHtml(false),
    uri(uri)
  {}
};

int on_header_field_callback(http_parser *parser, const char *p, size_t len) {
  assert(parser);
  HTTPParserState *state = (HTTPParserState*)parser->data;

  state->isContentTypeHeader = !strncmp(p, "Content-Type", len);
  
  return 0;
}

int on_header_value_callback(http_parser *parser, const char *p, size_t len) {
  assert(parser);
  HTTPParserState *state = (HTTPParserState*)parser->data;

  if(state->isContentTypeHeader) {
    len = std::min( len, strlen("text/html") );
    if( !strncmp(p, "text/html", len ) ) {
      state->isHtml = true;
    }
  }
  return 0;
}

int on_body_callback(http_parser *parser, const char *p, size_t len) {
  assert(parser);
  HTTPParserState *state = (HTTPParserState*)parser->data;

  if(state->isHtml) {
    HTMLParser parser;
    parser.parse(state->uri, reinterpret_cast<const uint8_t*>(p), len, &gstats);
  }

  return 0;
}

void parse_http_response(const std::string& buf, const std::string& uri) {
  http_parser_settings settings;
  memset(&settings, 0, sizeof(settings));
  settings.on_body = on_body_callback;
  settings.on_header_field = on_header_field_callback;
  settings.on_header_value = on_header_value_callback;

  http_parser parser;
  http_parser_init(&parser, HTTP_RESPONSE);
  HTTPParserState state(uri);
  parser.data = &state;
  
  size_t nparsed = http_parser_execute(&parser, &settings, buf.c_str(), buf.size());

}

int main(int argc, char* argv[]) {

  unsigned count = 0;
  warc::v1::WARCFile file(std::cin);
	for(auto it=file.begin(); it != file.end(); it++, count++) {
    bool is_response = false;
    std::string uri;
    for (auto field : it->get_fields()){
      if(field.get_name() == "WARC-Type") {
          is_response = field.get_value() == "response";
          if(!is_response) break;
      }
      if(field.get_name() == "WARC-Target-URI") {
        uri = field.get_value();
      }	
    }
    if(is_response && !uri.empty()) {
      parse_http_response(it->get_content(), uri);
    }
  }

  gstats.print_json();
  
  return 0;
}