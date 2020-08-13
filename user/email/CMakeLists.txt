include_directories(~/github/kunlib/user)
link_directories(~/github/kunlib/libs)
add_library(email SHARED aliMail.cpp aliConn.cpp)
target_link_libraries(email utcp pbase64)
