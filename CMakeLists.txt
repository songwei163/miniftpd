cmake_minimum_required(VERSION 3.14)
project(miniftpd C)

set(CMAKE_C_STANDARD 99)

add_executable(miniftpd
        main.c
        common.h
        ftpcodes.h
        miniftpd.conf
        sysutil.h       sysutil.c
        session.c       session.h
        ftpproto.c      ftpproto.h
        privparent.c    privparent.h
        str.c           str.h
        tunable.c       tunable.h
        parseconf.c     parseconf.h
        )
target_link_libraries(miniftpd crypt)