#include "aliMail.h"
#include "aliConn.h"
#include <functional>
#include "pbase64/base64.h"

static int ehloHandle(const char *info,
    AliMail *m) {
  if (strstr(info, "DSN")) {
    m->setState(AliMail::MailState::AUTH);
    return 0;
  }
  return 1;
}

static int authHandle(const char *info,
    AliMail *m) {
  m->setState(AliMail::MailState::LOGIN);
  return 0;
}
static int loginHandle(const char *info,
    AliMail *m) {
  m->setState(AliMail::MailState::MESSAGE);
  return 0;
}
static int messageHandle(const char *info,
      AliMail *m) {
    //m->setState(AliMail::MailState::QUIT);
    return 0;
}

static int quitHandle(const char *info,
    AliMail *m) {
  m->setState(AliMail::MailState::QUIT);
  return 0;
}

AliMail::AliDriver AliMail::drivers_[]  =
{
  {MailState::EHLO, ehloHandle},
  {MailState::AUTH, authHandle},
  {MailState::LOGIN, loginHandle},
  {MailState::MESSAGE, messageHandle},
  {MailState::QUIT, quitHandle},
  {MailState::DONE, NULL}
};

int AliMail::login(const std::string &userName,
    const std::string &pass) {
  conn_->sendMess("AUTH LOGIN\r\n", aliProcess, this);
  std::string account;
  std::string s = userName;
  std::vector<unsigned char> data(s.begin(), s.end());
  Base64::getBase64().encode(data, account);
  conn_->sendMess(account.c_str(), aliProcess, this);
  s = pass;
  std::string pass64;
  data.assign(s.begin(), s.end());
  Base64::getBase64().encode(data, pass64);
  conn_->sendMess(pass64.c_str(), aliProcess, this);
  return 0;

}

AliMail::AliMail(const std::string &name,
    const std::string &pass) :name_(name),pass_(pass){
  init();
}

AliMail::~AliMail() {
  conn_->sendMess( "QUIT\r\n", aliProcess, this);
  delete conn_;
}

int AliMail::init() {
  conn_ = new AliConn();
  int rc = 0;
  rc = ehlo();
  if (rc != 0) {
    return -1;
  }
  rc = login(name_, pass_);
  return rc;
}

int AliMail::sendMail(const std::string &subject,
    const std::string &body,
    const std::string &to) {
  conn_->sendMess("MAIL FROM:<panghao@221data.com>",
      aliProcess, this);
  conn_->sendMess("RCPT TO:<panghao@221data.com>",
      aliProcess, this);
  conn_->sendMess( "DATA\r\n", aliProcess, this);
  char buf[128];
  snprintf(buf,sizeof(buf), "subject:%s\r\n", subject.c_str());
  conn_->sendMess(buf, NULL, NULL);
  snprintf(buf, sizeof(buf), "from:<%s>\r\n", name_.c_str());
  conn_->sendMess(buf,NULL, NULL);
  snprintf(buf, sizeof(buf), "to:<%s>\r\n\r\n", to.c_str());
  conn_->sendMess(buf, NULL, NULL);
  snprintf(buf, sizeof(buf),"%s\r\n.\r\n",
      body.c_str());
  conn_->sendMess(buf, aliProcess, this);
  return 0;
}

int AliMail::aliProcess(const char*info,
    AliMail  *m) {
  std::cout << m->state_ << std::endl;
  if (m->drivers_[m->state_].handle) {
    return m->drivers_[m->state_].handle(info, m);
  }
  return -1;
}

int AliMail::ehlo() {
  state_ = MailState::EHLO;
  return conn_->sendMess("EHLO centos\r\n", aliProcess, this);
}

