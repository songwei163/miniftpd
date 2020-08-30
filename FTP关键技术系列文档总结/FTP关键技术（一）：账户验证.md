### FTP关键技术（一）：账户验证

对于Linux服务端的FTP服务而言，账户为Linux主机的用户。

一般步骤是：

* 以root用户权限启动FTP服务
* 获取客户端的验证信息
* 从系统获取用户名对应的加密后的密码
* 与客户端发来的密码进行对应加密，并对比

**如何验证是否以root用户启动服务？**

我们可以通过getuid函数获取当前程序运行的uid，一般root用户的uid为0，所以可以通过下面的方式验证是否以root用户启动：

```c
if (getuid() != 0) {
    perror("miniftpd must be start as root.\n");
    exit(-1);
}
```

**从系统获取用户名对应的加密后的密码**

UNIX 系统口令文件定义包含在<pwd.h>中定义的passwd结构体中。

```c
struct passwd {
    char   *pw_name;       /* username */
    char   *pw_passwd;     /* user password */
    uid_t   pw_uid;        /* user ID */
    gid_t   pw_gid;        /* group ID */
    char   *pw_gecos;      /* user information */
    char   *pw_dir;        /* home directory */
    char   *pw_shell;      /* shell program */
};
```

在这里就不在赘述其他东西，主要关心uid和pw_passwd，分别是用户id和用户所对应的密码。于此同时在其中定义了两个函数需要我们去关注。

```c
//通过用户名获取passwd
struct passwd *getpwnam(const char *name);
//通过uid获取passwd
struct passwd *getpwuid(uid_t uid);
```

除此之外需要我们注意的是这些函数需要运行在root权限下，这就是为什么要验证是否root启动服务原因之一。另外如果输入的用户名或者uid是错误的话，返回的passwd是一个NULL，于是我们便可以这样设计ftp的验证。

```c
static void do_user(session_t *sess)
{
    //略去了接受和分割命令细节，这里sess->arg是用户名
    struct passwd *pw  = getpwnam(sess->arg);
    if (pw == NULL)
    {
        ftp_reply(sess, FTP_LOGINERR, "Login incorrect");
        //发送FTP_LOGINERR命令
        return;
    }
    sess->uid = pw->pw_uid;
    ftp_reply(sess, FTP_GIVEPWORD, "Please specify the password.");  
    //发送FTP_GIVEPWORD命令
}
```

在这里将username和passwd分开验证可以优化一下体验，至少知道是什么输错了，由于是分开设计的，所以我们需要保存一个用户信息，因为uid是最好操作的，所以我们在sess（用户信息）中存储了一个uid。

**对客户端发过来的密码进行对应的加密，并对比**

这里可能有读者会问，为什么要将客户端的密码进行加密对比，而不是将系统的密码解密对比呢？为了安全考虑，Linux的加密口令是经单向加密算法处理过的用户口令副本。因此此算法是单向的，所以不能从加密猜测到原来的口令。基于此linux设计了一个叫阴影口令的文件。该文件至少包含用户名和加密口令，与该口令相关的其他信息也存放其中。

在<shadow.h>文件中定义了。

```c
struct spwd {
    char *sp_namp;     /* Login name */
    char *sp_pwdp;     /* Encrypted password */
    long  sp_lstchg;   /* Date of last change
                                     (measured in days since
                                     1970-01-01 00:00:00 +0000 (UTC)) */
    long  sp_min;      /* Min # of days between changes */
    long  sp_max;      /* Max # of days between changes */
    long  sp_warn;     /* # of days before password expires
                                     to warn user to change it */
    long  sp_inact;    /* # of days after password expires
                                     until account is disabled */
    long  sp_expire;   /* Date when account expires
                                     (measured in days since
                                     1970-01-01 00:00:00 +0000 (UTC)) */
    unsigned long sp_flag;  /* Reserved */
};

```

在这个文件中，我们主要用到sp_pwdp这个字段，sp_pwdp是指加密口令，通过这个加密口令我们可以通过crypt函数对获取客户端发送的密码进行加密。

同样定义了函数

```c
struct spwd *getspnam(const char *name);
```

当我们获得了spwd的后，我们还需要对客户端发送的pwsswd进行加密

在<unistd.h>中定义了一个函数

```c
char *crypt(const char *key, const char *salt);
/*
key：要加密的明文。

salt：密钥。

salt 默认使用DES加密方法。DES加密时，salt只能取两个字符，多出的字符会被丢弃。

需要注意的是这个函数在编译的时候需要链接lcrypt。
*/
```

于是我们便可以这样写do_pass函数来验证客户端的密码

```c
static void do_pass(session_t *sess)
{
    struct passwd *pw = getpwuid(sess->uid);   //获取pw_name
    if (pw == NULL)
    {
        ftp_reply(sess, FTP_LOGINERR, "Login incorrect");
        return;
    }
    struct spwd *sp = getspnam(pw->pw_name);  //获取sp_pwdp
    if (sp == NULL)
    {
        ftp_reply(sess, FTP_LOGINERR, "Login incorrect");
        return;
    }
    char *encrypted_pw = crypt(sess->arg, sp->sp_pwdp);   //加密
    if (strcmp(encrypted_pw, sp->sp_pwdp) != 0)    //比对加密后的密码和sp_pwdp
    {
        ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
        return;
    }
   
    setegid(pw->pw_gid);                      //更改gid
    seteuid(pw->pw_uid);                      //更改uid
    chdir(pw->pw_dir);                        //更改Dir
    ftp_reply(sess, FTP_LOGINOK, "Login success.");
}
```

通过客户端传来的用户名，验证是否存在，拿到uid，并由session持有，别的接口使用时，通过session中保存的uid可以拿到struct passwd结构，然后可以拿到struct spwd结构，从而获取加密口令，并对客户端传来的密码进行加密，比对加密口令，验证是否登录成功。在这里需要强调一下，系统中已经存在的用户密码单向加密的结果的前几位可以作为对将要进行单向加密并比对的明文密码的salt。