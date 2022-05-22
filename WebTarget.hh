#ifndef _WEB_TARGET_HH_
#define _WEB_TARGET_HH_

#include <string>
#include <boost/asio.hpp>
#include <jsoncpp/json/json.h>

using boost::asio::ip::tcp;

class WebTarget {
public:
  
  unsigned genCount() const { return _genCount; }
  static boost::asio::io_service ioService;
  void debug(bool isDebugged = true) { _isDebugged = isDebugged; }
  std::string ioStatus() const { return _ioStatus; }
  boost::system::error_code ioError() const { return _ioError; }
  unsigned httpStatus() const { return _httpStatus; }
  bool isActive() const;
  bool isFinished() const;
  std::string url() const {
    return
      _service + "://" + _host
      + ((_port == 80 || _port == 443) ? "" : (":" + std::to_string(_port)))
      + _path + _params;
  }
  virtual void call() = 0;
  static WebTarget *make(Json::Value);
  std::ostream &show(std::ostream &) const;
protected:
  
  std::string _service;
  std::string _host;
  unsigned _port;
  std::string _path;
  std::string _params;
  unsigned _genCount;
  static tcp::resolver _resolver;
  bool _isDebugged;
  std::string _ioStatus;
  boost::system::error_code _ioError;
  unsigned _httpStatus;
  std::string reqHeader() const {
    return
      "GET " + (_path == "" ? "/" : _path) + _params + " HTTP/1.1\n"
      + "Host: " + _host + "\n"
      + "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.40 Safari/537.36Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.40 Safari/537.36\n"
      + "Connection: close\n\n";
  }
  WebTarget(std::string /* service */
            , std::string /* host */
            , unsigned /* port */
            , std::string /* path */
            , std::string /* params */);
  WebTarget(Json::Value);

  
private:
  
  

  
};

#endif
