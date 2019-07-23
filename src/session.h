//
// Created by s on 19-5-14.
//

#ifndef __SESSION_H__
#define __SESSION_H__

#include "common.h"

typedef struct session {
    int uid;
    // 控制连接
    int ctrl_fd;
    char cmdline[MAX_COMMAND_LINE];
    char cmd[MAX_COMMAND];
    char cmd_arg[MAX_ARG];
    // 进程通信fd
    int parent_fd;
    int child_fd;
    // 传输模式
    int is_ascii;
    // PORT地址
    struct sockaddr_in *port_addr;
    // 数据传输fd
    int data_fd;
    int pasv_listen_fd;
    int data_process;

    // 断点续传偏移量
    long long restart_pos;

    // 重命名name
    char *rnfr_name;

    // 限速相关
    unsigned int bw_upload_rate_max;
    unsigned int bw_download_rate_max;
    long bw_transfer_start_sec;
    long bw_transfer_start_usec;

    int abor_received;

    //
    unsigned int num_clients;
    unsigned int num_this_ip;

} session_t;

void begin_session (session_t *sess);

#endif /* __SESSION_H__*/
