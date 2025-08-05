#include "speed_scan.h"

#if SPEED_SCAN_ENABLE

#if 0
// 标志位，是否由更新计数,由定时器来置位
// 0--未更新脉冲计数，1--有新的脉冲计数
volatile bit flag_is_update_speed_pulse_cnt = 0;
volatile u16 speed_scan_time_cnt = 0;        // 速度扫描时，用到的时间计数值，会在定时器中断中累加
volatile u16 speed_actual_scan_time_cnt = 0; // 存放实际的速度扫描时间(实际的速度扫描用时)
// volatile u32 detect_speed_pulse_cnt = 0; // 检测时速的脉冲计数值(用IO中断来检测)
/*
    存放 检测到的时速脉冲计数值，会在中断内累加
    使用了双缓冲，[0]用在定时器中断中，[1]用在处理函数中
    当 flag_is_update_speed_pulse_cnt == 1时，说明已经有数据更新
*/
volatile u32 detect_speed_pulse_cnt[2] = {0}; // 检测时速的脉冲计数值
#endif

// 时速扫描的配置
void speed_scan_config(void)
{
#if 1 // 使用定时器扫描IO电平变化来计算脉冲

    P1_MD1 &= ~GPIO_P15_MODE_SEL(0x3); // 输入模式
    P1_PU |= GPIO_P15_PULL_UP(0x1);    // 配置为上拉

#endif // 使用定时器扫描IO电平变化来计算脉冲
}
  
static volatile u8 speed_buff[SPEED_SCAN_BUFF_SIZE] = {0};
static volatile u8 cur_send_speed_buff_index = 0;
volatile bit flag_is_send_speed_time_come = 0; // 标志位，发送速度的时间到来

volatile bit flag_is_speed_scan_over_time = 0; // 速度检测是否一直没有脉冲到来，导致超时
volatile u32 speed_pulse_cnt = 0;              // 记录脉冲个数，在定时器中断累加
volatile u16 speed_scan_time_ms = 0;           // 记录扫描时间
static volatile u32 cur_speed_scan_time = 0;
static volatile u32 cur_speed_scan_pulse = 0;

void update_speed_scan_data(void) // 更新检测时速的数据
{
    cur_speed_scan_time += speed_scan_time_ms;
    speed_scan_time_ms = 0;
    cur_speed_scan_pulse += speed_pulse_cnt;
    speed_pulse_cnt = 0;
}

void speed_scan(void)
{
    volatile u16 cur_speed = 0;
    u32 tmp = 0;

    if (cur_speed_scan_time >= SPEED_SCAN_UPDATE_TIME || flag_is_speed_scan_over_time)
    {
        /*
            采集到的脉冲个数 / 一圈对应的脉冲个数 * 车轮一圈对应走过的距离（单位：mm），
            计算得到 采集的脉冲个数对应走过的距离（单位：mm）
        */
        // u32 tmp = (cur_speed_scan_pulse * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN);
        tmp = (((u32)cur_speed_scan_pulse * SPEED_SCAN_MM_PER_TURN) / SPEED_SCAN_PULSE_PER_TURN);
        // printf("cur_speed_scan_pulse %lu\n", cur_speed_scan_pulse);

        if (flag_is_speed_scan_over_time) // 超时，采集到的脉冲个数对应一直是0km/h，认为时速是0
        {
            cur_speed = 0;
        }
        else // 未超时，计算采集到的脉冲个数对应走过的距离，再转换成以km/h的单位
        {
            /*
                采集的脉冲个数对应走过的距离（单位：mm）/ 采集所用的时间（单位：ms） == 速度（单位：mm/ms）
                1mm/ms == 1m/s
                因为 1mm/ms * 1000 == 1m/ms
                     1m/ms  / 1000 == 1m/s
                     先乘以1000再除以1000，那么这个操作就可以去掉，直接化简为 1mm/ms == 1m/s

                1m/s == 3.6km/h，那么根据得到的 以 m/s 为单位的速度，先乘以36再除以10，得到以km/h为单位的速度值

                cur_speed == 采集的脉冲个数对应走过的距离（单位：mm）/ 采集所用的时间（单位：ms）* 3.6
                换成单片机可以计算的格式：
                cur_speed == 采集的脉冲个数对应走过的距离（单位：mm） * 36 / 10 / 采集所用的时间（单位：ms）
            */
            cur_speed = (u32)tmp * 36 / 10 / cur_speed_scan_time;
        }

        // 防止时速为0时（有可能是推车，记录不到速度），记录不到里程
        distance += tmp;

        // printf("cur distace 2 %lu\n", distance);
        // printf("cur distace %lu\n", distance);
        // printf("cur speed %lu km/h\n", cur_speed);

        cur_speed_scan_pulse = 0;
        cur_speed_scan_time = 0;
        flag_is_speed_scan_over_time = 0;

        // 限制要发送的时速:
        if (cur_speed > 999) // 999 km/h
        {
            cur_speed = 999;
        }

#if 0
        /*
            扫描完时速就发送的程序，在显示部分会有卡顿，
            显示做不了动画，只能单片机来调节
        */
        fun_info.speed = cur_speed;
        // 限制要发送的时速:
        if (fun_info.speed > 999)
        {
            fun_info.speed = 999;
        }

        flag_get_speed = 1; // 表示速度有数据更新
#endif

        speed_buff_update(cur_speed);

    } // if (cur_speed_scan_time >= 500 || flag_is_speed_scan_over_time)
}

void speed_buff_update(u16 speed)
{
    static u16 last_speed = 0;    // 存放上一次采集到的速度
    u16 speed_difference = 0;     // 存放速度的差值
    bit dir_of_speed_change = 0; // 速度变化的方向，0--速度变小，1--速度变大
    u8 i = 0;                    // 循环计数值

    if (speed > last_speed)
    {
        // 如果当前的速度 大于 上一次采集到的速度
        speed_difference = speed - last_speed;
        dir_of_speed_change = 1; // 表示速度变大
    }
    else if (speed < last_speed)
    {
        // 如果当前的速度 小于 上一次采集到的速度
        speed_difference = last_speed - speed;
        dir_of_speed_change = 0; // 表示速度变小
    }
    else
    {
        for (i = 0; i < SPEED_SCAN_BUFF_SIZE; i++)
        {
            speed_buff[i] = speed;
        }

        // // 没有差值，直接更新（修复没有差值且数值为0时，没有发送数据的问题）
        // fun_info.speed = speed;
        // flag_get_engine_speed = 1;

        cur_send_speed_buff_index = 0; // 游标复位
        return;
    }

    if (dir_of_speed_change)
    {
        // 如果速度在变大，数组从 [0] ~ [SPEED_SCAN_BUFF_SIZE - 1] 数值越来越大
        for (i = 0; i < SPEED_SCAN_BUFF_SIZE; i++)
        {
            speed_buff[i] = speed_difference * (i + 1) / SPEED_SCAN_BUFF_SIZE + last_speed;
        }
    }
    else
    {
        // 如果速度在变小，数组从 [0] ~ [SPEED_SCAN_BUFF_SIZE - 1] 数值越来越小
        for (i = 0; i < SPEED_SCAN_BUFF_SIZE; i++)
        {
            speed_buff[SPEED_SCAN_BUFF_SIZE - 1 - i] = last_speed - (u32)speed_difference * (SPEED_SCAN_BUFF_SIZE - i - 1) / SPEED_SCAN_BUFF_SIZE;
        }
    }

    last_speed = speed;
    cur_send_speed_buff_index = 0; // 游标复位
}

void speed_send_data(void)
{
    if (flag_is_send_speed_time_come) // 如果发送速度的时间到来
    {
        flag_is_send_speed_time_come = 0;

        if (cur_send_speed_buff_index >= SPEED_SCAN_BUFF_SIZE)
        {
            // 防止越界
            return;
        }

        fun_info.speed = speed_buff[cur_send_speed_buff_index];
        cur_send_speed_buff_index++;

        // printf("fun_info.speed = %lu\n", fun_info.speed);
        flag_get_speed = 1;
    }
}

#endif // SPEED_SCAN_ENABLE
