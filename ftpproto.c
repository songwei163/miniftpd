//
// Created by s on 19-5-14.
//

#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "privsock.h"

void ftp_reply (session_t *sess, int status, const char *text);
void ftp_lreply (session_t *sess, int status, const char *text);

int list_common (session_t *sess);
bool port_active (session_t *sess);
bool pasv_active (session_t *sess);
bool get_transfer_fd (session_t *sess);

static void do_user (session_t *sess);
static void do_pass (session_t *sess);
static void do_cwd (session_t *sess);
static void do_cdup (session_t *sess);
static void do_quit (session_t *sess);
static void do_port (session_t *sess);
static void do_pasv (session_t *sess);
static void do_type (session_t *sess);
static void do_stru (session_t *sess);
static void do_mode (session_t *sess);
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

typedef struct ftpcmd {
    const char *cmd;
    void (*cmd_handler) (session_t *sess);
} ftpcmd_t;

static ftpcmd_t ctrl_cmds[] = {
    /* 访问控制命令 */
    {"USER",                 do_user},
    {"PASS",                 do_pass},
    {"CWD",                  do_cwd},
    {"XCWD",                 do_cwd},
    {"CDUP",                 do_cdup},
    {"XCUP",                 do_cdup},
    {"QUIT",                 do_quit},
    {"ACCT",               NULL},
    {"SMNT",               NULL},
    {"REIN",               NULL},
    /* 传输参数命令 */
    {"PORT",                 do_port},
    {"PASV",                 do_pasv},
    {"TYPE",                 do_type},
    {"STRU",    /*do_stru*/NULL},
    {"MODE",    /*do_mode*/NULL},

    /* 服务命令 */
    {"RETR",                 do_retr},
    {"STOR",                 do_stor},
    {"APPE",                 do_appe},
    {"LIST",                 do_list},
    {"NLST",                 do_nlst},
    {"REST",                 do_rest},
    {"ABOR",                 do_abor},
    {"\377\364\377\362ABOR", do_abor},
    {"PWD",                  do_pwd},
    {"XPWD",                 do_pwd},
    {"MKD",                  do_mkd},
    {"XMKD",                 do_mkd},
    {"RMD",                  do_rmd},
    {"XRMD",                 do_rmd},
    {"DELE",                 do_dele},
    {"RNFR",                 do_rnfr},
    {"RNTO",                 do_rnto},
    {"SITE",                 do_site},
    {"SYST",                 do_syst},
    {"FEAT",                 do_feat},
    {"SIZE",                 do_size},
    {"STAT",                 do_stat},
    {"NOOP",                 do_noop},
    {"HELP",                 do_help},
    {"STOU",               NULL},
    {"ALLO",               NULL}
};

void handle_child (session_t *sess)
{
  ftp_reply (sess, FTP_GREET, "(miniftpd 0.1)");
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

      //printf ("cmdline=[%s]\n", sess->cmdline);
      //去除\r\n
      str_trim_crlf (sess->cmdline);
      printf ("cmdline=[%s]\n", sess->cmdline);
      //解析FTP命令与参数
      str_split (sess->cmdline, sess->cmd, sess->arg, ' ');
      printf ("cmd=[%s] arg=[%s]\n", sess->cmd, sess->arg);
      //将命令转换为大写
      str_upper (sess->cmd);
      //处理FTP命令
      //命令映射
      /*
      if (strcmp ("USER", sess->cmd) == 0)
        {
          do_user (sess);
        }
      else if (strcmp ("PASS", sess->cmd) == 0)
        {
          do_pass (sess);
        }
        */
      int i = 0;
      int size = sizeof (ctrl_cmds) / sizeof (ctrl_cmds[0]);
      for (i = 0; i < size; ++i)
        {
          if (strcmp (ctrl_cmds[i].cmd, sess->cmd) == 0)
            {
              if (ctrl_cmds[i].cmd_handler != NULL)
                {
                  ctrl_cmds[i].cmd_handler (sess);
                }
              else
                {
                  ftp_reply (sess, FTP_COMMANDNOTIMPL, "Unimplement command.");
                }

              break;
            }
        }
      if (i == size)
        {
          ftp_reply (sess, FTP_BADCMD, "Unkonw command.");
        }
    }
}

void ftp_lreply (session_t *sess, int status, const char *text)
{
  char buf[1024] = {0};
  sprintf (buf, "%d-%s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

void ftp_reply (session_t *sess, int status, const char *text)
{
  char buf[1024] = {0};
  sprintf (buf, "%d %s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

int list_common (session_t *sess)
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
              case S_IFREG:
                perms[0] = '-';
              break;
              case S_IFDIR:
                perms[0] = 'd';
              break;
              case S_IFLNK:
                perms[0] = 'l';
              break;
              case S_IFIFO:
                perms[0] = 'p';
              break;
              case S_IFSOCK:
                perms[0] = 's';
              break;
              case S_IFCHR:
                perms[0] = 'c';
              break;
              case S_IFBLK:
                perms[0] = 'b';
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

              //sprintf (buf + off, "%s\r\n", dt->d_name);
            }
          // printf ("%s", buf);
          writen (sess->data_fd, buf, sizeof (buf));
        }
      closedir (dir);
    }
  return 1;
}

bool port_active (session_t *sess)
{
  if (sess->port_addr)
    {
      return true;
    }
  return false;
}

bool pasv_active (session_t *sess)
{
  if (sess->pasv_listen_fd != -1)
    return true;
  return false;

}

bool get_transfer_fd (session_t *sess)
{
  if (!port_active (sess) && !pasv_active (sess))
    {
      //425 Use PORT or PASV first
      ftp_reply (sess, FTP_BADSENDCONN, "Use PORT or PASV first");
      return 0;
    }

  if (port_active (sess) && pasv_active (sess))
    {
      //425 PORT both PASV active.
      ftp_reply (sess, FTP_BADSENDCONN, "PORT both PASV active.");
      return 0;
    }

  int datafd;
  if (port_active (sess))
    {

      if ((datafd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        return 0;
      socklen_t addrlen = sizeof (struct sockaddr);

      /*
      struct sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_port = htons(20);
      address.sin_addr.s_addr = INADDR_ANY;

      if(bind(datafd, (struct sockaddr*)&address, addrlen) < 0)
          ERR_EXIT("bind 20");
      */

      if (connect (datafd, (struct sockaddr *) sess->port_addr, addrlen) < 0)
        return 0;

    }

  if (pasv_active (sess))
    {
      struct sockaddr_in addrcli;
      socklen_t addrlen = sizeof (addrcli);
      if ((datafd = accept (sess->pasv_listen_fd, (struct sockaddr *) &addrcli, &addrlen)) < 0)
        return 0;
      close (sess->pasv_listen_fd);
      sess->pasv_listen_fd = -1;
    }
  sess->data_fd = datafd;

  if (sess->port_addr)
    {
      free (sess->port_addr);
      sess->port_addr = 0;
    }

  return 1;

}

static void do_user (session_t *sess)
{
  // USER s
  struct passwd *pw = getpwnam (sess->arg);
  if (pw == NULL)
    {
      //用户不存在
      ftp_reply (sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }
  sess->uid = pw->pw_uid;
  ftp_reply (sess, FTP_GIVEPWORD, "Please specify the passwd.");

}

static void do_pass (session_t *sess)
{
  //PASS
  struct passwd *pw = getpwuid (sess->uid);
  if (pw == NULL)
    {
      //用户不存在
      ftp_reply (sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }

  /*
   * !!!!!!!
   */
  struct spwd *sp = getspnam (pw->pw_name);
  if (sp == NULL)
    {
      ftp_reply (sess, FTP_LOGINERR, "Login incorrect.");
      return;

    }

  //将明文进行加密，将加密的结果与阴影文件中的已经加密过的密码比对
  char *encrypted_pass = crypt (sess->arg, sp->sp_pwdp);
  if (strcmp (encrypted_pass, sp->sp_pwdp) != 0)
    {
      ftp_reply (sess, FTP_LOGINERR, "Login incorrect.");
    }

  //getspnam ()

  /*更改进程组ID用户ID，用户目录*/
  setegid (pw->pw_gid);
  seteuid (pw->pw_uid);
  chdir (pw->pw_dir);

  ftp_reply (sess, FTP_LOGINOK, "Login successful.");

}

static void do_cwd (session_t *sess)
{}

static void do_cdup (session_t *sess)
{}

static void do_quit (session_t *sess)
{}

static void do_port (session_t *sess)
{
  //PORT
  unsigned int v[6];

  sscanf (sess->arg, "%u,%u,%u,%u,%u,%u", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
  sess->port_addr = malloc (sizeof (struct sockaddr));
  memset (sess->port_addr, 0, sizeof (struct sockaddr));
  sess->port_addr->sin_family = AF_INET;

  unsigned char *p = (unsigned char *) &sess->port_addr->sin_port;
  p[0] = v[0];
  p[1] = v[1];
  p = (unsigned char *) &sess->port_addr->sin_addr.s_addr;
  p[0] = v[2];
  p[1] = v[3];
  p[2] = v[4];
  p[3] = v[5];

  ftp_reply (sess, FTP_PORTOK, "PORT command successful.Consider using PASV.");

}

static void do_pasv (session_t *sess)
{
  char ip[16] = {0};
  getlocalip (ip);
  int sockfd = tcp_server (ip, 0);

  struct sockaddr_in address;
  socklen_t addrlen = sizeof (address);
  if (getsockname (sockfd, (struct sockaddr *) &address, &addrlen) < 0)
    ERR_EXIT("getsockname");

  //printf("ip = %s\n", inet_ntoa(addr.sin_addr));
  //printf("port = %d\n",ntohs(addr.sin_port));
  unsigned short port = ntohs (address.sin_port);
  unsigned char addr[6] = {0};
  sscanf (ip, "%hhu.%hhu.%hhu.%hhu", &addr[0], &addr[1], &addr[2], &addr[3]);
  addr[4] = ((port >> 8) & 0x00ff);
  addr[5] = port & 0x00ff;

  char buf[MAX_BUFFER_SIZE] = {0};
  sprintf (buf, "Entering Passive Mode (%u,%u,%u,%u,%u,%u)", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  //port = 8080
  //227 Entering Passive Mode (192,168,0,200,76,240).
  sess->pasv_listen_fd = sockfd;
  ftp_reply (sess, FTP_PASVOK, buf);
}

static void do_type (session_t *sess)
{
  if (strcmp (sess->arg, "A") == 0)
    {
      sess->is_ascii = true;
      ftp_reply (sess, FTP_TYPEOK, "Switching to ASCII mode.");
    }
  else if (strcmp (sess->arg, "I") == 0)
    {
      sess->is_ascii = false;
      ftp_reply (sess, FTP_TYPEOK, "Switching to Binary mode.");
    }
  else
    {
      ftp_reply (sess, FTP_BADCMD, "Unrecognized Type command.");
    }
}

static void do_stru (session_t *sess)
{}

static void do_mode (session_t *sess)
{}

static void do_retr (session_t *sess)
{}

static void do_stor (session_t *sess)
{}

static void do_appe (session_t *sess)
{}

static void do_list (session_t *sess)
{
  //创建数据连接
  if (get_transfer_fd (sess) == 0)
    {
      //printf ("0\n");
      return;
    }
  //printf ("1\n");

  //150
  ftp_reply (sess, FTP_DATACONN, "Here comes the directory listing.");
  //传输列表
  list_common (sess);
  //关闭数据套接字
  close (sess->data_fd);
  //226
  ftp_reply (sess, FTP_TRANSFEROK, "Directory send OK.");

  /*
3、	客户端向服务器端发送LIST
    服务器端检测在收到LIST命令之前是否接收过PORT或PASV命令
    如果没有接受过，则响应425Use PORT or PASV first
    如果有接收过，并且是PORT，则服务器端创建数据套接字(bind 20端口)，
    调用connect主动连接客户端IP与端口，从而建立数据连接

4、	服务器发送150应答给客户端，表示准备就绪，可以开始传输了
5、	开始传输列表
6、	服务器发送226应答给客户端，表示数据传输结束
传输结束，服务器端主动关闭数据套接字
*/
}

static void do_nlst (session_t *sess)
{}

static void do_rest (session_t *sess)
{}

static void do_abor (session_t *sess)
{}

static void do_pwd (session_t *sess)
{
  char text[1024 + 3] = {0};
  char dir[1024 + 1] = {0};
  getcwd (dir, 1024);
  sprintf (text, "\"%s\"", dir);
  printf ("\"%s\"", text);
  ftp_reply (sess, FTP_PWDOK, text);
}

static void do_mkd (session_t *sess)
{}

static void do_rmd (session_t *sess)
{}

static void do_dele (session_t *sess)
{}

static void do_rnfr (session_t *sess)
{}

static void do_rnto (session_t *sess)
{}

static void do_site (session_t *sess)
{}

static void do_syst (session_t *sess)
{
  ftp_reply (sess, FTP_SYSTOK, "UNIX Type: L8");     //ftp_reply(sess, FTP_FEAT, "-Features:");
  writen (sess->ctrl_fd, "211-Features:\r\n", strlen ("211-Features:\r\n"));
  writen (sess->ctrl_fd, "EPRT\r\n", strlen ("EPRT\r\n"));
  writen (sess->ctrl_fd, "EPSV\r\n", strlen ("EPSV\r\n"));
  writen (sess->ctrl_fd, "MDTM\r\n", strlen ("MDTM\r\n"));
  writen (sess->ctrl_fd, "PASV\r\n", strlen ("PASV\r\n"));
  writen (sess->ctrl_fd, "REST STREAM\r\n", strlen ("REST STREAM\r\n"));
  writen (sess->ctrl_fd, "SIZE\r\n", strlen ("SIZE\r\n"));
  writen (sess->ctrl_fd, "TVFS\r\n", strlen ("TVFS\r\n"));
  writen (sess->ctrl_fd, "UTF8\r\n", strlen ("UTF8\r\n"));
  ftp_reply (sess, FTP_FEAT, "end");
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

static void do_size (session_t *sess)
{}

static void do_stat (session_t *sess)
{}

static void do_noop (session_t *sess)
{}

static void do_help (session_t *sess)
{}