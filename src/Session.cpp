#include "Session.h"
#include "Parser.h"
#include <iostream>

Session::Session(tcp::socket socket, const std::shared_ptr<Storage>& storage) 
  : socket_(std::move(socket))
  , storage_(storage)
{

}

void Session::start() {
  do_read();
}

void Session::do_read() {
  auto self(shared_from_this());
  boost::asio::async_read_until(socket_, request_,  "\n",
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        std::istream is(&request_);
        std::string line;
        while (std::getline(is, line)) {
          try {
            auto command_type = parser::getCommandType(line);
            switch(command_type) {
              case parser::CommandType::WRONG:
                response_.push_back("ERR request failed\n");
                break;
              case parser::CommandType::INSERT: {
                std::string table_name;
                uint id;
                std::string field;
                std::tie(table_name,id,field) = parser::getInsertData(line);
                storage_->insert(table_name,id,field);
                response_.push_back("OK\n");
                break;
              } 
              case parser::CommandType::TRUNCATE:
                storage_->truncate(parser::getTruncateData(line));
                response_.push_back("OK\n");                
                break;
              case parser::CommandType::INTERSECTION:
                response_.push_back(storage_->intersection() + "OK\n");
                break;
              case parser::CommandType::SYMMETRIC_DIFFERENCE:
                response_.push_back(storage_->symmetric_difference() + "OK\n");
                break;
            } 
          } catch (const std::exception& err) {
            response_.push_back(std::string("ERR ") + err.what() + "\n");
          }
          do_write();
        }
        do_read();
      }
    });
}

void Session::do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(socket_,
    boost::asio::buffer(response_.front(), response_.front().size()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        response_.pop_front();
        if (!response_.empty())
        {
          do_write();
        }
      }
    });
}