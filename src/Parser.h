#ifndef parser_h
#define parser_h

#include <string>
#include <tuple>

namespace parser {

enum class CommandType {
  WRONG = 0,
  INSERT,
  TRUNCATE,
  INTERSECTION,
  SYMMETRIC_DIFFERENCE,
};

enum Names {
  TABLE_NAME = 0,
  ID,
  FIELD,
};

uint getPort(int argc, char *argv[]);
CommandType getCommandType(const std::string& str);
std::tuple<std::string,uint,std::string> getInsertData(const std::string& str);
std::string getTruncateData(const std::string& str);
}

#endif
