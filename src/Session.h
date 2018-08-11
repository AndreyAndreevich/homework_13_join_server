#ifndef session_h
#define session_h

#include "Storage.h"
#include <boost/asio.hpp>
#include <deque>

class Session : public std::enable_shared_from_this<Session> {
  using tcp = boost::asio::ip::tcp;
public:
  Session(tcp::socket,const std::shared_ptr<Storage>&);
  void start();
private:
  void do_read();
  void do_write();

  tcp::socket socket_;
  std::shared_ptr<Storage> storage_;
  boost::asio::streambuf request_;
  std::deque<std::string> response_;
};

#endif