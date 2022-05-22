#ifndef _TARGET_HH_
#define _TARGET_HH_

#include "WebTarget.hh"
#include <boost/bind/bind.hpp>
#include <iostream>



class HTTPTarget: public WebTarget {
public:
  
  HTTPTarget(std::string service, std::string host, unsigned port, std::string path, std::string params):
    WebTarget(service, host, port, path, params) {}
  HTTPTarget(Json::Value json): WebTarget(json) {}
private:
  
  void call();
  void nextRequest(tcp::socket *);
  void sendRequest(const boost::system::error_code&
                   , tcp::resolver::iterator
                   , tcp::socket *);

  
};

#endif
