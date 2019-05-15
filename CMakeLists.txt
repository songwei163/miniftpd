cmake_minimum_required(VERSION 3.14)
project(miniftpd C)

set(CMAKE_C_STANDARD 99)

add_executable(miniftpd main.c sysutil.h sysutil.c common.c
        common.h session.c session.h ftpproto.c ftpproto.h
        privparent.c privparent.h str.c str.h tunable.c tunable.h
        parseconf.c parseconf.h ftpcodes.h miniftpd.conf)