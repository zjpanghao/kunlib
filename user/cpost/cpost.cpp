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
  std::string *result = (std::string *)userp;
  result->append((char*)buffer, 0, 
      size *nmemb); 
  return size * nmemb;
}

int CPost::post(const std::string &url, const std::string &body, std::string &result) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    return -1;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  struct curl_slist *list = NULL;
  list = curl_slist_append(list, "Content-Type:application/json;");
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
  curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, myWriteData);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,&result);
  int res = curl_easy_perform(curl);
  /* Check for errors */
  if(res != CURLE_OK){
    goto DONE;
  }
DONE:
  curl_slist_free_all(list);
  curl_easy_cleanup(curl);
  return res;
}
}
