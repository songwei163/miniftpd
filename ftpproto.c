//
// Created by s on 19-5-14.
//

#include "ftpproto.h"

void ftp_reply (session_t* sess, status, const char *text);
static void do_user (session_t *sess);
static void do_pass (session_t *sess);

void handle_child (session_t *sess)
{
  ftp_reply (sess, 220, "(miniftpd 0.1)");
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
      if (strcmp ("USER", sess->cmd) == 0)
        {
          do_user (sess);
        }
      else if (strcmp ("PASS", sess->cmd) == 0)
        {
          do_pass (sess);
        }
    }
}

void ftp_reply (session_t* sess, status, const char *text)
{
  char buf[1024] = {0};
  sprintf (buf, "%d %s\r\n", status, text);
  writen (sess->ctrl_fd,buf,strlen (buf));
}

static void do_user (session_t *sess)
{
  // USER s
  struct passwd *pw = getpwnam (sess->arg);
  if (pw == NULL)
    {

    }


  ftp_reply (sess,331,"Please specify the passwd.");

}

static void do_pass (session_t *sess)
{
  write (sess->ctrl_fd, "230 Login successful.\r\n",
         strlen ("230 Login successful.\r\n"));
}