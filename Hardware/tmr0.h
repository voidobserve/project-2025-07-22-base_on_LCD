#ifndef __TMR0_H
#define __TMR0_H

#include "include.h" // 使用芯片官方提供的头文件

#include "my_config.h" // 包含自定义的头文件

// extern volatile bit tmr0_flag; // tmr0中断服务函数中会置位的标志位
extern volatile u32 tmr0_cnt;            // 定时器TMR0的计数值（每次在中断服务函数中会加一）

void tmr0_config(void);
void tmr0_enable(void);
void tmr0_disable(void);


#endif
