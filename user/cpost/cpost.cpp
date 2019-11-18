#include <functional>
#include <curl/curl.h>
#include "cpost.h"
#include <iostream>
#include <json/json.h>
#include <glog/logging.h>
namespace kun {
CPost::CPost() {
}

static size_t myWriteData(void *buffer, size_t size, size_t nmemb, void *userp) {
  LOG(INFO) << "recv:" << size << " nm:" << nmemb;
  std::string *result = (std::string *)userp;
  LOG(INFO) << "userp" << result;
  result->append((char*)buffer, 0, 
      size *nmemb); 
  return size * nmemb;
}
size_t CPost::writeData(void *buffer, size_t size, size_t nmemb, void *userp) {
  //std::string *result = (std::string *)userp;
  int nbytes = size *nmemb;
  //result->append((char*)buffer, 0, nbytes); 
  return nbytes;
}

int CPost::post(const std::string &url, const std::string &body, std::string &result) {
  LOG(INFO) << "RESULT:" << &result;
  CURL *curl = curl_easy_init();
  if (!curl) {
    return -1;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  //curl_easy_setopt(curl, CURLOPT_POST, 1);
  struct curl_slist *list = NULL;
  list = curl_slist_append(list, "Content-Type:application/json;");
  //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, url.c_str());
  curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.c_str());
  std::function<void(void *, size_t, size_t, void*)>func = std::bind(&CPost::writeData, this, std::placeholders::_1, 
      std::placeholders::_2, 
      std::placeholders::_3,
      std::placeholders:: _4);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, myWriteData);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,&result);
  int res = curl_easy_perform(curl);
  curl_slist_free_all(list);
  /* Check for errors */
  if(res != CURLE_OK){
    curl_easy_cleanup(curl);
    return res;
  }
  curl_easy_cleanup(curl);
  return 0;
}
}
