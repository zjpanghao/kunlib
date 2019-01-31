cmake_minimum_required (VERSION 2.6)

set (KUN /home/panghao/kunlib)
message(STATUS ${externDir})

# add_definitions(-DMACOSX)
add_definitions(-DLINUX2)
add_definitions(-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_USE_FAST_MACRO -fPIC)
include_directories(
					./include
					${KUN}/include/ 
					${KUN}/include/glog
					${KUN}/include/zdb
					)

link_directories(${KUN}/libs
					)

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_library(dbpool SHARED dbpool.cpp)
target_link_libraries(dbpool  zdb glog  )
SET_TARGET_PROPERTIES(dbpool PROPERTIES VERSION 3 )

