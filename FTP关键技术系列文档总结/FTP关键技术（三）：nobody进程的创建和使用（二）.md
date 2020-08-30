### FTP关键技术（三）：nobody进程的创建和使用（二）

**如何给予nobody权限**

先看看在系统中ftp服务器是如何工作的吧

```c
[root@VM_0_11_redhat ~]# ps -ef | grep miniftp
root      6362     1  0 May13 ?        00:00:00 ./miniftpd
nobody   32305  6362  0 18:23 ?        00:00:00 ./miniftpd
root     32306 32305  0 18:23 ?        00:00:00 ./miniftpd
```

在这里，miniftpd的客户端是以root用户链接的，这显然不太合适，但是这不是重点，我们可以看到负责链接unix内核和用户环境的nobody进程，居然是以noobody权限启动的，这个权限显然不足以绑定固定端口20，所以我们需要对其提升权限。

首先我们将进程的用户更改成nobody

```c
if (setegid(pw->pw_gid) < 0)
    ERR_EXIT("setegid");
if (seteuid(pw->pw_uid) < 0)
    ERR_EXIT("seteuid");
```

在capablity.h文件中定义了以下的结构体

```c
typedef struct __user_cap_header_struct {
        __u32 version;
        int pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
        __u32 effective;
        __u32 permitted;
        __u32 inheritable;
} *cap_user_data_t;
```

对于version我们可以看到如下描述

```
Kernels prior to 2.6.25 prefer 32-bit capabilities with version
       _LINUX_CAPABILITY_VERSION_1.  Linux 2.6.25 added 64-bit capability
       sets, with version _LINUX_CAPABILITY_VERSION_2.  
```

所以对于version，由于我们电脑是64位的操作系统，所以用_LINUX_CAPABILITY_VERSION_2。

在man capabilities中我们找到我们需要绑定的权限

```
CAP_NET_BIND_SERVICE
              Bind a socket to Internet domain privileged ports (port numbers less than 1024).
```

- cap_effective:当一个进程要进行某个特权操作时,操作系统会检查cap_effective的对应位是否有效,而不再是检查进程的有效UID是否为0.

  例如,如果一个进程要设置系统的时钟,Linux的内核就会检查cap_effective的CAP_SYS_TIME位(第25位)是否有效.

- cap_permitted:表示进程能够使用的能力,在cap_permitted中可以包含cap_effective中没有的能力，这些能力是被进程自己临时放弃的,也可以说cap_effective是cap_permitted的一个子集.

- cap_inheritable:表示能够被当前进程执行的程序继承的能力.

所以我们就可以这样初始化权限

```c
    struct __user_cap_header_struct head;
    struct __user_cap_data_struct data;

    memset(&head, 0, sizeof(head));
    memset(&data, 0, sizeof(data));
    head.version = _LINUX_CAPABILITY_VERSION_2;	 // **
    head.pid = 0;
    __u32 mask = 0;
    mask |= (1 << CAP_NET_BIND_SERVICE);	// **
    data.effective = data.permitted = mask;
    data.inheritable = 0;
```

然后由于capset输入系统调用操作，所以我们需要用Syscall让内核来进行间接的函数调用。

```c
long syscall(long number, ...);
```

在asm/unistd.h文件中定义了一系列的宏，定义调用的具体内容

```c
#define __NR_capget 125
#define __NR_capset 126
```

然后我们就可以自己写一个capset函数来实现我们想要的功能

```c
int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
    return syscall(__NR_capset, hdrp, datap);
}
```

**nobody进程负责的任务以及实现**

在被动模式的sockfd获取函数中我们有以下步骤

- nobody进程接收PRIV_SOCK_GET_DATA_SOCK命令
- 进一步接收一个整数，也就是port = 20；
- 接收一个字符串，也就是ip；
- 调用系统函数绑定20端口；
- 回复用户进程ok
- 发送fd

```c
static void privop_pasv_get_data_sock(session_t *sess)
{
    unsigned int port = (unsigned int)priv_sock_get_int(sess->parent_fd);
    char ip[16] = {0};
    priv_sock_recv_buf(sess->parent_fd, ip, sizeof(ip));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    int fd = tcp_client(20);
    if (fd == -1)
    {
        priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
        return;
    }

    if (connect_timeout(fd, &addr, tunable_connect_timeout) < 0)
    {
        close(fd);
        priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
        return;
    }

    priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_OK);
    priv_sock_send_fd(sess->parent_fd, fd);
    close(fd);
}
```

获得主动模式的监听套接字

- nobody进程接收PRIV_SOCK_PASV_LISTEN命令
- 创建任意一个端口的套接字
- 将端口发送给客户端

```c
//创建一个监听套接字
static void privop_pasv_listen(session_t *sess)
{
    char ip[16] = {0};
    getlocalip(ip);

    sess->pasv_listen_fd = tcp_server(ip, 0);

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sess->pasv_listen_fd, (struct sockaddr*)&addr,&addrlen) < 0)
    {
        ERR_EXIT("getsockname");
    }
    unsigned short port = ntohs(addr.sin_port);
    priv_sock_send_int(sess->parent_fd, (int)port);
}

```

获得主动模式的客户端连接的fd

- nobody进程接收PRIV_SOCK_PASV_ACCEPT命令
- 关闭nobody进程的监听套接字
- 发送用户进程ok
- 发送用户进程客户端链接的fd

```c
//获取连接
static void privop_pasv_accept(session_t *sess)
{
    int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
    //得到一个已连接套接字
    close(sess->pasv_listen_fd);
    sess->pasv_listen_fd = -1;

    if (fd == -1)
    {
        priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
        return;
    }
    priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_OK);
    priv_sock_send_fd(sess->parent_fd, fd);
    close(fd);
}
```

实际上，所有的连接动作，都是ftp服务进程请求nobody进程去做的，然后nobody会将就绪的文件描述符回传给ftp服务进程。