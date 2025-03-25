// 引入Epoll类的头文件
#include "Epoll.h"
// 引入工具函数头文件
#include "util.h"
// 引入Unix标准库头文件，提供系统调用接口
#include <unistd.h>
// 引入字符串处理函数头文件
#include <string.h>

// 定义最大事件数量
#define MAX_EVENTS 1000

/**
 * @brief Epoll类的构造函数，用于初始化Epoll实例。
 * 
 * 该构造函数创建一个新的epoll实例，并分配内存以存储epoll事件。
 * 如果epoll实例创建失败，会调用errif函数输出错误信息。
 */
Epoll::Epoll() : epfd(-1), events(nullptr) {
    // 创建一个epoll实例，返回一个文件描述符
    epfd = epoll_create1(0);
    // 检查epoll实例是否创建成功，若失败则输出错误信息
    errif(epfd == -1, "epoll create error");
    // 为epoll事件数组分配内存
    events = new epoll_event[MAX_EVENTS];
    // 将事件数组内存初始化为零
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

/**
 * @brief Epoll类的析构函数，用于清理Epoll实例。
 * 
 * 该析构函数关闭epoll实例的文件描述符，并释放之前分配的事件数组内存。
 */
Epoll::~Epoll() {
    // 检查epoll文件描述符是否有效
    if (epfd != -1) {
        // 关闭epoll文件描述符
        close(epfd);
        // 将epfd置为-1，表示已关闭
        epfd = -1;
    }
    // 释放事件数组内存
    delete[] events;
}

/**
 * @brief 向epoll实例中添加一个文件描述符及其对应的事件操作。
 * 
 * 该函数创建一个epoll_event结构体，设置其文件描述符和事件操作，
 * 并使用epoll_ctl函数将其添加到epoll实例中。
 * 
 * @param fd 要添加的文件描述符。
 * @param op 要监听的事件操作，如EPOLLIN、EPOLLOUT等。
 */
void Epoll::addFd(int fd, uint32_t op) {
    // 定义一个epoll_event结构体
    struct epoll_event ev;
    // 将结构体内存初始化为零
    bzero(&ev, sizeof(ev));
    // 设置事件关联的文件描述符
    ev.data.fd = fd;
    // 设置要监听的事件
    ev.events = op;
    // 将文件描述符及其事件添加到epoll实例中，若失败则输出错误信息
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

/**
 * @brief 等待epoll实例中的事件发生。
 * 
 * 该函数调用epoll_wait函数等待事件发生，并将发生的事件存储在一个向量中返回。
 * 
 * @param timeout 等待事件的超时时间，单位为毫秒。
 * @return 一个包含所有发生事件的向量。
 */
std::vector<epoll_event> Epoll::poll(int timeout) {
    // 定义一个向量用于存储发生的事件
    std::vector<epoll_event> activeEvents;
    // 调用epoll_wait函数等待事件发生，返回发生事件的数量
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    // 检查epoll_wait是否出错，若出错则输出错误信息
    errif(nfds == -1, "epoll wait error");
    // 遍历所有发生的事件
    for (int i = 0; i < nfds; ++i) {
        // 将发生的事件添加到向量中
        activeEvents.push_back(events[i]);
    }
    // 返回包含所有发生事件的向量
    return activeEvents;
}