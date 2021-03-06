# miniftpd

##### 项目简介

​	Vsftpd的简化版本，拥有部分Vsftpd功能和相同的Ftp协议，使用C语言编写，采用多进程模型。每当有一个新的连接到达，主进程就会派生出一个服务进程为客户服务。同时实现了用户的鉴权登录和配置文件解析。考虑到NAT网络，实现了Ftp服务器主动模式和被动模式以应对不同的网络环境。为了绑定20端口，使用nobody进程做了特权提升，协助ftp服务进程绑定数据端口。除了基本的文件上传和下载功能，还拥有断点续传、限制连接数、空闲断开、限速等功能。

##### 主要技术

socket、I/O复用、进程间通信、HashTable



下面就来具体说一说这个项目，

这个项目时在Linux环境下用C语言开发的，主要用到，Socket网络编程，进程间通信机制，哈希表



系统的主要架构采用的是多进程模型，每当有一个新的客户连接到达，主进程就会派生出一个ftp服务进程来为客户提供服务。同时每个ftp服务进程配套了nobody进程（内部私有进程），主要是为了做权限提升和控制。

* 为了应对不同的网络环境，实现了服务器的主动模式和被动模式，这两种模式都是相对服务器来谈的，主动模式主要解决服务器了处于NAT网络内部的问题，数据连接的建立由服务器主动发起连接。

  * 对于NAT网络来说，服务器外部可以设置端口转发，通过21端口与客户端建立控制连接，客户端告知数据连接端口，由服务器主动连接，在这种情况下客户端不可能发起连接主动建立数据传输，因为对于做转发的那台主机来说可能并没有开放对应的端口，理论上来说，入流量是困难的，出相对来说简单）

  

* 对于Linux端的ftp服务而言，客户就是Linux主机的用户

  * 以root用户权限启动ftp服务，获取客户端的验证信息，通过都对密码的加密然后与阴影文件做比对，这种加密算法是单向的，不可逆，只能通过加密再比对的方式来验证用户。

* 简单来说一下为什么要用到nobody进程吧

  ftp服务进程为了绑定20端口做数据传输，必须引入nobody进程来做特权提升，普通用户是没有权限绑定1024以内端口的。

* 进程间通信用的是本地Unix域套接字通信

* 如何赋予nobody进程权限
* 空闲断开、断点续传续载、限制连接数、限速

*** 主要谈一谈这个项目中空闲断开的实现吧

*  为什么需要空闲断开

  * 由于系统的资源有限，特别是文件描述符和内存的限制，都属于宝贵的资源，我们必须要在规定时间内断开没有任何动作的客户，来腾出资源为其他客户服务。我们需要为每个连接来安装定时信号，当然如果在进行数据传输的时候，就不能安装定时信号

* 限制客户端单个IP连接数

  * 如果单个IP大量连接服务器，会占用大量的贷款、内存和文件句柄，实际上每个用户（IP）只需要两三个连接就足够了，所以对单个IP连接数进行限制，有助于维持服务端的性能稳定和防止恶意访问。

  * 为了实现上述想法，需要维护两张表，连接时，增加连接数即可，但想知道这个连接什么时候结束，我们需要注册对应的SIGCHLD信号，从而感知一个进程的结束。一个是pid to ip，一个是ip to conn。在父进程中创建PID-IP键值对，在父进程中检测子进程的推出，查找对应的IP，减少IP对应的连接数。

    

    

* 为什么不用多线程而用多进程？？

  原因很简单，在多线程或I/O复用的情况下，当前目录时共享的，无法根据每一个连接来用于自己的当前目录，也就是当前用户目录的切换会影响其他用户。
  
#### 项目中遇到的关键部分，值得记录下来

##### 账户验证

对于Linux端的ftp服务而言，账户即为Linux端的用户。

1.以root用户权限启动ftp服务

2.获取客户端的验证信息

3.从系统获取用户名对应加密后的密码

4.对客户端发过来的密码进行对应的加密，并比对

* 如何验证是否以root用户启动服务

  一般root用户的uid为0。

  ```c
  if(getuid() != 0)
  {
      fprintf(stderr, "miniftp must start be as root!\n");
  }
  ```

* 获取系统中用户名对应的加密后的密码（这个操作也要求有root权限）

  ```C
  struct passwd *getpwnam(const char* name);
  struct passwd *getpwuid(uid_t uid);
  ```

* 对客户端发过来的密码进行对应的加密，并比对

  拿到阴影口令文件

  ```C
  struct spwd *getspnam(const char* name);
  ```

  当我们获得了spwd后，需要对客户端发来的passwd进行加密。

  ```C
  char* encrypted_pw = crypt(sess->arg, sp->sp_pwdp);
  strcmp(encrypted_pw, sp->sp_pwdp); 	//比对
  ```

  这里简单提一下，为了安全考虑，Linux的加密口令是经单向加密算法处理过的用户口令副本。因此该算法是单向的，从而不能从加密结果猜测到原来的口令。

