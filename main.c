#include "common.h"
#include "sysutil.h"
#include "session.h"

int main (int argc, char *argv[])
{
  /*判断程序是否以root用户执行*/
  if (getuid () != 0)
    {
      fprintf (stderr, "%s: must be started as root\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  int listenfd = tcp_server (NULL, 5188);

  session_t sess = {
      /*控制连接套接字*/
      -1,
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

  return 0;
}