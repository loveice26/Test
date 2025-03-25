// 引入标准输入输出库，用于使用printf函数
#include <stdio.h>
// 引入套接字相关的头文件，用于网络编程
#include <sys/socket.h>
// 引入地址转换相关的头文件，用于处理IP地址
#include <arpa/inet.h>
// 引入字符串处理库，用于使用bzero函数
#include <string.h>

/**
 * @brief 主函数，程序的入口点
 * 
 * 该函数创建一个TCP服务器，绑定到指定的IP地址和端口，
 * 监听客户端连接，接受客户端连接并打印客户端信息。
 * 
 * @return int 程序的退出状态码，0表示正常退出
 */
int main() {
    // 创建一个TCP套接字，返回套接字描述符
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 将服务器地址结构体清零
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 将点分十进制的IP地址转换为网络字节序
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将端口号转换为网络字节序
    serv_addr.sin_port = htons(8888);

    // 将套接字绑定到指定的地址和端口
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 开始监听客户端连接，最大连接数为系统允许的最大值
    listen(sockfd, SOMAXCONN);
    
    // 定义客户端地址结构体
    struct sockaddr_in clnt_addr;
    // 定义客户端地址结构体的长度
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    // 将客户端地址结构体清零
    bzero(&clnt_addr, sizeof(clnt_addr));

    // 接受客户端连接，返回客户端套接字描述符
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);

    // 打印新客户端的套接字描述符、IP地址和端口号
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    return 0;
}

