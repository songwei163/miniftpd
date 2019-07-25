#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "str.h"
#include "tunable.h"
#include "parseconf.h"
#include "ftpproto.h"
#include "ftpcodes.h"
#include "hash.h"

extern session_t *p_sess;
static unsigned int s_children;

static hash_t *s_ip_count_hash;
static hash_t *s_pid_ip_hash;

void check_limits (session_t *sess);
void handle_sigchld (int sig);
unsigned int hash_func (unsigned int buckets, void *key);

unsigned int handle_ip_count (void *ip);
void drop_ip_count (void *ip);

int main (void)
{
  parseconf_load_file (MINIFTP_CONF);
  // daemon (0, 0);

  printf ("tunable_pasv_enable=%d\n", tunable_pasv_enable);
  printf ("tunable_port_enable=%d\n", tunable_port_enable);

  printf ("tunable_listen_port=%u\n", tunable_listen_port);
  printf ("tunable_max_clients=%u\n", tunable_max_clients);
  printf ("tunable_max_per_ip=%u\n", tunable_max_per_ip);
  printf ("tunable_accept_timeout=%u\n", tunable_accept_timeout);
  printf ("tunable_connect_timeout=%u\n", tunable_connect_timeout);
  printf ("tunable_idle_session_timeout=%u\n", tunable_idle_session_timeout);
  printf ("tunable_data_connection_timeout=%u\n", tunable_data_connection_timeout);
  printf ("tunable_local_umask=0%o\n", tunable_local_umask);
  printf ("tunable_upload_max_rate=%u\n", tunable_upload_max_rate);
  printf ("tunable_download_max_rate=%u\n", tunable_download_max_rate);

  if (tunable_listen_address == NULL)
    printf ("tunable_listen_address=NULL\n");
  else
    printf ("tunable_listen_address=%s\n", tunable_listen_address);

  if (getuid () != 0)
    {
      fprintf (stderr, "miniftpd: must be started as root\n");
      exit (EXIT_FAILURE);
    }
  session_t sess = {
      /* �������� */
      0, -1, "", "", "",
      /* �������� */
      NULL, -1, -1, 0,
      /* ���� */
      0, 0, 0, 0,
      /* ���ӽ���ͨ�� */
      -1, -1,
      /* FTPЭ��״̬ */
      0, 0, NULL, 0,
      /* ���������� */
      0, 0
  };

  p_sess = &sess;

  sess.bw_upload_rate_max = tunable_upload_max_rate;
  sess.bw_download_rate_max = tunable_download_max_rate;

  s_ip_count_hash = hash_alloc (256, hash_func);
  s_pid_ip_hash = hash_alloc (256, hash_func);

  signal (SIGCHLD, handle_sigchld);
  int listenfd = tcp_server (tunable_listen_address, tunable_listen_port);
  int conn;
  pid_t pid;
  struct sockaddr_in addr;

  while (1)
    {
      conn = accept_timeout (listenfd, &addr, 0);
      if (conn == -1)
        ERR_EXIT("accept_tinmeout");

      unsigned int ip = addr.sin_addr.s_addr;

      ++s_children;
      sess.num_clients = s_children;
      sess.num_this_ip = handle_ip_count (&ip);

      pid = fork ();
      if (pid == -1)
        {
          --s_children;
          ERR_EXIT("fork");
        }

      if (pid == 0)
        {
          close (listenfd);
          sess.ctrl_fd = conn;
          check_limits (&sess);
          signal (SIGCHLD, SIG_IGN);
          begin_session (&sess);
        }
      else
        {
          hash_add_entry (s_pid_ip_hash, &pid, sizeof (pid),
                          &ip, sizeof (unsigned int));

          close (conn);
        }
    }
  //return 0;
}

void check_limits (session_t *sess)
{
  if (tunable_max_clients > 0 && sess->num_clients > tunable_max_clients)
    {
      ftp_relply (sess, FTP_TOO_MANY_USERS,
                  "There are too many connected users, please try later.");

      exit (EXIT_FAILURE);
    }

  if (tunable_max_per_ip > 0 && sess->num_this_ip > tunable_max_per_ip)
    {
      ftp_relply (sess, FTP_IP_LIMIT,
                  "There are too many connections from your internet address.");

      exit (EXIT_FAILURE);
    }
}

void handle_sigchld (int sig)
{
  // ��һ���ͻ����˳���ʱ����ô�ÿͻ��˶�Ӧip��������Ҫ��1��
  // ��������������ģ������ǿͻ����˳���ʱ��
  // ��������Ҫ֪������ͻ��˵�ip�������ͨ����s_pid_ip_hash���ҵõ���


  pid_t pid;
  while ((pid = waitpid (-1, NULL, WNOHANG)) > 0)
    {
      --s_children;
      unsigned int *ip = hash_lookup_entry (s_pid_ip_hash, &pid, sizeof (pid));
      if (ip == NULL)
        {
          continue;
        }

      drop_ip_count (ip);
      hash_free_entry (s_pid_ip_hash, &pid, sizeof (pid));
    }

}

unsigned int hash_func (unsigned int buckets, void *key)
{
  unsigned int *number = (unsigned int *) key;

  return (*number) % buckets;
}

unsigned int handle_ip_count (void *ip)
{
  // ��һ���ͻ���¼��ʱ��Ҫ��s_ip_count_hash����������еĶ�Ӧ����,
  // ����ip��Ӧ��������Ҫ��1����������������ڣ�Ҫ�ڱ������һ����¼��
  // ���ҽ�ip��Ӧ����������1��

  unsigned int count;
  unsigned int *p_count = (unsigned int *) hash_lookup_entry (s_ip_count_hash,
                                                              ip, sizeof (unsigned int));
  if (p_count == NULL)
    {
      count = 1;
      hash_add_entry (s_ip_count_hash, ip, sizeof (unsigned int),
                      &count, sizeof (unsigned int));
    }
  else
    {
      count = *p_count;
      ++count;
      *p_count = count;
    }

  return count;
}

void drop_ip_count (void *ip)
{
  // �õ���ip�������ǾͿ�����s_ip_count_hash�����ҵ���Ӧ�����������������м�1������

  unsigned int count;
  unsigned int *p_count = (unsigned int *) hash_lookup_entry (s_ip_count_hash,
                                                              ip, sizeof (unsigned int));
  if (p_count == NULL)
    {
      return;
    }

  count = *p_count;
  if (count <= 0)
    {
      return;
    }
  --count;
  *p_count = count;

  if (count == 0)
    {
      hash_free_entry (s_ip_count_hash, ip, sizeof (unsigned int));
    }
}