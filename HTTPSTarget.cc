#include "HTTPSTarget.hh"

ssl::context HTTPSTarget::_sslContext(ssl::context::method::sslv23_client);

void HTTPSTarget::call() {
  _genCount += 1;

  _ioStatus = "none";
  _ioError = boost::system::error_code();
  _httpStatus = 0;

  resolve();
}
void HTTPSTarget::resolve() {
  _ioStatus = "called";

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << std::endl;
  }

  _resolver.async_resolve(tcp::resolver::query(_host, std::to_string(_port))
                          , boost::bind(&HTTPSTarget::connect, this
                                        , boost::asio::placeholders::error
                                        , boost::asio::placeholders::iterator));
  _ioStatus = "resolve";
}
void HTTPSTarget::connect(const boost::system::error_code &e
                          , tcp::resolver::iterator a) {
  _ioStatus = "resolved";
  _ioError = e;

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << ": " << _ioError.message() << std::endl;
  }

  if (_ioError) {
    _genCount -= 1;
    return;
  }

  auto g = new Generator();
  boost::asio::async_connect(g->_socket.lowest_layer(), a
                             , boost::bind(&HTTPSTarget::handshake
                                           , this, boost::asio::placeholders::error, g));
  _ioStatus = "connect";
}
void HTTPSTarget::handshake(const boost::system::error_code &e
                            , Generator *g) {
  _ioStatus = "connected";
  _ioError = e;

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << ": " << _ioError.message() << std::endl;
  }

  if (_ioError) {
    delete g;
    _genCount -= 1;
    return;
  }

  g->_socket.async_handshake(ssl::stream_base::handshake_type::client
                             , boost::bind(&HTTPSTarget::send
                                           , this, boost::asio::placeholders::error, g));
  _ioStatus = "handshake";
}
void HTTPSTarget::send(const boost::system::error_code &e
                            , Generator *g) {
  _ioStatus = "handshaked";
  _ioError = e;

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << ": " << _ioError.message() << std::endl;
  }

  if (_ioError) {
    delete g;
    _genCount -= 1;
    return;
  }

  std::ostream os(&g->_buffer);
  os << reqHeader();

  boost::asio::async_write(g->_socket, g->_buffer
                           , boost::bind(&HTTPSTarget::receive, this
                                         , boost::asio::placeholders::error
                                         , boost::asio::placeholders::bytes_transferred
                                         , g));
  _ioStatus = "send";
}
void HTTPSTarget::receive(const boost::system::error_code &e
                          , std::size_t bt
                          , Generator *g) {
  _ioStatus = "sent";
  _ioError = e;

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << ": " << _ioError.message() << " " << g->_buffer.size() << "/" << bt << std::endl;
  }

  if (_ioError || g->_buffer.size() != 0) {
    delete g;
    _genCount -= 1;
    return;
  }

  boost::asio::async_read_until(g->_socket, g->_buffer, "\n"
                                , boost::bind(&HTTPSTarget::process, this
                                              , boost::asio::placeholders::error
                                              , boost::asio::placeholders::bytes_transferred
                                              , g));
  _ioStatus = "receive";
}
void HTTPSTarget::process(const boost::system::error_code &e
                          , std::size_t br
                          , Generator *g) {
  _ioStatus = "received";
  _ioError = e;

  if (_isDebugged) {
    std::cout << url() << " " << _ioStatus << ": " << _ioError.message() << std::endl;
  }

  if (_ioError) {
    delete g;
    _genCount -= 1;
    return;
  }

  _ioStatus = "processed";

  std::istream is(&g->_buffer);
  std::string v;
  is >> v;
  is >> _httpStatus;

  if (_isDebugged) {
    std::string m;
    std::getline(is, m);
    std::cout << url() << " " << _ioStatus << ": " << v << " " << _httpStatus << " " << m << std::endl;
  }

  if (_httpStatus != 200
      && _httpStatus != 301
      && _httpStatus != 302
      && _httpStatus != 303
      ) {
    if (_isDebugged) {
      boost::system::error_code e;
      boost::asio::read(g->_socket, g->_buffer, e);
      std::string l;
      while (std::getline(is, l)) {
        std::cout << l << std::endl;
      }
    }
    delete g;
    _genCount -= 1;
    return;
  }

  delete g;
  resolve();
  if (!_isDebugged) {
    call();
  }
}
