int __fastcall __noreturn main(int argc, const char **argv, const char **envp)
{
  Socket *v3; // rbx
  InetAddress *v4; // rbx
  Epoll *v5; // rbx
  int Fd; // ecx
  int v7; // ebx
  InetAddress *v8; // rbx
  int v9; // r12d
  Socket *v10; // rbx
  char *v11; // r12
  int v12; // eax
  int v13; // ecx
  __int64 v14; // rax
  int i; // [rsp+0h] [rbp-60h]
  int v16; // [rsp+4h] [rbp-5Ch]
  Socket *v17; // [rsp+8h] [rbp-58h]
  Epoll *v18; // [rsp+18h] [rbp-48h]
  InetAddress *v19; // [rsp+20h] [rbp-40h]
  Socket *v20; // [rsp+28h] [rbp-38h]
  _BYTE v21[24]; // [rsp+30h] [rbp-30h] BYREF
  unsigned __int64 v22; // [rsp+48h] [rbp-18h]

  v22 = __readfsqword(0x28u);
  v3 = (Socket *)operator new(4uLL);
  Socket::Socket(v3);
  v17 = v3;
  v4 = (InetAddress *)operator new(0x14uLL);
  InetAddress::InetAddress(v4, "127.0.0.1", 0x22B8u);
  Socket::bind(v17, v4);
  Socket::listen(v17);
  v5 = (Epoll *)operator new(0x10uLL);
  Epoll::Epoll(v5);
  v18 = v5;
  Socket::setnonblocking(v17);
  Fd = Socket::getFd(v17);
  Epoll::addFd(v5, Fd, 0x80000001);
  while ( 1 )
  {
    Epoll::poll((Epoll *)v21, (int)v18);
    v16 = std::vector<epoll_event>::size(v21);
    for ( i = 0; i < v16; ++i )
    {
      v7 = *(_DWORD *)(std::vector<epoll_event>::operator[](v21, i) + 4);
      if ( v7 == (unsigned int)Socket::getFd(v17) )
      {
        v8 = (InetAddress *)operator new(0x14uLL);
        InetAddress::InetAddress(v8);
        v19 = v8;
        v9 = Socket::accept(v17, v8);
        v10 = (Socket *)operator new(4uLL);
        Socket::Socket(v10, v9);
        v20 = v10;
        LODWORD(v10) = ntohs(*((_WORD *)v19 + 1));
        v11 = inet_ntoa(*(struct in_addr *)((char *)v19 + 4));
        v12 = Socket::getFd(v20);
        printf("new client fd %d! IP: %s Port: %d\n", v12, v11, (_DWORD)v10);
        Socket::setnonblocking(v20);
        v13 = Socket::getFd(v20);
        Epoll::addFd(v18, v13, 0x80000001);
      }
      else if ( (*(_DWORD *)std::vector<epoll_event>::operator[](v21, i) & 1) != 0 )
      {
        v14 = std::vector<epoll_event>::operator[](v21, i);
        handleReadEvent(*(_DWORD *)(v14 + 4));
      }
      else
      {
        puts("something else happened");
      }
    }
    std::vector<epoll_event>::~vector(v21);
  }
}