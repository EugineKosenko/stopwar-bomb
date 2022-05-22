#include "Target.hh"

boost::asio::io_service Target::ioService;

tcp::resolver::iterator Target::address() const {
  tcp::resolver resolver(ioService);
  tcp::resolver::query query(_host, _proto);
  return resolver.resolve(query);
}
bool Target::isActive() const {
  return _genCount > 0;
}
void Target::firstRequest() {
  auto s = new tcp::socket(ioService);
  nextRequest(s);
  _genCount += 1;
}

void Target::nextRequest(tcp::socket *s) {
  try {
    auto a = address();
    boost::asio::async_connect(*s, a, boost::bind(&Target::sendRequest, this, boost::asio::placeholders::error, a, s));
  } catch (std::exception &e) {
    delete s;
    _genCount -= 1;
  }
}
void Target::sendRequest(const boost::system::error_code& e
                         , tcp::resolver::iterator a
                         , tcp::socket * s) {
  if (e) {
    delete s;
    _genCount -= 1;
    return;
  }

  boost::asio::streambuf req;
  std::ostream rs(&req);
  rs << "GET / HTTP/1.1\n";
  rs << "Host: " << _host << "\n";
  rs << "Connection: close\n\n";
  
  boost::system::error_code ee;
  boost::asio::write(*s, req, ee);
  if (ee) {
    delete s;
    _genCount -= 1;
    return;
  }

  boost::asio::streambuf resp;
  boost::asio::read_until(*s, resp, "\n", ee);
  
  if (ee) {
      delete s;
      _genCount -= 1;
      return;
  }
  
  std::istream respStream(&resp);
  std::string httpVersion;
  respStream >> httpVersion;
  unsigned statusCode;
  respStream >> statusCode;
  if (statusCode != 301) {
    delete s;
    _genCount -= 1;
    return;
  }

  s->close();
  nextRequest(s);
  firstRequest();
}
Target::Target(unsigned id
               , std::string proto
               , std::string host
               , unsigned port
               , std::string path)
  : _id(id)
  , _proto(proto)
  , _host(host)
  , _port(port)
  , _path(path)
  , _genCount(0) {
}

std::ostream &Target::show(std::ostream &os) const {
  os << _host << " " << _genCount << std::endl;
  return os;
}
