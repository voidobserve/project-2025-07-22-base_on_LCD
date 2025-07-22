#ifndef __SPEED_SCAN_H
#define __SPEED_SCAN_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

// ======================================================
// 检测时速所需的配置：

/*
    已知 40.4Hz 对应 50km/h
        80.7Hz 对应 100km/h

    假设有方程 y（速度） = kx（频率，单位：Hz） 对应 频率和速度的关系
    将 （40，50），（80，100）代入计算：
    y = 1.25x

    根据得到的方程，当频率为 288Hz 时，速度为 360km/h，对应100m/s
    每个脉冲对应走过的距离：
    100m / 288Hz 约等于 0.347222m / Hz
    347.222mm / Hz

    假设 N个脉冲表示车轮走过一圈，C表示车轮的一圈对应的毫米数
    有： N = C / (347.222mm / Hz)

    假设 N = 3，那么 C = 1041.666 mm
    N = 4, C = 1388.888 mm
*/

#define SPEED_SCAN_PIN P15 // 检测时速的引脚
// 检测到 多少个脉冲 表示 车轮走过一圈
// #define SPEED_SCAN_PULSE_PER_TURN (32)
#define SPEED_SCAN_PULSE_PER_TURN (4)
// 车轮的一圈对应多少毫米
// #define SPEED_SCAN_MM_PER_TURN (1795) // 一圈 xx 毫米
#define SPEED_SCAN_MM_PER_TURN (1389) // 一圈 xx 毫米

#if 0 // 使用定时检测时速的方式
// 累计检测多久的时速：(单位：ms)
#define SPEED_SCAN_TIME_MS (100)
// 重复检测多少次时速，才更新：(不能为0，也不能太大)
// 例如 5次，  5(次) *  SPEED_SCAN_TIME_MS 之后，才更新速度
#define SPEED_SCAN_FILTER_CNT (200)
#endif //

/*
    时速扫描的超时时间，单位：ms
    如果超过这个时间还没有检测到脉冲个数，则认为时速为0
*/
#define SPEED_SCAN_OVER_TIME (1000)
// 时速的更新时间，单位：ms
#define SPEED_SCAN_UPDATE_TIME (700) // 至少要大于500ms，防止滤掉1Hz信号
// #define SPEED_SCAN_UPDATE_TIME (300) // 时间太短，时速会经常跳动（提供1Hz信号，会在0km/h、3km/h这两个值来回切换）

// 检测时速所需的配置
// ======================================================

extern volatile bit flag_is_update_speed_pulse_cnt; // 标志位，是否由更新计数,由定时器来置位
extern volatile u16 speed_scan_time_cnt;            // 速度扫描时，用到的时间计数值，会在定时器中断中累加
extern volatile u16 speed_actual_scan_time_cnt;     // 存放实际的速度扫描时间(实际的速度扫描用时)
extern volatile u32 detect_speed_pulse_cnt[2];      // 检测时速的脉冲计数值

extern volatile bit flag_is_speed_scan_over_time;
extern volatile u32 speed_pulse_cnt;
extern volatile u32 speed_scan_time_ms;

void speed_scan_config(void);
void speed_scan(void);

#endif
