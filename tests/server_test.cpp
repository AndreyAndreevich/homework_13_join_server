#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Server.h"
#include <iostream>

using namespace boost::asio;

std::mutex mtx;
std::condition_variable cv;
bool is_ready = false;
char data[128];

auto start_server(io_service& service, unsigned short port) {
    auto thread = std::make_unique<std::thread>([&service,port](){
        ip::tcp::endpoint ep(ip::tcp::v4(),port);
        Server server(service, ep, std::make_shared<Storage>()); 
        {
          std::unique_lock<std::mutex> lk(mtx);
          is_ready = true;
          cv.notify_one();
        }
        service.run();
    });
    {
      std::unique_lock<std::mutex> lk(mtx);
      cv.wait(lk,[]() {
          return is_ready;
      });
    }
    return std::move(thread);
}

void check_message(ip::tcp::socket& sock, std::string request, std::string response) {
    write(sock, buffer(request, request.size()));
    BOOST_CHECK_EQUAL(sock.read_some(buffer(data)),response.size());
    auto data_str = std::string(data);
    data_str.resize(response.size());
    BOOST_CHECK_EQUAL(data_str,response);
}

BOOST_AUTO_TEST_SUITE(server_test)
    
    BOOST_AUTO_TEST_CASE(insert_and_truncate)
    {
        io_service service;
        auto server_thread = start_server(service,9000);
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 9000);
        ip::tcp::socket sock(service);
        sock.connect(ep);

        check_message(sock, "INSERT A 0 lean\n", "OK\n");
        check_message(sock, "...\n", "ERR request failed\n");
        check_message(sock, "INSERT A 0 lean\n", "ERR duplicate 0\n");
        check_message(sock, "INSERT A -1 lean\n", "ERR Incorrect id: INSERT A -1 lean\n");
        check_message(sock, "INSERT A 1 sweater\n", "OK\n");
        check_message(sock, "TRUNCATE A ...\n", "ERR Incorrect context: TRUNCATE A ...\n");
        check_message(sock, "INSERT B 1 ...\n", "OK\n");
        check_message(sock, "INSERT C 1 ...\n", "ERR Table C not exist\n");
        check_message(sock, "TRUNCATE A\n", "OK\n");

        service.stop();
        server_thread->join();
    }

    BOOST_AUTO_TEST_CASE(intersaction_and_symmetric_difference)
    {
        is_ready = false;
        io_service service;
        auto server_thread = start_server(service,9000);
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 9000);
        ip::tcp::socket sock(service);
        sock.connect(ep);

        check_message(sock, "INSERT A 0 lean\n", "OK\n");
        check_message(sock, "INSERT A 1 sweater\n", "OK\n");
        check_message(sock, "INSERT A 2 frank\n", "OK\n");
        check_message(sock, "INSERT A 3 violation\n", "OK\n");
        check_message(sock, "INSERT A 4 quality\n", "OK\n");
        check_message(sock, "INSERT A 5 precision\n", "OK\n");
        check_message(sock, "INSERT B 3 proposal\n", "OK\n");
        check_message(sock, "INSERT B 4 example\n", "OK\n");
        check_message(sock, "INSERT B 5 lake\n", "OK\n");
        check_message(sock, "INSERT B 6 flour\n", "OK\n");
        check_message(sock, "INSERT B 7 wonder\n", "OK\n");
        check_message(sock, "INSERT B 8 selection\n", "OK\n");
        std::string response =  "3,violation,proposal\n"
                                "4,quality,example\n"
                                "5,precision,lake\n"
                                "OK\n";
        check_message(sock, "INTERSECTION\n", response);

        response =  "0,lean,\n"
                    "1,sweater,\n"
                    "2,frank,\n"
                    "6,,flour\n"
                    "7,,wonder\n"
                    "8,,selection\n"
                    "OK\n";
        check_message(sock, "SYMMETRIC_DIFFERENCE\n", response);

        service.stop();
        server_thread->join();
    }

BOOST_AUTO_TEST_SUITE_END()
