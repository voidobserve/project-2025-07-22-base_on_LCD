#include "tmr4.h"

// 周期值=系统时钟/分频/频率 - 1
// 1KHz == 1ms计数一次
#define TMR4_PEROID_VAL (SYSCLK / 1 / 1000 - 1)

volatile u32 tmr4_cnt = 0; //

// void tmr4_pwm_config(void)
// {
//     // 配置TIMER4的PWM端口：P21--TMR4_PWM
//     P2_MD0 &= ~GPIO_P21_MODE_SEL(0x03);
//     P2_MD0 |= GPIO_P21_MODE_SEL(0x01);
//     FOUT_S21 = GPIO_FOUT_TMR4_PWMOUT; // 选择tmr4_pwm_o

// #define PEROID_VAL (SYSCLK / 1 / 1000 - 1) // 周期值=系统时钟/分频/频率 - 1
//     // TIMER4配置1kHz频率PWM
//     __EnableIRQ(TMR4_IRQn);                                      // 打开TIMER4模块中断
//     TMR_ALLCON = TMR4_CNT_CLR(0x1);                              // Timer4 计数清零
//     TMR4_CAP10 = TMR4_PRD_VAL_L((PEROID_VAL >> 0) & 0xFF);       // 周期低八位寄存器
//     TMR4_CAP11 = TMR4_PRD_VAL_H((PEROID_VAL >> 8) & 0xFF);       // 周期高八位寄存器
//     TMR4_CAP20 = TMR4_PWM_VAL_L(((PEROID_VAL / 2) >> 0) & 0xFF); // 比较值高八位寄存器
//     TMR4_CAP21 = TMR4_PWM_VAL_H(((PEROID_VAL / 2) >> 8) & 0xFF); // 比较值低八位寄存器
//     TMR4_CON0 = TMR4_PRESCALE_SEL(0x0) |                         // 不分频
//                 TMR4_SOURCE_SEL(0x7) |                           // 计数源选择系统时钟
//                 TMR4_MODE_SEL(0x1);                              // PWM输出模式
//     TMR4_EN = TMR4_EN(0x1);                                      // 使能定时器

//     IE_EA = 1; // 使能总中断
// }

// 定时器计数(计时)配置
void tmr4_cnt_config(void)
{
    // TIMER4配置频率为1kHz的中断
    __EnableIRQ(TMR4_IRQn);                                     // 打开TIMER4模块中断
    TMR_ALLCON = TMR4_CNT_CLR(0x1);                             // Timer4 计数清零
    TMR4_CAP10 = TMR4_PRD_VAL_L((TMR4_PEROID_VAL >> 0) & 0xFF); // 周期低八位寄存器
    TMR4_CAP11 = TMR4_PRD_VAL_H((TMR4_PEROID_VAL >> 8) & 0xFF); // 周期高八位寄存器
    TMR4_CON0 = TMR4_PRESCALE_SEL(0x0) |                        // 不分频
                TMR4_SOURCE_SEL(0x7) |                          // 计数源选择系统时钟
                TMR4_MODE_SEL(0x0);                             // 计数模式
    TMR4_IE0 = TMR4_PRD_IRQ_EN(0x1);                            // 使能计数值等于周期值中断
    TMR4_EN = TMR4_EN(0x1);                                     // 使能定时器

    IE_EA = 1; // 使能总中断
}

void TMR4_IRQHandler(void) interrupt TMR4_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR4_IRQn);

    // ---------------- 用户函数处理 -------------------

    // 计数值等于周期值中断
    if (TMR4_FLAG0 & TMR4_PRD_FLAG(0x1))
    {
        TMR4_CLR0 |= TMR4_PRD_FLG_CLR(0x1);
        tmr4_cnt++;
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR4_IRQn);
}