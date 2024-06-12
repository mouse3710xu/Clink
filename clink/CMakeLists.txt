cmake_minimum_required(VERSION 3.10.2)
project(clink)

set(CMAKE_CXX_STANDARD 17)
#set link flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -Wall")

#add local source code
add_executable(clink main.cpp
        sshUtils.cpp
        sshUtils.h
        utils.cpp
        utils.h
        Constants.h
        Constants.cpp
)

#3rd party library
set(COMMON_INCLUDE /usr/local/include)
set(OPENSSL_INCLUDE /usr/local/include/openssl)

set(LIBSSH_INCLUDE /usr/local/include/libssh)
set(LIBSSH_LIBRARY /usr/local/lib)

set(BOOST_INCLUDE /user/local/include/boost)
set(BOOST_LIBRARY /usr/local/lib)

#output directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set include directories and lib directories
include_directories(${COMMON_INCLUDE})
include_directories(${OPENSSL_INCLUDE})

include_directories(${LIBSSH_INCLUDE})
link_directories(${LIBSSH_LIBRARY})

include_directories(${BOOST_INCLUDE})

#link 3rd party library
#if you use static library, for example, you use libssh.a as your 3rd party library, you should add all the static library that libssh.a rely on
#libssh.a rely on the dl, pthread, libcrypto and ssl, you should add them after libssh.a in a certain order.
target_link_libraries(${PROJECT_NAME} libboost_regex.a libboost_program_options.a libssh.a libz.a libssl.a libcrypto.a pthread dl curses)

