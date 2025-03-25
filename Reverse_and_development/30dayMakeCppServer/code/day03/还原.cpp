int __fastcall __noreturn main(int argc, const char **argv, const char **envp)
{
    int fd = socket(2, 1, 0);
    errif(fd == -1, "socket create error");

    struct sockaddr addr;
    addr.sa_family = 2;
    addr.sa_data[6] = 0LL;
    addr.sa_data[2] = inet_addr("127.0.0.1");
    addr.sa_data = htons(8888);

    errif(bind(fd, &addr, 0x10) == -1, "socket bind error");

    errif(listen(fd, 4096) == -1, "socket listen error");

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event s;
    bzero(&s, sizeof(s));

    struct epoll_event event;
    event.data.fd = (unsigned int)fd;
    event.events = 0x80000001;

    setnonblocking(fd);
    epoll_ctl(epfd, 1, fd, &event);

    while ( 1 )
    {
        int v12 = epoll_wait(epfd, s, 1024, -1);
        errif(v12 == -1, "epoll wait error");

        for ( i = 0; i < v12; ++i )
        {
            if ( fd == s[i].data.fd )
            {
              struct sockaddr v17;
              v17.sa_family = 0;
              v17.sa_data = 0;
              socklen_t addr_len = 16;
              int v13 = accept(fd, &v17, &addr_len);
              errif(v13 == -1, "socket accept error");

              printf("new client fd %d! IP: %s Port: %d\n", v13, inet_ntoa(*(struct in_addr *)&v17.sa_data[2]), ntohs(v17.sa_data));

              event.data.fd = v13;
              event.events = 0x80000001;
              setnonblocking(v13);
              epoll_ctl(epfd, 1, v13, &event);
            }
            else if((s[i].events & 1) != 0)
            {
                while(1)
                {
                    char buf[0x400];
                    memset(buf, 0, 0x400);
                    ssize_t v14 = read(s[i].data.fd, buf, 0x400);

                    if(v14 > 0)
                    {
                        printf("message from client fd %d: %s\n", s[i].data.fd, buf);
                        write(s[i].data.fd, buf, 0x400);
                    }
                    else if( v14 == -1 || *__errno_location() == 4)
                    {
                        printf("continue reading");
                        continue;
                    }
                    else if ( v14 == -1 && (*__errno_location() == 11 || *__errno_location() == 11) )
                    {
                        printf("finish reading once, errno: %d\n", *__errno_location());
                        break;
                    }
                    else if( v14 == 0 )
                    {
                        printf("EOF, client fd %d disconnected\n", s[i].data.fd);
                        close(s[i].data.fd);
                        break;
                    }
                }
            }
            else
            {
                puts("something else happened");
            }
        }
    }
}