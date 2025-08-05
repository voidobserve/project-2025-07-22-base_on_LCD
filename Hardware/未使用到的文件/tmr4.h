#ifndef __TMR4_H
#define __TMR4_H

#include "include.h" // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

extern volatile u32 tmr4_cnt;

// 定时器计数(计时)配置
void tmr4_cnt_config(void);

#endif

