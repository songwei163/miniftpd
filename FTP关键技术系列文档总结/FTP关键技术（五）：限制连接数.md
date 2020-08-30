### FTP关键技术（五）：限制连接数


通常在一些网站中，为了防止恶意大量的访问和超大量访问导致内存占满，会对单个连接数和总连接数做出一个限制。

以本FTP服务端为例，假设每个客户连接，我们都需要两个进程来处理它，假设了一个客户需要分配总共1M的栈内存出来，1000个连接，接近1G的内存就没有了。另一方面，如果单个ip大量连接服务端，会占用大量的带宽、内存和文件句柄，实际上每个用户（ip)只需要两三个链接就可以解决问题，所以对单个ip连接数进行限制，有助于维持服务端的性能稳定和防止恶意访问。

 在系统自带vsftpd中有如下的配置文件

```c
max_clients=300 最大客户端连接数为300
max_per_ip=10 每个IP最大连接数
```

现在我们知道，我们需要一个数据结构来存储ip和ip对应的连接数，所以我们首先想到的就是用键值对模型，即每个ip对应一个链接数，并存储起来，并且我们需要能够快速插入、删除和查找，适合的就是树和hash表。但是连接数真的能解决问题吗？

首先，何时连接，我们何时增加一个连接数这个毋容置疑，但是，我们要如何知道这个连接结束了呢？我们可以在进程结束的时候，获得进程结束的信号，从而感知到一个进程的结束，所以这个时候，我们就不能单单依靠连接数，而是依靠进程的pid来对应ip。

所以我们需要两个hash表，一个hash表是pid to ip，另一个表是ip to conn。

至于hash_table我们可以自己定制写一个，也可以用stl库中的，但是还是自己写一个吧，我们只需要用到hash_table的部分功能。

下面的函数只是对主要的成员函数进行注释，方便理解(这里并没用泛型，而是借鉴了redis的实现方式用void *实现的hashtable)。

```c
//寻找并返回key所对应的value, 如value为空，则返回空。
void* hash_lookup_entry(void* key, unsigned int key_size);
//新增一个key-value
void hash_add_entry(void *key, unsigned int key_size,
                        void *value, unsigned int value_size);
//删除一个key-value
void hash_free_entry(void *key, unsigned int key_size);
```

 下面我们需要两个表

```c
static hash* s_ip_conn_hash;
static hash* s_pid_ip_hash;
```

 在接受连接成功后，我们可以获得一个unsigned int类型的ip，我们可以根据这个ip,找到ip所对应的p_count,再对p_count进行操作就完成了ip-conn的创建和增长。

```c
unsigned int ip = addr.sin_addr.s_addr;
sess.num_this_ip = handle_ip_count(&ip);
......
unsigned int handle_ip_count(void *ip)
{
    unsigned int count;
    unsigned int *p_count = (unsigned int*)s_ip_conn_hash->hash_lookup_entry(ip, sizeof(unsigned int));
    if (p_count == NULL)
    {
        count = 1;
        //不存在即创建
        s_ip_conn_hash->hash_add_entry(ip, sizeof(unsigned int), &count, sizeof(unsigned int));
    }
    else
    {
        //存在便增1，不过没有考虑到原子操作，失误失误
        count = *p_count;
        ++count;
        *p_count = count;
    }
    return count;
}
```

为了减少主进程的工作，我们将检测链接过限制放到子进程中。

```C
if (pid == 0)    //子进程
{
    ...
        check_limits(&sess);
    ...
}
void check_limits(session_t *sess)
{
    if (tunable_max_clients > 0 && sess->num_clients > tunable_max_clients)
    {
        ftp_reply(sess, FTP_TOO_MANY_USERS, "There are too many connection, please try later");
        exit(EXIT_FAILURE);
    }
    if (tunable_max_per_ip > 0 && sess->num_this_ip > tunable_max_per_ip)
    {
        ftp_reply(sess, FTP_IP_LIMIT, "There are too many connection,from internet address");
        exit(EXIT_FAILURE);
    }
}
```

 在连接数的删除上，我们需要明白一个流程。

 1.创建/增加 ip-value

 2.在父进程中创建pid-ip键值对

 3.在父进程中检测到子进程的退出

 4.查找pid对应的ip，删除ip对于的两个键值对。

 那如何实现检测呢？我们可以设置一个信号，当检测到SIGCHLD时候，执行操作四。

```c
signal(SIGCHLD, handle_sighid);
                //接受连接后
if (pid == 0)    //子进程
{
    //防止子进程的子进程退出的干扰
    signal(SIGCHLD, SIG_IGN);
}
//检测到子进程退出后执行
void handle_sighid(int sig)
{
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        --s_children;
        unsigned int*ip = (unsigned int *)s_pid_ip_hash->hash_lookup_entry(&pid, sizeof(pid));  //查找pid对于的ip
        if (ip == NULL)
        {
            continue;
        }
        drop_ip_count(ip);    //对ip-value减一或者删除操作
        s_pid_ip_hash->hash_free_entry(&pid, sizeof(pid));
    }
}

void drop_ip_count(void *ip)
{
    unsigned int count;
    unsigned int *p_count = (unsigned  int *)s_ip_conn_hash->hash_lookup_entry(ip, sizeof(unsigned int));
    if (p_count == NULL)
    {
        return;
    }

    count = *p_count;
    if (count <= 0)
    {
        s_ip_conn_hash->hash_free_entry(ip, sizeof(unsigned int));
        return;
    }
    --count;
    *p_count = count;
    if (count == 0)
    {
        s_ip_conn_hash->hash_free_entry(ip, sizeof(unsigned int));
    }
}
```

