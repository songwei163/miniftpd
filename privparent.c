//
// Created by s on 19-5-14.
//

#include "privparent.h"
//#include "privsock.h"
#include "sysutil.h"
#include "tunable.h"

void handle_parent (session_t *sess)
{
  char cmd;
  while(1)
    {
      readn (sess->parent_fd, &cmd,1);
      //解析内部命令
      //处理内部命令
    }
}
