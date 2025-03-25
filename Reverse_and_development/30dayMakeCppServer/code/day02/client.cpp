// 引入输入输出流库，用于处理标准输入输出
#include <iostream>
// 引入套接字相关的系统库，用于创建和操作网络套接字
#include <sys/socket.h>
// 引入地址转换相关的库，用于将IP地址从字符串转换为二进制形式
#include <arpa/inet.h>
// 引入字符串处理库，用于内存操作
#include <string.h>
// 引入Unix标准库，包含了许多Unix系统调用，如close和read
#include <unistd.h>
// 引入自定义的工具库，可能包含错误处理函数
#include "util.h"

/**
 * @brief 客户端程序的主函数，建立与服务器的连接并进行数据交互
 * 
 * 此函数创建一个TCP套接字，连接到指定的服务器地址和端口。
 * 它允许用户输入消息并将其发送到服务器，然后接收服务器的响应并显示。
 * 
 * @return int 程序退出状态码，0表示正常退出
 */
int main() {
    // 创建一个TCP套接字，返回套接字描述符
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字是否创建成功，如果失败则输出错误信息
    errif(sockfd == -1, "socket create error");

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 清空服务器地址结构体的内存
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 将服务器的IP地址从字符串转换为二进制形式
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将服务器的端口号从主机字节序转换为网络字节序
    serv_addr.sin_port = htons(8888);

    // 连接到服务器，如果失败则输出错误信息
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    // 进入循环，持续与服务器进行数据交互
    while(true){
        // 定义一个缓冲区，用于存储用户输入和服务器响应
        char buf[1024];
        // 清空缓冲区
        bzero(&buf, sizeof(buf));
        // 从标准输入读取用户输入的字符串
        scanf("%s", buf);
        // 将用户输入的字符串发送到服务器
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // 检查写入是否成功，如果失败则输出错误信息并退出循环
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // 清空缓冲区，准备接收服务器响应
        bzero(&buf, sizeof(buf));
        // 从服务器读取响应
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        // 如果读取到数据，则输出服务器响应
        if(read_bytes > 0){
            printf("message from server: %s\n", buf);
        // 如果读取到0字节，表示服务器已断开连接
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        // 如果读取失败，则关闭套接字并输出错误信息
        }else if(read_bytes == -1){
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    // 关闭套接字
    close(sockfd);
    // 返回程序退出状态码
    return 0;
}
