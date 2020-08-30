//
// Created by s on 19-5-14.
//

#ifndef  __SYSUTIL_H_
#define __SYSUTIL_H_

#include "common.h"

/*系统工具模块*/

int getlocalip(char *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);

void send_fd(int sock_fd, int fd);
int recv_fd(const int sock_fd);

int tcp_server(const char *host, unsigned short port);
int tcp_client(unsigned int port);

void get_file_mode(char str[10], mode_t mode);
const char *get_stat_databuf(struct stat *sbuf);

long get_time_sec();
long get_time_usec();

void nano_sleep(double seconds);

// 开启fd接受带外数据的功能
void activate_oobinline(int fd);

// 当fd上有带外数据的时候，将产生SIGURG信号
// 该函数设定当前进程接收fd的带外数据
void activate_sigurg(int fd);

#endif /* __SYSUTIL_H_ */
