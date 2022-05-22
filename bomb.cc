#include <boost/asio.hpp>
#include <iostream>
#include "WebTarget.hh"
#include <jsoncpp/json/json.h>
#include <list>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;



void monitor(const boost::system::error_code &
           , std::list<WebTarget *> *ts
           , boost::asio::deadline_timer *t) {
  std::cout << "Active:\n";
  for (auto i = ts->begin(); i != ts->end(); i++) {
    if ((*i)->isActive()) {
      if ((*i)->genCount() > 1) {
        (*i)->show(std::cout);
      }
    } else {
      (*i)->call();
    }
  }
  t->expires_at(t->expires_at() + boost::posix_time::seconds(60));
  t->async_wait(boost::bind(monitor, boost::asio::placeholders::error, ts, t));
}

int main(int argc, char *argv[]) {
  boost::asio::io_service io;
  tcp::socket s(io);
  boost::asio::connect(s, tcp::resolver(io).resolve(tcp::resolver::query("stopwar.kosenko.info", "http")));
  
  boost::asio::streambuf rb;
  std::ostream os(&rb);
  os << "GET /targets/web HTTP/1.1\n"
     << "Host: stopwar.kosenko.info\n"
     << "Connection: close\n\n";
  
  boost::asio::write(s, rb);
  boost::asio::read_until(s, rb, "\n");
  
  std::istream is(&rb);
  std::string httpVersion;
  unsigned statusCode;
  std::string statusMessage;
  is >> httpVersion
     >> statusCode;
  getline(is, statusMessage);
  std::cout << httpVersion << std::endl
            << statusCode << std::endl
            << statusMessage << std::endl;
  
  boost::asio::read_until(s, rb, "\r\n");
  std::string header;
  while (getline(is, header) && header != "\r") {
    std::cout << header << std::endl;
  }
  
  boost::system::error_code e;
  boost::asio::read(s, rb, e);
  std::string result;
  getline(is, result);
  std::cout << result;
  std::istringstream ps(result);
  Json::Value root;
  ps >> root;
  
  std::list<WebTarget *> ts;
  for (auto i = root.begin(); i != root.end(); i++) {
    if ((*i)["service"].asString() == "https") {
      ts.push_back(WebTarget::make(*i));
    }
  }
  boost::asio::deadline_timer t(WebTarget::ioService, boost::posix_time::seconds(0));
  t.async_wait(boost::bind(monitor, boost::asio::placeholders::error, &ts, &t));
  
  WebTarget::ioService.run();

  return 0;
}
