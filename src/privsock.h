//
// Created by s on 19-5-14.
//

#ifndef __PRIVSOCK_H__
#define __PRIVSOCK_H__

#include "session.h"

// 内部进程自定义协议
// 用于FTP服务进程和nobody进程进行通信

// FTP服务进程向nobody进程请求的命令
#define PRIV_SOCK_GET_DATA_SOCK    1
#define PRIV_SOCK_PASV_ACTIVE    2
#define PRIV_SOCK_PASV_LISTEN    3
#define PRIV_SOCK_PASV_ACCEPT    4

// nobody进程对FTP服务进程的应答
#define PRIV_SOCK_RESULT_OK    1
#define PRIV_SOCK_RESULT_BAD    2

// 初始化sockpair
void priv_sock_init(session_t *sess);
void priv_sock_close(session_t *sess);
void priv_sock_set_parent_context(session_t *sess);
void priv_sock_set_child_context(session_t *sess);

void priv_sock_send_cmd(int fd, char cmd);
char priv_sock_get_cmd(int fd);
void priv_sock_send_result(int fd, char res);
char priv_sock_get_result(int fd);

void priv_sock_send_int(int fd, int the_int);
int priv_sock_get_int(int fd);
void priv_sock_send_buf(int fd, const char *buf, unsigned int len);
void priv_sock_recv_buf(int fd, char *buf, unsigned int len);
void priv_sock_send_fd(int sock_fd, int fd);
int priv_sock_recv_fd(int sock_fd);

#endif /* __PRIVSOCK_H__ */
