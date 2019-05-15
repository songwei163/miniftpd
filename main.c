#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "str.h"
#include "parseconf.h"
#include "tunable.h"

int main (int argc, char *argv[])
{



  /*字符串模块测试
    //char *str1="               a b";
    char* str2 = "               ";
    if (str_all_space(str2)) {
        printf("str2 all space\n");
    }
    else {
        printf("str2 not all space");
    }
    char str3[] = "avxcAsada";
    str_upper(str3);
    printf("%s\n", str3);

    long long result = str_to_longlong("12345678901234");
    printf("%lld\n", result);

    printf("result=%d\n", str_octal_to_uint ("665"));

  */




  parseconf_load_file (MINIFTP_CONF);
  printf ("tunable_pasv_enable = %u\n", tunable_pasv_enable);
  printf ("tunable_port_enable = %u\n", tunable_port_enable);

  printf ("tunable_listen_port = %u\n", tunable_listen_port);
  printf ("tunable_max_clients = %u\n", tunable_max_clients);
  printf ("tunable_max_per_ip = %u\n", tunable_max_per_ip);
  printf ("tunable_accept_timeout = %u\n", tunable_accept_timeout);
  printf ("tunable_connect_timeout = %u\n", tunable_connect_timeout);
  printf ("tunable_idle_session_timeout = %u\n", tunable_idle_session_timeout);
  printf ("tunable_data_connection_timeout = %u\n", tunable_data_connection_timeout);
  printf ("tunable_local_umask = 0%o\n", tunable_local_umask);
  printf ("tunable_upload_max_rate = %u\n", tunable_upload_max_rate);
  printf ("tunable_download_max_rate = %u\n", tunable_download_max_rate);
  if (tunable_listen_address == NULL)
    {
      printf ("tunable_listen_address = NULL");
    }
  else
    {
      printf ("tunable_listen_address = %s\n", tunable_listen_address);
    }

#if 1
  /*判断程序是否以root用户执行*/
  if (getuid () != 0)
    {
      fprintf (stderr, "%s: must be started as root\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  int listenfd = tcp_server (tunable_listen_address, tunable_listen_port);

  session_t sess = {
      /*控制连接套接字*/
      0,-1,
      /*控制连接*/
      "", "", "",
      /*父子进程通道*/
      -1, -1
  };

  int connfd = 0;
  pid_t pid;
  while (1)
    {
      connfd = accept_timeout (listenfd, NULL, 0);
      if (connfd == -1)
        {
          ERR_EXIT ("accept_timeout");
        }

      pid = fork ();
      if (pid == -1)
        {
          ERR_EXIT ("fork");
        }
      if (pid == 0)
        {
          /*派生子进程服务*/
          close (listenfd);
          sess.ctrl_fd = connfd;
          begin_session (&sess);
        }
      else
        {
          close (connfd);
        }

    }
#endif
  return 0;
}