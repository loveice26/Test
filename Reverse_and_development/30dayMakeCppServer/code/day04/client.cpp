// 引入标准输入输出流库，用于输入输出操作
#include <iostream>
// 引入套接字相关的系统库，用于网络编程
#include <sys/socket.h>
// 引入网络地址转换相关的库，用于处理IP地址
#include <arpa/inet.h>
// 引入字符串处理库，用于操作字符串
#include <string.h>
// 引入Unix标准库，提供了一些系统调用接口
#include <unistd.h>
// 引入自定义的工具库
#include "util.h"

// 定义缓冲区大小为1024字节
#define BUFFER_SIZE 1024

/**
 * @brief 客户端程序的主函数
 * 
 * 该函数创建一个TCP套接字，连接到指定的服务器地址和端口，
 * 并在一个无限循环中从标准输入读取数据，发送给服务器，
 * 然后接收服务器的响应并打印出来。
 * 
 * @return int 程序的退出状态码，0表示正常退出
 */
int main() {
    // 创建一个TCP套接字，返回套接字描述符
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字是否创建成功，如果失败则输出错误信息
    errif(sockfd == -1, "socket create error");

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 将服务器地址结构体清零
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 将服务器的IP地址转换为网络字节序
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将服务器的端口号转换为网络字节序
    serv_addr.sin_port = htons(8888);

    // 尝试连接到服务器，如果失败则输出错误信息
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    // 进入无限循环，持续与服务器进行通信
    while(true){
        // 定义一个缓冲区，用于存储发送和接收的数据
        char buf[BUFFER_SIZE];  //在这个版本，buf大小必须大于或等于服务器端buf大小，不然会出错，想想为什么？
        // 将缓冲区清零
        bzero(&buf, sizeof(buf));
        // 从标准输入读取数据到缓冲区
        scanf("%s", buf);
        // 将缓冲区的数据发送到服务器，返回实际发送的字节数
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // 检查是否发送成功，如果失败则输出错误信息并退出循环
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // 将缓冲区清零，准备接收服务器的响应
        bzero(&buf, sizeof(buf));
        // 从服务器读取数据到缓冲区，返回实际读取的字节数
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        // 根据读取的字节数进行不同的处理
        if(read_bytes > 0){
            // 输出服务器的响应信息
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            // 服务器断开连接，输出提示信息并退出循环
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            // 读取数据失败，关闭套接字并输出错误信息
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    // 关闭套接字
    close(sockfd);
    // 返回程序的退出状态码
    return 0;
}
