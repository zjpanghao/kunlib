set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KUN}/libs)
set(SRC_LIST kafka_producer.cpp kafka_consumer.cpp)
add_definitions(-g -fPIC -std=c++11)
include_directories(/usr/local/include/librdkafka)
LINK_DIRECTORIES(/usr/local/lib /usr/local/ssl/lib)
add_library(pkafka SHARED ${SRC_LIST})
target_link_libraries(pkafka z  ssl crypto rdkafka glog)
