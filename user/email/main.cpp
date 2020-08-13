#include "aliConn.h"
#include "aliMail.h"
#include <unistd.h>

int main() {
  AliMail mail("panghao@221data.com", "3351970-ph");
  mail.sendMail("testmail", "This is testmail", "panghao@221data.com");
  while(true) {
    sleep(10);
  }
  return 0;
}
