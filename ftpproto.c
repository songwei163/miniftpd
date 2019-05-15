//
// Created by s on 19-5-14.
//

#include "ftpproto.h"

void handle_child (session_t *sess)
{
  writen (sess->ctrl_fd, "220 (miniftpd 0.1)\r\n", strlen ("220 (miniftpd 0.1)\r\n"));
  int ret;
  while (1)
    {
      memset (sess->cmdline, 0, sizeof (sess->cmdline));
      memset (sess->cmd, 0, sizeof (sess->cmd));
      memset (sess->arg, 0, sizeof (sess->arg));
      ret = readline (sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);

      if (ret == -1)
        {
          ERR_EXIT ("readline");

        }
      else if (ret == 0)
        {
          exit (EXIT_SUCCESS);
        }

      printf ("cmdline=[%s]\n", sess->cmdline);
      //去除\r\n

      //处理FTP命令
      str_trim_crlf (sess->cmdline);
      printf ("cmdline=[%s]\n", sess->cmdline);
      //解析FTP命令与参数
      str_split (sess->cmdline, sess->cmd, sess->arg, ' ');
      printf ("cmd=[%s] arg=[%s]\n", sess->cmd, sess->arg);
      //将命令转换为大写
      str_upper (sess->cmd);
      //处理FTP命令
    }
}