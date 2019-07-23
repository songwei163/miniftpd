//
// Created by s on 19-5-14.
//

#include "privparent.h"
#include "common.h"
#include "privsock.h"
#include "tunable.h"
#include "sysutil.h"

void privop_pasv_get_data_sock (session_t *sess);
void privop_pasv_active (session_t *sess);
void privop_pasv_listen (session_t *sess);
void privop_pasv_accept (session_t *sess);

int capset (cap_user_header_t hdrp, const cap_user_data_t datap)
{
  return syscall (__NR_capset, hdrp, datap);
}

void minimize_privilege ()
{
  struct __user_cap_header_struct cap_header;
  struct __user_cap_data_struct cap_data;

  memset (&cap_header, 0, sizeof (cap_header));
  memset (&cap_data, 0, sizeof (cap_data));

  cap_header.version = _LINUX_CAPABILITY_VERSION_2;
  cap_header.pid = 0;

  __u32 cap_mask = 0;
  cap_mask |= (1 << CAP_NET_BIND_SERVICE);
  cap_data.effective = cap_data.permitted = cap_mask;
  cap_data.inheritable = 0;

  capset (&cap_header, &cap_data);
}

void handle_parent (session_t *sess)
{
  struct passwd *pw = getpwnam ("nobody");
  if (pw == NULL)
    return;

  if (setegid (pw->pw_gid) < 0)
    {
      ERR_EXIT("setegid");
    }

  if (seteuid (pw->pw_uid) < 0)
    {
      ERR_EXIT("seteuid");
    }

  // add cur process bind 20 port privilege
  minimize_privilege ();

  char cmd;
  while (1)
    {
      // 读取来自子进程的数据
      cmd = priv_sock_get_cmd (sess->parent_fd);
      //printf("cmd: %d\n", (int)cmd);
      // 解析内部命令
      switch (cmd)
        {
          case PRIV_SOCK_GET_DATA_SOCK:
            privop_pasv_get_data_sock (sess);
          break;
          case PRIV_SOCK_PASV_ACTIVE:
            privop_pasv_active (sess);
          break;
          case PRIV_SOCK_PASV_LISTEN:
            privop_pasv_listen (sess);
          break;
          case PRIV_SOCK_PASV_ACCEPT:
            privop_pasv_accept (sess);
          break;
        }

    }
}

void privop_pasv_get_data_sock (session_t *sess)
{
  // recv short,then ip
  unsigned short port = (unsigned short) priv_sock_get_int (sess->parent_fd);

  char ip[16] = {0};
  priv_sock_recv_buf (sess->parent_fd, ip, sizeof (ip));

  struct sockaddr_in addr;
  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr.s_addr = inet_addr (ip);

  //printf("address ip: %s\n", ip);

  int data_fd = tcp_client (20);
  if (data_fd == -1)
    {
      printf ("tcp_client error data fd: %d\n", data_fd);
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
      return;
    }

  if (connect_timeout (data_fd, &addr, tunable_connect_timeout) < 0)
    {
      printf ("connect timeout\n");
      close (data_fd);
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
      return;
    }
  priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_OK);
  priv_sock_send_fd (sess->parent_fd, data_fd);
  close (data_fd);
}

void privop_pasv_active (session_t *sess)
{
  int active;
  if (sess->pasv_listen_fd != -1)
    {
      active = 1;
    }
  else
    {
      active = 0;
    }
  priv_sock_send_int (sess->parent_fd, active);
}

void privop_pasv_listen (session_t *sess)
{
  char local_ip[16] = {0};
  getlocalip (local_ip);

  sess->pasv_listen_fd = tcp_server (local_ip, 0);
  struct sockaddr_in sa_in;
  socklen_t sa_in_len = sizeof (sa_in);
  if (getsockname (sess->pasv_listen_fd, (struct sockaddr *) &sa_in, &sa_in_len) < 0)
    {
      ERR_EXIT("getsockname");
    }

  unsigned short port = ntohs (sa_in.sin_port);

  priv_sock_send_int (sess->parent_fd, (int) port);
}

void privop_pasv_accept (session_t *sess)
{
  int fd = accept_timeout (sess->pasv_listen_fd, NULL, tunable_accept_timeout);
  close (sess->pasv_listen_fd);
  sess->pasv_listen_fd = -1;
  if (fd == -1)
    {
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
    }
  else
    {
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_OK);
      priv_sock_send_fd (sess->parent_fd, fd);
      close (fd);
    }
}