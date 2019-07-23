cmake_minimum_required(VERSION 3.14)
project(miniftpd C)

set(CMAKE_C_STANDARD 11)

add_executable(miniftpd
        src/main.c
        src/common.h
        src/ftpcodes.h
        config/miniftpd.conf
        src/sysutil.h src/sysutil.c
        src/session.c src/session.h
        src/ftpproto.c src/ftpproto.h
        src/privparent.c src/privparent.h
        src/str.c src/str.h
        src/tunable.c src/tunable.h
        src/parseconf.c src/parseconf.h
        src/privsock.c src/privsock.h
        src/hash.c src/hash.h)

target_link_libraries(miniftpd crypt)