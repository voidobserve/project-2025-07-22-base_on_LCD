#ifndef __ENGINE_SPEED_SCAN_H
#define __ENGINE_SPEED_SCAN_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

// ======================================================
// 检测发动机转速所需的配置：
#define ENGINE_SPEED_SCAN_PIN P02 // 检测发动机转速的引脚
// 检测到 多少个脉冲 表示 发动机转过一圈
#define ENGINE_SPEED_SCAN_PULSE_PER_TURN (16)
// // 累计检测多久的发动机转速： (单位：ms)(不能大于1min,不能大于变量类型对应的最大值)
// #define ENGINE_SPEED_SCAN_TIME_MS (100)
// // 重复检测多少次发动机转速，才更新：
// #define ENGINE_SPEED_SCAN_FILTER_CNT (5)

// 发动机转速的更新时间，单位：ms
#define ENGINE_SPEED_SCAN_UPDATE_TIME (500)
/* 
    发动机转速的超时时间，单位：ms
    如果超时时间到来，还没有检测到脉冲，认为发动机转速为0
*/ 
#define ENGINE_SPEED_SCAN_OVER_TIME (600)

// 检测发动机转速所需的配置
// ======================================================

// extern volatile bit flag_is_update_engine_pulse_cnt; // 标志位，是否有更新脉冲计数
// extern volatile u16 engine_scan_time_cnt; // 发动机转速扫描时，用到的时间计数值，会在定时器中断中累加
// extern volatile u16 engine_actual_scan_time_cnt; // 实际的发动机转速扫描用时
// extern volatile u32 detect_engine_pulse_cnt[2]; // 检测发送机转速的脉冲计数值


extern volatile bit flag_is_engine_speed_scan_over_time; // 标志位，发动机转速检测是否超时
extern volatile u32 engine_speed_scan_cnt;
extern volatile u32 engine_speed_scan_ms;

extern void engine_speed_scan_config(void); // 发动机转速扫描的配置
extern void engine_speed_scan(void); // 发动机转速扫描

#endif