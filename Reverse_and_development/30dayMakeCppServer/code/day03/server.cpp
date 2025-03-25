// 引入标准输入输出库
#include <stdio.h>
// 引入套接字相关的系统库
#include <sys/socket.h>
// 引入IP地址转换相关的库
#include <arpa/inet.h>
// 引入字符串处理库
#include <string.h>
// 引入Unix标准库，包含文件操作等函数
#include <unistd.h>
// 引入文件控制相关的库
#include <fcntl.h>
// 引入epoll相关的系统库
#include <sys/epoll.h>
// 引入错误号相关的库
#include <errno.h>
// 引入自定义的工具库
#include "util.h"

// 定义epoll事件的最大数量
#define MAX_EVENTS 1024
// 定义读取缓冲区的大小
#define READ_BUFFER 1024

/**
 * @brief 设置文件描述符为非阻塞模式
 * 
 * 此函数使用fcntl系统调用将指定的文件描述符设置为非阻塞模式。
 * 
 * @param fd 要设置为非阻塞模式的文件描述符
 */
void setnonblocking(int fd){
    // 获取当前文件描述符的状态标志，并添加非阻塞标志
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

/**
 * @brief 主函数，程序的入口点
 * 
 * 此函数创建一个TCP服务器，使用epoll机制处理多个客户端连接。
 * 
 * @return int 程序的退出状态码
 */
int main() {
    // 创建一个TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字创建是否成功
    errif(sockfd == -1, "socket create error");

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 清空服务器地址结构体
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 设置服务器IP地址为本地回环地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 设置服务器端口号为8888
    serv_addr.sin_port = htons(8888);

    // 将套接字绑定到指定的地址和端口
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    // 开始监听客户端连接请求
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    
    // 创建一个epoll实例
    int epfd = epoll_create1(0);
    // 检查epoll实例创建是否成功
    errif(epfd == -1, "epoll create error");

    // 定义epoll事件数组和单个事件结构体
    struct epoll_event events[MAX_EVENTS], ev;
    // 清空epoll事件数组
    bzero(&events, sizeof(events));

    // 清空单个epoll事件结构体
    bzero(&ev, sizeof(ev));
    // 设置事件关联的文件描述符为监听套接字
    ev.data.fd = sockfd;
    // 设置事件类型为可读和边缘触发模式
    ev.events = EPOLLIN | EPOLLET;
    // 将监听套接字设置为非阻塞模式
    setnonblocking(sockfd);
    // 将监听套接字添加到epoll实例中
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    // 进入事件循环
    while(true){
        // 等待epoll事件发生
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        // 检查epoll_wait是否出错
        errif(nfds == -1, "epoll wait error");
        // 遍历所有发生的事件
        for(int i = 0; i < nfds; ++i){
            // 如果是监听套接字有事件发生，说明有新的客户端连接
            if(events[i].data.fd == sockfd){        
                // 定义客户端地址结构体
                struct sockaddr_in clnt_addr;
                // 清空客户端地址结构体
                bzero(&clnt_addr, sizeof(clnt_addr));
                // 定义客户端地址结构体的长度
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                // 接受新的客户端连接
                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
                // 检查连接接受是否成功
                errif(clnt_sockfd == -1, "socket accept error");
                // 打印新客户端的信息
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                // 清空单个epoll事件结构体
                bzero(&ev, sizeof(ev));
                // 设置事件关联的文件描述符为客户端套接字
                ev.data.fd = clnt_sockfd;
                // 设置事件类型为可读和边缘触发模式
                ev.events = EPOLLIN | EPOLLET;
                // 将客户端套接字设置为非阻塞模式
                setnonblocking(clnt_sockfd);
                // 将客户端套接字添加到epoll实例中
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } 
            // 如果是可读事件
            else if(events[i].events & EPOLLIN){      
                // 定义读取缓冲区
                char buf[READ_BUFFER];
                // 循环读取客户端数据
                while(true){    
                    // 清空读取缓冲区
                    bzero(&buf, sizeof(buf));
                    // 从客户端套接字读取数据
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    // 如果读取到数据
                    if(bytes_read > 0){
                        // 打印客户端发送的消息
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        // 将读取到的数据原样返回给客户端
                        write(events[i].data.fd, buf, sizeof(buf));
                    } 
                    // 如果读取被中断，继续读取
                    else if(bytes_read == -1 && errno == EINTR){  
                        printf("continue reading");
                        continue;
                    } 
                    // 如果数据全部读取完毕
                    else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                        // 打印读取完成信息和错误号
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } 
                    // 如果客户端断开连接
                    else if(bytes_read == 0){  
                        // 打印客户端断开连接信息
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        // 关闭客户端套接字
                        close(events[i].data.fd);   
                        break;
                    }
                }
            } 
            // 其他事件，之后的版本实现
            else{         
                printf("something else happened\n");
            }
        }
    }
    // 关闭监听套接字
    close(sockfd);
    return 0;
}
