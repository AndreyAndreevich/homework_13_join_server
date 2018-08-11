#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Storage.h"

struct TestStorage : Storage {
  table_t& getTable(const std::string& table_name) {
    return Storage::getTable(table_name);
  } 
};

BOOST_AUTO_TEST_SUITE(storage)

  BOOST_AUTO_TEST_CASE(insert)
  {
    TestStorage storage;
    BOOST_CHECK_THROW(storage.insert("",1,"field"),std::exception);
    BOOST_CHECK_NO_THROW(storage.insert("A",1,"field"));
    BOOST_CHECK_EQUAL(storage.getTable("A").size(),1);
    BOOST_CHECK_EQUAL(storage.getTable("B").size(),0);
    BOOST_CHECK_EQUAL(storage.getTable("A").at(1),"field");

    BOOST_CHECK_THROW(storage.insert("A",1,"field1"),std::exception);
    BOOST_CHECK_EQUAL(storage.getTable("A").at(1),"field");

    BOOST_CHECK_NO_THROW(storage.insert("A",2,"field2"));
    BOOST_CHECK_EQUAL(storage.getTable("A").size(),2);
    BOOST_CHECK_EQUAL(storage.getTable("A").at(2),"field2");

    BOOST_CHECK_NO_THROW(storage.insert("B",2,"field3"));
    BOOST_CHECK_EQUAL(storage.getTable("B").size(),1);
    BOOST_CHECK_EQUAL(storage.getTable("B").at(2),"field3");
    BOOST_CHECK_THROW(storage.insert("C",1,"field4"),std::exception);
  }

  BOOST_AUTO_TEST_CASE(truncate)
  {
    TestStorage storage;
    storage.insert("A",0,"field0");
    storage.insert("A",1,"field1");
    storage.insert("A",2,"field2");
    storage.insert("B",1,"field3");
    storage.insert("B",2,"field4");
    BOOST_CHECK_THROW(storage.truncate(""),std::exception);
    BOOST_CHECK_THROW(storage.truncate("..."),std::exception);
    BOOST_CHECK_NO_THROW(storage.truncate("A"));
    BOOST_CHECK_EQUAL(storage.getTable("A").size(),0);
  }


    BOOST_AUTO_TEST_CASE(intersection)
  {
    TestStorage storage;
    BOOST_CHECK_EQUAL(storage.intersection(),"");
    storage.insert("A",1,"field1");
    storage.insert("A",2,"field2");
    storage.insert("A",3,"field3");
    storage.insert("A",4,"field4");
    storage.insert("A",6,"field5");
    BOOST_CHECK_EQUAL(storage.intersection(),"");
    storage.insert("B",0,"field6");
    storage.insert("B",2,"field7");
    storage.insert("B",4,"field8");
    storage.insert("B",5,"field9");

    BOOST_CHECK_EQUAL(storage.intersection(),"2,field2,field7\n4,field4,field8\n");
  }

  BOOST_AUTO_TEST_CASE(symmetric_difference)
  {
    TestStorage storage;
    BOOST_CHECK_EQUAL(storage.symmetric_difference(),"");
    storage.insert("A",1,"field1");
    storage.insert("A",2,"field2");
    storage.insert("A",3,"field3");
    storage.insert("A",4,"field4");
    storage.insert("A",6,"field5");
    std::string result =  "1,field1,\n"
                          "2,field2,\n"
                          "3,field3,\n"
                          "4,field4,\n"
                          "6,field5,\n";
    BOOST_CHECK_EQUAL(storage.symmetric_difference(),result);

    storage.insert("B",0,"field6");
    storage.insert("B",2,"field7");
    storage.insert("B",4,"field8");
    storage.insert("B",5,"field9");
    result =  "0,,field6\n"
              "1,field1,\n"
              "3,field3,\n"
              "5,,field9\n"
              "6,field5,\n";

    storage.truncate("A");
    result =  "0,,field6\n"
              "2,,field7\n"
              "4,,field8\n"
              "5,,field9\n";
    BOOST_CHECK_EQUAL(storage.symmetric_difference(),result);
  }

BOOST_AUTO_TEST_SUITE_END()