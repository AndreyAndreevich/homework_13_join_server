#include "Parser.h"
#include <stdexcept>
#include <vector>

namespace parser {

uint getPort(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("The parameter is missing");
  }
  auto N = std::atoi(argv[1]);
  if (N <= 0 || std::string(argv[1]) != std::to_string(N)) {
    throw std::runtime_error("Incorrect parameter");
  }
  return N;
}

CommandType getCommandType(const std::string& str) {
  if (str.find("INSERT") == 0) return CommandType::INSERT;
  if (str.find("TRUNCATE") == 0) return CommandType::TRUNCATE;
  if (str == "INTERSECTION") return CommandType::INTERSECTION;
  if (str == "SYMMETRIC_DIFFERENCE") return CommandType::SYMMETRIC_DIFFERENCE;
  return CommandType::WRONG;
}

std::tuple<std::string,uint,std::string> getInsertData(const std::string& str) {
  std::string prefix = "INSERT ";
  auto context_size(3);
  std::tuple<std::string,uint,std::string> data;
  if (str.find(prefix) != 0) {
    throw std::runtime_error("Incorrect insert: " + str);
  }

  char separator = ' ';
  auto start = prefix.size();
  auto stop = str.find_first_of(separator,start);
  std::vector<std::string> context;
  context.reserve(context_size);

  auto verification = [&context](const auto& substr) {
    if (!substr.empty()) {
      context.push_back(substr);
    } 
  };

  while(stop != std::string::npos)
  {
    verification(str.substr(start, stop - start));
    start = stop + 1;
    stop = str.find_first_of(separator, start);
  }
  verification(str.substr(start));

  if (context.size() != context_size) {
    throw std::runtime_error("Incorrect context: " + str);
  }

  auto id = std::atoi(context.at(1).c_str());
  if (id < 0 || context.at(1) != std::to_string(id)) {
    throw std::runtime_error("Incorrect id: " + str);
  }

  std::get<TABLE_NAME>(data) = context.at(0);
  std::get<ID>(data) = id;
  std::get<FIELD>(data) = context.at(2);
 
  return data;
}

std::string getTruncateData(const std::string& str) {
  std::string prefix = "TRUNCATE ";
  std::string data;

  if (str.find(prefix) != 0) {
    throw std::runtime_error("Incorrect truncate: " + str);
  }

  char separator = ' ';
  auto start = prefix.size();
  if (str.find_first_of(separator,start) != std::string::npos) {
    throw std::runtime_error("Incorrect context: " + str);
  }

  data = str.substr(start);
  if (data.empty()) {
    throw std::runtime_error("Table name is empty: " + str);
  }

  return data;
}

}