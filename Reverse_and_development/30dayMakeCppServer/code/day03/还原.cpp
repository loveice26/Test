int __fastcall __noreturn main(int argc, const char **argv, const char **envp)
{
  int v3; // eax
  int v4; // eax
  int v5; // ebx
  char *v6; // rax
  int *v7; // rax
  socklen_t addr_len; // [rsp+0h] [rbp-3470h] BYREF
  int i; // [rsp+4h] [rbp-346Ch]
  int fd; // [rsp+8h] [rbp-3468h]
  int epfd; // [rsp+Ch] [rbp-3464h]
  int v12; // [rsp+10h] [rbp-3460h]
  int v13; // [rsp+14h] [rbp-345Ch]
  ssize_t v14; // [rsp+18h] [rbp-3458h]
  epoll_event event; // [rsp+24h] [rbp-344Ch] BYREF
  sockaddr addr; // [rsp+30h] [rbp-3440h] BYREF
  struct sockaddr v17; // [rsp+40h] [rbp-3430h] BYREF
  struct epoll_event s[87]; // [rsp+50h] [rbp-3420h] BYREF
  char buf[1032]; // [rsp+3050h] [rbp-420h] BYREF
  unsigned __int64 v20; // [rsp+3458h] [rbp-18h]

  v20 = __readfsqword(0x28u);
  fd = socket(2, 1, 0);
  errif(fd == -1, "socket create error");

  addr.sa_family = 2;
  *(_QWORD *)&addr.sa_data[6] = 0LL;
  *(_DWORD *)&addr.sa_data[2] = inet_addr("127.0.0.1");
  *(_WORD *)addr.sa_data = htons(8888u);

  v3 = bind(fd, &addr, 16u);
  errif(v3 == -1, "socket bind error");

  v4 = listen(fd, 4096);
  errif(v4 == -1, "socket listen error");

  epfd = epoll_create1(0);
  errif(epfd == -1, "epoll create error");

  memset(s, 0, 0x3000uLL);
  event.data.u64 = (unsigned int)fd;
  event.events = 0x80000001;

  setnonblocking(fd);
  epoll_ctl(epfd, 1, fd, &event);

  while ( 1 )
  {
    v12 = epoll_wait(epfd, s, 1024, -1);
    errif(v12 == -1, "epoll wait error");

    for ( i = 0; i < v12; ++i )
    {
      if ( fd == s[i].data.fd )
      {
        *(_QWORD *)&v17.sa_family = 0LL;
        *(_QWORD *)&v17.sa_data[6] = 0LL;
        addr_len = 16;
        v13 = accept(fd, &v17, &addr_len);
        errif(v13 == -1, "socket accept error");

        v5 = ntohs(*(uint16_t *)v17.sa_data);
        v6 = inet_ntoa(*(struct in_addr *)&v17.sa_data[2]);
        printf("new client fd %d! IP: %s Port: %d\n", v13, v6, v5);

        event.data.u64 = (unsigned int)v13;
        event.events = 0x80000001;
        setnonblocking(v13);
        epoll_ctl(epfd, 1, v13, &event);
      }else if ( (s[i].events & 1) != 0 )
      {
        while ( 1 )
        {
          memset(buf, 0, 0x400uLL);
          v14 = read(s[i].data.fd, buf, 0x400uLL);
          if ( v14 > 0 )
          {
            printf("message from client fd %d: %s\n", s[i].data.fd, buf);
            write(s[i].data.fd, buf, 0x400uLL);
          }
          else if(v14 == -1 || *__errno_location() == 4)
          {
            printf("continue reading");
            continue;
          }
          else if(v14 == -1 && (*__errno_location() == 11 || *__errno_location() == 11))
          {
            v7 = __errno_location();
            printf("finish reading once, errno: %d\n", *v7);
            break;
          }else if(v14 == 0)
          {
            printf("EOF, client fd %d disconnected\n", s[i].data.fd);
            close(s[i].data.fd);
            break;
          }
        }


        printf("EOF, client fd %d disconnected\n", s[i].data.fd);
        close(s[i].data.fd);
      }
      else
      {
        puts("something else happened");
      }
LABEL_19:
      ;
    }
  }
}