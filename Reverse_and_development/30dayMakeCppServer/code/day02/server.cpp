// 引入标准输入输出库
#include <stdio.h>
// 引入套接字编程所需的头文件
#include <sys/socket.h>
// 引入网络地址转换相关的头文件
#include <arpa/inet.h>
// 引入字符串处理函数的头文件
#include <string.h>
// 引入Unix标准库函数的头文件
#include <unistd.h>
// 引入自定义的工具函数头文件
#include "util.h"

/**
 * @brief 主函数，服务器程序的入口点
 * 
 * 该函数创建一个TCP服务器，监听指定地址和端口，接受客户端连接，并处理客户端消息。
 * 
 * @return int 程序退出状态码，0表示正常退出
 */
int main() {
    // 创建一个套接字，使用IPv4地址族和TCP协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字是否创建成功，如果失败则输出错误信息
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

    // 将套接字绑定到指定的服务器地址和端口
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    // 开始监听客户端连接，最大连接队列长度为系统默认最大值
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    // 定义客户端地址结构体
    struct sockaddr_in clnt_addr;
    // 客户端地址结构体的长度
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    // 清空客户端地址结构体
    bzero(&clnt_addr, sizeof(clnt_addr));

    // 接受一个客户端连接，返回一个新的套接字用于与客户端通信
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    // 检查客户端连接是否接受成功，如果失败则输出错误信息
    errif(clnt_sockfd == -1, "socket accept error");

    // 输出新客户端的套接字描述符、IP地址和端口号
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    // 进入循环，持续处理客户端消息
    while (true) {
        // 定义一个缓冲区用于存储客户端消息
        char buf[1024];
        // 清空缓冲区
        bzero(&buf, sizeof(buf));
        // 从客户端套接字读取消息
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
        // 如果读取到消息
        if(read_bytes > 0){
            // 输出客户端消息
            printf("message from client fd %d: %s\n", clnt_sockfd, buf);
            // 将消息原样发送回客户端
            write(clnt_sockfd, buf, sizeof(buf));
        } 
        // 如果客户端关闭连接
        else if(read_bytes == 0){
            // 输出客户端断开连接的信息
            printf("client fd %d disconnected\n", clnt_sockfd);
            // 关闭客户端套接字
            close(clnt_sockfd);
            // 跳出循环
            break;
        } 
        // 如果读取消息时发生错误
        else if(read_bytes == -1){
            // 关闭客户端套接字
            close(clnt_sockfd);
            // 输出读取错误信息
            errif(true, "socket read error");
        }
    }
    // 关闭服务器套接字
    close(sockfd);
    // 返回程序退出状态码
    return 0;
}
