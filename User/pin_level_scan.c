#include "pin_level_scan.h"

u16 pin_level_scan_time_cnt = 0;

void pin_level_scan_config(void)
{
    p21_input_config(); // 远光灯状态对应的引脚
    p22_input_config(); // 右转向灯状态对应的引脚
    // p23_input_config(); // 刹车状态对应的引脚
    p25_input_config(); // 左转向灯状态对应的引脚

    p27_input_config(); // 6档对应的引脚
    p30_input_config(); // 5档对应的引脚
    p14_input_config(); // 4档对应的引脚
    p13_input_config(); // 3档对应的引脚
    p10_input_config(); // 2档对应的引脚
    p07_input_config(); // 1档对应的引脚
    p06_input_config(); // 空挡对应的引脚

    // 检测故障状态的引脚:
    P2_MD0 &= ~(GPIO_P20_MODE_SEL(0x03)); // 输入模式
    P2_PU |= GPIO_P20_PULL_UP(0x01);      // 上拉

    // 测试用
    // P0_MD0 &= ~(GPIO_P02_MODE_SEL(0x03)); // 输入模式
    // P0_PU |= GPIO_P02_PULL_UP(0x01);      // 上拉
}

// 引脚电平扫描，都是低电平有效
void pin_level_scan(void)
{
    // 如果到了扫描时间，再更新挡位、转向灯和刹车的状态
    if (pin_level_scan_time_cnt >= PIN_LEVEL_SCAN_TIME_MS)
    {
        pin_level_scan_time_cnt = 0;

#if 0  // 刹车检测
        // if (PIN_DETECT_BRAKE)
        // {
        //     // 如果没有刹车
        //     fun_info.brake = OFF;
        // }
        // else
        // {
        //     // 如果有刹车
        //     fun_info.brake = ON;
        // }

        // flag_get_brake = 1;
#endif // 刹车检测

        if (PIN_DETECT_LEFT_TURN)
        {
            // 如果左转向灯未开启
            fun_info.left_turn = OFF;
        }
        else
        {
            // 如果左转向灯开启
            fun_info.left_turn = ON;
        }
        flag_get_left_turn = 1;

        if (PIN_DETECT_RIGHT_TURN)
        {
            // 如果右转向灯未开启
            fun_info.right_turn = OFF;
        }
        else
        {
            // 如果右转向灯开启
            fun_info.right_turn = ON;
        }
        flag_get_right_turn = 1;

        if (PIN_DETECT_HIGH_BEAM)
        {
            // 如果远光灯未开启
            fun_info.high_beam = OFF;
        }
        else
        {
            // 如果远光灯开启
            fun_info.high_beam = ON;
        }
        flag_get_high_beam = 1;

        // 以最低挡位优先，当最低档有信号时，不管其他挡位的信号，直接以最低档的为主

        if (0 == PIN_DETECT_NEUTRAL_GEAR)
        {
            // 空挡
            fun_info.gear = GEAR_NEUTRAL;
        }
        else if (0 == PIN_DETECT_FIRST_GEAR)
        {
            // 一档
            fun_info.gear = GEAR_FIRST;
        }
        else if (0 == PIN_DETECT_SECOND_GEAR)
        {
            // 二档
            fun_info.gear = GEAR_SECOND;
        }
        else if (0 == PIN_DETECT_THIRD_GEAR)
        {
            // 三档
            fun_info.gear = GEAR_THIRD;
        }
        else if (0 == PIN_DETECT_FOURTH_GEAR)
        {
            // 四档
            fun_info.gear = GEAR_FOURTH;
        }
        else if (0 == PIN_DETECT_FIFTH_GEAR)
        {
            // 五档
            fun_info.gear = GEAR_FIFTH;
        }
        else if (0 == PIN_DETECT_XIXTH_GEAR)
        {
            // 六档
            fun_info.gear = GEAR_SIXTH;
        }

        // printf("cur gear %bu\n", fun_info.gear);
        flag_get_gear = 1;

        if (0 == PIN_DETECT_MALFUNCTION)
        {
            // 如果检测到了故障
            fun_info.flag_is_detect_malfunction = 1;

            // 没有引脚检测abs的状态，这里检测到故障后，也顺便设置abs的状态
            fun_info.flag_is_detect_abs = 1;
        }
        else
        {
            // 如果未检测到故障
            fun_info.flag_is_detect_malfunction = 0;

            // 没有引脚检测abs的状态，这里更新故障的状态后，也顺便设置abs的状态
            fun_info.flag_is_detect_abs = 0;
        }

        //   printf("cur malfunction %bu\n", fun_info.flag_is_detect_malfunction);

        flag_update_malfunction_status = 1;
    }

    // if () // ACC引脚检测，检测到高电平，P03也输出高电平，检测到低电平，让P03输出低电平
    // if (P04)
    // {
    //     // 检测到ACC为高电平,
    //     P03 = 1;
    // }
    // else
    // {
    //     P03 = 0;
    // }
}
