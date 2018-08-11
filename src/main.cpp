#include <iostream>
#include <exception>
#include "Parser.h"
#include "Storage.h"
#include "Server.h"

using namespace boost::asio;

int main(int argc, char *argv[])
{
  try {
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(),parser::getPort(argc,argv));
    auto storage = std::make_shared<Storage>();
    Server server(service, ep, storage);
    service.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
