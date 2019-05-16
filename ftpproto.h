//
// Created by s on 19-5-14.
//

#ifndef _FTPPROTO_H_
#define _FTPPROTO_H_

#include "session.h"
#include "sysutil.h"
#include "str.h"
#include "common.h"
#include "ftpcodes.h"

void handle_child (session_t *sess);
int list_common (void);


#endif //_FTPPROTO_H_
