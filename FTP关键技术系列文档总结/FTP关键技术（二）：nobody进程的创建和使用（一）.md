### FTP关键技术（二）：nobody进程的创建和使用（一）

**为什么需要nobody进程**

* 在PORT模式下，服务器会主动建立数据通道连接客户端，服务器可能就没有权限做这种事情，就需要nobody进程来帮忙。 Nobody进程会通过unix域协议（本机通信效率高）  将套接字传递给服务进程。普通用户没有权限绑定20端口，需要nobody进程的协助，所以需要nobody进程作为控制进程。

* 事实上无论是PORT模式还是PASV模式，创建套接字还是后面对套接字的监听这些操作涉及到于内核的相关操作放在服务进程都是不安全。其实最近看到一个文章，文中指出以root启动在验证后转到用户进程也会不安全的。

**为什么使用多进程而不是多线程？**

原因是在多线程或IO复用的情况下，当前目录是共享的，无法根据每一个连接来拥有自己的当前目录，也就是说当前用户目录的切换会影响到其他的用户。

**进程间通信的协议制定**

首先采用Unix域的内部通信协议需要创建一个Unix的套接字进行通信

```c
void priv_sock_init(session_t *sess)
{
    int sockfds[2];
    if (socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds) < 0)
        ERR_EXIT("socketpair");
    sess->parent_fd = sockfds[0];
    sess->child_fd = sockfds[1];
}
void priv_sock_set_parent_context(session_t *sess)
{
    if (sess->child_fd != -1)
    {
        close(sess->child_fd);
        sess->child_fd = -1;
    }
}
void priv_sock_set_child_context(session_t *sess)
{
    if (sess->parent_fd != -1)
    {
        close(sess->parent_fd);
        sess->parent_fd = -1;
    }
}
```

sess作为两个进程共有的用户信息，在两个进程创建初期sess内部便被写入了Unix域套接字通信。（fork的时候会拷贝一份数据）

```c
void begin_session(session_t *sess)
{
    activate_oobinline(sess->ctrl_fd);
    priv_sock_init(sess);                           //写入套接字
    pid_t pid;
    pid = fork();
    if (pid < 0)
        ERR_EXIT("fork");

    if (pid == 0) 
    {
        priv_sock_set_child_context(sess);
        handle_child(sess);
    } 
    else 
    {
        priv_sock_set_parent_context(sess);
        handle_parent(sess);
    }
}
```

然后让我们看看内部协议制定包装了一系列函数

```c
void priv_sock_send_cmd(int fd, char cmd);
char priv_sock_get_cmd(int fd);
void priv_sock_send_result(int fd, char res);
char priv_sock_get_result(int fd);
void priv_sock_send_int(int fd, int the_int);
int priv_sock_get_int(int fd);
void priv_sock_send_buf(int fd, const char *buf, unsigned int len);
void priv_sock_recv_buf(int fd, char *buf, unsigned int len);
void priv_sock_send_fd(int sock_fd, int fd);
int priv_sock_recv_fd(int sock_fd);
```

我们可以看到主要有两个功能的函数，一是负责内部的命令的接受、实现和返回结果，二是负责传输数据。

首先看看第一部分是怎么实现的吧

```c
// FTP服务进程向nobody进程请求的命令
#define PRIV_SOCK_GET_DATA_SOCK     1
#define PRIV_SOCK_PASV_ACTIVE       2
#define PRIV_SOCK_PASV_LISTEN       3
#define PRIV_SOCK_PASV_ACCEPT       4

// nobody进程对FTP服务进程的应答
#define PRIV_SOCK_RESULT_OK         1
#define PRIV_SOCK_RESULT_BAD        2
```

这里提供部分实现

```c
void priv_sock_send_cmd(int fd, char cmd)
{
    int ret;
    ret = writen(fd, &cmd, sizeof(cmd));
    if (ret != sizeof(cmd))
    {
        fprintf(stderr, "priv_sock_send_cmd error\n");
        exit(EXIT_FAILURE);
    }
}
char priv_sock_get_cmd(int fd)
{
    char res;
    int ret;
    ret = readn(fd, &res, sizeof(res));
    if (ret == 0)
    {
        printf("ftp process exit\n");
        exit(EXIT_SUCCESS);
    }
    if (ret != sizeof(res))
    {
        fprintf(stderr, "priv_sock_get_cmd error\n");
        exit(EXIT_FAILURE);
    }
    return res;
}
```

这里提供了get_cmd和send_cmd的实现，可以看到只是简单包装下send和read函数

这里就不再赘述其他函数，对于这些函数，我们主要关注一组特殊函数

```c
void priv_sock_send_fd(int sock_fd, int fd)
{
    send_fd(sock_fd, fd);
}
int priv_sock_recv_fd(int sock_fd)
{
    return  recv_fd(sock_fd);
}
```

为什么这个比较特殊呢？因为这不是传输一个四个字节的整型，而是传输一个打开的文件描述符，我们想让发送进程和接受进程共享同一文件表项。在技术上，我们是将一个打开文件表项的指针从一个进程发送到另一个进程，该指针被分配到接受进程第一个可用的描述符中。发送结束后，发送进程通常会关闭该描述符。

 为了在UNIX域套接字交换文件描述符，我们需要关注以下系统函数

```c
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);          
struct msghdr 
{
      void         *msg_name;       /* optional address */
      socklen_t     msg_namelen;    /* size of address */
      struct iovec *msg_iov;        /* scatter/gather array */
      size_t        msg_iovlen;     /* # elements in msg_iov */
      void         *msg_control;    /* ancillary data, see below */
      size_t        msg_controllen; /* ancillary data buffer len */
      int           msg_flags;      /* flags on received message */
};
```

 前两个元素主要用于网络通信，msg_name存数据包的目的地址，网络包指向struct sockaddr_in,msg_namelen值地址长度，一般为16。一般在UNIX域设置为NULL， 0。

 接下来的两个元素我们可以指定一个或多个内存缓存区，第一个元素指向一个数据包缓存区的buff 。其中 iov_base指向数据包缓冲区，即参数buff，iov_len是buff的长度。msghdr中允许一次传递多buff，以数组的形式组织在 msg_iov中，msg_iovlen就记录**数组的长度**（即有多少个buff）。

```c
struct iovec {                    /* Scatter/gather array items */                    
    void  *iov_base;              /* Starting address */
    size_t iov_len;               /* Number of bytes to transfer */
};
```

最后两个元素，msg_flags字段包含了描述接收到的消息的标志，如带外数据MSG_OOB等。mgs_controllen字段指向cmsghdr结构，用于控制信息字节数

```c
struct cmsghdr {
    socklen_t cmsg_len;    /* data byte count, including header */
    int       cmsg_level;  /* originating protocol */
    int       cmsg_type;   /* protocol-specific type */
     /* followed by unsigned char cmsg_data[]; */
};
```

 为了发送文件描述符，需要将cmsg_len设置为cmsghdr结构的长度加一个文件描述符的长度，将cmg_level设计为SOL_SOCKET， cmsg_type字段设置为SCM_RIGHTS，用以表明传送访问权，描述符紧随cmsg_type字段之后储存，用CMSG_DATA宏获得该整型量的指针。

```c
//获得指向与msghadr结构关联的第一个cmsghdr结构
struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh);
//计算 cmsghdr 头结构加上附属数据大小，并包括对其字段和可能的结尾填充字符
size_t CMSG_SPACE(size_t length);
//计算 cmsghdr 头结构加上附属数据大小
size_t CMSG_LEN(size_t length);
//返回一个指针和cmsghdr结构关联的数据
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);
```

我们可以通过控制这些宏对这些进行cmsghdr初始化，具体实现看下面。

```c
/**
 * send_fd -向sock_fd 发送 fd
 * @sock_fd: 发送目标套接字
 * @fd: 发送套接字
 */
void send_fd(int sock_fd, int fd)
{
    int ret;
    struct msghdr msg;
    struct cmsghdr *p_cmsg;
    struct iovec vec;
    char cmsgbuf[CMSG_SPACE(sizeof(fd))];     //配置cmsgbuf的大小
    int *p_fds;
    char sendchar = 0;
    msg.msg_control = cmsgbuf;                
    msg.msg_controllen = sizeof(cmsgbuf);
    p_cmsg = CMSG_FIRSTHDR(&msg);            //通过宏获得struct cmsghdr指针
    p_cmsg->cmsg_level = SOL_SOCKET;         //指定是socket协议
    p_cmsg->cmsg_type = SCM_RIGHTS;          //套接字控制信息，仅UNIX域可以传递该信息
    p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd)); //用宏储存fd所需的对象长度，一般是整型+ cmsghdr长度
    p_fds = (int*)CMSG_DATA(p_cmsg);         //获得关联数据 即fd的指针
    *p_fds = fd;  
    msg.msg_name = NULL;                      //UNIX域 初始化为NULL
    msg.msg_namelen = 0;
    msg.msg_iov = &vec;                       //初始化缓冲区buff
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;     
    
    vec.iov_base = &sendchar;
    vec.iov_len = sizeof(sendchar);
    ret = sendmsg(sock_fd, &msg, 0);
    if (ret != 1)
        ERR_EXIT("sendmsg");
}
/**
 * send_fd -向sock_fd 发送 fd
 * @sock_fd: 接受目标套接字
 * 返回目标套接字
 */
int recv_fd(const int sock_fd)
{
    int ret;
    struct msghdr msg;
    char recvchar;
    struct iovec vec;
    int recv_fd;
    char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
    struct cmsghdr *p_cmsg;
    int *p_fd;
    vec.iov_base = &recvchar;
    vec.iov_len = sizeof(recvchar);
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    msg.msg_flags = 0;

    p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
    *p_fd = -1;  
    ret = recvmsg(sock_fd, &msg, 0);
    if (ret != 1)
        ERR_EXIT("recvmsg");

    p_cmsg = CMSG_FIRSTHDR(&msg);            //通过宏获得信息头
    if (p_cmsg == NULL)
        ERR_EXIT("no passed fd");


    p_fd = (int*)CMSG_DATA(p_cmsg);        //通过宏获得数据
    recv_fd = *p_fd;
    if (recv_fd == -1)
        ERR_EXIT("no passed fd");
    return recv_fd;
}
```

