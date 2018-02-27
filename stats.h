#include <string>
#include <unordered_map>

#ifndef H_URLSTATS
#define H_URLSTATS

struct Stats{
  struct UrlStats {
    std::string bestUrl;
    size_t totalFields;
    size_t countUrls;
    double bestMeanFieldsPerPage;
    UrlStats(): totalFields(0), countUrls(0), bestMeanFieldsPerPage(0) {}
  };
  std::unordered_map<std::string, UrlStats> map;

  void addEntity(const std::string& url, bool isAttrName, const std::string& name, size_t count);

  void addEntity(const std::string& url, 
                        size_t totalFields,
                        size_t countUrls, 
                        const std::string& bestUrl, 
                        size_t bestMeanFieldsPerPage);

  void print_json();

};

#endif