#include "HTTPSTarget.hh"
#include <iostream>

int main(int argc, char *argv[]) {
  HTTPSTarget t("https", argv[1], 443, "/index.php", "");
  t.debug();
  t.call();

  WebTarget::ioService.run();

  return 0;
}
