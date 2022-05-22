#include "WebTarget.hh"
#include "HTTPTarget.hh"
#include "HTTPSTarget.hh"

boost::asio::io_service WebTarget::ioService;
tcp::resolver WebTarget::_resolver(ioService);

bool WebTarget::isActive() const {
  return _genCount > 0;
}

bool WebTarget::isFinished() const {
  return !isActive() && _ioError.value() == 0 && _httpStatus == 0;
}
WebTarget::WebTarget(std::string service
                     , std::string host
                     , unsigned port
                     , std::string path
                     , std::string params)
  : _service(service)
  , _host(host)
  , _port(port)
  , _path(path)
  , _params(params)
  , _genCount(0)
  , _isDebugged(false)
  , _ioStatus("none")
  , _ioError()
  , _httpStatus(0) { }
WebTarget::WebTarget(Json::Value json)
  :  WebTarget(json["service"].asString()
               , json["host"].asString()
               , json["port"].asUInt()
               , json["path"].asString()
               , json["params"].asString()) { }
WebTarget *WebTarget::make(Json::Value json) {
  if (json["service"].asString() == "http") {
    return new HTTPTarget(json);
  } else if (json["service"].asString() == "https") {
    return new HTTPSTarget(json);
  } else {
    throw "Unknown service";
  }
}
std::ostream &WebTarget::show(std::ostream &os) const {
  os << url() << " " << _ioStatus << ": " << _ioError.message() << " " << _httpStatus << ", " << _genCount << std::endl;
  return os;
}
