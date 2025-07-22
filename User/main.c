/**
 ******************************************************************************
 * @file    main.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    05-11-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
 *
 * 版权说明后续补上
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "my_config.h"

#if 0

// 更新时间
// void time_update(void)
// {
//     if (tmr4_cnt >= 1000) // 如果计时满1s(1000ms)
//     {
//         tmr4_cnt -= 1000; // 为了尽量让时间准确，这里是 -=1000 ，而不是清零

//         if (fun_info.save_info.time_sec < 59) // 最大加到59s，测试通过
//         {
//             fun_info.save_info.time_sec++;
//         }
//         else
//         {
//             fun_info.save_info.time_sec = 0;
//             fun_info_save(); // 每分钟保存一次时间到flash

//             // printf("cur min %d \n", (u16)fun_info.time_min);

//             if (fun_info.save_info.time_min < 59) // 最大加到59min，测试通过
//             {
//                 fun_info.save_info.time_min++;
//             }
//             else
//             {
//                 fun_info.save_info.time_min = 0;
//                 // printf("cur hour %d \n", (u16)fun_info.time_hour);

//                 if (fun_info.save_info.time_hour < 23) // 最大加到23h，测试通过
//                 {
//                     fun_info.save_info.time_hour++;
//                 }
//                 else
//                 {
//                     // 加到 23:59:59 后，将时间全部清零
//                     fun_info.save_info.time_hour = 0;
//                     fun_info.save_info.time_min = 0;
//                     fun_info.save_info.time_sec = 0;
//                 }
//             }
//         }

// #if USE_MY_DEBUG // 测试单片机发送的数据
//         // printf("cur sec %d \n", (u16)fun_info.time_sec);

//         // 测试用，可以在这里修改数据，然后发送
//         {
//             /*
//                 #define TEST_SEND_BATTERY // 测试通过
//                 #define TEST_SEND_BRAKE // MP5上面没有显示，串口有发送对应的数据
//                 #define TEST_SEND_LEFT_TURN // 测试通过
//                 #define TEST_SEND_RIGHT_TURN // 测试通过
//                 #define TEST_SEND_HIGH_BEAM // 测试通过

//                 #define TEST_SEND_SPEED // 测试通过，但是要注意时速的显示范围
//                 #define TEST_SEND_ENGINE_SPEED // 测试通过
//                 #define TEST_SEND_FUEL // 测试通过

//                 #define TEST_SEND_TEMP_OF_WATER // 没有显示，改用水温报警
//                 #define TEST_SEND_TOTAL_MILEAGE // 测试通过
//                 #define TEST_SEND_SUBTOTAL_MILEAGE // 测试通过

//                 #define TEST_SEND_TOUCH_KEY
//                 #define TEST_SEND_TEMP_OF_WATER_WARNING // 测试通过
//                 #define TEST_SEND_VOLTAGE_OF_BATTERY // 测试通过
//             */

//             // #define TEST_SEND_VOLTAGE_OF_BATTERY

// #ifdef TEST_SEND_BATTERY
//             // =============================================
//             // 测试发送电池电量--单位：百分比
//             if (fun_info.battery < 100)
//             {
//                 fun_info.battery++;
//             }
//             else
//             {
//                 fun_info.battery = 0;
//             }

//             // printf("cur battery %d \%\n", (u16)fun_info.battery);
//             flag_get_battery = 1;
// #endif

// #ifdef TEST_SEND_BRAKE
//             // =============================================
//             // 测试发送 刹车状态
//             if (fun_info.brake)
//             {
//                 fun_info.brake = 0;
//             }
//             else
//             {
//                 fun_info.brake = 1;
//             }

//             flag_get_brake = 1;
// #endif

// #ifdef TEST_SEND_LEFT_TURN
//             // =============================================
//             // 测试发送 左转向灯
//             if (fun_info.left_turn)
//             {
//                 fun_info.left_turn = 0;
//             }
//             else
//             {
//                 fun_info.left_turn = 1;
//             }

//             flag_get_left_turn = 1;
// #endif

// #ifdef TEST_SEND_RIGHT_TURN
//             // =============================================
//             // 测试发送 右转向灯
//             if (fun_info.right_turn)
//             {
//                 fun_info.right_turn = 0;
//             }
//             else
//             {
//                 fun_info.right_turn = 1;
//             }

//             flag_get_right_turn = 1;
// #endif

// #ifdef TEST_SEND_HIGH_BEAM
//             // #if 1
//             // =============================================
//             // 测试发送 远光灯 状态
//             if (fun_info.high_beam)
//             {
//                 fun_info.high_beam = 0;
//             }
//             else
//             {
//                 fun_info.high_beam = 1;
//             }

//             flag_get_high_beam = 1;
// #endif

// #ifdef TEST_SEND_SPEED
//             // =============================================
//             // 测试发送 时速
//             if (fun_info.speed < 0xFFFF)
//             {
//                 fun_info.speed++;
//             }
//             else
//             {
//                 fun_info.speed = 0;
//             }

//             flag_get_speed = 1;
// #endif

// #ifdef TEST_SEND_ENGINE_SPEED
//             // =============================================
//             // 测试发送 发动机转速
//             if (fun_info.engine_speeed < (0xFFFF - 1000))
//             {
//                 fun_info.engine_speeed += 1000;
//             }
//             else
//             {
//                 fun_info.engine_speeed = 1000;
//             }

//             flag_get_engine_speed = 1;
// #endif

// #ifdef TEST_SEND_FUEL
//             // =============================================
//             // 测试发送 油量
//             if (fun_info.fuel < 0xFF)
//             {
//                 fun_info.fuel++;
//             }
//             else
//             {
//                 fun_info.fuel = 0;
//             }

//             flag_get_fuel = 1;

// #endif

// #ifdef TEST_SEND_TEMP_OF_WATER
//             // =============================================
//             // 测试发送 水温
//             if (fun_info.temp_of_water < 0xFF)
//             {
//                 fun_info.temp_of_water++;
//             }
//             else
//             {
//                 fun_info.temp_of_water = 0;
//             }

//             flag_get_temp_of_water = 1;
// #endif

// #ifdef TEST_SEND_TOTAL_MILEAGE
//             // =============================================
//             // 测试发送 大计里程
//             // fun_info.save_info.total_mileage = 90000;
//             if (fun_info.save_info.total_mileage < (0xFFFFFFFF - 1000))
//             {
//                 fun_info.save_info.total_mileage += 1000;
//             }
//             else
//             {
//                 fun_info.save_info.total_mileage = 0xFFFF0000;
//             }

//             flag_get_total_mileage = 1;

// #endif

// #ifdef TEST_SEND_SUBTOTAL_MILEAGE
//             // =============================================
//             // 测试发送 小计里程
//             if (fun_info.save_info.subtotal_mileage < 1000)
//             {
//                 fun_info.save_info.subtotal_mileage++;
//             }
//             else
//             {
//                 fun_info.save_info.subtotal_mileage = 0;
//             }

//             flag_get_sub_total_mileage = 1;
// #endif

// #ifdef TEST_SEND_TEMP_OF_WATER_WARNING
//             // =============================================
//             // 测试发送 水温报警

//             if (fun_info.flag_is_in_water_temp_warning)
//             {
//                 fun_info.flag_is_in_water_temp_warning = 0;
//             }
//             else
//             {
//                 fun_info.flag_is_in_water_temp_warning = 1;
//             }

//             flag_set_temp_of_water_warning = 1;
// #endif

// #ifdef TEST_SEND_VOLTAGE_OF_BATTERY
//             // 测试 发送电池电压
//             if (fun_info.voltage_of_battery < (0xFFFF - 10))
//             {
//                 fun_info.voltage_of_battery += 10;
//             }
//             else
//             {
//                 fun_info.voltage_of_battery = 10000;
//             }

//             flag_get_voltage_of_battery = 1;
// #endif
//         }

// #endif // #if USE_MY_DEBUG // 测试单片机发送的数据
//     }
// }
#endif

void user_init(void)
{

#if USE_MY_DEBUG

    uart1_config();

#endif

    // 控制上电指示灯的引脚：
    P2_MD0 &= ~(GPIO_P23_MODE_SEL(0x03)); // 清空寄存器配置
    P2_MD0 |= GPIO_P23_MODE_SEL(0x01);    // 输出模式
    FOUT_S23 |= GPIO_FOUT_AF_FUNC;

    fun_info_init(); // 初始化用于存放信息的变量

    tmr0_config();           // 串口检测数据超时需要使用到的定时器
    uart0_config();          // 发送和接收指令使用到的串口
    pin_level_scan_config(); // 刹车、转向灯、挡位的检测引脚配置

    tmr1_config(); // 检测一段时间内的脉冲个数所需的定时器(用于计时)

    speed_scan_config();        // 时速扫描的配置
    engine_speed_scan_config(); // 发动机转速扫描的配置

    adc_config();

    tk_param_init();  // 触摸按键模块初始化
    aip1302_config(); // 初始化时钟ic，函数内部会读取时间信息，并存放到全局变量中
    tmr2_config();    // 扫描脉冲(电平变化)的定时器

    tmr1_enable(); // 打开 检测引脚电平、检测时速、发动机转速、更新里程、定时检测油量 使用的定时器
    tmr2_enable(); // 打开定时检测脉冲的定时器

    // delay_ms(10); // 等待系统稳定
    // tk_param_init();  // 触摸按键模块初始化
    delay_ms(1); // 等待系统稳定
    // delay_ms(2000); // 等待系统稳定
}

void main(void)
{
    // 看门狗默认打开, 复位时间2s
    system_init();

    // WDT_KEY = WDT_KEY_VAL(0xDD); //  关闭看门狗

    // 关闭HCK和HDA的调试功能
    WDT_KEY = 0x55;  // 解除写保护
    IO_MAP &= ~0x01; // 清除这个寄存器的值，实现关闭HCK和HDA引脚的调试功能（解除映射）
    WDT_KEY = 0xBB;

    /* 用户代码初始化接口 */
    user_init();

    // 测试用到的配置：
    // P1_MD0 &= (~GPIO_P11_MODE_SEL(0x3));
    // P1_MD0 |= GPIO_P11_MODE_SEL(0x1); // 输出模式
    // FOUT_S11 |= GPIO_FOUT_AF_FUNC;
    // P11 = 0;
    // P2_MD0 &= ~GPIO_P20_MODE_SEL(0x3); // 清空对应的配置
    // P2_MD0 |= GPIO_P20_MODE_SEL(0x1);  // 输出模式
    // FOUT_S20 |= GPIO_FOUT_AF_FUNC;
    // P20 = 0;

    // printf("sys reset\n");

    // 上电后，需要点亮一下所有的指示灯，再关闭:
    P23 = 1;
    delay_ms(1000);
    P23 = 0;

    // printf("scan_times %bu\n", ad_key_para.scan_times);
    // printf("cur_scan_times %bu\n", ad_key_para.cur_scan_times);
    // printf("last_key %bu\n", ad_key_para.last_key);
    // printf("filter_value %bu\n", ad_key_para.filter_value);
    // printf("filter_cnt %bu\n", ad_key_para.filter_cnt);
    // printf("filter_time %bu\n", ad_key_para.filter_time);
    // printf("long_time %bu\n", ad_key_para.long_time);
    // printf("hold_time %bu\n", ad_key_para.hold_time);
    // printf("press_cnt %bu\n", ad_key_para.press_cnt);

    // printf("click_cnt %bu\n", ad_key_para.click_cnt);
    // printf("click_delay_cnt %bu\n", ad_key_para.click_delay_cnt);
    // printf("click_delay_time %bu\n", ad_key_para.click_delay_time);
    // printf("notify_value %bu\n", ad_key_para.notify_value);
    // printf("key_type %bu\n", ad_key_para.key_type);

    // printf("latest_key_val %bu\n", ad_key_para.latest_key_val);
    // printf("latest_key_event %bu\n", ad_key_para.latest_key_event);

    // printf("size %bu\n", (u8)sizeof(struct key_driver_para_t *));
    // printf("addr %p\n", (void *)&ad_key_para);

#if 0  // 测试发送日期和时间
    // 当前日期
    fun_info.aip1302_saveinfo.year = 2024;
    fun_info.aip1302_saveinfo.month = 12;
    fun_info.aip1302_saveinfo.day = 05;

    // 当前时间
    fun_info.aip1302_saveinfo.time_hour = 13;
    fun_info.aip1302_saveinfo.time_min = 45;
    fun_info.aip1302_saveinfo.time_sec = 32;

    aip1302_update_all_data(fun_info.aip1302_saveinfo);

    printf("year %u month %bu day %bu \n", fun_info.aip1302_saveinfo.year, fun_info.aip1302_saveinfo.month, fun_info.aip1302_saveinfo.day);
    printf("hour %bu min %bu sec %bu \n", fun_info.aip1302_saveinfo.time_hour, fun_info.aip1302_saveinfo.time_min, fun_info.aip1302_saveinfo.time_sec);
#endif // 测试发送日期和时间

    // printf("sys reset\n");

    // 测试程序：
    // fun_info.save_info.total_mileage = (u32)999970 * 1000;
    // // fun_info.save_info.subtotal_mileage = (u32)999970 * 100;
    // fun_info.save_info.subtotal_mileage = (u32)99997 * 100;
    // // fun_info.save_info.subtotal_mileage = (u32)10051 * 100;
    // fun_info.save_info.subtotal_mileage_2 = (u32)999970 * 100;

    /* 系统主循环 */
    while (1)
    {
        // printf("main circle\n");

#if USE_MY_DEBUG

        // if (flag_is_printf_time)
        // {
        //     flag_is_printf_time = 0;
        //     aip1302_read_all();
        //     printf("year %u \n", fun_info.aip1302_saveinfo.year);
        //     printf("month %bu \n", fun_info.aip1302_saveinfo.month);
        //     printf("day %bu \n", fun_info.aip1302_saveinfo.day);
        //     printf("hour %bu \n", fun_info.aip1302_saveinfo.time_hour);
        //     printf("min %bu \n", fun_info.aip1302_saveinfo.time_min);
        //     printf("sec %bu \n", fun_info.aip1302_saveinfo.time_sec);
        // }

#endif

#if 1
        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending
        /* 按键扫描函数 */
        __tk_scan();                 // 使用了库里面的接口（闭源库）
        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending

        key_driver_scan(&ad_key_para);
        ad_key_handle(); // ad按键处理函数

        key_driver_scan(&touch_key_para);
        touch_key_handle(); // 触摸按键处理函数

        speed_scan();   // 检测时速
        mileage_scan(); // 检测大计里程和小计里程

        engine_speed_scan();  // 检测发动机转速
        fuel_capacity_scan(); // 油量检测

        battery_scan(); // 电池电量检测

        uart0_scan_handle();  // 检查串口接收缓冲区的数据是否符合协议,如果有正确的指令，会存到另一个缓冲区中（接下来让instruction_scan()函数来处理）
        instruction_scan();   // 扫描是否有合法的指令
        instruction_handle(); // 扫描是否有对应的获取/状态更新操作(最占用时间,因为要等待串口的数据发送完成)

#endif //

        // aip1302_test();
        // {
        //     u8 ret = 0;
        //     ret = aip1302_is_running();
        //     if (ret)
        //     {
        //         // 如果时钟芯片aip1302的晶振正在运行
        //         printf("aip1302 is running\n");
        //     }
        //     else
        //     {
        //         // 如果时钟芯片aip1302的晶振不在运行
        //         printf("aip1302 is sleep\n");
        //     }
        // }

        // AIP1302_CE_PIN = 1;
        // delay_ms(500);
        // AIP1302_CE_PIN = 0;
        // delay_ms(500);

        heart_beat_handle(); //

        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending
    }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2022 HUGE-IC ***** END OF FILE *****/
