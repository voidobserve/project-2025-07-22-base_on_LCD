#include "send_data.h"

/**
 * @brief 通过串口发送数据
 *
 * @param instruct 指令
 * @param send_data 待发送的数据，如果要发送时间，该参数无效，因为时间信息一共7个字节
 */
void send_data(u8 instruct, u32 send_data)
{
    u32 check_num = 0; // 存放校验和

    uart0_sendbyte(FORMAT_HEAD); // 先发送格式头

    check_num += FORMAT_HEAD; // 使用if()语句时，才使用这一条程序

    if (SEND_GEAR == instruct ||                /* 发送挡位的状态 */
        SEND_BATTERY == instruct ||             /* 发送电池电量的状态 */
        SEND_BARKE == instruct ||               /* 发送刹车的状态 */
        SEND_LEFT_TURN == instruct ||           /* 发送左转向灯的状态 */
        SEND_RIGHT_TURN == instruct ||          /* 发送右转向灯的状态 */
        SEND_HIGH_BEAM == instruct ||           /* 发送远光灯的状态 */
        SEND_FUEL == instruct ||                /* 发送油量 */
        SEND_WATER_TEMP == instruct ||          /* 发送水温 */
        SEND_TEMP_OF_WATER_ALERT == instruct || /* 发送水温报警 */
        SEND_MALFUNCTION_STATUS == instruct ||  /* 发送故障的状态 */
        SEND_ABS_STATUS == instruct             /* 发送ABS的状态 */
        )                                       // 如果指令的总长度只有5个字节
    {
        uart0_sendbyte(0x05);     // 发送指令的总长度
        uart0_sendbyte(instruct); // 发送指令
        uart0_sendbyte(send_data);

        check_num += 0x05 + (u8)instruct + (u8)send_data;
    }
    else if (SEND_ENGINE_SPEED == instruct ||    /* 发送发动机的转速 */
             SEND_SPEED == instruct ||           /* 发送时速 */
             SEND_VOLTAGE_OF_BATTERY == instruct /* 发送电池电压 */
             )                                   // 如果指令的总长度为6个字节
    {
        uart0_sendbyte(0x06);           // 发送指令的总长度
        uart0_sendbyte(instruct);       // 发送指令
        uart0_sendbyte(send_data >> 8); // 发送信息
        uart0_sendbyte(send_data);      // 发送信息

        check_num += 0x06 + (u8)instruct + (u8)(send_data >> 8) + (u8)(send_data);
    }
    else if (SEND_TOTAL_MILEAGE == instruct ||      /* 发送大计里程 */
             SEND_SUBTOTAL_MILEAGE == instruct ||   /* 发送小计里程 */
             SEND_SUBTOTAL_MILEAGE_2 == instruct || /* 发送小计里程2 */
             SEND_TOUCH_KEY_STATUS == instruct      /* 发送触摸按键的状态 */
             )                                      // 如果指令的总长度为7个字节
    {

        uart0_sendbyte(0x07);     // 发送指令的总长度
        uart0_sendbyte(instruct); // 发送指令

        uart0_sendbyte(send_data >> 16); // 发送信息
        uart0_sendbyte(send_data >> 8);  // 发送信息
        uart0_sendbyte(send_data);       // 发送信息

        check_num += 0x07 + (u8)instruct + (u8)(send_data >> 16) + (u8)(send_data >> 8) + (u8)send_data;
    }
    // else if (SEND_DATE == instruct /* 发送日期(年月日) */
    //          )                     // 如果指令的总长度为8个字节
    // {
    //     uart0_sendbyte(0x08);     // 发送指令的总长度
    //     uart0_sendbyte(instruct); // 发送指令

    //     uart0_sendbyte(send_data >> 24); // 发送信息
    //     uart0_sendbyte(send_data >> 16); // 发送信息
    //     uart0_sendbyte(send_data >> 8);  // 发送信息
    //     uart0_sendbyte(send_data);       // 发送信息

    //     check_num += 0x08 + (u8)instruct + (u8)(send_data >> 24) + (u8)(send_data >> 16) + (u8)(send_data >> 8) + (u8)send_data;
    // }
    else if (SEND_TIME == instruct /* 发送时间 */)
    {
        uart0_sendbyte(0x0B);      // 发送指令的总长度
        uart0_sendbyte(SEND_TIME); // 发送指令

        // uart0_sendbyte(send_data >> 24); // 发送信息
        // uart0_sendbyte(send_data >> 16); // 发送信息
        // uart0_sendbyte(send_data >> 8);  // 发送信息
        // uart0_sendbyte(send_data);       // 发送信息

        uart0_sendbyte(fun_info.aip1302_saveinfo.year >> 8);
        uart0_sendbyte(fun_info.aip1302_saveinfo.year & 0xFF);
        uart0_sendbyte(fun_info.aip1302_saveinfo.month);
        uart0_sendbyte(fun_info.aip1302_saveinfo.day);
        uart0_sendbyte(fun_info.aip1302_saveinfo.time_hour);
        uart0_sendbyte(fun_info.aip1302_saveinfo.time_min);
        uart0_sendbyte(fun_info.aip1302_saveinfo.time_sec);

        check_num += 0x0B +
                     (u8)SEND_TIME +
                     (u8)(fun_info.aip1302_saveinfo.year >> 8) +
                     (u8)(fun_info.aip1302_saveinfo.year & 0xFF) +
                     (u8)(fun_info.aip1302_saveinfo.month) +
                     (u8)(fun_info.aip1302_saveinfo.day) +
                     (u8)(fun_info.aip1302_saveinfo.time_hour) +
                     (u8)(fun_info.aip1302_saveinfo.time_min) +
                     (u8)(fun_info.aip1302_saveinfo.time_sec);
    }

    // check_num &= 0x0F;         // 取前面的数字相加的低四位
    check_num &= 0xFF;         // 取前面的数字相加的低八位
    uart0_sendbyte(check_num); // 发送校验和

    delay_ms(1);
    // delay_ms(10); // 每次发送完成后，延时10ms
}
