#ifndef INCLUDE_ALIM_H
#define INCLUDE_ALIM_H
#include <string>
#include <mutex>
#include <memory>
class AliConn;
class AliMail {
 public:
  enum MailState {
    EHLO,
    AUTH,
    LOGIN,
    MESSAGE,
    QUIT,
    DONE
  };

  struct AliDriver {
    MailState  state;
    int (*handle)(const char *info,
        AliMail *m);
  };
    
  
  AliMail(const std::string &name,
      const std::string &pass);
  ~AliMail();

  int login(const std::string &userName,
          const std::string &pass);
  int init();
  int sendMail(const std::string &subject,
      const std::string &body,
      const std::string &to);
  static int aliProcess(const char *info,
     AliMail *m);
  MailState state() {
    return state_;
  }

  void setState(MailState st) {
    state_ = st;
  }
 private:
  std::unique_ptr<AliConn> conn_{nullptr};

  static AliDriver drivers_[] ;
  int ehlo();
  MailState state_{MailState::EHLO};
  std::once_flag flag_;
  std::string name_;
  std::string pass_;
  char bodyBuf_[1024*10];
};
#endif
