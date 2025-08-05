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

// DEBUG
// volatile bit flag_is_debug_update = 0; // 测试时使用
volatile bit flag_debug_is_send_time = 0; // 测试时使用
// volatile bit flag_debug_is_send_time_2 = 0; // 测试时使用

void user_init(void)
{
    // 控制上电指示灯的引脚：
    P2_MD0 &= ~(GPIO_P23_MODE_SEL(0x03)); // 清空寄存器配置
    P2_MD0 |= GPIO_P23_MODE_SEL(0x01);    // 输出模式
    FOUT_S23 |= GPIO_FOUT_AF_FUNC;

    

    // tmr0_config();  // 串口检测数据超时需要使用到的定时器
    uart0_config(); // 发送和接收指令使用到的串口

#if PIN_LEVEL_SCAN_ENABLE
    pin_level_scan_config(); // 刹车、转向灯、挡位的检测引脚配置
#endif

    tmr1_config(); // 检测一段时间内的脉冲个数所需的定时器(用于计时)

#if SPEED_SCAN_ENABLE
    speed_scan_config(); // 时速扫描的配置
#endif                   // SPEED_SCAN_ENABLE

#if ENGINE_SPEED_SCAN_ENABLE
    engine_speed_scan_config(); // 发动机转速扫描的配置
#endif                          // #if ENGINE_SPEED_SCAN_ENABLE

#if (BATTERY_SCAN_ENABLE || AD_KEY_ENABLE || FUEL_CAPACITY_SCAN_ENABLE || TEMP_OF_WATER_SCAN_ENABLE)
    adc_config();
#endif

#if TOUCH_KEY_ENABLE
    tk_param_init(); // 触摸按键模块初始化
#endif

#if IC_1302_ENABLE
    aip1302_config(); // 初始化时钟ic，函数内部会读取时间信息，并存放到全局变量中
#endif

    tmr2_config(); // 扫描脉冲(电平变化)的定时器

    // tmr1_enable(); // 打开 检测引脚电平、检测时速、发动机转速、更新里程、定时检测油量 使用的定时器 【现在直接放到了tmr1_config()函数中】
    // tmr2_enable(); // 打开定时检测脉冲的定时器

    iic_config();

    fun_info_init(); // 初始化用于存放信息的变量（要放在iic初始化后面）

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

    printf("sys reset\n");

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

#if 1
        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending

#if TOUCH_KEY_ENABLE
        /* 按键扫描函数 */
        __tk_scan();                 // 使用了库里面的接口（闭源库）
        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending
#endif

#if AD_KEY_ENABLE
        key_driver_scan(&ad_key_para);
        ad_key_handle(); // ad按键处理函数
#endif                   //  #if AD_KEY_ENABLE

#if TOUCH_KEY_ENABLE
        key_driver_scan(&touch_key_para);
        touch_key_handle(); // 触摸按键处理函数
#endif
#if PIN_LEVEL_SCAN_ENABLE
        pin_level_scan();
#endif
#if SPEED_SCAN_ENABLE

        speed_scan();      // 检测时速
        speed_send_data(); // 发送时速数据，需要时间到来才会执行

#if 0 // 调试使用
        if (flag_debug_is_send_time)
        {
            if (flag_get_speed == 0) // 等待上一次发送完成
            {
                static u16 tmp = 0;
                tmp += 10;
                if (tmp >= 170)
                {
                    tmp = 0;
                }

                fun_info.speed = tmp;
                // fun_info.speed = 140;
                flag_get_speed = 1;

                flag_debug_is_send_time = 0;
            }
        }
#endif

#endif // #if SPEED_SCAN_ENABLE

#if ENGINE_SPEED_SCAN_ENABLE
        engine_speed_scan();      // 检测发动机转速
        engine_speed_send_data(); // 发动发动机转速数据，需要时间到来才会执行

#if 0 // 调试使用
        if (flag_debug_is_send_time_2)
        {
            if (flag_get_engine_speed == 0) // 等待上一次发送完成
            {
                static u32 tmp = 0;
                tmp += 100;
                if (tmp >= 13000)
                {
                    tmp = 0;
                }

                fun_info.engine_speeed = tmp;
                // fun_info.speed = 140;
                flag_get_engine_speed = 1;

                flag_debug_is_send_time_2 = 0;
            }
        }
#endif

#endif // #if ENGINE_SPEED_SCAN_ENABLE

        mileage_scan(); // 检测大计里程和小计里程

#if FUEL_CAPACITY_SCAN_ENABLE
        fuel_capacity_scan(); // 油量检测
#endif                        // FUEL_CAPACITY_SCAN_ENABLE
#if BATTERY_SCAN_ENABLE
        battery_scan(); // 电池电量检测
#endif                  // BATTERY_SCAN_ENABLE

#if 0
        // DEBUG
        if (flag_is_debug_update)
        {
            flag_is_debug_update = 0;

            // fun_info.fuel++;
            // if (fun_info.fuel > 100)
            // {
            //     fun_info.fuel = 0;
            // }

            // {
            //     static u16 cnt = 0;
            //     if (cnt < 4095 - 100)
            //     {
            //         cnt += 100;
            //         fuel_adc_val = cnt;
            //     }
            //     else
            //     {
            //         cnt = 0;
            //         fuel_adc_val = cnt;
            //     }

            //     // flag_get_fuel = 1;
            // }

            fun_info.save_info.total_mileage += 1000;
            flag_get_total_mileage = 1;

            fun_info.save_info.subtotal_mileage += 1000;
            flag_get_sub_total_mileage = 1;

            fun_info.save_info.subtotal_mileage_2 += 1000;
            flag_get_sub_total_mileage_2 = 1;

            fun_info.engine_speeed += 100;
            flag_get_engine_speed = 1;

            {
                static u8 cnt = 0;

                fun_info.speed = cnt;
                cnt++;
                if (cnt >= 255)
                {
                    cnt = 0;
                }

                flag_get_speed = 1;
            } 

            {
                static u8 fuel_percent = 0;

                /*
                    记录上一次采集到的油量挡位
                    用于控制每隔 40s 更新一次油量的格数，0~6格油量
                */
                static u8 fuel_gear = 0;

                if (fuel_capacity_scan_cnt >= 40000)
                // if (fuel_capacity_scan_cnt >= 4000)
                {
                    u8 cur_fuel_gear = 0;
                    u8 i;
                    // 如果到了扫描更新时间
                    fuel_capacity_scan_cnt = 0;

                    fuel_gear++;
                    if (fuel_gear > 6)
                    {
                        fuel_gear = 0;
                    }

                    for (i = 0; i < 255; i++)
                    {
                        extern u8 convert_fuel_percent_to_gear(u8 fuel_percent); 
                        u8 tmp = convert_fuel_percent_to_gear(i);
                        if (tmp == fuel_gear)
                        {
                            fuel_percent = i; // 得到变化一个挡位后对应的油量百分比
                            break;
                        }
                    }

                } //  if (fuel_capacity_scan_cnt >= FUEL_UPDATE_TIME)

                fun_info.fuel = fuel_percent;
                flag_get_fuel = 1; // 发送油量百分比数据
            }
        }
#endif

        uart0_scan_handle();  // 检查串口接收缓冲区的数据是否符合协议,如果有正确的指令，会存到另一个缓冲区中（接下来让instruction_scan()函数来处理）
        instruction_scan();   // 扫描是否有合法的指令
        instruction_handle(); // 扫描是否有对应的获取/状态更新操作(最占用时间,因为要等待串口的数据发送完成)

        if (flag_debug_is_send_time)
        {
            flag_debug_is_send_time = 0;
            eeprom_printf_all();
        }

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

#if 0
        if (flag_debug_is_send_time)
        {
            flag_debug_is_send_time = 0;
            aip1302_read_all();
            printf("year %u \n", fun_info.aip1302_saveinfo.year);
            printf("month %bu \n", fun_info.aip1302_saveinfo.month);
            printf("day %bu \n", fun_info.aip1302_saveinfo.day);
            printf("hour %bu \n", fun_info.aip1302_saveinfo.time_hour);
            printf("min %bu \n", fun_info.aip1302_saveinfo.time_min);
            printf("sec %bu \n", fun_info.aip1302_saveinfo.time_sec);
        }
#endif

        // heart_beat_handle(); //

        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending
    }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2022 HUGE-IC ***** END OF FILE *****/
