#include <string>
#include "yuarel.h"
#include <unordered_map>
#include <sstream>
#include <json/json.h>
#include <iostream>

#ifndef H_URLSTATS
#define H_URLSTATS

struct Stats{
  struct UrlStats {
    std::string url;
    size_t totalFields;
    size_t countUrls;
    double bestMeanFieldsPerPage;
    UrlStats(): totalFields(0), countUrls(0), bestMeanFieldsPerPage(0) {}
  };
  std::unordered_map<std::string, UrlStats> map;

  void add_entity(const std::string& url, bool isAttrName, const std::string& name, size_t count) {
    struct yuarel parsedUrl;
    std::string url2(url);
    if(-1 == yuarel_parse(&parsedUrl, &url2.front()) ) {
      return;
    }

    std::ostringstream stringStream;
    stringStream << parsedUrl.host << "#" << isAttrName << "#" << name;
    UrlStats &stats = map[stringStream.str()];
    if(stats.bestMeanFieldsPerPage<count) {
      stats.bestMeanFieldsPerPage = count;
      stats.url = url;
    }
    stats.countUrls += 1;
    stats.totalFields += count;
  }

  void print_json() {
    for(auto& it: map) {
      const UrlStats& stats = it.second;
      
      // Json::Value root;
      // root["url"] = it.first;
      // root["total"] = stats.totalFields;
      // root["urls"] = stats.countUrls;
      // root["best_url"]=stats.url;
      // root["best_count"]=stats.bestMeanFieldsPerPage;
      // std::cout<<root<<"\n";

      std::cout<<it.first<<";"<<stats.totalFields<<";"<<stats.countUrls<<";"<<stats.url<<";"<<stats.bestMeanFieldsPerPage<<std::endl;
    }
  }
};

#endif