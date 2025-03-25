// 引入socket编程所需的头文件
#include <sys/socket.h>
// 引入inet_addr函数所需的头文件
#include <arpa/inet.h>
// 引入bzero函数所需的头文件
#include <string.h>

/**
 * @brief 客户端程序的主函数，建立与服务器的连接
 *
 * 此函数创建一个TCP套接字，并尝试连接到指定的服务器地址和端口。
 *
 * @return 程序执行状态码，0表示成功
 */
int main() {
    // 创建一个TCP套接字，AF_INET表示使用IPv4地址族，SOCK_STREAM表示使用TCP协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 定义服务器地址结构体
    struct sockaddr_in serv_addr;
    // 将服务器地址结构体清零，确保没有随机数据
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族为IPv4
    serv_addr.sin_family = AF_INET;
    // 将服务器的IP地址转换为网络字节序
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将服务器的端口号转换为网络字节序
    serv_addr.sin_port = htons(8888);

    // 客户端通常不需要绑定操作，因为操作系统会自动分配本地地址和端口
    // bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); 客户端不进行bind操作

    // 尝试连接到服务器
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 程序正常结束，返回0表示成功
    return 0;
}
