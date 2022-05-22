#ifndef _HTTPS_TARGET_HH_
#define _HTTPS_TARGET_HH_

#include "WebTarget.hh"
#include <boost/bind/bind.hpp>
#include <iostream>
#include <boost/asio/ssl.hpp>

using namespace boost::asio;

class HTTPSTarget: public WebTarget {
public:
  
  void call();
  HTTPSTarget(std::string service, std::string host, unsigned port, std::string path, std::string params)
    : WebTarget(service, host, port, path, params) {}
  HTTPSTarget(Json::Value json): WebTarget(json) {}
private:
  class Generator {
  public:
    ssl::stream<ip::tcp::socket> _socket;
    boost::asio::streambuf _buffer;
    Generator()
      : _socket(HTTPSTarget::ioService, HTTPSTarget::_sslContext)
      , _buffer() { }
  };
  static ssl::context _sslContext;
  void resolve();
  void connect(const boost::system::error_code &
               , tcp::resolver::iterator);
  void handshake(const boost::system::error_code &, Generator *);
  void send(const boost::system::error_code &, Generator *);
  void receive(const boost::system::error_code &
               , std::size_t
               , Generator *);
  void process(const boost::system::error_code &
               , std::size_t
               , Generator *);

  
};

#endif
