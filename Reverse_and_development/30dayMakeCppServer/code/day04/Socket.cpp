// 包含Socket类的头文件
#include "Socket.h"
// 包含InetAddress类的头文件
#include "InetAddress.h"
// 包含自定义的工具函数头文件
#include "util.h"
// 包含Unix标准库头文件，提供了如close等函数
#include <unistd.h>
// 包含文件控制头文件，提供了如fcntl等函数
#include <fcntl.h>
// 包含套接字编程的系统头文件
#include <sys/socket.h>

/**
 * @brief 构造函数，创建一个新的套接字
 * 
 * 此构造函数通过调用socket函数创建一个新的TCP套接字，并检查是否创建成功。
 * 如果创建失败，会调用errif函数输出错误信息。
 */
Socket::Socket() : fd(-1) {
    // 创建一个IPv4的TCP套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字是否创建成功，如果失败则输出错误信息
    errif(fd == -1, "socket create error");
}

/**
 * @brief 构造函数，使用已有的文件描述符初始化套接字
 * 
 * 此构造函数使用传入的文件描述符初始化套接字，并检查文件描述符是否有效。
 * 如果文件描述符无效，会调用errif函数输出错误信息。
 * 
 * @param _fd 已有的文件描述符
 */
Socket::Socket(int _fd) : fd(_fd) {
    // 检查文件描述符是否有效，如果无效则输出错误信息
    errif(fd == -1, "socket create error");
}

/**
 * @brief 析构函数，关闭套接字
 * 
 * 此析构函数在对象销毁时关闭套接字，并将文件描述符置为-1。
 */
Socket::~Socket() {
    // 检查文件描述符是否有效，如果有效则关闭套接字
    if (fd != -1) {
        close(fd);
        // 将文件描述符置为-1，表示套接字已关闭
        fd = -1;
    }
}

/**
 * @brief 将套接字绑定到指定的地址
 * 
 * 此函数将套接字绑定到传入的InetAddress对象所表示的地址。
 * 如果绑定失败，会调用errif函数输出错误信息。
 * 
 * @param addr 指向InetAddress对象的指针，包含要绑定的地址信息
 */
void Socket::bind(InetAddress *addr) {
    // 调用系统的bind函数将套接字绑定到指定地址
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error");
}

/**
 * @brief 开始监听套接字
 * 
 * 此函数将套接字设置为监听状态，允许客户端连接。
 * 如果监听失败，会调用errif函数输出错误信息。
 */
void Socket::listen() {
    // 调用系统的listen函数开始监听套接字
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

/**
 * @brief 将套接字设置为非阻塞模式
 * 
 * 此函数使用fcntl函数将套接字设置为非阻塞模式。
 */
void Socket::setnonblocking() {
    // 使用fcntl函数将套接字设置为非阻塞模式
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

/**
 * @brief 接受客户端连接
 * 
 * 此函数接受一个客户端的连接请求，并返回一个新的套接字描述符用于与客户端通信。
 * 如果接受连接失败，会调用errif函数输出错误信息。
 * 
 * @param addr 指向InetAddress对象的指针，用于存储客户端的地址信息
 * @return 新的套接字描述符，用于与客户端通信
 */
int Socket::accept(InetAddress *addr) {
    // 调用系统的accept函数接受客户端连接
    int clnt_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    // 检查是否成功接受客户端连接，如果失败则输出错误信息
    errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

/**
 * @brief 获取套接字的文件描述符
 * 
 * 此函数返回套接字的文件描述符。
 * 
 * @return 套接字的文件描述符
 */
int Socket::getFd() {
    return fd;
}