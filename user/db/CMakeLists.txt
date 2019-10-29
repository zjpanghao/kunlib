cmake_minimum_required (VERSION 2.6)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KUN}/libs)
message(STATUS ${externDir})

# add_definitions(-DMACOSX)
add_definitions(-DLINUX2)
add_definitions(-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_USE_FAST_MACRO -fPIC)
include_directories(
					./include
					${KUN}/include/ 
					${KUN}/include/glog
					${KUN}/user
          /usr/local/include/zdb
					)

link_directories(${KUN}/libs
					)

add_library(dbpool SHARED dbpool.cpp)
target_link_libraries(dbpool  zdb glog  )
SET_TARGET_PROPERTIES(dbpool PROPERTIES VERSION 3 )

