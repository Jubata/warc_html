#include "hubbub/hubbub.h"
#include <string>
#include <sstream>
#include <algorithm>

extern "C"
{
  #include "tokeniser.h" //WA bad func declaration
}
#include <cassert>
#include "html_parser.h"

bool fixComment(std::string& word) {
  word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalpha(c); } ), word.end());
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  return (std::string::npos != word.find("comment") || 
    std::string::npos != word.find("review") || 
    std::string::npos != word.find("feedback")
    );
}

void HTMLParser::onAttributeValue(const uint8_t* buf, size_t len) {
  std::string sbuf((const char*)buf, len);
  std::istringstream strm(sbuf);
  std::string word;
  while(strm>>word) {
    if( fixComment(word) ) {
        attrValue[word]++;
    }
  }
}

void HTMLParser::onAttributeName(const uint8_t* buf, size_t len) {
  std::string word((const char*)buf, len);
  if( fixComment(word) ) {
      attrName[word]++;
  }
}

hubbub_error token_handler(const hubbub_token *token, void *pw)
{
  assert(pw);
  HTMLParser *parser = reinterpret_cast<HTMLParser*>(pw);

  if (token->type==HUBBUB_TOKEN_START_TAG) {
		for (size_t i = 0; i < token->data.tag.n_attributes; i++) {
      auto attr = token->data.tag.attributes[i];
      std::string attrname((const char*)attr.name.ptr, attr.name.len);
      if(attrname!="href") {
        parser->onAttributeName(attr.name.ptr, attr.name.len);
        parser->onAttributeValue(attr.value.ptr, attr.value.len);
      }
    }
	}

	return HUBBUB_OK;
}

void HTMLParser::parse(const std::string &url, const uint8_t* buf, size_t len, Stats* stats) {
  parserutils_inputstream *stream;
	hubbub_tokeniser *tok;
	hubbub_tokeniser_optparams params;

	assert(parserutils_inputstream_create("UTF-8", 0, NULL,
			&stream) == PARSERUTILS_OK);

	assert(hubbub_tokeniser_create(stream, &tok) == HUBBUB_OK);

  params.token_handler.handler = token_handler;
	params.token_handler.pw = this;
  assert(hubbub_tokeniser_setopt(tok, HUBBUB_TOKENISER_TOKEN_HANDLER,
			&params) == HUBBUB_OK);

  assert(parserutils_inputstream_append(stream,
      buf, len) == PARSERUTILS_OK);

  hubbub_tokeniser_run(tok); //ignore result
  
	hubbub_tokeniser_destroy(tok);

	parserutils_inputstream_destroy(stream);

  for(auto& attr: attrName) {
    stats->addEntity(url, true, attr.first, attr.second);
  }
  for(auto& attr: attrValue) {
    stats->addEntity(url, false, attr.first, attr.second);
  }
}

