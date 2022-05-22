#ifndef _TARGET_HH_
#define _TARGET_HH_

#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class Target {
public:
  
  static boost::asio::io_service ioService;
  tcp::resolver::iterator address() const;
  bool isActive() const;
  void firstRequest();
  void nextRequest(tcp::socket *);
  Target(unsigned /* id */
         , std::string /* proto */
         , std::string /* host */
         , unsigned /* port */
         , std::string /* path */);
  std::ostream &show(std::ostream &) const;
private:
  
  unsigned _id;
  std::string _proto;
  std::string _host;
  unsigned _port;
  std::string _path;
  unsigned _genCount;
  void sendRequest(const boost::system::error_code&
                   , tcp::resolver::iterator
                   , tcp::socket *);

  
};

#endif
