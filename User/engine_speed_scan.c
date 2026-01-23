#include "engine_speed_scan.h"

#if ENGINE_SPEED_SCAN_ENABLE

// static volatile u32 engine_speed_buff[ENGINE_SPEED_SCAN_BUFF_SIZE];
// static volatile u8 cur_send_engine_speed_buff_index; // 当前要发送的缓冲区索引
// volatile bit flag_is_send_engine_speed_time_come;    // 标志位，发送发动机转速的时间到来

volatile u32 engine_speed_scan_pulse_cnt; // 检测到的脉冲个数，在定时器中断累加
volatile u16 engine_speed_scan_ms;        // 在定时器中断累加

static volatile u32 cur_engine_speed_scan_cnt;
static volatile u32 cur_engine_speed_scan_ms;

// volatile bit flag_is_engine_speed_scan_over_time; // 标志位，检测是否超时

// 发动机转速的相关配置
void engine_speed_scan_config(void)
{
#if 1 // 使用定时器扫描IO电平的方式

    P0_MD0 &= ~GPIO_P02_MODE_SEL(0x3); // 输入模式
    P0_PU |= GPIO_P02_PULL_UP(0x01);   // 上拉

#endif // 使用定时器扫描IO电平的方式
}

void update_engine_speed_scan_data(void) // 更新检测发动机转速的数据
{
    cur_engine_speed_scan_cnt += engine_speed_scan_pulse_cnt;
    engine_speed_scan_pulse_cnt = 0;
    cur_engine_speed_scan_ms += engine_speed_scan_ms;
    engine_speed_scan_ms = 0;
}

/**
 * @brief 获取两个发动机转速的差值
 *
 */
u16 get_diff_engine_speed(u16 cur_rpm, u16 last_rpm)
{
    u16 diff_rpm = 0;

    if (cur_rpm >= last_rpm)
    {
        diff_rpm = cur_rpm - last_rpm;
    }
    else
    {
        diff_rpm = last_rpm - cur_rpm;
    }

    return diff_rpm;
}

// 发动机转速扫描
void engine_speed_scan(void)
{
    static volatile u16 last_rpm;
    volatile u32 rpm; // 由下面的语句赋值，这里为了节省程序空间，没有赋初始值
    volatile u16 diff_rpm;

    // IE_EA = 0;

    if (cur_engine_speed_scan_ms >= ENGINE_SPEED_SCAN_UPDATE_TIME)
    {
        /*
            (1min / 1min转过的圈数) == (扫描时间 / 扫描时间内的转过的圈数)
            1min转过的圈数 == 1min * 扫描时间内转过的圈数 / 扫描时间
            1min转过的圈数 == 1min * (扫描时间内采集到的脉冲个数 / 发动机转过一圈对应的脉冲个数) / 扫描时间
            转换成单片机能计算的形式：
            1min转过的圈数 == 扫描时间内采集到的脉冲个数 * 1min / 发动机转过一圈对应的脉冲个数 / 扫描时间
            1min转过的圈数 == 扫描时间内采集到的脉冲个数 * 1min / 扫描时间 / 发动机转过一圈对应的脉冲个数
        */
        // rpm = (u32)cur_engine_speed_scan_cnt * ((u32)CONVER_ONE_MINUTE_TO_MS / ENGINE_SPEED_SCAN_PULSE_PER_TURN) / cur_engine_speed_scan_ms;

        /*
            扫描时间内转过的圈数 == 一个脉冲对应转过的圈数 *　扫描时间内采集到的脉冲个数
            1min转过的圈数　== 扫描时间内转过的圈数 / 扫描时间 * 1min
        */
        // rpm = (u32)cur_engine_speed_scan_cnt *
        //       ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS *
        //       CONVER_ONE_MINUTE_TO_MS /
        //       cur_engine_speed_scan_ms;

        rpm = (u32)cur_engine_speed_scan_cnt *
              CONVER_ONE_MINUTE_TO_MS /
              cur_engine_speed_scan_ms;

        // printf("cur engine speed pulse cnt:%lu\n",cur_engine_speed_scan_cnt);

        cur_engine_speed_scan_cnt = 0;
        cur_engine_speed_scan_ms = 0;

        // 限制待发送的发动机转速
        if (rpm >= 65535)
        {
            rpm = 65535;
        }

        diff_rpm = get_diff_engine_speed((u16)(rpm & 0xFFFF), last_rpm);
        if (diff_rpm >= ENGINE_SPEED_MAX_DIFF_RPM) // 限制每次发送的发动机转速，差值不能太大，否则会导致LCD显示的滑动条跳动
        {
            u8 i = 0;

            if (last_rpm >= rpm)
            {
                // last_rpm -= ENGINE_SPEED_MAX_DIFF_RPM;
                for (i = 0; i < ENGINE_SPEED_MAX_DIFF_RPM; i++)
                {
                    if (last_rpm > 0)
                    {
                        last_rpm--;
                    }
                }
            }
            else
            {
                // last_rpm += ENGINE_SPEED_MAX_DIFF_RPM;
                for (i = 0; i < ENGINE_SPEED_MAX_DIFF_RPM; i++)
                {
                    if (last_rpm < 65535)
                    {
                        last_rpm++;
                    }
                }
            }
        }
        else
        {
            last_rpm = rpm;
        }

#if 1  
        fun_info.engine_speeed = last_rpm; // 向全局变量存放发动机转速
        flag_get_engine_speed = 1;         // 发送发动机转速
#endif 
    }

    // IE_EA = 1;
}

#endif // #if ENGINE_SPEED_SCAN_ENABLE
