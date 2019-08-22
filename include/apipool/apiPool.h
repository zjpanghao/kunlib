#ifndef INCLUDE_KUNLIB_API_POOL_H
#define INCLUDE_KUNLIB_API_POOL_H
#include <map>
#include <memory>

#include <string>
#include <vector>
#include <list>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
namespace kunyan {
 class Config;
}
template<class T>
class ApiBuffer {
 public:
  ApiBuffer() {
  }

  int init(int bufferNums) {
    for (int i = 0; i < bufferNums; i++) {
      auto api = getInitApi();
      api->init();
      if (api != nullptr) {
        apis_.push_back(api);
      }
    }
    return apis_.empty() ? -1 : 0; 
  }

  int init(int bufferNums, const kunyan::Config &config) {
    for (int i = 0; i < bufferNums; i++) {
      auto api = getInitApi();
      api->init(config);
      if (api != nullptr) {
        apis_.push_back(api);
      }
    }
    return apis_.empty() ? -1 : 0; 
  }

  void offerBufferedApi(std::shared_ptr<T> api) {
    if (api == nullptr) {
      return;
    }
    std::unique_lock<std::mutex> ulock(lock_);
    if (apis_.empty()) {
      bufferFull_.notify_one();
    }
    apis_.push_back(api);
  }

    std::shared_ptr<T> borrowBufferedApi() {
      std::unique_lock<std::mutex> ulock(lock_);
      bufferFull_.wait(ulock, [this](){return !apis_.empty();});
      auto api = apis_.front();
      apis_.pop_front();
      return api;
  }

 private:
  std::shared_ptr<T> getInitApi() {
    std::shared_ptr<T> api(new T());
    return api;
  }
  std::list<std::shared_ptr<T>> apis_;
  std::condition_variable bufferFull_;
  std::mutex lock_;
};

template<class T>
class ApiWrapper {
 public:
   explicit ApiWrapper(ApiBuffer<T> &buffers)
    : buffers_(buffers) {
      api_ = buffers.borrowBufferedApi();
   }
   ~ApiWrapper() {
     if (api_ != nullptr) {
       buffers_.offerBufferedApi(api_);
     }
   }

   std::shared_ptr<T> getApi() {
     return api_;
   }

 private:
   std::shared_ptr<T> api_{nullptr};
   ApiBuffer<T> &buffers_;
};
#endif
