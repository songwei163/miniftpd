cmake_minimum_required(VERSION 3.11)
project(miniftpd C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(miniftpd
        src/common.h
        src/ftpcodes.h
        src/ftpproto.c
        src/ftpproto.h
        src/hash.c
        src/hash.h
        src/main.c
        src/parseconf.c
        src/parseconf.h
        src/privparent.c
        src/privparent.h
        src/privsock.c
        src/privsock.h
        src/session.c
        src/session.h
        src/str.c
        src/str.h
        src/sysutil.c
        src/sysutil.h
        src/tunable.c
        src/tunable.h)
target_link_libraries(miniftpd crypt)