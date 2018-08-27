#include <pthread.h>
#include <map>
#include <memory>
#include <glog/logging.h>
#include "kafka_consumer.h"

static void InitGlog(const char *name) {
  google::InitGoogleLogging(name);
  google::SetLogDestination(google::INFO,"log/teleloginfo");
  google::SetLogDestination(google::WARNING,"log/telelogwarn");
  google::SetLogDestination(google::GLOG_ERROR,"log/telelogerror");
  FLAGS_logbufsecs = 10;
}

class MarketKafkaConsumer : public KafkaConsumer {
  public:
   void ProcessMessage(const char *buf, int len) override{
     LOG(INFO) << pthread_self() << "  " <<  buf << "---" << len << std::endl;
   }
};

int main(int argc, char*argv[]) {
  InitGlog(argv[0]);
  std::vector<std::unique_ptr<MarketKafkaConsumer> > vec;
  const char *servers = {"192.168.1.85:9092,192.168.1.82:9092"};
  std::unique_ptr<MarketKafkaConsumer> consumer(new MarketKafkaConsumer());
  int rc = consumer->Init(servers, "east_market", "group_test");
  if (rc < 0)
    return rc;
  consumer->set_partition(10);
  if(0 != (rc = consumer->StartAll()))
    return rc;
  while (1) {
    sleep(10000);
  }
  return 0;
}
