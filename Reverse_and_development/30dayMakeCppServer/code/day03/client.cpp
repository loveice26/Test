// 引入标准输入输出流库
#include <iostream>
// 引入套接字相关的系统库
#include <sys/socket.h>
// 引入地址转换相关的系统库
#include <arpa/inet.h>
// 引入字符串操作相关的系统库
#include <string.h>
// 引入Unix标准系统调用库
#include <unistd.h>
// 引入自定义工具库
#include "util.h"

// 定义缓冲区大小
#define BUFFER_SIZE 1024 

/**
 * @brief 客户端程序的主函数，负责与服务器建立连接并进行数据交互。
 * 
 * 该函数创建一个TCP套接字，连接到指定的服务器地址和端口，然后进入一个循环，
 * 不断从用户输入读取数据并发送到服务器，同时接收服务器的响应并输出。
 * 
 * @return int 程序退出状态码，0表示正常退出。
 */
int main() {
    // 创建一个TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字是否创建成功
    errif(sockfd == -1, "socket create error");

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 清空服务器地址结构体
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 将服务器IP地址转换为网络字节序
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将服务器端口号转换为网络字节序
    serv_addr.sin_port = htons(8888);

    // 连接到服务器
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    
    // 进入循环，持续与服务器进行数据交互
    while(true){
        // 定义一个缓冲区，用于存储用户输入和服务器响应
        char buf[BUFFER_SIZE];  //在这个版本，buf大小必须大于或等于服务器端buf大小，不然会出错，想想为什么？
        // 清空缓冲区
        bzero(&buf, sizeof(buf));
        // 从标准输入读取用户输入
        scanf("%s", buf);
        // 将用户输入发送到服务器
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // 检查写入是否成功
        if(write_bytes == -1){
            // 输出错误信息
            printf("socket already disconnected, can't write any more!\n");
            // 跳出循环
            break;
        }
        // 清空缓冲区
        bzero(&buf, sizeof(buf));
        // 从服务器读取响应
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        // 检查读取的字节数
        if(read_bytes > 0){
            // 输出服务器响应
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            // 输出服务器断开连接信息
            printf("server socket disconnected!\n");
            // 跳出循环
            break;
        }else if(read_bytes == -1){
            // 关闭套接字
            close(sockfd);
            // 输出读取错误信息
            errif(true, "socket read error");
        }
    }
    // 关闭套接字
    close(sockfd);
    // 返回程序退出状态码
    return 0;
}
