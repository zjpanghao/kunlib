#include <string>
#include <mutex>
#include <curl/curl.h>

namespace kun {
class CPost {
 public:
  static CPost & instance() {
    static CPost post;
    post.initOnce();
    return post;
  }
  size_t writeData(void *buffer, size_t size, size_t nmemb, void *userp);
  int post(const std::string &url, const std::string &body, std::string &result);
  void initOnce(){
    std::call_once(init_, []{
        curl_global_init(CURL_GLOBAL_ALL);
        });
  }
 private:
  CPost();
  std::once_flag  init_;
};

} // namespace kun
