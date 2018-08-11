#include "Server.h"
#include "Session.h"

Server::Server(io_service& service, const tcp::endpoint& ep, const std::shared_ptr<Storage>& storage)
  : acceptor_(service, ep)
  , socket_(service)
{
  do_accept(storage);
}

Server::~Server() {

}

void Server::do_accept(const std::shared_ptr<Storage>& storage) {
  acceptor_.async_accept(socket_, [this,storage](boost::system::error_code ec) {
    if (!ec) {
      std::make_shared<Session>(std::move(socket_),storage)->start();
    }
    do_accept(storage);
  });
}