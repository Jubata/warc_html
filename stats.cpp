#include "stats.h"
#include "yuarel.h"
#include <sstream>
#include <json/json.h>
#include <iostream>
#include "csv/ostream.hpp"

void Stats::addEntity(const std::string& url, bool isAttrName, const std::string& name, size_t count) {
 struct yuarel parsedUrl;
  std::string url2(url);
  if(-1 == yuarel_parse(&parsedUrl, &url2.front()) ) {
    return;
  }

  std::ostringstream stringStream;
  stringStream << parsedUrl.host << "#" << isAttrName << "#" << name;
  
  addEntity(stringStream.str(), count, 1, url, count);
}

void Stats::addEntity(const std::string& url, 
                        size_t totalFields,
                        size_t countUrls, 
                        const std::string& bestUrl, 
                        size_t bestMeanFieldsPerPage) {
  Stats::UrlStats &entry = map[url];
  if(entry.bestMeanFieldsPerPage < bestMeanFieldsPerPage) {
    entry.bestMeanFieldsPerPage = bestMeanFieldsPerPage;
    entry.bestUrl = bestUrl;
  }
  
  entry.totalFields += totalFields;
  entry.countUrls += countUrls;
}

void Stats::print_json() {
  for(auto& it: map) {
    const UrlStats& stats = it.second;
    
    // Json::Value root;
    // root["url"] = it.first;
    // root["total"] = stats.totalFields;
    // root["urls"] = stats.countUrls;
    // root["best_url"]=stats.bestUrl;
    // root["best_count"]=stats.bestMeanFieldsPerPage;
    // std::cout<<root<<","<<std::endl;

    namespace csv = ::text::csv;
    csv::csv_ostream csvs(std::cout);

    csvs << it.first.c_str() 
         << (int)stats.totalFields
         << (int)stats.countUrls
         << stats.bestUrl.c_str()
         << (double)stats.bestMeanFieldsPerPage
         << csv::endl;
  }
}