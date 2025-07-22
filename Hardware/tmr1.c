#include "tmr1.h"

// #define TMR1_CNT_TIME 152 // 152 * 0.65625us 约等于100us

// 定时器定时周期 (单位:Hz)
// 周期值 = 系统时钟 / 定时器分频 / 频率 - 1
#define TMR1_PERIOD (SYSCLK / 128 / 1000 - 1) // 1000Hz,1ms

// volatile bit tmr1_flag = 0; // TMR1中断服务函数中会置位的标志位
volatile u32 tmr1_cnt = 0; // 定时器TMR1的计数值（每次在中断服务函数中会加一）

#if USE_MY_DEBUG

// volatile bit flag_is_printf_time = 0;

#endif

/**
 * @brief 配置定时器TMR1，配置完成后，定时器默认关闭
 */
void tmr1_config(void)
{
    __SetIRQnIP(TMR1_IRQn, TMR1_IQn_CFG); // 设置中断优先级（TMR1）
    __DisableIRQ(TMR1_IRQn);              // 禁用中断
    IE_EA = 1;                            // 打开总中断

    TMR_ALLCON = TMR1_CNT_CLR(0x1); // 清除计数值

    TMR1_CONL &= ~TMR_PRESCALE_SEL(0x07); // 清除TMR1的预分频配置寄存器
    TMR1_CONL |= TMR_PRESCALE_SEL(0x07);  // 定时器预分频
    TMR1_CONL &= ~TMR_MODE_SEL(0x03);     // 清除TMR1的模式配置寄存器
    TMR1_CONL |= TMR_MODE_SEL(0x01);      // 配置TMR1的模式为计数器模式，最后对系统时钟的脉冲进行计数

    TMR1_CONH &= ~TMR_PRD_PND(0x01); // 清除TMR1的计数标志位，表示未完成计数
    TMR1_CONH |= TMR_PRD_IRQ_EN(1);  // 使能TMR1的计数中断

    // 配置TMR1的计数周期
    TMR1_PRH = TMR_PERIOD_VAL_H((TMR1_PERIOD >> 8) & 0xFF); // 周期值
    TMR1_PRL = TMR_PERIOD_VAL_L((TMR1_PERIOD >> 0) & 0xFF);

    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除TMR1的时钟源配置寄存器
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);    // 配置TMR1的时钟源，不用任何时钟
}

/**
 * @brief 开启定时器TMR1，开始计时
 */
void tmr1_enable(void)
{
    // 重新给TMR1配置时钟
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR1_CONL |= TMR_SOURCE_SEL(0x06);    // 配置定时器的时钟源，使用系统时钟

    __EnableIRQ(TMR1_IRQn); // 使能中断
    IE_EA = 1;              // 打开总中断
}

#if 0  // void tmr1_disable(void)
/**
 * @brief 关闭定时器，清空计数值
 */
void tmr1_disable(void)
{
    // 不给定时器提供时钟，让它停止计数
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);    // 配置定时器的时钟源，不用任何时钟

    TMR_ALLCON = TMR1_CNT_CLR(0x1); // 清除计数值

    __DisableIRQ(TMR1_IRQn); // 关闭中断（不使能中断）
}
#endif // void tmr1_disable(void)

void heart_beat_handle(void)
{
    static volatile u32 old_time_ms_cnt = 0; //
    volatile u32 cur_time_ms_cnt = tmr1_cnt; // 先读出计时,防止被中断打断
    volatile u32 diff_ms_cnt = 0;
    tmr1_cnt = 0;                                    // 读出计时后,立刻清除定时器的计时
    diff_ms_cnt = cur_time_ms_cnt - old_time_ms_cnt; // 计算时间差
    old_time_ms_cnt = 0;

    if (pin_level_scan_time_cnt < 65535) // 防止计数溢出
    {
        // pin_level_scan_time_cnt++;
        pin_level_scan_time_cnt += diff_ms_cnt;
    }

#if 0

    // if (engine_scan_time_cnt < 65535 - diff_ms_cnt) // 防止计数溢出
    // {
    //     // engine_scan_time_cnt++;
    //     engine_scan_time_cnt += diff_ms_cnt;
    //     if (engine_scan_time_cnt >= ENGINE_SPEED_SCAN_TIME_MS) // 如果已经到了累计的时间
    //     {
    //         engine_actual_scan_time_cnt = engine_scan_time_cnt; // 更新实际的扫描检测时间
    //         engine_scan_time_cnt = 0;
    //         // detect_engine_pulse_cnt[1] += detect_engine_pulse_cnt[0]; // 将另一个定时器中断扫描到的脉冲更新到[1]
    //         // 不能在定时器中断内累加脉冲计数，主循环执行时间过长，相关的计数函数就不能定期取到这个值，
    //         // 会影响计算结果，可能多加了几个脉冲计数
    //         // 下面直接让数据覆盖，如果计算函数没有及时取到数值，也不影响计算结果
    //         detect_engine_pulse_cnt[1] = detect_engine_pulse_cnt[0]; // 将另一个定时器中断扫描到的脉冲更新到[1]
    //         detect_engine_pulse_cnt[0] = 0;
    //         flag_is_update_engine_pulse_cnt = 1; // 表示有数据更新
    //     }
    // }

    // if (speed_scan_time_cnt < 65535 - diff_ms_cnt) // 防止计数溢出
    // {
    //     // speed_scan_time_cnt++;
    //     speed_scan_time_cnt += diff_ms_cnt;
    //     if (speed_scan_time_cnt >= SPEED_SCAN_TIME_MS) // 如果经过了 xx ms
    //     {
    //         speed_actual_scan_time_cnt = speed_scan_time_cnt; // 更新实际的扫描检测时间
    //         speed_scan_time_cnt = 0;
    //         // detect_speed_pulse_cnt[1] += detect_speed_pulse_cnt[0]; // 将另一个定时器中断扫描到的脉冲更新到[1]
    //         // 不能在定时器中断内累加脉冲计数，主循环执行时间过长，相关的计数函数就不能定期取到这个值，
    //         // 会影响计算结果，可能多加了几个脉冲计数
    //         // 下面直接让数据覆盖，如果计算函数没有及时取到数值，也不影响计算结果
    //         detect_speed_pulse_cnt[1] = detect_speed_pulse_cnt[0]; // 将另一个定时器中断扫描到的脉冲更新到[1]
    //         detect_speed_pulse_cnt[0] = 0;                         //
    //         flag_is_update_speed_pulse_cnt = 1;                    // 表示有数据更新
    //     } 
    // }

#endif

    if (mileage_save_time_cnt < 4294967295 - diff_ms_cnt) // 防止计数溢出
    {
        // mileage_save_time_cnt++;
        mileage_save_time_cnt += diff_ms_cnt;
    }

    if (fuel_capacity_scan_cnt < 4294967295 - diff_ms_cnt) // 防止计数溢出
    {
        // fuel_capacity_scan_cnt++;
        fuel_capacity_scan_cnt += diff_ms_cnt;
    }

    if (synchronous_request_status == SYN_REQUEST_STATUS_HANDLING)
    {
        // synchronous_request_time_cnt++; // 同步请求的冷却计时
        synchronous_request_time_cnt += diff_ms_cnt; // 同步请求的冷却计时
        if (synchronous_request_time_cnt >= 2500)
        {
            // 如果接收同步请求已经过了 xx s，清除冷却状态
            synchronous_request_time_cnt = 0;
            synchronous_request_status = SYN_REQUEST_STATUS_NONE;
        }
    }

    if (update_date_status == UPDATE_STATUS_HANDLING)
    {
        // 如果更新日期进入冷却状态，进行冷却计时
        // update_date_cooling_cnt++;
        update_date_cooling_cnt += diff_ms_cnt;
        if (update_date_cooling_cnt >= 100) // xx ms
        {
            // 过了冷却时间，退出冷却状态
            update_date_cooling_cnt = 0;
            update_date_status = UPDATE_STATUS_NONE;
        }
    }

    if (update_time_status == UPDATE_STATUS_HANDLING)
    {
        // 如果更新时间进入冷却状态，进行冷却计时
        // update_time_cooling_cnt++;
        update_time_cooling_cnt += diff_ms_cnt;
        if (update_time_cooling_cnt >= 100) // xx ms
        {
            // 过了冷却时间，退出冷却状态
            update_time_cooling_cnt = 0;
            update_time_status = UPDATE_STATUS_NONE;
        }
    }

    if (mileage_update_time_cnt < 65535)
    {
        mileage_update_time_cnt++;
    }

    if (battery_scan_time_cnt < 4294967295)
    {
        battery_scan_time_cnt++;
    }
}

// TMR1中断服务函数
void TIMR1_IRQHandler(void) interrupt TMR1_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR1_IRQn);

    /*
        测试中断持续时间
        内部未调用函数，约           74.5us,
        调用engine_speed_scan()时约 200us,
        调用speed_scan()时约        271us,
        两个一起调用，约             426us,

        将 engine_speed_scan()和speed_scan()放到主函数来计算，
        最短用时约 43us，
        相继执行了engine_speed_scan和speed_scan对应的条件，进行更新时，约 75us

        将 对其他功能的时间进行计数的操作全部转移到外部函数,只在该中断对 tmr1_cnt 加一,
        约 5us
    */
    // P20 = 1;

    // ---------------- 用户函数处理 -------------------

    // 周期中断
    if (TMR1_CONH & TMR_PRD_PND(0x1))
    {
        TMR1_CONH |= TMR_PRD_PND(0x1); // 清除pending

        if (tmr1_cnt < 4294967295)
        {
            tmr1_cnt++;
        }

        {
            // static u8 cnt = 0;
            // cnt++;
            // if (cnt >= TOUCH_KEY_SCAN_CIRCLE_TIMES) // xx ms
            // {
            //     cnt = 0;
            //     // flag_is_touch_key_scan_circle_arrived = 1; // 表示扫描周期到来，执行一次按键扫描

            //     // {
            //     //     static u8 send_cnt = 0;
            //     //     send_cnt++;
            //     //     if (send_cnt >= 100)
            //     //     {
            //     //         send_cnt = 0;
            //     //         printf("touch key scan\n");
            //     //     }
            //     // }
            // }

#if AD_KEY_ENABLE
            // 在定时器注册按键扫描：
            if (ad_key_para.cur_scan_times < 255)
            {
                ad_key_para.cur_scan_times++;
            }
#endif // AD_KEY_ENABLE

#if TOUCH_KEY_ENABLE
            if (touch_key_para.cur_scan_times < 255)
            {
                touch_key_para.cur_scan_times++;
            }
#endif // TOUCH_KEY_ENABLE

#if USE_MY_DEBUG

            // {
            //     static u16 cnt;
            //     cnt++;
            //     if (cnt >= 2000)
            //     {
            //         cnt = 0;
            //         flag_is_printf_time = 1;
            //     }
            // }

#endif
        }
    }

    // P20 = 0;// 测试中断持续时间
    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR1_IRQn);
}