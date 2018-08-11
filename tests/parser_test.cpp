#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Parser.h"

BOOST_AUTO_TEST_SUITE(parser)

  BOOST_AUTO_TEST_CASE(port)
  {
    char* argv[2];
    argv[1] = "9000";
    BOOST_CHECK_THROW(parser::getPort(0,argv),std::exception);
    BOOST_CHECK_THROW(parser::getPort(0,argv),std::exception);
    BOOST_CHECK_THROW(parser::getPort(1,argv),std::exception);
    BOOST_CHECK_EQUAL(parser::getPort(2,argv),9000);
    BOOST_CHECK_EQUAL(parser::getPort(10,argv),9000);
    argv[1] = "asd";
    BOOST_CHECK_THROW(parser::getPort(2,argv),std::exception);
    argv[1] = "0";
    BOOST_CHECK_THROW(parser::getPort(2,argv),std::exception);
    argv[1] = "-1";
    BOOST_CHECK_THROW(parser::getPort(2,argv),std::exception);
    argv[1] = "asd4";
    BOOST_CHECK_THROW(parser::getPort(2,argv),std::exception);
    argv[1] = "4fd";
    BOOST_CHECK_THROW(parser::getPort(10,argv),std::exception);
    argv[1] = "4";
    BOOST_CHECK_EQUAL(parser::getPort(10,argv),4);
  }

  BOOST_AUTO_TEST_CASE(command_code)
  {
    BOOST_CHECK(parser::getCommandType("incorrect string") == parser::CommandType::WRONG);
    BOOST_CHECK(parser::getCommandType("INSERT ...") == parser::CommandType::INSERT);
    BOOST_CHECK(parser::getCommandType("... INSERT ...") == parser::CommandType::WRONG);
    BOOST_CHECK(parser::getCommandType("TRUNCATE ...") == parser::CommandType::TRUNCATE);
    BOOST_CHECK(parser::getCommandType("INTERSECTION") == parser::CommandType::INTERSECTION);
    BOOST_CHECK(parser::getCommandType("INTERSECTION ...") == parser::CommandType::WRONG);
    BOOST_CHECK(parser::getCommandType("... INTERSECTION") == parser::CommandType::WRONG);
    BOOST_CHECK(parser::getCommandType("SYMMETRIC_DIFFERENCE") == parser::CommandType::SYMMETRIC_DIFFERENCE);
    BOOST_CHECK(parser::getCommandType("SYMMETRIC_DIFFERENCE ...") == parser::CommandType::WRONG);
    BOOST_CHECK(parser::getCommandType("... SYMMETRIC_DIFFERENCE") == parser::CommandType::WRONG);
  }

  BOOST_AUTO_TEST_CASE(insert)
  {
    BOOST_CHECK_THROW(parser::getInsertData("..."),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("... table_name 0 field"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT 0 field"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name field"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name 0"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name  field"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name 0 field field"),std::exception);
    BOOST_CHECK_NO_THROW(parser::getInsertData("INSERT table_name 0 field"));
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name -1 field"),std::exception);
    BOOST_CHECK_THROW(parser::getInsertData("INSERT table_name 4s field"),std::exception);

    std::string table_name;
    uint id;
    std::string field;
    std::tie(table_name,id,field) = parser::getInsertData("INSERT table_name 12 field");
    BOOST_CHECK_EQUAL(table_name,"table_name");
    BOOST_CHECK_EQUAL(id,12);
    BOOST_CHECK_EQUAL(field,"field");
  }

  BOOST_AUTO_TEST_CASE(truncate) 
  {
    BOOST_CHECK_THROW(parser::getTruncateData("..."),std::exception);
    BOOST_CHECK_THROW(parser::getTruncateData("TRUNCATE"),std::exception);
    BOOST_CHECK_THROW(parser::getTruncateData("TRUNCATE   "),std::exception);
    BOOST_CHECK_THROW(parser::getTruncateData("TRUNCATE table_name ..."),std::exception);
    BOOST_CHECK_NO_THROW(parser::getTruncateData("TRUNCATE table_name"));
    BOOST_CHECK_EQUAL(parser::getTruncateData("TRUNCATE table_name"),"table_name");
  }

BOOST_AUTO_TEST_SUITE_END()