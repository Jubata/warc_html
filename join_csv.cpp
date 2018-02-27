#include <iostream>
#include "stats.h"
#include <string>
#include "csv/istream.hpp"

//concatenate several records on unique urls
//summarizes urls count, and total fields
//selects bestUrl, bestMeanFieldsPerPage according to maximum bestMeanFieldsPerPage
int main(int argc, char* argv[]) {
  Stats stats;

  namespace csv = ::text::csv;
  csv::csv_istream csvs(std::cin);
  std::string url;
  size_t totalFields;
  size_t countUrls;
  std::string bestUrl;
  size_t bestMeanFieldsPerPage;

  while(csvs) {
    csvs >> url;
    csvs >> totalFields;
    csvs >> countUrls;
    csvs >> bestUrl;
    csvs >> bestMeanFieldsPerPage;
    if(csvs.has_more_fields()) {
      std::cerr<<"too many columns at line "<<csvs.line_number();
      return -1;
    }

    stats.addEntity(url, totalFields, countUrls, bestUrl, bestMeanFieldsPerPage);
  }

  stats.print_json();
}