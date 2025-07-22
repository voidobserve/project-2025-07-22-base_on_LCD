#include "speed_scan.h"

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

/*
    采用定时检测脉冲的方式来检测速度，但是每段时间内会先计算速度，最后再求平均值，会有1~2km/h的误差
*/
#if 0 
// 速度扫描函数
void speed_scan(void)
{
    volatile u32 cur_speed = 0;                    // 记录当前采集到的速度
    static volatile u32 cur_speed_average_val = 0; // 存放当前速度的平均值(单位：km/h)
    static volatile u32 cur_speed_actual_scan_time_cnt = 0; // 存放当前 速度扫描时间

    static volatile u8 speed_scan_cnt = 0;

    if (flag_is_update_speed_pulse_cnt) // 如果有数据更新
    {
        flag_is_update_speed_pulse_cnt = 0;
        /*
            计算 xx ms内走过了多少毫米
            xx ms内走过了多少毫米 == 当前扫描时间内检测到的脉冲个数 / 车轮一圈对应多少个脉冲 * 一圈对应 xx 毫米
            换成单片机可以计算的形式：
            xx ms内走过了多少毫米 == 当前扫描时间内检测到的脉冲个数 * 一圈对应 xx 毫米 / 车轮一圈对应多少个脉冲
        */
        cur_speed = detect_speed_pulse_cnt[1] * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN;

        // printf("cur pulse cnt %lu \n", detect_speed_pulse_cnt[1]); // 临时测试用

        detect_speed_pulse_cnt[1] = 0; // 清空脉冲计数
        distance += cur_speed;         // 存放走过的距离，单位：毫米

        /*
            已知在扫描时间内走过了xx mm
            时速的计算公式:
            扫描时间内走过的距离 / 1000 * (1 / 扫描时间对1s的占比) * 3.6
                扫描时间内走过的距离 / 1000，转换成 m/扫描时间 的单位
                * (1 / 扫描时间对1s的占比)，转换成以s为单位的速度
                * 3.6，因为 1m/s == 3.6km/h，最后转换成 以km/h的单位
            转换成单片机可以计算的形式：
            时速 == 扫描时间内走过的距离 * 36 * (1 / 扫描时间对1s的占比) / 10000；

            逐渐变换成单片机可以计算的形式：
            cur_speed = cur_speed * 36 * (1 / (SPEED_SCAN_TIME_MS / 1000)) / 10000;
            cur_speed = cur_speed * 36 * 1000 / SPEED_SCAN_TIME_MS / 10000;
            cur_speed = cur_speed * 36 / SPEED_SCAN_TIME_MS / 10;
        */
        // cur_speed = (cur_speed * 36) / speed_actual_scan_time_cnt / 10;

        // printf("cur speed %lu \n", cur_speed);

        if (speed_scan_cnt < SPEED_SCAN_FILTER_CNT)
        {
            // 如果未达到重复检测的次数
            speed_scan_cnt++;
            cur_speed_average_val += cur_speed ; // 累加当前得到的时速(单位：km/h)
            cur_speed_actual_scan_time_cnt += speed_actual_scan_time_cnt ; // 累加当前

            // if (cur_speed)
            // {
            //     printf("ori speed %lu\n", cur_speed);
            // }
        }
        else
        {
            // 如果达到了重复检测的次数
            speed_scan_cnt = 0;
            cur_speed_average_val = (cur_speed_average_val * 36) / cur_speed_actual_scan_time_cnt / 10 ;
            // cur_speed_average_val /= SPEED_SCAN_FILTER_CNT; // 时速取平均值
            fun_info.speed = cur_speed_average_val;         // 存放得到的时速
            cur_speed_average_val = 0;                      // 清空变量的值
            cur_speed_actual_scan_time_cnt = 0;
            

            // printf("cur speed %lu km/h\n", fun_info.speed);
#if USE_MY_DEBUG

            if (fun_info.speed != 0)
            {
                printf("cur speed %lu km/h\n", fun_info.speed);
            }

#endif

            // 限制要发送的时速:
            if (fun_info.speed > 999)
            {
                fun_info.speed = 999;
            }

            flag_get_speed = 1; //
        }
    }
}
#endif

#if 0
// 速度扫描函数
void speed_scan(void)
{
    volatile u32 cur_speed_pulse_cnt = 0;                   // 记录当前采集到的速度
    static volatile u32 cur_speed_average_val = 0;          // 存放当前速度的平均值(单位：km/h)
    static volatile u32 cur_speed_actual_scan_time_cnt = 0; // 存放当前 速度扫描时间

    static volatile u8 speed_scan_cnt = 0;

    if (flag_is_update_speed_pulse_cnt) // 如果有数据更新
    {
        flag_is_update_speed_pulse_cnt = 0;
        /*
            计算 xx ms内走过了多少毫米
            xx ms内走过了多少毫米 == 当前扫描时间内检测到的脉冲个数 / 车轮一圈对应多少个脉冲 * 一圈对应 xx 毫米
            换成单片机可以计算的形式：
            xx ms内走过了多少毫米 == 当前扫描时间内检测到的脉冲个数 * 一圈对应 xx 毫米 / 车轮一圈对应多少个脉冲
        */
        // cur_speed_pulse_cnt = detect_speed_pulse_cnt[1] * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN;
        cur_speed_pulse_cnt = detect_speed_pulse_cnt[1]; // 得到一段时间内采集到的脉冲个数

        distance += detect_speed_pulse_cnt[1] * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN; // 存放走过的距离，单位：毫米
        detect_speed_pulse_cnt[1] = 0;                                                              // 清空脉冲计数

        if (speed_scan_cnt < SPEED_SCAN_FILTER_CNT)
        {
            // 如果未达到重复检测的次数
            speed_scan_cnt++;
            cur_speed_average_val += cur_speed_pulse_cnt;                 // 累加当前得到的脉冲个数
            cur_speed_actual_scan_time_cnt += speed_actual_scan_time_cnt; // 累加当前扫描速度所用的时间
        }
        else
        {
            // 如果达到了重复检测的次数
            speed_scan_cnt = 0;

            /*
                已知在扫描时间内走过了xx mm
                时速的计算公式:
                扫描时间内走过的距离 / 1000 * (1 / 扫描时间对1s的占比) * 3.6
                    扫描时间内走过的距离 / 1000，转换成 m/扫描时间 的单位
                    * (1 / 扫描时间对1s的占比)，转换成以s为单位的速度
                    * 3.6，因为 1m/s == 3.6km/h，最后转换成 以km/h的单位
                转换成单片机可以计算的形式：
                时速 == 扫描时间内走过的距离 * 36 * (1 / 扫描时间对1s的占比) / 10000；

                逐渐变换成单片机可以计算的形式：
            */
            cur_speed_average_val = (cur_speed_average_val * 36 * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN) / 10 / cur_speed_actual_scan_time_cnt;
            // cur_speed_average_val /= SPEED_SCAN_FILTER_CNT; // 时速取平均值
            fun_info.speed = cur_speed_average_val; // 存放得到的时速
            cur_speed_average_val = 0;              // 清空变量的值
            cur_speed_actual_scan_time_cnt = 0;

            printf("cur speed %lu km/h\n", fun_info.speed);
#if USE_MY_DEBUG

            if (fun_info.speed != 0)
            {
                printf("cur speed %lu km/h\n", fun_info.speed);
            }

#endif

            // 限制要发送的时速:
            if (fun_info.speed > 999)
            {
                fun_info.speed = 999;
            }

            flag_get_speed = 1; //
        }
    }
}
#endif

volatile bit flag_is_speed_scan_over_time = 0; // 速度检测是否一直没有脉冲到来，导致超时
volatile u32 speed_pulse_cnt = 0;              // 记录脉冲个数，在定时器中断累加
volatile u32 speed_scan_time_ms = 0;           // 记录扫描时间
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
    volatile u32 cur_speed = 0;
    u32 tmp = 0;

    if (cur_speed_scan_time >= SPEED_SCAN_UPDATE_TIME || flag_is_speed_scan_over_time)
    {
        /*
            采集到的脉冲个数 / 一圈对应的脉冲个数 * 车轮一圈对应走过的距离（单位：mm），
            计算得到 采集的脉冲个数对应走过的距离（单位：mm）
        */
        // u32 tmp = (cur_speed_scan_pulse * SPEED_SCAN_MM_PER_TURN / SPEED_SCAN_PULSE_PER_TURN);
        tmp = ((cur_speed_scan_pulse * SPEED_SCAN_MM_PER_TURN) / SPEED_SCAN_PULSE_PER_TURN);
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
            cur_speed = tmp * 36 / 10 / cur_speed_scan_time;
        }

        // 防止时速为0时（有可能是推车，记录不到速度），记录不到里程
        distance += tmp;

        // printf("cur distace 2 %lu\n", distance);
        // printf("cur distace %lu\n", distance);
        // printf("cur speed %lu km/h\n", cur_speed);

        cur_speed_scan_pulse = 0;
        cur_speed_scan_time = 0;
        flag_is_speed_scan_over_time = 0;

        fun_info.speed = cur_speed;
        // 限制要发送的时速:
        if (fun_info.speed > 999)
        {
            fun_info.speed = 999;
        }

        flag_get_speed = 1; // 表示速度有数据更新
    } // if (cur_speed_scan_time >= 500 || flag_is_speed_scan_over_time)
}
