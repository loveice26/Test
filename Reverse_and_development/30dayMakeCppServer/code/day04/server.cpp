// 包含标准输入输出库，提供基本的输入输出功能，如printf、scanf等
#include <stdio.h>
// 包含字符串处理函数库，提供字符串操作功能，如strcpy、strlen等
#include <string.h>
// 包含Unix标准库，提供系统调用接口，如read、write、close等
#include <unistd.h>
// 包含文件控制库，提供文件描述符控制功能，如fcntl、open等
#include <fcntl.h>
// 包含错误号库，提供系统错误号定义，如errno、perror等
#include <errno.h>
// 包含标准模板库中的向量容器，用于存储动态数组
#include <vector>
// 包含自定义的工具类头文件，提供一些常用的工具函数
#include "util.h"
// 包含自定义的Epoll类头文件，用于实现I/O多路复用
#include "Epoll.h"
// 包含自定义的InetAddress类头文件，用于处理网络地址
#include "InetAddress.h"
// 包含自定义的Socket类头文件，用于处理网络套接字
#include "Socket.h"

/**
 * 定义最大事件数量，用于限制epoll一次返回的事件数量
 */
#define MAX_EVENTS 1024
/**
 * 定义读取缓冲区大小，用于存储从客户端读取的数据
 */
#define READ_BUFFER 1024

/**
 * 将指定文件描述符设置为非阻塞模式
 * @param fd 需要设置为非阻塞模式的文件描述符
 */
void setnonblocking(int fd) {
    // 通过fcntl函数设置文件描述符的状态标志，添加O_NONBLOCK标志使其变为非阻塞模式
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

/**
 * 处理可读事件的函数声明
 * @param sockfd 发生可读事件的套接字文件描述符
 */
void handleReadEvent(int sockfd);

/**
 * 主函数，程序入口点
 * @return 程序退出状态码
 */
int main() {
    // 创建一个服务器套接字对象
    Socket *serv_sock = new Socket();
    // 创建一个服务器地址对象，指定服务器监听的IP地址和端口号
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    // 将服务器套接字绑定到指定的地址和端口
    serv_sock->bind(serv_addr);
    // 开始监听客户端连接请求
    serv_sock->listen();
    // 创建一个Epoll对象，用于实现I/O多路复用
    Epoll *ep = new Epoll();
    // 将服务器套接字设置为非阻塞模式
    serv_sock->setnonblocking();
    // 将服务器套接字的文件描述符添加到Epoll实例中，监听读事件，并使用边缘触发模式
    ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);

    while (true) {
        // 调用Epoll的poll方法，等待事件发生，返回一个包含所有发生事件的向量
        std::vector<epoll_event> events = ep->poll();
        // 获取发生事件的数量
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i) {
            // 如果发生事件的文件描述符是服务器套接字的文件描述符，说明有新的客户端连接
            if (events[i].data.fd == serv_sock->getFd()) {
                // 创建一个新的客户端地址对象，用于存储客户端的地址信息
                InetAddress *clnt_addr = new InetAddress();
                // 接受客户端连接，创建一个新的客户端套接字对象
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                // 打印新客户端的文件描述符、IP地址和端口号
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                // 将客户端套接字设置为非阻塞模式
                clnt_sock->setnonblocking();
                // 将客户端套接字的文件描述符添加到Epoll实例中，监听读事件，并使用边缘触发模式
                ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            } else if (events[i].events & EPOLLIN) {
                // 如果发生的是可读事件，调用handleReadEvent函数处理
                handleReadEvent(events[i].data.fd);
            } else {
                // 处理其他类型的事件，这里只是简单打印信息，后续版本可完善
                printf("something else happened\n");
            }
        }
    }
    // 释放服务器套接字对象的内存
    delete serv_sock;
    // 释放服务器地址对象的内存
    delete serv_addr;
    // 返回程序退出状态码
    return 0;
}

/**
 * 处理可读事件的函数实现
 * @param sockfd 发生可读事件的套接字文件描述符
 */
void handleReadEvent(int sockfd) {
    // 定义一个缓冲区，用于存储从客户端读取的数据
    char buf[READ_BUFFER];
    while (true) {
        // 将缓冲区清零，确保每次读取的数据不会受到之前数据的影响
        bzero(&buf, sizeof(buf));
        // 从套接字读取数据到缓冲区
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            // 如果读取到数据，打印客户端发送的消息
            printf("message from client fd %d: %s\n", sockfd, buf);
            // 将读取到的数据原样写回客户端
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            // 如果读取过程中发生中断错误，继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            // 如果读取返回-1且错误码为EAGAIN或EWOULDBLOCK，表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) {
            // 如果读取返回0，表示客户端关闭了连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // 关闭套接字，会自动将文件描述符从epoll树上移除
            close(sockfd);
            break;
        }
    }
}
