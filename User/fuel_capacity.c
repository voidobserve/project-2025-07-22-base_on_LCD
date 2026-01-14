#include "fuel_capacity.h"

#if FUEL_CAPACITY_SCAN_ENABLE

// 计算油量百分比时，使用到的对照表
const fuel_calibration_t fuel_calibration_buff[] = {
    {FUEL_TANK_0_ADC_VAL_BEGIN, FUEL_TANK_0_ADC_VAL_END, 0, 13},
    {FUEL_TANK_1_ADC_VAL_BEGIN, FUEL_TANK_1_ADC_VAL_END, 14, 27},
    {FUEL_TANK_2_ADC_VAL_BEGIN, FUEL_TANK_2_ADC_VAL_END, 28, 41},
    {FUEL_TANK_3_ADC_VAL_BEGIN, FUEL_TANK_3_ADC_VAL_END, 42, 55},
    {FUEL_TANK_4_ADC_VAL_BEGIN, FUEL_TANK_4_ADC_VAL_END, 56, 69},
    {FUEL_TANK_5_ADC_VAL_BEGIN, FUEL_TANK_5_ADC_VAL_END, 70, 83},
    {FUEL_TANK_6_ADC_VAL_BEGIN, FUEL_TANK_6_ADC_VAL_END, 84, 100},
};

volatile u16 fuel_capacity_scan_cnt; // 扫描时间计数，在1ms定时器中断中累加

/*
    标志位，（如果当前油量百分比与上一次的油量百分比不在同一个油量格数下）
    是否要更新上一次的油量百分比
    由定时器置一，软件清零
*/
volatile bit flag_update_fuel_gear;
/*
    标志位，是否让定时器累计累计要更新油量挡位的时间
    如果当前油量百分比与上一次的油量百分比不在同一个挡位，由软件置一，定时器开始累计时间（flag_timer_scan_update_fuel_gear_cnt）
*/
volatile bit flag_timer_scan_update_fuel_gear;
/*
    定时器扫描计数，累计要更新油量挡位的时间
    由定时器累加
*/
volatile u16 timer_scan_update_fuel_gear_cnt;

// 滑动平均：
#define SAMPLE_COUNT 20 // 样本计数
static volatile u16 samples[SAMPLE_COUNT];
static volatile u8 sample_index;
u16 get_filtered_adc(u16 adc_val)
{
    u8 i = 0;
    u32 sum = 0;
    samples[sample_index++] = adc_val;
    if (sample_index >= SAMPLE_COUNT)
        sample_index = 0;

    for (i = 0; i < SAMPLE_COUNT; i++)
        sum += samples[i];

    return sum / SAMPLE_COUNT;
}

// 给滑动平均使用到的数组进行初始化
void samples_init(u16 adc_val)
{
    u8 i = 0;
    for (; i < SAMPLE_COUNT; i++)
    {
        samples[i] = adc_val;
    }
}

// 将油量检测对应的ad值转换成百分比值
u8 convert_fuel_adc_to_percent(u16 fuel_adc_val)
{
    u8 ret = 0;

#if USE_MY_DEBUG
    // 打印采集到的ad值：
    // printf("fuel_adc_val = %u\n", fuel_adc_val);
#endif

    // 如果ad值接近参考电压值，说明没有接油量检测模块，只剩下了外部上拉电路
    // 这里结合客户给到的数据：如果检测脚电压值大于2.23V，说明没有接油量检测模块，程序里面用2.3V作为阈值
    if (fuel_adc_val >= (u16)((u32)2300 * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE))
    {
        ret = 0xFF; // 根据串口收发协议，0xFF对应没有接油量检测
    }
    else
    {
        /*
            用采集到的ad值减去当前油量格数最小的ad值，再除以（当前油量格数最大的ad值 - 当前油量格数最小的ad值），
            得到采集到的ad值对应当前油量格数的占比，再乘以当前油量格数对应的最大百分比，便映射到当前的百分比

            由于ad值越大，油量反而越小，这里的百分比值是反过来的，
            还需要用当前油量格数对应的最大百分比 减去 当前计算得到的百分比
        */
        u8 buff_index = 0; // 存放当前ad值对应的油量格数
        for (buff_index = 0; buff_index < ARRAY_SIZE(fuel_calibration_buff); buff_index++)
        {
            if (fuel_adc_val <= fuel_calibration_buff[buff_index].adc_val_begin &&
                fuel_adc_val >= fuel_calibration_buff[buff_index].adc_val_end)
            {
                break;
            }
        }

        // printf("buff_index = %bu\n", buff_index);

        ret = (u32)(fuel_adc_val - fuel_calibration_buff[buff_index].adc_val_end) *
              fuel_calibration_buff[buff_index].cur_tank_max_percent /
              (fuel_calibration_buff[buff_index].adc_val_begin -
               fuel_calibration_buff[buff_index].adc_val_end);
        ret = fuel_calibration_buff[buff_index].cur_tank_max_percent - ret;
        if (ret < fuel_calibration_buff[buff_index].cur_tank_min_percent)
        {
            // 上面的计算结果有可能是0，这里要给一个当前油量格数对应的最小百分比
            ret = fuel_calibration_buff[buff_index].cur_tank_min_percent;
        }
    }

    /*
        -> 需要修改成：
        14以下，油桶以及第一格同时闪烁
        14及以上，显示1格
        28及以上，显示2格
        42及以上，显示3格
        56及以上，显示4格
        70及以上，显示5格
        84及以上，显示6格

        255 油量格数整段闪但油桶不闪（跟油量悬空状态一样）
    */

#if USE_MY_DEBUG
    // 打印最后计算得到的百分比值：
    // printf("fuel percent = %bu\n", ret);
#endif

    return ret;
}

// 将油量百分比转换成油量挡位
u8 convert_fuel_percent_to_gear(u8 fuel_percent)
{
    u8 fuel_gear = 0;
    if (0 == fuel_percent)
    {
        fuel_gear = 0;
    }
    else if (fuel_percent >= 1 && fuel_percent <= 17)
    {
        fuel_gear = 1;
    }
    else if (fuel_percent >= 18 && fuel_percent <= 33)
    {
        fuel_gear = 2;
    }
    else if (fuel_percent >= 34 && fuel_percent <= 50)
    {
        fuel_gear = 3;
    }
    else if (fuel_percent >= 51 && fuel_percent <= 67)
    {
        fuel_gear = 4;
    }
    else if (fuel_percent >= 68 && fuel_percent <= 83)
    {
        fuel_gear = 5;
    }
    // else if (fuel_percent >= 84)
    else
    {
        fuel_gear = 6;
    }

    return fuel_gear;
}

enum
{
    STATUS_JUST_POWER_ON = 0, // 刚上电
    STATUS_IN_SERVICE,        // 运行中
};

void fuel_capacity_scan(void)
{
    u8 fuel_percent = 0;
    u16 fuel_adc_val = 0;

    /*
        记录上一次采集到的油量挡位
        用于控制每隔 40s 更新一次油量的格数，0~6格油量
    */
    static u8 fuel_gear = 0;

    /*
        刚上电直接获取一次，作为油量的状态
    */
    {
        static u8 status = STATUS_JUST_POWER_ON;
        if (STATUS_JUST_POWER_ON == status) // 如果是第一次上电
        {
            if (fuel_capacity_scan_cnt >= FUEL_UPDATE_TIME_WHEN_POWER_ON)
            {
                fuel_capacity_scan_cnt = 0;
                adc_sel_pin(ADC_PIN_FUEL); // 内部至少占用1ms
                adc_val = adc_getval();    //
                samples_init(adc_val);     // 滑动平均滤波初始化
                fuel_adc_val = adc_val;

                fuel_percent = convert_fuel_adc_to_percent(fuel_adc_val);

                fuel_gear = convert_fuel_percent_to_gear(fuel_percent);

                // printf("power on, fuel_percent:%bu\n", fuel_percent);
                fun_info.fuel = fuel_percent;
                fuel_adc_val = 0;
                flag_get_fuel = 1;

                status = STATUS_IN_SERVICE;
            }
        }
    }

    adc_sel_pin(ADC_PIN_FUEL); // 内部至少占用1ms
    adc_val = adc_getval();    //
    fuel_adc_val = get_filtered_adc(adc_val);

    if (fuel_capacity_scan_cnt >= FUEL_UPDATE_TIME)
    {
        u8 cur_fuel_gear = 0;
        // 如果到了扫描更新时间
        fuel_capacity_scan_cnt = 0;

        // printf("adc_val: %u\n", adc_val);
        // printf("fuel_adc_val: %u\n", fuel_adc_val);
        fuel_percent = convert_fuel_adc_to_percent(fuel_adc_val);
        cur_fuel_gear = convert_fuel_percent_to_gear(fuel_percent);

        if (cur_fuel_gear != fuel_gear)
        {
            /*
                如果当前油量挡位和之前的油量挡位不同，
                给对应的标志位置一，让定时器累计时间
            */
            flag_timer_scan_update_fuel_gear = 1;
        }
        else
        {
            // 如果当前油量挡位与上一次的油量挡位相同
            flag_timer_scan_update_fuel_gear = 0;

            fun_info.fuel = fuel_percent;
            flag_get_fuel = 1; // 发送油量百分比数据
        }

        // 如果更新油量的时间到来，也发送一次油量百分比数据，更新 last_fuel_percent
        if (flag_update_fuel_gear)
        {
            flag_update_fuel_gear = 0;

#if 1
            // 每次更新油量百分比，只变化一个挡位；
            // 只有当前油量百分比与上一次记录的油量百分比不同，且更新油量挡位的时间到来，才会进入
            if (cur_fuel_gear != fuel_gear)
            {
                u8 i;

                if (fuel_gear > cur_fuel_gear)
                {
                    if (fuel_gear > 0)
                    {
                        fuel_gear--;
                    }
                }
                else
                {
                    if (fuel_gear < 6)
                    {
                        fuel_gear++;
                    }
                }

                for (i = 0; i < 255; i++)
                {
                    u8 tmp = convert_fuel_percent_to_gear(i);
                    if (tmp == fuel_gear)
                    {
                        fuel_percent = i; // 得到变化一个挡位后对应的油量百分比
                        break;
                    }
                }
            }
#endif
            fun_info.fuel = fuel_percent;
#if USE_MY_DEBUG
            // printf("fuel_percent: %bu\n", fuel_percent);
#endif 
            flag_get_fuel = 1; // 发送油量百分比数据
        }
    }
}

#endif // FUEL_CAPACITY_SCAN_ENABLE
