#include "fuel_capacity.h"

#if FUEL_CAPACITY_SCAN_ENABLE

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

    // 如果超出了 最大油量的ad值和最小油量的ad值之间的范围 ，说明没有接油量检测
    if (fuel_adc_val >= (4095 - 500) ||
        fuel_adc_val <= (0 + 500))
    {
        ret = 0xFF; // 根据串口收发协议，0xFF对应没有接油量检测
    }
    else
    {
        if (fuel_adc_val > FUEL_LEVEL_0_ADC_VAL) // 如果检测到的ad值比最小油量对应的ad值还要大
        {
            ret = 0; // 0% 油量
        }
        else if (fuel_adc_val <= FUEL_LEVEL_6_ADC_VAL) // 如果检测到的油量比100%还要大一些
        {
            ret = 100;
        }
        else if (fuel_adc_val <= FUEL_LEVEL_0_ADC_VAL && fuel_adc_val > FUEL_LEVEL_1_ADC_VAL) /* 0格油量到1格油量，0% ~ 18% */
        {
            /*
                用采集到的ad值减去当前挡位最小的ad值，再除以（当前挡位最大的ad值-当前挡位最小的ad值），得到
                采集到的ad值对应当前挡位的占比，再乘以当前挡位对应的百分比
            */
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_1_ADC_VAL) * 18 / ((u32)FUEL_LEVEL_0_ADC_VAL - FUEL_LEVEL_1_ADC_VAL);
            ret = 18 - ret;
        }
        else if (fuel_adc_val <= FUEL_LEVEL_1_ADC_VAL && fuel_adc_val > FUEL_LEVEL_2_ADC_VAL) /* 1格油量到2格油量，18% ~ 34% */
        {
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_2_ADC_VAL) * 34 / ((u32)FUEL_LEVEL_1_ADC_VAL - FUEL_LEVEL_2_ADC_VAL);
            ret = 34 - ret;
            if (ret < 18)
            {
                ret = 18;
            }
        }
        else if (fuel_adc_val <= FUEL_LEVEL_2_ADC_VAL && fuel_adc_val > FUEL_LEVEL_3_ADC_VAL) /* 2格油量到3格油量，34% ~ 51% */
        {
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_3_ADC_VAL) * 51 / ((u32)FUEL_LEVEL_2_ADC_VAL - FUEL_LEVEL_3_ADC_VAL);
            ret = 51 - ret;
            if (ret < 34)
            {
                ret = 34;
            }
        }
        else if (fuel_adc_val <= FUEL_LEVEL_3_ADC_VAL && fuel_adc_val > FUEL_LEVEL_4_ADC_VAL) /* 3格油量到4格油量，51% ~ 68% */
        {
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_4_ADC_VAL) * 68 / ((u32)FUEL_LEVEL_3_ADC_VAL - FUEL_LEVEL_4_ADC_VAL);
            ret = 68 - ret;
            if (ret < 51)
            {
                ret = 51;
            }
        }
        else if (fuel_adc_val <= FUEL_LEVEL_4_ADC_VAL && fuel_adc_val > FUEL_LEVEL_5_ADC_VAL) /* 4格油量到5格油量，68% ~ 84% */
        {
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_5_ADC_VAL) * 84 / ((u32)FUEL_LEVEL_4_ADC_VAL - FUEL_LEVEL_5_ADC_VAL);
            ret = 84 - ret;
            if (ret < 68)
            {
                ret = 68;
            }
        }
        else if (fuel_adc_val <= FUEL_LEVEL_5_ADC_VAL && fuel_adc_val > FUEL_LEVEL_6_ADC_VAL) /* 5格油量到6格油量，84% ~ 100.0% */
        {
            ret = ((u32)fuel_adc_val - FUEL_LEVEL_6_ADC_VAL) * 100 / ((u32)FUEL_LEVEL_5_ADC_VAL - FUEL_LEVEL_6_ADC_VAL);
            ret = 100 - ret;
            if (ret < 84)
            {
                ret = 84;
            }
        }
    }

    // 0，显示0格，游标闪烁
    // 18以下，不含18，显示1格
    // ret = 18; // 18及以上，显示2格
    // ret = 34; // 34及以上，显示3格
    // ret = 51; // 51及以上，显示4格
    // ret = 68; // 68及以上，显示5格
    // ret = 84; // 84及以上，显示6格
    return ret;
}

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

// volatile u16 fuel_adc_val = 0; // DEBUG 测试用
void fuel_capacity_scan(void)
{
    u8 fuel_percent = 0;
    u16 fuel_adc_val = 0;

    /*
        记录上一次采集到的油量挡位
        用于控制每隔 40s 更新一次油量的格数，0~6格油量
    */
    static u8 fuel_gear = 0;

    // printf("fuel_adc_val: %u\n", fuel_adc_val);

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

        fuel_percent = convert_fuel_adc_to_percent(fuel_adc_val);
        cur_fuel_gear = convert_fuel_percent_to_gear(fuel_percent);

        if (cur_fuel_gear != fuel_gear)
        {
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
            flag_get_fuel = 1; // 发送油量百分比数据
        }

        // printf("fuel_percent:%bu\n", fuel_percent);
        // fun_info.fuel = fuel_percent;
        // fuel_adc_val = 0;
        // flag_get_fuel = 1;
    } //  if (fuel_capacity_scan_cnt >= FUEL_UPDATE_TIME)
}

#endif // FUEL_CAPACITY_SCAN_ENABLE
