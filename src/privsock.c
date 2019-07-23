//
// Created by s on 19-5-14.
//

#include "privsock.h"
#include "sysutil.h"
#include "common.h"

void priv_sock_init (session_t *sess)
{
  int sockfds[2];
  if (socketpair (AF_LOCAL, SOCK_STREAM, 0, sockfds) < 0)
    {
      ERR_EXIT("setsockpair");
    }
  sess->parent_fd = sockfds[0];
  sess->child_fd = sockfds[1];
}

void priv_sock_close (session_t *sess)
{
  if (sess->parent_fd != -1)
    {
      close (sess->parent_fd);
      sess->parent_fd = -1;
    }
  if (sess->child_fd != -1)
    {
      close (sess->child_fd);
      sess->child_fd = -1;
    }
}

void priv_sock_set_parent_context (session_t *sess)
{
  if (sess->child_fd != -1)
    {
      close (sess->child_fd);
      sess->child_fd = -1;
    }
}

void priv_sock_set_child_context (session_t *sess)
{
  if (sess->parent_fd != -1)
    {
      close (sess->parent_fd);
      sess->parent_fd = -1;
    }

}

// FTP service process -> nobody process
void priv_sock_send_cmd (int fd, char cmd)
{
  int ret;
  ret = writen (fd, &cmd, sizeof (cmd));
  if (ret != sizeof (cmd))
    {
      fprintf (stderr, "priv_sock_send_cmd error\n");;
      exit (EXIT_FAILURE);
    }
}

// nobody process get cmd
char priv_sock_get_cmd (int fd)
{
  char cmd;
  int ret = readn (fd, &cmd, sizeof (cmd));
  if (ret == 0)
    {
      // nobody process exit
      printf ("ftp process exit.\n");
      exit (EXIT_SUCCESS);
    }
  if (ret != sizeof (cmd))
    {
      fprintf (stderr, "priv_sock_get_cmd error\n");
      exit (EXIT_FAILURE);
    }
  return cmd;
}

// nobody process -> FTP service process
void priv_sock_send_result (int fd, char res)
{
  int ret;
  ret = writen (fd, &res, sizeof (res));
  if (ret != sizeof (res))
    {
      fprintf (stderr, "priv_sock_send_result error\n");
      exit (EXIT_FAILURE);
    }
}

// FTP service process get result
char priv_sock_get_result (int fd)
{
  char res;
  int ret;
  ret = readn (fd, &res, sizeof (res));
  if (ret != sizeof (res))
    {
      fprintf (stderr, "priv_sock_get_result error\n");
      exit (EXIT_FAILURE);
    }
  return res;
}

void priv_sock_send_int (int fd, int the_int)
{
  int ret;
  ret = writen (fd, &the_int, sizeof (the_int));
  if (ret != sizeof (the_int))
    {
      fprintf (stderr, "priv_sock_send_result error\n");
      exit (EXIT_FAILURE);
    }
}

int priv_sock_get_int (int fd)
{
  int res;
  int ret;
  ret = readn (fd, &res, sizeof (res));
  if (ret != sizeof (res))
    {
      fprintf (stderr, "priv_sock_get_result error\n");
      exit (EXIT_FAILURE);
    }
  return res;
}

void priv_sock_send_buf (int fd, const char *buf, unsigned int len)
{
  // send the str len
  priv_sock_send_int (fd, len);
  // send the str
  int ret = writen (fd, buf, len);
  if (ret != (int) len)
    {
      fprintf (stderr, "priv_sock_send_buf error\n");
      exit (EXIT_FAILURE);
    }
}

void priv_sock_recv_buf (int fd, char *buf, unsigned int len)
{
  unsigned int recv_len = (unsigned int) priv_sock_get_int (fd);
  if (recv_len > len)
    {
      fprintf (stderr, "priv_sock_recv_buf error\n");
      exit (EXIT_FAILURE);
    }

  int ret = readn (fd, buf, recv_len);
  if (ret != (int) recv_len)
    {
      fprintf (stderr, "priv_sock_recv_buf error\n");
      exit (EXIT_FAILURE);
    }
}

void priv_sock_send_fd (int sock_fd, int fd)
{
  send_fd (sock_fd, fd);
}

int priv_sock_recv_fd (int sock_fd)
{
  return recv_fd (sock_fd);
}
