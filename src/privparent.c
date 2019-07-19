//
// Created by s on 19-5-14.
//

#include "privparent.h"
#include "privsock.h"
#include "sysutil.h"
#include "tunable.h"

static void privop_pasv_get_data_sock (session_t *sess);
static void privop_pasv_active (session_t *sess);
static void privop_pasv_listen (session_t *sess);
static void privop_pasv_accept (session_t *sess);

int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
  return syscall(__NR_capset, hdrp, datap);
}


void minimize_privilege(void)
{
  struct passwd *pw = getpwnam("nobody");
  if (pw == NULL)
    return;

  if (setegid(pw->pw_gid) < 0)
    ERR_EXIT("setegid");
  if (seteuid(pw->pw_uid) < 0)
    ERR_EXIT("seteuid");
  /*
   typedef struct __user_cap_header_struct {
       __u32 version;
       int pid;
   } *cap_user_header_t;

   typedef struct __user_cap_data_struct {
       __u32 effective;
       __u32 permitted;
       __u32 inheritable;
   } *cap_user_data_t;
   */
  struct __user_cap_header_struct head;
  struct __user_cap_data_struct data;

  memset(&head, 0, sizeof(head));
  memset(&data, 0, sizeof(data));
  head.version = _LINUX_CAPABILITY_VERSION_1;
  //head.version = _LINUX_CAPABILITY_VERSION_2;
  head.pid = 0;

  __u32 mask = 0;
  mask |= (1 << CAP_NET_BIND_SERVICE);
  data.effective = data.permitted = mask;
  data.inheritable = 0;

  capset(&head, &data);
}

void handle_parent (session_t *sess)
{
  minimize_privilege ();

  char cmd;
  while (1)
    {
      //read(sess->parent_fd, &cmd, 1);
      cmd = priv_sock_get_cmd (sess->parent_fd);
      // 解析内部命令
      // 处理内部命令
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

static void privop_pasv_get_data_sock (session_t *sess)
{
  /*
  nobody进程接收PRIV_SOCK_GET_DATA_SOCK命令
进一步接收一个整数，也就是port
接收一个字符串，也就是ip

fd = socket
bind(20)
connect(ip, port);

OK
send_fd
BAD
*/
  unsigned short port = (unsigned short) priv_sock_get_int (sess->parent_fd);
  char ip[16] = {0};
  priv_sock_recv_buf (sess->parent_fd, ip, sizeof (ip));

  struct sockaddr_in addr;
  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr.s_addr = inet_addr (ip);

  int fd = tcp_client (20);
  if (fd == -1)
    {
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
      return;
    }
  if (connect_timeout (fd, &addr, tunable_connect_timeout) < 0)
    {
      close (fd);
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
      return;
    }

  priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_OK);
  priv_sock_send_fd (sess->parent_fd, fd);
  close (fd);
}

static void privop_pasv_active (session_t *sess)
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

static void privop_pasv_listen (session_t *sess)
{
  //char ip[16] = {0};
  //getlocalip (ip);
  char ip[] = "192.168.43.67";


  sess->pasv_listen_fd = tcp_server (ip, 0);
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof (addr);
  if (getsockname (sess->pasv_listen_fd, (struct sockaddr *) &addr, &addrlen) < 0)
    {
      ERR_EXIT("getsockname");
    }

  unsigned short port = ntohs (addr.sin_port);

  priv_sock_send_int (sess->parent_fd, (int) port);
}

static void privop_pasv_accept (session_t *sess)
{
  int fd = accept_timeout (sess->pasv_listen_fd, NULL, tunable_accept_timeout);
  close (sess->pasv_listen_fd);
  sess->pasv_listen_fd = -1;

  if (fd == -1)
    {
      priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_BAD);
      return;
    }

  priv_sock_send_result (sess->parent_fd, PRIV_SOCK_RESULT_OK);
  priv_sock_send_fd (sess->parent_fd, fd);
  close (fd);
}
