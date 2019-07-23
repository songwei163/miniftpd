//
// Created by s on 19-5-14.
//

#include "ftpproto.h"
#include "common.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "privsock.h"

session_t *p_sess;

// 访问控制命令
static void do_user (session_t *sess);
static void do_pass (session_t *sess);
static void do_cwd (session_t *sess);
static void do_cdup (session_t *sess);
static void do_quit (session_t *sess);

// 传输参数命令
static void do_port (session_t *sess);
static void do_pasv (session_t *sess);
static void do_type (session_t *sess);

// 服务命令
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

static ftpcmd_t ctrl_cmds_map[] =
    {
        // 访问控制映射
        {"USER",                 do_user},
        {"PASS",                 do_pass},
        {"CWD",                  do_cwd},
        {"XCWD",                 do_cwd},
        {"CDUP",                 do_cdup},
        {"XDUP",                 do_cdup},
        {"QUIT",                 do_quit},
        {"ACCT", NULL},
        {"SMNT", NULL},
        {"REIN", NULL},

        // 传输参数命令
        {"PORT",                 do_port},
        {"PASV",                 do_pasv},
        {"TYPE",                 do_type},
        {"STRU", NULL},
        {"MODE", NULL},

        // 服务命令
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
        {"STOU", NULL},
        {"ALLO", NULL}
    };

int get_transfer_fd (session_t *sess);
int port_active (session_t *sess);
int pasv_active (session_t *sess);
int get_port_fd (session_t *sess);
int get_pasv_fd (session_t *sess);
int lock_file_read (int fd);
int lock_file_write (int fd);
int lock_internal (int fd, int lock_type);
int unlock_file (int fd);

void limit_rate (session_t *sess, int bytes_transfered, int is_upload);
void start_cmdio_alarm ();
void start_data_alarm ();
void handle_alarm_timeout (int sig);
void handle_siglarm (int sig);

void handle_sigurg (int sig);

void check_abor (session_t *sess);

void do_site_chmod (session_t *sess, char *chmod_arg);
void do_site_umask (session_t *sess, char *umask_arg);

void handle_child (session_t *sess)
{
  ftp_relply (sess, FTP_GREET, "(miniftpd 0.1)");
  int ret;
  while (1)
    {
      memset (sess->cmdline, 0, MAX_COMMAND_LINE);
      memset (sess->cmd, 0, MAX_COMMAND);
      memset (sess->cmd_arg, 0, MAX_ARG);

      start_cmdio_alarm ();

      ret = readline (sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);

      if (ret == -1)
        ERR_EXIT("readline");
      else if (ret == 0)
        exit (EXIT_SUCCESS);
      // 解析读取到的FTP命令与参数，处理FTP命令，然后发送给父进程
      //printf("%s", sess->cmdline);
      str_trim_crlf (sess->cmdline);
      //printf("%s\n", sess->cmdline);

      // 将命令与参数分割
      str_split (sess->cmdline, sess->cmd, sess->cmd_arg, ' ');
      //printf("%s %s\n",sess->cmd,sess->cmd_arg);
      // 将命令全部转化为大写
      str_upper (sess->cmd);

      int i;
      int map_size = sizeof (ctrl_cmds_map) / sizeof (ctrl_cmds_map[0]);
      for (i = 0; i < map_size; ++i)
        {
          if (strcmp (ctrl_cmds_map[i].cmd, sess->cmd) == 0)
            {
              if (ctrl_cmds_map[i].cmd_func != NULL)
                {
                  ctrl_cmds_map[i].cmd_func (sess);
                }
              else
                {
                  ftp_relply (sess, FTP_COMMANDNOTIMPL, "Unimplement command.");
                }
              break;
            }
        }
      if (i == map_size)
        {
          ftp_relply (sess, FTP_BADCMD, "Unknown command.");
        }
    }
}

static void do_user (session_t *sess)
{
  struct passwd *pw = getpwnam (sess->cmd_arg);
  if (pw == NULL)
    {
      // user is not exist
      ftp_relply (sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }
  sess->uid = pw->pw_uid;
  ftp_relply (sess, FTP_GIVEPWORD, "Please specify the password.");
}

static void do_pass (session_t *sess)
{
  struct passwd *pw = getpwuid (sess->uid);
  if (pw == NULL)
    {
      ftp_relply (sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }
  // only root can do this
  struct spwd *sp = getspnam (pw->pw_name);
  if (sp == NULL)
    {
      ftp_relply (sess, FTP_LOGINERR, "Login incorrect.");
      return;
    }

  // encrypt the sess passwd
  char *encrypt_pass = crypt (sess->cmd_arg, sp->sp_pwdp);
  if (strcmp (encrypt_pass, sp->sp_pwdp) != 0)
    {
      ftp_relply (sess, FTP_LOGINERR, "Password incorrect.");
      return;
    }
  // login successful,set process egid and euid
  if (setegid (pw->pw_gid) < 0)
    {
      ERR_EXIT("setegid");
    }

  if (seteuid (pw->pw_uid) < 0)
    {
      ERR_EXIT("seteuid");
    }

  signal (SIGURG, handle_sigurg);
  activate_sigurg (sess->ctrl_fd);

  umask (tunable_local_umask);

  chdir (pw->pw_dir);
  ftp_relply (sess, FTP_LOGINOK, "Login successful.");
}

// change cur_pwd
void do_cwd (session_t *sess)
{
  if (chdir (sess->cmd_arg) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Directory changed fail.");
    }
  else
    {
      ftp_relply (sess, FTP_CWDOK, "Directory successfully changed.");
    }
}

void do_cdup (session_t *sess)
{
  if (chdir ("..") < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Directory changed fail.");
    }
  else
    {
      ftp_relply (sess, FTP_CWDOK, "Directory successfully changed.");
    }
}

void do_quit (session_t *sess)
{
  ftp_relply (sess, FTP_GOODBYE, "Goodbye.");
  exit (EXIT_SUCCESS);
}

void do_port (session_t *sess)
{
  unsigned int v[6];

  sscanf (sess->cmd_arg, "%u,%u,%u,%u,%u,%u", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
  sess->port_addr = (struct sockaddr_in *) malloc (sizeof (struct sockaddr_in));
  memset (sess->port_addr, 0, sizeof (struct sockaddr_in));
  sess->port_addr->sin_family = AF_INET;
  unsigned char *p = (unsigned char *) &sess->port_addr->sin_port;
  p[0] = v[0];
  p[1] = v[1];

  p = (unsigned char *) &sess->port_addr->sin_addr;
  p[0] = v[2];
  p[1] = v[3];
  p[2] = v[4];
  p[3] = v[5];

  ftp_relply (sess, FTP_PORTOK, "PORT command successful. Consider using PASV.");
}

void do_pasv (session_t *sess)
{
  char local_ip[16] = {0};
  getlocalip (local_ip);

  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_LISTEN);
  unsigned short port = (int) priv_sock_get_int (sess->child_fd);

  unsigned int v[4];
  sscanf (local_ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
  char text[MAX_LINE] = {0};
  sprintf (text, "Entering Passive Mode (%u,%u,%u,%u,%u,%u).", v[0], v[1], v[2], v[3], (port >> 8), (port & 0xFF));

  ftp_relply (sess, FTP_PASVOK, text);

}

void do_type (session_t *sess)
{
  if (strcmp (sess->cmd_arg, "A") == 0)
    {
      sess->is_ascii = 1;
      ftp_relply (sess, FTP_TYPEOK, "Switch to ASCII mode");
    }
  else if (strcmp (sess->cmd_arg, "I") == 0)
    {
      sess->is_ascii = 0;
      ftp_relply (sess, FTP_TYPEOK, "Switch to Binary mode");
    }
  else
    {
      ftp_relply (sess, FTP_BADCMD, "Unrecognised command.");
    }
}

void do_retr (session_t *sess)
{
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }

  long long offset = sess->restart_pos;
  sess->restart_pos = 0;

  int fd = open (sess->cmd_arg, O_RDONLY);
  if (fd == -1)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Open file failed.");
      return;
    }

  // add read lock
  int ret;
  ret = lock_file_read (fd);

  if (ret == -1)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Open file faild.");
      return;
    }

  // 判断是否为普通文件
  struct stat sbuf;
  ret = fstat (fd, &sbuf);

  if (!S_ISREG(sbuf.st_mode))
    {
      ftp_relply (sess, FTP_FILEFAIL, "Open file faild.");
      return;
    }

  // 断点续传，seek到发送的偏移位置
  if (offset != 0)
    {
      ret = lseek (fd, offset, SEEK_SET);
      if (ret == -1)
        {
          ftp_relply (sess, FTP_FILEFAIL, "Failed to open file.");
          return;
        }
    }

  char text[MAX_LINE] = {0};
  if (sess->is_ascii)
    {
      sprintf (text, "Opening ASCII mode data connection %s (%lld bytes)",
               sess->cmd_arg, (long long) sbuf.st_size);
    }
  else
    {
      sprintf (text, "Opening BINARY mode data connection %s (%lld bytes)",
               sess->cmd_arg, (long long) sbuf.st_size);

    }

  ftp_relply (sess, FTP_DATACONN, text);

  int flag = 0;
  long long bytes_to_send = sbuf.st_size;
  if (offset > bytes_to_send)
    {
      bytes_to_send = 0;
    }
  else
    {
      bytes_to_send -= offset;
    }

  sess->bw_transfer_start_sec = get_time_sec ();
  sess->bw_transfer_start_usec = get_time_usec ();

  while (bytes_to_send > 0)
    {
      int num_this_time = bytes_to_send > (4 * MAX_LINE) ? (4 * MAX_LINE) : bytes_to_send;
      ret = sendfile (sess->data_fd, fd, NULL, num_this_time);
      if (ret == -1)
        {
          flag = 2;
        }

      limit_rate (sess, ret, 0);
      if (sess->abor_received)
        {
          flag = 2;
          break;
        }
      bytes_to_send -= ret;
    }

  if (bytes_to_send == 0)
    {
      flag = 0;
    }

  close (sess->data_fd);
  sess->data_fd = -1;
  close (fd);

  if (flag == 0 && !sess->abor_received)
    {
      ftp_relply (sess, FTP_TRANSFEROK, "Transfer complete.");
    }
  else if (flag == 1)
    {
      ftp_relply (sess, FTP_BADSENDFILE, "Failure reading from local file.");;
    }
  else if (flag == 2)
    {
      ftp_relply (sess, FTP_BADSENDNET, "Failure writting to network stream.");
    }

  check_abor (sess);

  start_cmdio_alarm ();
}

void do_stor (session_t *sess)
{
  upload_common (sess, 0);
}

void do_appe (session_t *sess)
{
  upload_common (sess, 1);
}

void do_list (session_t *sess)
{
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }
  ftp_relply (sess, FTP_DATACONN, "Here comes the directory list.");

  list_common (sess, 1);

  close (sess->data_fd);

  ftp_relply (sess, FTP_TRANSFEROK, "Directory send OK.");
}

void do_nlst (session_t *sess)
{
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }
  ftp_relply (sess, FTP_DATACONN, "Here comes the directory list.");

  list_common (sess, 0);

  close (sess->data_fd);

  ftp_relply (sess, FTP_TRANSFEROK, "Directory send OK.");
}

void do_rest (session_t *sess)
{
  // 断点续传位移
  sess->restart_pos = str_to_longlong (sess->cmd_arg);

  char text[MAX_LINE] = {0};
  sprintf (text, "%s (%lld).", "Restart position accepted", sess->restart_pos);

  ftp_relply (sess, FTP_RESTOK, text);
}

// 紧急模式数据接收
void do_abor (session_t *sess)
{
  ftp_relply (sess, FTP_ABOR_NOCONN, "No transfer to ABOR.");
}

void do_pwd (session_t *sess)
{
  char cur_dir[MAX_LINE + 1] = {0};
  char text[MAX_LINE] = {0};
  getcwd (cur_dir, MAX_LINE);
  sprintf (text, "\"%s\"", cur_dir);
  ftp_relply (sess, FTP_PWDOK, text);
}

void do_mkd (session_t *sess)
{
  // 0777 & umask
  if (mkdir (sess->cmd_arg, 0777) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Create directory operation failed.");
    }
  else
    {
      char text[MAX_LINE] = {0};
      if (sess->cmd_arg[0] == '/')
        {
          sprintf (text, "%s created", sess->cmd_arg);
        }
      else
        {
          char cur_dir[MAX_LINE + 1] = {0};
          getcwd (cur_dir, MAX_LINE);
          if (cur_dir[strlen (cur_dir) - 1] == '/')
            {
              sprintf (text, "%s%s created", cur_dir, sess->cmd_arg);
            }
          else
            {
              sprintf (text, "%s/%s created", cur_dir, sess->cmd_arg);
            }
        }

      ftp_relply (sess, FTP_MKDIROK, text);
    }
}

void do_rmd (session_t *sess)
{
  if (rmdir (sess->cmd_arg) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Rmove directory operation failed.");
    }

  ftp_relply (sess, FTP_RMDIROK, "Remove directory operation successful.");
}

void do_dele (session_t *sess)
{
  if (unlink (sess->cmd_arg) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Delete file operation failed.");
      return;
    }
  ftp_relply (sess, FTP_DELEOK, "Delete file operation successful.");
}

// rename file request
void do_rnfr (session_t *sess)
{
  sess->rnfr_name = (char *) malloc (strlen (sess->cmd_arg) + 1);
  memset (sess->rnfr_name, 0, strlen (sess->cmd_arg) + 1);

  strcpy (sess->rnfr_name, sess->cmd_arg);
  ftp_relply (sess, FTP_RNFROK, "Ready for RNTO.");
}

// rename
void do_rnto (session_t *sess)
{
  if (sess->rnfr_name == NULL)
    {
      ftp_relply (sess, FTP_NEEDRNFR, "RNFR required first.");
      return;
    }
  if (rename (sess->rnfr_name, sess->cmd_arg) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "Rename operation failed.");
    }
  else
    {
      ftp_relply (sess, FTP_RENAMEOK, "Rename successful.");
    }

  if (sess->rnfr_name)
    {
      free (sess->rnfr_name);
      sess->rnfr_name = NULL;
    }
}

void do_site (session_t *sess)
{
  // SITE CHMOD <param> <file>
  // SITE UMASK [umask]
  // SITE HELP
  char cmd[100] = {0};
  char arg[100] = {0};

  str_split (sess->cmd_arg, cmd, arg, ' ');
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
      ftp_relply (sess, FTP_SITEHELP, "CHMOD UMASK HELP");
    }
  else
    {
      ftp_relply (sess, FTP_BADCMD, "Unknown SITE command");
    }
}

void do_syst (session_t *sess)
{
  ftp_relply (sess, FTP_SYSTOK, "UNIX Type: L8");
}

void do_feat (session_t *sess)
{
  ftp_lrelply (sess, FTP_FEAT, "Features:");
  writen (sess->ctrl_fd, "EPRT\r\n", strlen ("EPRT\r\n"));
  writen (sess->ctrl_fd, "EPSV\r\n", strlen ("EPSV\r\n"));
  writen (sess->ctrl_fd, "MDTM\r\n", strlen ("MDTM\r\n"));
  writen (sess->ctrl_fd, "PASV\r\n", strlen ("PASV\r\n"));
  writen (sess->ctrl_fd, "REST STREAM\r\n", strlen ("REST STREAM\r\n"));
  writen (sess->ctrl_fd, "SIZE\r\n", strlen ("SIZE\r\n"));
  writen (sess->ctrl_fd, "TVFS\r\n", strlen ("TVFS\r\n"));
  writen (sess->ctrl_fd, "UTF8\r\n", strlen ("UTF8\r\n"));

  ftp_relply (sess, FTP_FEAT, "End");
}

void do_size (session_t *sess)
{
  struct stat s_buf;
  if (stat (sess->cmd_arg, &s_buf) < 0)
    {
      ftp_relply (sess, FTP_FILEFAIL, "SIZE operation ");
      return;
    }

  if (!S_ISREG(s_buf.st_mode))
    {
      ftp_relply (sess, FTP_FILEFAIL, "Could not get file size.");
      return;
    }

  char text[MAX_LINE];
  sprintf (text, "%lld", (long long) s_buf.st_size);
  ftp_relply (sess, FTP_SIZEOK, text);
}

void do_stat (session_t *sess)
{
  ftp_lrelply (sess, FTP_STATOK, "FTP server stats: ");

  if (sess->bw_upload_rate_max == 0)
    {
      char text[MAX_LINE];
      sprintf (text, "No session upload bandwidth limit\r\n");
      writen (sess->ctrl_fd, text, strlen (text));
    }
  else if (sess->bw_upload_rate_max > 0)
    {
      char text[MAX_LINE];
      sprintf (text, "Session upload bandwidth limit int bytes/s is %u\r\n", sess->bw_upload_rate_max);
      writen (sess->ctrl_fd, text, strlen (text));
    }

  if (sess->bw_download_rate_max == 0)
    {
      char text[MAX_LINE];
      sprintf (text, "No session download bandwidth limit\r\n");
      writen (sess->ctrl_fd, text, strlen (text));
    }
  else if (sess->bw_download_rate_max > 0)
    {
      char text[MAX_LINE];
      sprintf (text, "Session download bandwidth limit in byte/s is %u\r\n", sess->bw_download_rate_max);
      writen (sess->ctrl_fd, text, strlen (text));
    }

  char text[MAX_LINE] = {0};
  sprintf (text, "At session startup,client count was %u\r\n", sess->num_clients);
  writen (sess->ctrl_fd, text, strlen (text));

  ftp_relply (sess, FTP_STATOK, "End of status.");
}

// no operation
void do_noop (session_t *sess)
{
  ftp_relply (sess, FTP_NOOPOK, "NOOP ok.");
}

void do_help (session_t *sess)
{
  ftp_lrelply (sess, FTP_HELP, "The following commands are recognized.");

  writen (sess->ctrl_fd, " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n",
          strlen (" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));
  writen (sess->ctrl_fd, " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n",
          strlen (" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));

  writen (sess->ctrl_fd, " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n",
          strlen (" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));

  writen (sess->ctrl_fd, " XPWD XRMD\r\n", strlen (" XPWD XRMD\r\n"));

  ftp_relply (sess, FTP_HELP, "Help OK.");
}

void ftp_relply (session_t *sess, int status, const char *text)
{
  char buf[MAX_LINE] = {0};
  sprintf (buf, "%d %s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

void ftp_lrelply (session_t *sess, int status, const char *text)
{
  char buf[MAX_LINE] = {0};
  sprintf (buf, "%d-%s\r\n", status, text);
  writen (sess->ctrl_fd, buf, strlen (buf));
}

int list_common (session_t *sess, int detail)
{
  DIR *dir = opendir (".");
  if (dir == NULL)
    {
      return 0;
    }
  struct dirent *dt;
  struct stat sbuf;
  while ((dt = readdir (dir)) != NULL)
    {
      if (lstat (dt->d_name, &sbuf) < 0)
        {
          continue;
        }
      if (dt->d_name[0] == '.')
        {
          continue;
        }
      char buf[MAX_LINE] = {0};
      if (detail)
        {
          char perms[] = "----------";
          get_file_mode (perms, sbuf.st_mode);

          int off = 0;
          off += sprintf (buf, "%s ", perms);
          off += sprintf (buf
                          + off, "%3d %-8d  %-8d", (unsigned int) sbuf.st_nlink, (unsigned int) sbuf.st_uid, (unsigned int) sbuf.st_gid);
          off += sprintf (buf + off, "%8lu ", (unsigned long) sbuf.st_size);

          const char *databuf = get_stat_databuf (&sbuf);

          off += sprintf (buf + off, "%s ", databuf);
          // link file
          if (S_ISLNK(sbuf.st_mode))
            {
              char tmp[MAX_LINE] = {0};
              readlink (dt->d_name, tmp, sizeof (tmp));
              sprintf (buf + off, "%s -> %s\r\n", dt->d_name, tmp);
            }
          else
            {
              sprintf (buf + off, "%s\r\n", dt->d_name);
            }
        }
      else
        {
          sprintf (buf, "%s\r\n", dt->d_name);
        }
      writen (sess->data_fd, buf, strlen (buf));
    }

  closedir (dir);

  return 1;
}

void limit_rate (session_t *sess, int bytes_transfered, int is_upload)
{
  sess->data_process = 1;

  long cur_sec = get_time_sec ();
  long cur_usec = get_time_usec ();

  double elapsed;
  elapsed = (double) cur_sec - sess->bw_transfer_start_sec;
  elapsed += (double) (cur_usec - sess->bw_transfer_start_usec) / (double) 1000000;

  if (elapsed <= (double) 0)
    {
      elapsed = (double) 0.01;
    }

  // cal cur transfer v
  unsigned int bw_rate = (unsigned int) ((double) bytes_transfered / elapsed);

  double rate_ratio;
  if (is_upload)
    {
      if (bw_rate < sess->bw_upload_rate_max)
        {
          // needn't limit rate
          sess->bw_transfer_start_sec = cur_sec;
          sess->bw_transfer_start_usec = cur_usec;
          return;
        }
      rate_ratio = bw_rate / sess->bw_upload_rate_max;
    }
  else
    {
      if (bw_rate < sess->bw_download_rate_max)
        {
          // needn't limit rate
          sess->bw_transfer_start_sec = cur_sec;
          sess->bw_transfer_start_usec = cur_usec;
          return;
        }
      rate_ratio = bw_rate / sess->bw_download_rate_max;
    }

  // 睡眠时间 = （当前传输速度 / 最大传输速度 - 1) * 当前传输时间
  double pause_time;
  pause_time = (rate_ratio - (double) 1) * elapsed;

  nano_sleep (pause_time);

  sess->bw_transfer_start_sec = get_time_sec ();
  sess->bw_transfer_start_usec = get_time_usec ();
}

void upload_common (session_t *sess, int is_append)
{
  if (get_transfer_fd (sess) == 0)
    {
      return;
    }

  long long offset = sess->restart_pos;
  sess->restart_pos = 0;

  int fd = open (sess->cmd_arg, O_CREAT | O_WRONLY, 0666);
  if (fd == -1)
    {
      ftp_relply (sess, FTP_UPLOADFAIL, "Create file failed1.");
      return;
    }

  // add read lock
  int ret;
  ret = lock_file_write (fd);

  if (ret == -1)
    {
      ftp_relply (sess, FTP_UPLOADFAIL, "Create file faild2.");
      return;
    }

  // STOR
  // REST+STOR
  // APPE
  if (!is_append && offset == 0)    // STOP
    {
      ftruncate (fd, 0);
      if (lseek (fd, 0, SEEK_SET) < 0)
        {
          ftp_relply (sess, FTP_UPLOADFAIL, "Create file failed3.");
          return;
        }
    }
  else if (!is_append && offset != 0)    // REST + STOR
    {
      if (lseek (fd, offset, SEEK_SET) < 0)
        {
          ftp_relply (sess, FTP_UPLOADFAIL, "Create file failed4.");
          return;
        }
    }
  else if (is_append)
    {
      if (lseek (fd, 0, SEEK_END) < 0)
        {
          ftp_relply (sess, FTP_UPLOADFAIL, "Create file failed5.");
          return;
        }

    }

  struct stat sbuf;
  ret = fstat (fd, &sbuf);

  char text[MAX_LINE] = {0};
  if (sess->is_ascii)
    {
      sprintf (text, "Opening ASCII mode data connection %s (%lld bytes)",
               sess->cmd_arg, (long long) sbuf.st_size);
    }
  else
    {
      sprintf (text, "Opening BINARY mode data connection %s (%lld bytes)",
               sess->cmd_arg, (long long) sbuf.st_size);

    }

  ftp_relply (sess, FTP_DATACONN, text);

  // 下载文件
  int flag = 0;

  char buf[MAX_LINE];

  // 睡眠时间 = （当前传输速度 / 最大传输速度 - 1) * 当前传输时间
  sess->bw_transfer_start_sec = get_time_sec ();
  sess->bw_transfer_start_usec = get_time_usec ();

  while (1)
    {
      ret = read (sess->data_fd, buf, sizeof (buf));
      if (ret == -1)
        {
          if (errno == EINTR)
            {
              continue;
            }
          else
            {
              flag = 2;
              break;
            }
        }
      else if (ret == 0)
        {
          flag = 0;
          break;
        }

      limit_rate (sess, ret, 1);
      if (sess->abor_received)
        {
          flag = 2;
          break;
        }

      if (writen (fd, buf, ret) != ret)
        {
          flag = 1;
          break;
        }
    }

  close (sess->data_fd);
  sess->data_fd = -1;
  close (fd);

  if (flag == 0 && !sess->abor_received)
    {
      ftp_relply (sess, FTP_TRANSFEROK, "Transfer complete.");
    }
  else if (flag == 1)
    {
      ftp_relply (sess, FTP_BADSENDFILE, "Failure writing to local file.");;
    }
  else if (flag == 2)
    {
      ftp_relply (sess, FTP_BADSENDNET, "Failure reading from network stream.");
    }

  check_abor (sess);

  // restart ctrl link alarm
  start_cmdio_alarm ();
}

int get_transfer_fd (session_t *sess)
{
  // 检测是否收到port或者pasv命令
  if (!port_active (sess) && !pasv_active (sess))
    {
      ftp_relply (sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
      return 0;
    }
  int ret = 1;

  if (port_active (sess))
    {
      if (get_port_fd (sess) == 0)
        {
          printf ("get port fd ret: %d\n", ret);
          ret = 0;
        }

    }

  if (pasv_active (sess))
    {
      if (get_pasv_fd (sess) == 0)
        {
          printf ("get pasv fd ret: %d\n", ret);
          ret = 0;
        }
    }

  if (sess->port_addr)
    {
      free (sess->port_addr);
      sess->port_addr = NULL;
    }

  if (ret)
    {
      start_data_alarm ();
    }

  return ret;
}

int port_active (session_t *sess)
{
  if (sess->port_addr)
    {
      if (pasv_active (sess))
        {
          fprintf (stderr, "both port and pasv are active");
          exit (EXIT_FAILURE);
        }
      return 1;
    }
  return 0;
}

int pasv_active (session_t *sess)
{
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_ACTIVE);

  int active = priv_sock_get_int (sess->child_fd);
  if (active)
    {
      if (port_active (sess))
        {
          fprintf (stderr, "both port and pasv are active");
          return 0;
        }
      return 1;
    }
  return 0;
}

int get_port_fd (session_t *sess)
{
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_GET_DATA_SOCK);
  unsigned short port = ntohs (sess->port_addr->sin_port);
  char *conn_ip = inet_ntoa (sess->port_addr->sin_addr);
  priv_sock_send_int (sess->child_fd, (int) port);
  priv_sock_send_buf (sess->child_fd, conn_ip, strlen (conn_ip));

  char result = priv_sock_get_result (sess->child_fd);
  if (result == PRIV_SOCK_RESULT_BAD)
    {
      return 0;
    }
  else if (result == PRIV_SOCK_RESULT_OK)
    {
      sess->data_fd = priv_sock_recv_fd (sess->child_fd);
      return 1;
    }
  return 0;
}

int get_pasv_fd (session_t *sess)
{
  priv_sock_send_cmd (sess->child_fd, PRIV_SOCK_PASV_ACCEPT);
  char result = priv_sock_get_result (sess->child_fd);
  if (result == PRIV_SOCK_RESULT_BAD)
    {
      return 0;
    }
  else if (result == PRIV_SOCK_RESULT_OK)
    {
      sess->data_fd = priv_sock_recv_fd (sess->child_fd);
      return 1;
    }
  return 0;
}

int lock_file_read (int fd)
{
  return lock_internal (fd, F_RDLCK);
}

int lock_file_write (int fd)
{
  return lock_internal (fd, F_WRLCK);
}

int lock_internal (int fd, int lock_type)
{
  int ret;
  struct flock the_lock;
  memset (&the_lock, 0, sizeof (the_lock));
  the_lock.l_type = lock_type;
  the_lock.l_whence = SEEK_SET;
  the_lock.l_start = 0;
  the_lock.l_len = 0;
  do
    {
      ret = fcntl (fd, F_SETLKW, &the_lock);
    }
  while (ret < 0 && errno == EINTR);

  return ret;
}

int unlock_file (int fd)
{
  int ret;
  struct flock the_lock;
  memset (&the_lock, 0, sizeof (the_lock));

  the_lock.l_type = F_UNLCK;
  the_lock.l_whence = SEEK_SET;
  the_lock.l_start = 0;
  the_lock.l_len = 0;

  ret = fcntl (fd, F_SETLK, &the_lock);

  return ret;
}

void start_cmdio_alarm ()
{
  if (tunable_idle_session_timeout > 0)
    {
      signal (SIGALRM, handle_alarm_timeout);
      alarm (tunable_idle_session_timeout);
    }
}

void handle_alarm_timeout (int sig)
{
  shutdown (p_sess->ctrl_fd, SHUT_RD);
  ftp_relply (p_sess, FTP_IDLE_TIMEOUT, "Timeout.");
  shutdown (p_sess->ctrl_fd, SHUT_WR);
  exit (EXIT_FAILURE);
}

void start_data_alarm ()
{
  if (tunable_data_connection_timeout > 0)
    {
      signal (SIGALRM, handle_siglarm);
      alarm (tunable_data_connection_timeout);
    }
  else if (tunable_idle_session_timeout > 0)
    {
      alarm (0);
    }
}

void handle_siglarm (int sig)
{
  if (p_sess->data_process == 0)
    {
      ftp_relply (p_sess, FTP_DATA_TIMEOUT, "Data timeout. Reconnect. Sorry.");
      exit (EXIT_FAILURE);
    }

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

  str_trim_crlf (cmdline);

  if (strcmp (cmdline, "ABOR") == 0
      || strcmp (cmdline, "\377\364\377\362ABOR") == 0)
    {
      p_sess->abor_received = 1;
      shutdown (p_sess->data_fd, SHUT_RDWR);
    }
  else
    {
      ftp_relply (p_sess, FTP_BADCMD, "Unknown command.");
    }
}

void check_abor (session_t *sess)
{
  if (sess->abor_received)
    {
      sess->abor_received = 0;
      ftp_relply (sess, FTP_ABOROK, "ABOR successful.");
    }
}

void do_site_chmod (session_t *sess, char *chmod_arg)
{
  // chmod <param> <file>
  if (strlen (chmod_arg) == 0)
    {
      ftp_relply (sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
      return;
    }
  char perms[100] = {0};
  char file[100] = {0};

  str_split (chmod_arg, perms, file, ' ');
  if (strlen (file) == 0)
    {
      ftp_relply (sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
      return;
    }

  unsigned int mode = str_octal_to_uint (perms);
  if (chmod (file, mode) < 0)
    {
      ftp_relply (sess, FTP_BADMODE, "SITE CHMOD failed.");
    }
  else
    {
      ftp_relply (sess, FTP_CHMODOK, "SITE CHMOD ok.");
    }
}

void do_site_umask (session_t *sess, char *umask_arg)
{
  // umask <param>
  if (strlen (umask_arg) == 0)
    {
      char text[MAX_LINE] = {0};
      sprintf (text, "Your current UMASK is 0%o", tunable_local_umask);
      ftp_relply (sess, FTP_UMASKOK, text);
    }
  else
    {
      unsigned int umode = str_octal_to_uint (umask_arg);
      umask (umode);

      char text[MAX_LINE] = {0};
      sprintf (text, "UMASK set to 0%o", umode);
      ftp_relply (sess, FTP_UMASKOK, text);
    }
}