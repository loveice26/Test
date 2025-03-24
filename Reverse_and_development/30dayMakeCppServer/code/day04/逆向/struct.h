struct InetAddress{
        struct sockaddr_in client_addr;
        int client_addr_size;

        InetAddress();
        InetAddress(const char *a2, short a3);
};

struct Socket{
        int fd;

        Socket();

        void __fastcall bind(InetAddress *a2);

        void __fastcall listen();

        int __fastcall setnonblocking();

        int __fastcall getFd();

        int __fastcall accept(InetAddress *a2);
};

struct Epoll
{
    int epfd;
    struct epoll_event *events;
    
    Epoll();

    void addFd(int fd, int op);

    // std::vector<epoll_event> poll(int timeout);
};
