// encoding UTF-8
// gpio_interrupt.c -- 存放GPIO中断的服务函数
#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

#if 0

// P0中断服务函数
void P0_IRQHandler(void) interrupt P0_IRQn
{
    // Px_PND寄存器写任何值都会清标志位
    u8 p0_pnd = P0_PND;

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(P0_IRQn);
    // ---------------- 用户函数处理 -------------------
    // 如果是检测时速的引脚触发的中断
    if (p0_pnd & GPIO_P02_IRQ_PNG(0x1))
    {
        if (detect_speed_pulse_cnt < 4294967295) // 防止计数溢出
        {
            detect_speed_pulse_cnt++;
        }
    }
    P0_PND = p0_pnd; // 清P0中断标志位
    // -------------------------------------------------
    // 退出中断设置IP，不可删除
    __IRQnIPnPop(P0_IRQn);
}

// P1中断服务函数
void P1_IRQHandler(void) interrupt P1_IRQn
{
    // Px_PND寄存器写任何值都会清标志位
    u8 p1_pnd = P1_PND;

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(P1_IRQn);
    // ---------------- 用户函数处理 -------------------

    // 如果是检测发动机转速的引脚触发的中断
    if (p1_pnd & GPIO_P15_IRQ_PNG(0x1))
    {
        if (detect_engine_pulse_cnt < 4294967295) // 防止计数溢出
        {
            detect_engine_pulse_cnt++;
        }
    }
    P1_PND = p1_pnd; // 清P1中断标志位

    // -------------------------------------------------
    // 退出中断设置IP，不可删除
    __IRQnIPnPop(P1_IRQn);
}

#endif
