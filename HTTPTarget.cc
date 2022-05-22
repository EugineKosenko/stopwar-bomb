#include "HTTPTarget.hh"



void HTTPTarget::call() {
  auto s = new tcp::socket(ioService);
  nextRequest(s);
  _genCount += 1;
}

void HTTPTarget::nextRequest(tcp::socket *s) {
  try {
    //auto a = address();
    //boost::asio::async_connect(*s, a, boost::bind(&HTTPTarget::sendRequest, this, boost::asio::placeholders::error, a, s));
  } catch (std::exception &e) {
    if (_isDebugged) {
      std::cout << url() << " resolve: " << e.what() << std::endl;
    }
    delete s;
    _ioError = boost::system::error_code(boost::system::errc::stream_timeout
                                         , boost::system::system_category());
    _httpStatus = 0;
    _genCount -= 1;
  }
}
void HTTPTarget::sendRequest(const boost::system::error_code& e
                         , tcp::resolver::iterator a
                         , tcp::socket *s) {
  if (e) {
    if (_isDebugged) {
      std::cout << url() << " connect: " << e.message() << std::endl;
    }
    delete s;
    _ioError = e;
    _httpStatus = 0;
    _genCount -= 1;
    return;
  }

  boost::asio::streambuf req;
  std::ostream rs(&req);
  rs << reqHeader();
  
  boost::system::error_code ee;
  boost::asio::write(*s, req, ee);
  if (ee) {
    if (!_isDebugged) {
      std::cout << url() << " write: " << ee.message() << std::endl;
    }
    delete s;
    _ioError = ee;
    _httpStatus = 0;
    _genCount -= 1;
    return;
  }

  boost::asio::streambuf resp;
  boost::asio::read_until(*s, resp, "\n", ee);
  
  if (ee) {
    if (!_isDebugged) {
      std::cout << url() << " read: " << ee.message() << std::endl;
    }
    delete s;
    _ioError = ee;
    _httpStatus = 0;
    _genCount -= 1;
    return;
  }
  
  std::istream respStream(&resp);
  std::string httpVersion;
  respStream >> httpVersion;
  respStream >> _httpStatus;
  if (_isDebugged) {
    std::cout << url() << " status: " << _httpStatus << std::endl;
  }
  if (_httpStatus != 200 && _httpStatus != 301) {
    if (_isDebugged) {
      boost::asio::read(*s, resp, ee);
      std::string line;
      while (std::getline(respStream, line)) {
        std::cout << line << std::endl;
      }
    }
    delete s;
    _genCount -= 1;
    return;
  }

  delete s;
  call();
  call();
}
