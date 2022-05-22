#include "Target.hh"
#include <iostream>
#include <chrono>

Target ts[] =
  { Target(0, "http", "afanasy.biz", 80, "/")
    , Target(0, "http", "business-gazeta.ru", 80, "/")
    , Target(0, "http", "osnmedia.ru", 80, "/")
    , Target(0, "http", "rbc.ru", 80, "/")
    , Target(0, "http", "tvzvezda.ru", 80, "/")
    //, Target(0, "http", "360tv.ru", 80, "/")
  };

void print(const boost::system::error_code&
           , boost::asio::deadline_timer* t) {
  for (unsigned i = 0; i < 5; i += 1) {
    ts[i].show(std::cout);
    if (!ts[i].isActive()) {
      ts[i].firstRequest();
    }
  }
  t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
  t->async_wait(boost::bind(print
                            , boost::asio::placeholders::error, t));
}

void controller(Target ts[]) {
  while (true) {
    for (unsigned i = 0; i < 5; i += 1) {
      if (!ts[i].isActive()) {
        ts[i].firstRequest();
      }
    }
    Target::ioService.run_one();
  }
}

int main(int argc, char *argv[]) {
  boost::asio::io_service ioService;

  std::thread ct(controller, ts);

  while (true) {
    for (unsigned i = 0; i < 5; i += 1) {
      ts[i].show(std::cout);
    }
    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  ct.join();

  return 0;
}
