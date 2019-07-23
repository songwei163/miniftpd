//
// Created by s on 19-5-14.
//

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/capability.h>
#include <sys/time.h>
#include <libgen.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
//shdows password
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>

//socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//readdir
#include<sys/stat.h>
#include<dirent.h>

#define ERR_EXIT(err_str) do { perror(err_str);  \
        exit(EXIT_FAILURE); } while(0)

#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND 32
#define MAX_ARG 1024
#define MAX_BUFFER_SIZE 1024

#define MINIFTP_CONF "../config/miniftpd.conf"

#define LISTENQ     1024

#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND     32
#define MAX_ARG         1024

#define MAX_LINE         1024
#define HALF_YEAR_SEC    182*24*3600

#

#endif //_COMMON_H_