// 引入自定义的头文件util.h
#include "util.h"
// 引入标准输入输出库
#include <stdio.h>
// 引入标准库，提供了一些常用的函数和宏定义，如exit函数
#include <stdlib.h>

/**
 * @brief 检查条件并在条件为真时输出错误信息并退出程序
 *
 * 该函数用于检查给定的条件是否为真。如果条件为真，则使用perror函数输出错误信息，
 * 并使用exit函数以EXIT_FAILURE状态退出程序。
 *
 * @param condition 要检查的条件
 * @param errmsg 错误信息的字符串指针
 */
void errif(bool condition, const char *errmsg){
    // 如果条件为真
    if(condition){
        // 输出错误信息
        perror(errmsg);
        // 以失败状态退出程序
        exit(EXIT_FAILURE);
    }
}