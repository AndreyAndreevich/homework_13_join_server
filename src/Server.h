#ifndef server_h
#define server_h

#include "Storage.h"
#include <boost/asio.hpp>

class Server {
  using io_service = boost::asio::io_service;
  using tcp = boost::asio::ip::tcp;
public:
  Server(io_service&, const tcp::endpoint&, const std::shared_ptr<Storage>&);
  ~Server();
private:
  void do_accept(const std::shared_ptr<Storage>&);
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif