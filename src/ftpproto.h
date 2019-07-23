//
// Created by s on 19-5-14.
//

#ifndef __FTPPROTO_H__
#define __FTPPROTO_H__

#include "session.h"

typedef struct ftpcmd {
    const char *cmd;
    void (*cmd_func) (session_t *sess);
} ftpcmd_t;

void handle_child (session_t *sess);
int list_common (session_t *sess, int detail);
void upload_common (session_t *sess, int is_append);

void ftp_relply (session_t *sess, int status, const char *text);
void ftp_lrelply (session_t *sess, int status, const char *text);

#endif /*__FTPPROTO_H__ */
