#ifndef __TMR1_H
#define __TMR1_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

// extern volatile bit tmr1_flag; // TMR1中断服务函数中会置位的标志位
// extern volatile u32 tmr1_cnt;  // 定时器TMR1的计数值（每次在中断服务函数中会加一）

#if USE_MY_DEBUG

// extern volatile bit flag_is_printf_time;

#endif

void tmr1_config(void);  // 配置定时器（初始化）
void tmr1_enable(void);  // 开启定时器
void tmr1_disable(void); // 关闭定时器，清除硬件的计数值

extern void heart_beat_handle(void);

#endif
