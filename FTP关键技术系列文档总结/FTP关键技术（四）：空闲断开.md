### FTP关键技术（四）：空闲断开

**我们为什么需要空闲断开？**

对于服务端而言，由于连接数和内存的限制，我们不可能对一个长时间处于不活跃的客户端，单独维护一个fd，一个进程/线程始终为其服务，fd单个进程上限默认值为1024，由于内存的限制，也不能无限制的分配出进程或者线程为其服务，这个时候我们就需要断开在规定时间内没有任何动作的客户端，腾出内存为其他客户端服务。

**要基于什么实现空闲断开呢？**

我们可以先设想需要一个可以在一定时间后能唤醒一个断开的服务的东西，而不是我们去维护一个进程，负责更新时间和断开（这样代价太大）。

然后就想到了信号，设置一个定时的信号。

等等，在设置一个定时信号的时候，我们是不是在该考虑一个问题，假设我们服务一个终端的ftp客户端，恰巧它开始下载一个非常大的文件，恰巧下载时间超过了服务端设置的空闲断开时间，这个时候就尴尬了，按照之前的想法，我们会空闲断开（注意：终端的ftp客户端下载时候，不可以发送其他命令，当然ctrl +c 的 abort除外）。

所以我们就需要先定义一个data_process，如果在进行数据传输的时候，就设置data_process为true,安装信号的时候，检测到data_process就不再安装限时信号。

**如何实现呢？**

首先，我们需要现在session中，安装data_process，session是每个进程维护一个包含配置文件和进程信息的一个全局节点。

```c
typedef struct session_t
{
    ...
    // 是否数据连接
    int data_process;
    ...
}session_t ;
```

然后我们就可以开始设置信号了，注意为我们需要为cmd模式和transfer data模式各自设置一个信号。

```c
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

首先看到这个函数，可能很多人都不明白这个函数是在做什么，大致可以理解它传入一个signum(待会见)和一个函数指针。所以我从以下三个方面说明这个函数：

- 参数是一个int
- 无返回值
- 作用是用来处理产生的信号(signum)

于是我们在关注这个signal函数，它是一个典型的回调函数，在我们设置一个信号的时候，我们会传入一个new handler，它会返回一个old handler，在出错的时候会返回一个SIG_ERR。

我们可以man 7 signal，获得详细的signum所对应的宏定义和相关说明。

```
 SIGALRM      14       Term    Timer signal from alarm(2)
```

这个用于设置一个alrm信号，用来捕捉alarm产生的信号。

```
If seconds is zero, any pending alarm is canceled.

In any event any previously set alarm() is canceled.
```

这个是man 文档中的原文，我觉得这个写的非常干练，就不在累赘翻译了。而且这个也是可以重复刷新空闲时间的原因！

所以下面我们就开始写函数的实现吧！

```c
void start_cmdio_alarm()
{
    if (tunable_idle_session_timeout > 0)
    {
        //安装信号
        signal(SIGALRM, handle_alarm_timeout);
        //启动闹钟
        alarm(tunable_idle_session_timeout);
    }
}
void start_data_alarm()
{
    if (tunable_data_connection_timeout > 0)
    {
        //安装信号
        signal(SIGALRM, handle_sigalrm);
        //启动闹钟
        alarm(tunable_data_connection_timeout);
    }
    else if(tunable_idle_session_timeout > 0)
    {
        alarm(0);
    }
}
```

在这里我们实现了两种信号，分别用于设置两种模式，固然也需要两种handler函数用于处理这个信号！

注意我们这里增加了一个tunable_data_connection_timeout，这个是用来控制数据传输连接超时的情况，我们不永远等待一个始终不进行的数据连接传输却发送数据链接请求的进程吧！！！

请再等等，这里我们就产生了第四个问题。

**我们要怎么样关闭连接才合适？**

先假设一种情况，我们在处理函数先发送一个进程连接被终止信号，然后直接close 掉连接的fd，这会发生一种什么情况，说一种很无聊的情况，假设一个客户端在收到被终止信号，立即发送一个命令，无论什么命令，这个时候会产生什么情况，信号又被重新设置了？？？到底会发生？？？可以看来这样并不安全。 这个时候就有一种较为安全的情况，我们可以先关闭读，然后给客户端发送终止信号，然后关闭写。

```c
void handle_alarm_timeout(int sig)
{
    shutdown(p_sess->ctrl_fd, SHUT_RD);
    ftp_reply(p_sess, FTP_IDLE_TIMEOUT, "Timeout");
    shutdown(p_sess->ctrl_fd, SHUT_WR);
    exit(EXIT_FAILURE);
}
```

然后我们要怎么处理data模式呢？

```c
void handle_sigalrm(int sig)
{
    if (!p_sess->data_process)
    {
        ftp_reply(p_sess, FTP_DATA_TIMEOUT, "Data timeout, Reconnect sorry");
        exit(EXIT_FAILURE);
    }
    //否则　当前处于数据传输的状态收到了超时信号
    p_sess->data_process = 1;
    start_data_alarm();
}
```

