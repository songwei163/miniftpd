//
// Created by s on 19-5-14.
//

#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "privsock.h"

void ftp_lreply (session_t *sess, int status, const char *text);
void ftp_reply (session_t *sess, int status, const char *text);
void handle_alarm_timeout (int sig);
void handle_sigalrm (int sig);
void handle_sigurg (int sig);
void start_cmdio_alarm (void);
void start_data_alarm (void);

void check_abor (session_t *sess);

int list_common (session_t *sess, int detail);
void limit_rate (session_t *sess, int bytes_transfered, int is_upload);
static void upload_common (session_t *sess, int is_append);

bool get_port_fd (session_t *sess);
bool get_pasv_fd (session_t *sess);
bool get_transfer_fd (session_t *sess);
bool port_active (session_t *sess);
bool pasv_active (session_t *sess);

static void do_user (session_t *sess);
static void do_pass (session_t *sess);
static void do_cwd (session_t *sess);
static void do_cdup (session_t *sess);
static void do_quit (session_t *sess);
static void do_port (session_t *sess);
static void do_pasv (session_t *sess);
static void do_type (session_t *sess);
//static void do_stru(session_t *sess);
//static void do_mode(session_t *sess);
static void do_retr (session_t *sess);
static void do_stor (session_t *sess);
static void do_appe (session_t *sess);
static void do_list (session_t *sess);
static void do_nlst (session_t *sess);
static void do_rest (session_t *sess);
static void do_abor (session_t *sess);
static void do_pwd (session_t *sess);
static void do_mkd (session_t *sess);
static void do_rmd (session_t *sess);
static void do_dele (session_t *sess);
static void do_rnfr (session_t *sess);
static void do_rnto (session_t *sess);
static void do_site (session_t *sess);
static void do_syst (session_t *sess);
static void do_feat (session_t *sess);
static void do_size (session_t *sess);
static void do_stat (session_t *sess);
static void do_noop (session_t *sess);
static void do_help (session_t *sess);

static void do_site_chmod (session_t *sess, char *chmod_arg);
static void do_site_umask (session_t *sess, char *umask_arg);

typedef struct ftpcmd {
    const char *cmd;
    void (*cmdhandler) (session_t *sess);
} ftpcmd_t;

static ftpcmd_t ctrl_cmds[] = {
    /* 访问控制命令 */
    {"USER", do_user},
    {"PASS", do_pass},
    {"CWD", do_cwd},
    {"XCWD", do_cwd},
    {"CDUP", do_cdup},
    {"XCUP", do_cdup},
    {"QUIT", do_quit},
    {"ACCT", NULL},
    {"SMNT", NULL},
    {"REIN", NULL},
    /* 传输参数命令 */
    {"PORT", do_port},
    {"PASV", do_pasv},
    {"TYPE", do_type},
    {"STRU",    /*do_stru*/NULL},
    {"MODE",    /*do_mode*/NULL},

    /* 服务命令 */
    {"RETR", do_retr},
    {"STOR", do_stor},
    {"APPE", do_appe},
    {"LIST", do_list},
    {"NLST", do_nlst},
    {"REST", do_rest},
    {"ABOR", do_abor},
    {"\377\364\377\362ABOR", do_abor},
    {"PWD", do_pwd},
    {"XPWD", do_pwd},
    {"MKD", do_mkd},
    {"XMKD", do_mkd},
    {"RMD", do_rmd},
    {"XRMD", do_rmd},
    {"DELE", do_dele},
    {"RNFR", do_rnfr},
    {"RNTO", do_rnto},
    {"SITE", do_site},
    {"SYST", do_syst},
    {"FEAT", do_feat},
    {"SIZE", do_size},
    {"STAT", do_stat},
    {"NOOP", do_noop},
    {"HELP", do_help},
    {"STOU", NULL},
    {"ALLO", NULL}
};

session_t *p_sess;
#if 0
void handle_alarm_timeout (int sig)
{
  shutdown (p_sess->ctrl_fd, SHUT_RD);
  ftp_reply (p_sess, FTP_IDLE_TIMEOUT, "Timeout");
  shutdown (p_sess->ctrl_fd, SHUT_WR);
  exit (EXIT_FAILURE);
}
void handle_sigalrm (int sig)
{
  if (!p_sess->data_process)
    {
      ftp_reply (p_sess, FTP_DATA_TIMEOUT, "Data timeout, Reconnect sorry");
      exit (EXIT_FAILURE);
    }

  //否则　当前处于数据传输的状态收到了超时信号
  p_sess->data_process = 0;
  start_data_alarm ();
}
void handle_sigurg (int sig)
{
  if (p_sess->data_fd == -1)
    {
      return;
    }

  char cmdline[MAX_COMMAND_LINE] = {0};
  int ret = readline (p_sess->ctrl_fd, cmdline, MAX_COMMAND_LINE);
  if (ret <= 0)
    {
      ERR_EXIT("readline");
    }
  printf ("cmdline = %s", cmdline);
  str_trim_crlf (cmdline);

  if (strcmp (cmdline, "ABOR") == 0
      || strcmp (cmdline, "\377\364\377\362ABOR") == 0)
    {
      p_sess->abor_received = 1;
      shutdown (p_sess->data_fd, SHUT_RDWR);
    }
  else
    {
      ftp_reply (p_sess, FTP_BADCMD, "Unknown command.");
    }
}
void start_cmdio_alarm ()
{
  if (tunable_idle_session_timeout > 0)
    {
      //安装信号
      signal (SIGALRM, handle_alarm_timeout);
      //启动闹钟
      alarm (tunable_idle_session_timeout);
    }
}
void start_data_alarm ()
{
  if (tunable_data_connection_timeout > 0)
    {
      //安装信号
      signal (SIGALRM, handle_sigalrm);
      //启动闹钟
      alarm (tunable_data_connection_timeout);
    }
  else if (tunable_idle_session_timeout > 0)
    {
      alarm (0);
    }
}

void check_abor (session_t *sess)
{
  if (sess->abor_received)
    {
      sess->abor_received = 0;
      ftp_reply (sess, FTP_ABOROK, "ABOR successful.");
    }
}
#endif
bool get_port_fd (session_t *sess)
{
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_GET_DATA_SOCK);
  unsigned short port = ntohs (sess->port_addr->sin_port);
  char *ip = inet_ntoa (sess->port_addr->sin_addr);

  priv_sock_send_int (sess->child_fd, (int) port);
  priv_sock_send_buf (sess->child_fd, ip, strlen (ip));
  char res = priv_sock_get_result (sess->child_fd);
  if (res == PRIV_SOCK_RESULT_BAD)
    {
      return false;
    }
  else if (res == PRIV_SOCK_RESULT_OK)
    {
      sess->data_fd = priv_sock_recv_fd (sess->child_fd);
    }

  return true;
}

bool get_pasv_fd (session_t *sess)
{

  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_ACCEPT);
  char res = priv_sock_get_cmd (sess->child_fd);
  if (res == PRIV_SOCK_RESULT_BAD)
    {
      return false;
    }
  else if (res == PRIV_SOCK_RESULT_OK)
    {
      sess->data_fd = priv_sock_recv_fd (sess->child_fd);
    }

  return true;
  /*
  int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
  //得到一个已连接套接字
  close(sess->pasv_listen_fd);
  sess->pasv_listen_fd = -1;
  sess->data_fd = fd;
  if (fd == -1)
  {
      return 0;
  }
  return 1;
   */
}
bool get_transfer_fd (session_t *sess)
{
  // 检测是否收到post和pasv命令
  if (!port_active (sess) && !pasv_active (sess))
    {
      ftp_reply (sess, FTP_BADSENDCONN, "Use PORT or PASV first");
      return false;
    }

  int ret = 1;
  // 主动模式
  if (port_active (sess))
    {
      /*
      //tcp_clint(20)
      int fd = tcp_client(0);
      if (connect_timeout(fd, sess->port_addr, tunable_connect_timeout) < 0 )
      {
          close(fd);
          return 0;
      }
      sess->data_fd = fd;
      */
      if (get_port_fd (sess) == 0)
        {
          ret = false;
        }
    }

  if (pasv_active (sess))
    {
      /*
      int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
      close(sess->pasv_listen_fd);
      if (fd == -1)
      {
          ret = 0;
      }

      sess->pasv_listen_fd = -1;
      sess->data_fd = fd;
       */
      if (get_pasv_fd (sess) == 0)
        {
          ret = false;
        }
    }

  if (sess->port_addr)
    {
      free (sess->port_addr);
      sess->port_addr = NULL;
    }

  //重新安装信号　并启动
  if (ret)
    {
      //start_data_alarm ();
    }
  return ret;
}
bool port_active (session_t *sess)
{
  if (sess->port_addr != NULL)
    {
      if (pasv_active (sess))
        {
          fprintf (stderr, "both port and pasv are active");
        }
      return true;
    }
  else
    {
      return false;
    }
}
bool pasv_active (session_t *sess)
{

  /*

       if (sess->pasv_listen_fd != -1)
       {
           if (port_active(sess))
           {
               fprintf(stderr, "both port and pasv are active");
           }
           return 1;
       }
       return 0;
  */
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_ACTIVE);
  int active = priv_sock_get_int (sess->child_fd);
  if (active == 1)
    {
      if (port_active (sess))
        {
          fprintf (stderr, "both port and pasv are active");
          exit (EXIT_FAILURE);
        }
      return true;
    }
  return false;

}

/**
 * 返回０失败，返回１成功。
 */
int list_common (session_t *sess, int detail)
{
  DIR *dir = opendir (".");
  if (dir == NULL)
    {
      return 0;
    }
  else
    {
      struct dirent *dt = NULL;
      struct stat stbuf = {0};
      while ((dt = readdir (dir)) != NULL)
        {

          if (lstat (dt->d_name, &stbuf) == -1)
            {
              continue;
            }
          if (dt->d_name[0] == '.')
            {
              continue;
            }

          char perms[] = "----------";
          perms[1] = '?';

          mode_t mode = stbuf.st_mode;
          switch (mode & S_IFMT)
            {
              case S_IFREG:perms[0] = '-';
              break;
              case S_IFDIR:perms[0] = 'd';
              break;
              case S_IFLNK:perms[0] = 'l';
              break;
              case S_IFIFO:perms[0] = 'p';
              break;
              case S_IFSOCK:perms[0] = 's';
              break;
              case S_IFCHR:perms[0] = 'c';
              break;
              case S_IFBLK:perms[0] = 'b';
              break;
            }

          if (mode & S_IRUSR)
            {
              perms[1] = 'r';
            }
          if (mode & S_IWUSR)
            {
              perms[2] = 'w';
            }
          if (mode & S_IXGRP)
            {
              perms[3] = 'x';
            }
          if (mode & S_IRGRP)
            {
              perms[4] = 'r';
            }
          if (mode & S_IWGRP)
            {
              perms[5] = 'w';
            }
          if (mode & S_IWGRP)
            {
              perms[6] = 'x';
            }
          if (mode & S_IROTH)
            {
              perms[7] = 'r';
            }
          if (mode & S_IWOTH)
            {
              perms[8] = 'w';
            }
          if (mode & S_IROTH)
            {
              perms[9] = 'x';
            }
          if (mode & S_ISUID)
            {
              perms[3] = (perms[3] == 'x' ? 's' : 'S');
            }
          if (mode & S_ISGID)
            {
              perms[6] = (perms[6] == 'x' ? 's' : 'S');
            }
          if (mode & S_ISVTX)
            {
              perms[3] = (perms[3] == 'x' ? 't' : 'T');
            }

          char buf[1024] = {0};
          int off = 0;
          off += sprintf (buf, "%s", perms);
          off += sprintf (buf + off, "%3ld %-8d %-8d", stbuf.st_nlink, stbuf.st_uid, stbuf.st_gid);
          off += sprintf (buf + off, "%8lu ", (unsigned long) stbuf.st_size);

          const char *p_date_format = "%b %e %H:%M";
          struct timeval tv;
          gettimeofday (&tv, NULL);
          long local_time = tv.tv_sec;
          if (stbuf.st_mtime > local_time || (local_time - stbuf.st_mtime) > 182 * 24 * 60 * 60)
            {
              p_date_format = "%b %e  %Y";
            }
          else
            {
              char datebuf[64] = {0};
              struct tm *p_tm = localtime (&local_time);
              strftime (datebuf, sizeof (datebuf), p_date_format, p_tm);
              off += sprintf (buf + off, "%s ", datebuf);
              //off += sprintf (buf + off, "%s ", dt->d_name);

              if (S_ISLNK (stbuf.st_mode))
                {
                  char tmp[1024] = {0};
                  readlink (dt->d_name, tmp, sizeof (tmp));
                  off += sprintf (buf + off, "%s -> %s\r\n", dt->d_name, tmp);
                }
              else
                {
                  off += sprintf (buf + off, "%s\r\n", dt->d_name);
                }
            }
          writen (sess->data_fd, buf, sizeof (buf));
        }
      closedir (dir);
    }
  return 1;
}

void limit_rate (session_t *sess, int bytes_transfered, int is_upload)
{

}

static void upload_common (session_t *sess, int is_append)
{

}

void handle_child (session_t *sess)
{
  ftp_reply(sess, FTP_GREET, "(miniftpd 1.0)");
  int ret;
  while(1)
    {
      memset(sess->cmdline, 0, sizeof(sess->cmdline));
      memset(sess->cmd, 0, sizeof(sess->cmd));
      memset(sess->arg, 0, sizeof(sess->arg));

      //ssize_t readline(int sockfd, void *buf, size_t maxline);
      ret = readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
      if(ret == 0)
        {
          exit(EXIT_SUCCESS);
        }
      else if(ret < 0)
        {
          ERR_EXIT("readline");
        }
      str_trim_crlf(sess->cmdline);
      //printf("cmdline = [%s]\n", sess->cmdline);
      str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
      //printf("command = [%s]\n", sess->cmd);
      //printf("arg = [%s]\n", sess->arg);

      size_t n = sizeof(ctrl_cmds) / sizeof(ftpcmd_t);
      size_t i;
      for(i=0; i<n; ++i)
        {
          if(strcmp(ctrl_cmds[i].cmd, sess->cmd) == 0)
            {
              if(ctrl_cmds[i].cmdhandler != NULL)
                {
                  (*ctrl_cmds[i].cmdhandler)(sess);
                }
              else
                {
                  char buf[1024] = {0};
                  sprintf(buf, "%s not implemented.", sess->cmd);
                  ftp_reply(sess, FTP_COMMANDNOTIMPL, buf);
                }
              break;
            }
        }
      if(i >= n)
        ftp_reply(sess, FTP_BADCMD, "Unknown command.");
    }
}

void ftp_reply (session_t *sess, int status, const char *text)
{
  char buf[1024] = {0};
  sprintf (buf, "%d %s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

void ftp_lreply (session_t *sess, int status, const char *text)
{
  char buf[1024] = {0};
  sprintf (buf, "%d-%s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

static void do_user (session_t *sess)
{
  struct passwd *pw = getpwnam (sess->arg);
  if (pw == NULL)
    {
      ftp_reply (sess, FTP_LOGINERR, "Login incorrect");
      return;
    }
  sess->uid = pw->pw_uid;
  ftp_reply (sess, FTP_GIVEPWORD, "Please specify the password.");
}

static void do_pass (session_t *sess)
{
  struct passwd *pw = getpwuid(sess->uid);
  if(pw == NULL)
    {
      //530 Login incorrect.
      ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }

  struct spwd *spw = getspnam(pw->pw_name);

  char *crypt_passwd = crypt(sess->arg, spw->sp_pwdp);
  if(strcmp(spw->sp_pwdp, crypt_passwd) != 0)
    {
      ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }

  setegid(pw->pw_gid);
  seteuid(pw->pw_uid);
  chdir(pw->pw_dir);

  //230 Login successful.
  ftp_reply(sess, FTP_LOGINOK, "Login successful.");
}

static void do_syst (session_t *sess)
{
  ftp_reply (sess, FTP_SYSTOK, "UNIX Type: L8");
}

static void do_feat (session_t *sess)
{
  ftp_lreply (sess, FTP_FEAT, "Features:");
  writen (sess->ctrl_fd, " EPRT\r\n", strlen (" EPRT\r\n"));
  writen (sess->ctrl_fd, " EPSV\r\n", strlen (" EPSV\r\n"));
  writen (sess->ctrl_fd, " MDTM\r\n", strlen (" MDTM\r\n"));
  writen (sess->ctrl_fd, " PASV\r\n", strlen (" PASV\r\n"));
  writen (sess->ctrl_fd, " REST STREAM\r\n", strlen (" REST STREAM\r\n"));
  writen (sess->ctrl_fd, " SIZE\r\n", strlen (" SIZE\r\n"));
  writen (sess->ctrl_fd, " TVFS\r\n", strlen (" TVFS\r\n"));
  writen (sess->ctrl_fd, " UTF8\r\n", strlen (" UTF8\r\n"));
  ftp_reply (sess, FTP_FEAT, "End");
}

static void do_cwd (session_t *sess)
{
  if (chdir (sess->arg) >= 0)
    {
      ftp_reply (sess, FTP_CWDOK, "Directory successfully changed.");
      return;
    }
  ftp_reply (sess, FTP_FILEFAIL, "Fail to change directory");
}

static void do_cdup (session_t *sess)
{
  if (chdir ("..") >= 0)
    {
      ftp_reply (sess, FTP_CWDOK, "Directory successfully changed.");
      return;
    }
  ftp_reply (sess, FTP_FILEFAIL, "Fail to change directory");
}

static void do_quit (session_t *sess)
{
  ftp_reply (sess, FTP_GOODBYE, "Good bye");
  exit (EXIT_SUCCESS);
}

static void do_port (session_t *sess)
{
  //PORT 127,0,0,1,227,31
  unsigned int v[6];
  sscanf (sess->arg, "%u,%u,%u,%u,%u,%u", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
  sess->port_addr = (struct sockaddr_in *) malloc (sizeof (struct sockaddr_in));
  sess->port_addr->sin_family = AF_INET;
  unsigned char *p = (unsigned char *) &sess->port_addr->sin_port;
  p[0] = v[4];

  p[1] = v[5];
  p = (unsigned char *) &sess->port_addr->sin_addr;
  p[0] = v[0];
  p[1] = v[1];
  p[2] = v[2];
  p[3] = v[3];
  ftp_reply (sess, FTP_PORTOK, "PORT command successful. Consider using PASV.");
}

static void do_pasv (session_t *sess)
{
  //char ip[16] = {0};
  //getlocalip (ip);
  char ip[] = "192.168.43.67";

  /*
  sess->pasv_listen_fd = tcp_server(ip, 0);
  struct sockaddr_in addr;
  socklen_t socklen = sizeof(addr);
  if (getsockname(sess->pasv_listen_fd, (struct sockaddr *)&addr, &socklen) < 0)
  {
      ERR_EXIT("getsockname");
  }

  unsigned short port = ntohs(addr.sin_port);
  */
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_LISTEN);
  unsigned short port = (int) priv_sock_get_int (sess->child_fd);
  //unsigned int* p=(unsigned int*)ip;
  //printf ("%u.%u.%u.%u", *(p[0]), p[1], p[2], p[3]);
  unsigned int v[4];
  sscanf (ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
  char text[1024] = {0};
  sprintf (text, "Entering Passive Mode (%u,%u,%u,%u,%u,%u).",
           v[0], v[1], v[2], v[3], port >> 8, port & 0xFF);

  ftp_reply (sess, FTP_PASVOK, text);

}

static void do_type (session_t *sess)
{
  if (strcmp (sess->arg, "A") == 0)
    {
      ftp_reply (sess, FTP_TYPEOK, "Switching to ASCII mode.");
    }
  else if (strcmp (sess->arg, "I") == 0)
    {
      ftp_reply (sess, FTP_TYPEOK, "Switching to Binary mode.");
    }
  else
    {
      ftp_reply (sess, FTP_BADCMD, "Unrecognised Type cmd.");
    }
}

static void do_retr (session_t *sess)
{

}

static void do_stor (session_t *sess)
{
  upload_common (sess, 0);
}

static void do_appe (session_t *sess)
{
  upload_common (sess, 1);
}

static void do_list (session_t *sess)
{
  //创建数据链接
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }
  //150
  ftp_reply (sess, FTP_DATACONN, "Here comes the directory listing.");
  //传输列表
  list_common (sess, 1);
  //关闭数据套接字
  close (sess->data_fd);
  sess->data_fd = -1;
  //226
  ftp_reply (sess, FTP_TRANSFEROK, "Directory send OK.");
}

static void do_nlst (session_t *sess)
{
  //创建数据链接
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }
  //150
  ftp_reply (sess, FTP_DATACONN, "Here comes the directory listing.");
  //传输列表
  list_common (sess, 0);
  //关闭数据套接字
  close (sess->data_fd);
  sess->data_fd = -1;
  //226
  ftp_reply (sess, FTP_TRANSFEROK, "Directory send OK.");
}

static void do_rest (session_t *sess)
{

}

static void do_abor (session_t *sess)
{
  ftp_reply (sess, FTP_ABOR_NOCONN, "NO transfer to ABOR.");
}

static void do_pwd (session_t *sess)
{
  char dir[1024] = {0};
  char buf[1024] = {0};
  list_common (sess, 0);
  getcwd (dir, 1024);
  sprintf (buf, "\"%s\"", dir);
  ftp_reply (sess, FTP_PWDOK, buf);
}

static void do_mkd (session_t *sess)
{
  if (mkdir (sess->arg, 0777) < 0)
    {
      ftp_reply (sess, FTP_FILEFAIL, "Create directory failed.");
      return;
    }
  char text[4096] = {0};
  if (sess->arg[0] == '/')
    {
      sprintf (text, "%s create", sess->arg);
    }
  else
    {
      char dir[4096 + 1] = {0};
      getcwd (dir, sizeof (dir));
      if (dir[strlen (dir) - 1] == '/')
        {
          sprintf (text, "%s%s create", dir, sess->arg);
        }
      else
        {
          sprintf (text, "%s/%s create", dir, sess->arg);
        }
    }
  ftp_reply (sess, FTP_MKDIROK, text);
}

static void do_rmd (session_t *sess)
{
  if (rmdir (sess->arg) < 0)
    {
      ftp_reply (sess, FTP_FILEFAIL, "Remove Directory failed.");
      return;
    }
  ftp_reply (sess, FTP_RMDIROK, "Remove Directory Successful.");
}

static void do_dele (session_t *sess)
{
  if (unlink (sess->arg) < 0)
    {
      ftp_reply (sess, FTP_FILEFAIL, "Delete file failed.");
      return;
    }
  ftp_reply (sess, FTP_DELEOK, "Delete file Successful.");
}

static void do_rnfr (session_t *sess)
{

}

static void do_rnto (session_t *sess)
{

}

static void do_site (session_t *sess)
{
  char cmd[100] = {0};
  char arg[100] = {0};

  str_split (sess->arg, cmd, arg, ' ');
  if (strcmp (cmd, "CHMOD") == 0)
    {
      do_site_chmod (sess, arg);
    }
  else if (strcmp (cmd, "UMASK") == 0)
    {
      do_site_umask (sess, arg);
    }
  else if (strcmp (cmd, "HELP") == 0)
    {
      ftp_reply (sess, FTP_SITEHELP, "CHMOD UMASK HELP");
    }
  else
    {
      ftp_reply (sess, FTP_BADCMD, "Unknown SITE command.");
    }
}
static void do_size (session_t *sess)
{
  struct stat buf;
  if (stat (sess->arg, &buf) < 0)
    {
      ftp_reply (sess, FTP_FILEFAIL, "Size failed.");
      return;
    }

  if (!S_ISREG(buf.st_mode))
    {
      ftp_reply (sess, FTP_FILEFAIL, "Could not get file size.");
      return;
    }

  char text[1024] = {0};
  sprintf (text, "%ld", buf.st_size);
  ftp_reply (sess, FTP_SIZEOK, "Could not get file size.");
}

static void do_stat (session_t *sess)
{

}

static void do_noop (session_t *sess)
{
  ftp_reply (sess, FTP_NOOPOK, "NOOP ok");
}

static void do_help (session_t *sess)
{
  ftp_lreply (sess, FTP_HELP, "The following commands are recognized");
  writen (sess->ctrl_fd, "ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n", strlen ("ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));
  writen (sess->ctrl_fd, "MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n", strlen ("MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));
  writen (sess->ctrl_fd, "RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n", strlen ("RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));
  writen (sess->ctrl_fd, "XPWD XRMD\r\n", strlen ("XPWD XRMD\r\n"));
  ftp_reply (sess, FTP_HELP, "HELP OK");
}

static void do_site_chmod (session_t *sess, char *chmod_arg)
{
  if (strlen (chmod_arg) == 0)
    {
      ftp_reply (sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
      return;
    }
  char perm[100] = {0};
  char file[100] = {0};
  str_split (chmod_arg, perm, file, ' ');
  if (strlen (file) == 0)
    {
      ftp_reply (sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
      return;
    }

  unsigned int mode = str_octal_to_uint (perm);
  if (chmod (file, mode) < 0)
    {
      ftp_reply (sess, FTP_CHMODOK, "SITE CHMOD command failed.");
    }
  else
    {
      ftp_reply (sess, FTP_CHMODOK, "SITE CHMOD command ok.");
    }
}
static void do_site_umask (session_t *sess, char *umask_arg)
{
  if (strlen (umask_arg) == 0)
    {
      char text[1024] = {0};
      sprintf (text, "Your current UMASK is 0%o", tunable_local_umask);
      ftp_reply (sess, FTP_UMASKOK, text);
    }
  else
    {
      unsigned int um = str_octal_to_uint (umask_arg);
      umask (um);
      char text[1024] = {0};
      sprintf (text, "UMASK set to 0%o", um);
      ftp_reply (sess, FTP_UMASKOK, text);
    }
}