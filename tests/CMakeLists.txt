cmake_minimum_required(VERSION 3.2)

function(add_properties name)
        set_target_properties(${name} PROPERTIES
                COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK 
                CXX_STANDARD_REQUIRED ON   
                COMPILE_OPTIONS -Wpedantic -Wall -Wextra
                )
endfunction(add_properties)


include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIR}
        )

add_executable(parser_tst parser_test.cpp ${CMAKE_SOURCE_DIR}/src/Parser.cpp)
add_properties(parser_tst)
target_link_libraries(parser_tst ${Boost_LIBRARIES})
add_test(parser_tst parser_tst)

add_executable(storage_tst storage_test.cpp ${CMAKE_SOURCE_DIR}/src/Storage.cpp)
add_properties(storage_tst)
target_link_libraries(storage_tst ${Boost_LIBRARIES})
add_test(storage_tst storage_tst)

set(SERVER_SOURCE
        ${CMAKE_SOURCE_DIR}/src/Parser.cpp   
        ${CMAKE_SOURCE_DIR}/src/Storage.cpp
        ${CMAKE_SOURCE_DIR}/src/Session.cpp
        ${CMAKE_SOURCE_DIR}/src/Server.cpp 
)

add_executable(server_tst server_test.cpp ${SERVER_SOURCE})
add_properties(server_tst)
target_link_libraries(server_tst ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(server_tst server_tst)

#target_link_libraries(${TEST_NAME}
#        ${Boost_LIBRARIES}
#        ${CMAKE_THREAD_LIBS_INIT}
#        )

#add_test(${TEST_NAME} ${TEST_NAME})