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
            errif( bind(this->fd, (const struct sockaddr *)a2, *((_DWORD *)a2 + 4)) == -1, "socket bind error");
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