//
// Created by s on 19-5-14.
//

#include "common.h"
#include "session.h"
#include "ftpproto.h"
#include "privparent.h"
#include "privsock.h"
//#include "sysutil.h"

void begin_session (session_t *sess)
{
  priv_sock_init (sess);

  pid_t pid;
  pid = fork ();

  if (pid < 0)
    {
      ERR_EXIT("fork");
    }
  else if (pid == 0)
    {
      priv_sock_set_child_context (sess);
      handle_child (sess);
    }
  else
    {
      priv_sock_set_parent_context (sess);
      handle_parent (sess);
    }
}