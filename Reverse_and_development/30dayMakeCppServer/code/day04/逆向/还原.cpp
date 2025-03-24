#include "还原.h"

void __fastcall handleReadEvent(int a1)
{
    char buf[1024];
    int readflag = 0;

    while(1)
    {
        memset(buf, 0, 1024);
        readflag = read(a1, buf, 0x400);

        if(readflag)
        {
            printf("message from client fd %d: %s\n", a1, buf);
            write(a1, buf, 0x400);
        }
        else if(readflag == -1 && *__errno_location() == 4)
        {
            printf("continue reading");
            continue;
        }
        else if(v2 == -1 && (*__errno_location() == 11 || *__errno_location() == 11))
        {
            printf("finish reading once, errno: %d\n", *__errno_location());
            return;
        }
        else if(readflag == 0)
        {
            printf("EOF, client fd %d disconnected\n", a1);
            close(a1);
            return;
        }
    }
}


int __fastcall __noreturn main(int argc, const char **argv, const char **envp)
{

    Socket* serv_sock = new Socket();
    InetAddress* v4 = new InetAddress("127.0.0.1", 8888);

    serv_sock->bind(v4);
    serv_sock->listen();

    Epoll* v5 = new  Epoll();

    serv_sock->setnonblocking();

    int Fd = serv_sock->getFd();

    v5->addFd(Fd, 0x80000001);

    while(1)
    {
        std::vector<epoll_event> events = v5->poll(-1);
        int v16 = events.size();

        for (size_t i = 0; i < v16; i++)
        {
            if(events[i] == serv_sock->getFd())
            {
                InetAddress* v8 = new InetAddress();

                int v9 = serv_sock->accept(v8);
                Socket* v10 = new Socket(serv_sock->accept(v8));

                printf("new client fd %d! IP: %s Port: %d\n", v10->getFd(), inet_ntoa(v8->client_addr.sin_addr), ntohs(v8->client_addr.sin_port));

                v10->setnonblocking();
                v5->addFd(v10->getFd(), 0x80000001);
            }
            else if(events[i] != 0)
            {
                handleReadEvent(events[i].data.ptr);
            }
            else
            {
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete v4;
    return 0;
}