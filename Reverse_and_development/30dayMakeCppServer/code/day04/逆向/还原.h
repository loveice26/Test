#pragma once

struct InetAddress{
    private:
        struct sockaddr_in client_addr;
        int client_addr_size;

    public:
        void __fastcall InetAddress::InetAddress(const char *a2, short a3)
        {
            bzero(&this->client_addr, sizeof(sockaddr_in));
            this->client_addr.sin_family = AF_INET;
            this->client_addr.sin_addr = inet_addr(a2);
            this->client_addr.sin_port = htons(a3);

            this->client_addr_size = sizeof(sockaddr_in);
        }
};

struct Socket{
    private:
        int fd = 0;
    public:
        void __fastcall Socket::Socket()
        {
            this->fd = -1;
            this->fd = socket(2, 1, 0);
            errif(this->fd == -1, "socket create error");
        }

        void __fastcall Socket::bind(InetAddress *a2)
        {
            errif( bind(this->fd, a2->client_addr, a2->client_addr_size) == -1, "socket bind error");
        }

        void __fastcall Socket::listen()
        {
            errif(listen(this->fd, 4096) == -1, "socket listen error");
        }

        int __fastcall Socket::setnonblocking()
        {
            return fcntl(this->fd, 4, fcntl(this->fd, 3) | 8);
        }

        int __fastcall Socket::getFd()
        {
            return this->fd;
        }

        int __fastcall Socket::accept(InetAddress *a2)
        {
          int v3; // [rsp+1Ch] [rbp-4h]

          v3 = accept(this->fd, a2->client_addr, a2->client_addr_size);
          errif(v3 == -1, "socket accept error");
          return v3;
        }
};

class Epoll{
    private:
        int epfd;
        struct epoll_event *events;
    public:
        void addFd(int fd, int op)
        {
            struct epoll_event event;
            bezro(event, sizeof(event));
            event.data.fd = fd;
            event.events = op;
            int v3 = epoll_ctl(this->epfd, 1, op, &event);
            errif(v3 == -1, "epoll add event error");
        }

        // 不会还原回头问一下
        // Epoll *__fastcall Epoll::poll(Epoll *this, __int64 a2, int a3)
        // {
        // int i; // [rsp+20h] [rbp-20h]
        // int v6; // [rsp+24h] [rbp-1Ch]

        // std::vector<epoll_event>::vector((__int64)this);
        // v6 = epoll_wait(*(_DWORD *)a2, *(struct epoll_event **)(a2 + 8), 1000, a3);
        // errif(v6 == -1, "epoll wait error");
        // for ( i = 0; i < v6; ++i )
        //     std::vector<epoll_event>::push_back((__int64)this, *(_QWORD *)(a2 + 8) + 12LL * i);
        // return this;
        // }
        std::vector<epoll_event> poll(int timeout){
            std::vector<epoll_event> activeEvents;
            int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
            errif(nfds == -1, "epoll wait error");
            for(int i = 0; i < nfds; ++i){
                activeEvents.push_back(events[i]);
            }
            return activeEvents;
        }
}