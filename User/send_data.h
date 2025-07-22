#ifndef __SEND_DATA_H
#define __SEND_DATA_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

#ifndef FORMAT_HEAD // 定义格式头
#define FORMAT_HEAD ((u8)0xA5)
#endif // FORMAT_HEAD

// 定义单片机向MP5发送的带有数据的指令
enum
{
    SEND_GEAR = 0x01,             // 发送挡位的状态
    SEND_BATTERY = 0x02,          // 发送电池电量的状态
    SEND_BARKE = 0x03,            // 发送刹车的状态
    SEND_LEFT_TURN = 0x04,        // 发送左转向灯的状态
    SEND_RIGHT_TURN = 0x05,       // 发送右转向灯的状态
    SEND_HIGH_BEAM = 0x06,        // 发送远光灯的状态
    SEND_ENGINE_SPEED = 0x07,     // 发送发动机的转速
    SEND_SPEED = 0x08,            // 发送时速
    SEND_FUEL = 0x09,             // 发送油量
    SEND_WATER_TEMP = 0x0A,       // 发送水温
    SEND_TOTAL_MILEAGE = 0x0B,    // 发送大计里程
    SEND_SUBTOTAL_MILEAGE = 0x0C, // 发送小计里程

    SEND_TOUCH_KEY_STATUS = 0x0D,    // 发送触摸按键的状态
    // SEND_DATE = 0x0E,                // 发送日期(年月日)
    // SEND_TIME = 0x0F,                // 发送时间(24H)

    SEND_TIME = 0x0E, // 发送时间（年月日，时分秒）

    SEND_VOLTAGE_OF_BATTERY = 0x10,     // 发送电池电压
    SEND_TEMP_OF_WATER_ALERT = 0x11, // 发送水温报警

    SEND_MALFUNCTION_STATUS = 0x12, // 发送故障的状态
    SEND_ABS_STATUS = 0x13, // 发送ABS的状态

    SEND_SUBTOTAL_MILEAGE_2 = 0x14, // 发送小计里程2
};

void send_data(u8 instruct, u32 send_data);

#endif
