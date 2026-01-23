#ifndef __FUEL_CAPACITY_H__
#define __FUEL_CAPACITY_H__

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

#if FUEL_CAPACITY_SCAN_ENABLE

/*
    油量检测脚外围：

    油量检测脚 + 2.2K（与检测脚串联） + （220R上拉 +　油量模块作为下拉）

    补充：下拉额外串联了一个0.7V压降的二极管，
    例如，检测脚检测到的电压是 1.81V，则油量检测模块不接地的那一端的电压是1.11V
*/

// 检测油量时，adc使用的参考电压，单位：mV
#define FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE (2400)
// 油量检测脚外部上拉的电压（可能不是5V，这样会影响整个油量检测结果）（单位：mV）
// #define FUEL_CAPACITY_SCAN_EXTERNAL_PULL_UP_VOLTAGE (4430)

/*
    根据客户提供的数据，定义各个油量挡位对应的电压值阈值（单位：mV）
*/
enum
{
    FUEL_TANK_0_VOLTAGE_BEGIN = (u16)2230,
    FUEL_TANK_0_VOLTAGE_END = (u16)1750,

    FUEL_TANK_1_VOLTAGE_BEGIN = (u16)1750,
    FUEL_TANK_1_VOLTAGE_END = (u16)1650,

    FUEL_TANK_2_VOLTAGE_BEGIN = (u16)1650,
    FUEL_TANK_2_VOLTAGE_END = (u16)1590,

    FUEL_TANK_3_VOLTAGE_BEGIN = (u16)1590,
    FUEL_TANK_3_VOLTAGE_END = (u16)1400,

    FUEL_TANK_4_VOLTAGE_BEGIN = (u16)1400,
    FUEL_TANK_4_VOLTAGE_END = (u16)1200,

    FUEL_TANK_5_VOLTAGE_BEGIN = (u16)1200,
    FUEL_TANK_5_VOLTAGE_END = (u16)950,

    FUEL_TANK_6_VOLTAGE_BEGIN = (u16)950,
    // FUEL_TANK_6_VOLTAGE_END = (u16)100, // 客户给到的数据
    FUEL_TANK_6_VOLTAGE_END = (u16)0, // 由于客户给到 100 与 0 很接近，这里直接用0
};

/*
    根据客户提供的数据，定义各个油量档位对应的ad值阈值

    ad值 == 电压值(单位：mV) * 4096 / adc参考电压（单位：mV） 
*/
enum
{
    FUEL_TANK_0_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_0_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_0_ADC_VAL_END = (u16)((u32)FUEL_TANK_0_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_1_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_1_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_1_ADC_VAL_END = (u16)((u32)FUEL_TANK_1_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_2_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_2_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_2_ADC_VAL_END = (u16)((u32)FUEL_TANK_2_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_3_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_3_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_3_ADC_VAL_END = (u16)((u32)FUEL_TANK_3_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_4_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_4_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_4_ADC_VAL_END = (u16)((u32)FUEL_TANK_4_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_5_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_5_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_5_ADC_VAL_END = (u16)((u32)FUEL_TANK_5_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),

    FUEL_TANK_6_ADC_VAL_BEGIN = (u16)((u32)FUEL_TANK_6_VOLTAGE_BEGIN * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
    FUEL_TANK_6_ADC_VAL_END = (u16)((u32)FUEL_TANK_6_VOLTAGE_END * 4096 / FUEL_CAPACITY_SCAN_ADC_REF_VOLTAGE),
};

// 定义计算油量百分比时使用到的数据
typedef struct 
{
    u16 adc_val_begin; // 当前油量格数 ad值范围起点
    u16 adc_val_end;   // 当前油量格数 ad值范围终点
    u8 cur_tank_min_percent; // 当前油量格数对应的油量百分比最小值
    u8 cur_tank_max_percent; // 当前油量格数对应的油量百分比最大值

} fuel_calibration_t;


/*
    根据客户提供的数据，定义各个油量格数对应的ad值
    单片机使用内部2V参考电压，12位精度
*/
enum
{
    FUEL_LEVEL_0_ADC_VAL = (u16)((u32)1750 * 4096 / 2 / 1000), /* 油量为0 单片机引脚测得1.448V */
    FUEL_LEVEL_1_ADC_VAL = (u16)((u32)1650 * 4096 / 2 / 1000), /* 油量为一格 */
    FUEL_LEVEL_2_ADC_VAL = (u16)((u32)1590 * 4096 / 2 / 1000), /* 油量为两格 */
    FUEL_LEVEL_3_ADC_VAL = (u16)((u32)1400 * 4096 / 2 / 1000), /* 油量为三格 */
    FUEL_LEVEL_4_ADC_VAL = (u16)((u32)1200 * 4096 / 2 / 1000), /* 油量为四格 单片机引脚测得0.9924 V */
    FUEL_LEVEL_5_ADC_VAL = (u16)((u32)950 * 4096 / 2 / 1000),  /* 油量为五格 单片机引脚测得0.888 V */
    /* 大于五格，油量为6格 */
    FUEL_LEVEL_6_ADC_VAL = (u16)((u32)800 * 4096 / 2 / 1000), /* 假设 单片机引脚测得0.800 V是对应6格100%油量 */
    /* 如果采集到的ad值接近0，或是接近4095，说明没有接油量检测模块 */
};

// 油量最大时，对应的ad值
// #define FUEL_MAX_ADC_VAL (950)
// // 油量最小时，对应的ad值
// #define FUEL_MIN_ADC_VAL (2000)

// 刚上电时，更新油量的时间，单位：ms
#define FUEL_UPDATE_TIME_WHEN_POWER_ON (500)
// 更新油量的时间，单位：ms
#define FUEL_UPDATE_TIME ((u16)1000)

// 如果当前油量百分比与上一次油量百分比不在同一个挡位，要更新这个挡位所需的时间，单位：ms
#define FUEL_UPDATE_LAST_FUEL_PERCENTAGE_TIME ((u16)40000)
// #define FUEL_UPDATE_LAST_FUEL_PERCENTAGE_TIME ((u16)2000) // USER_TO_DO 测试时使用，实际要改回去

// 油量检测配置
// ======================================================

extern u16 fuel_capacity_scan_cnt; // 扫描时间计数，在1ms定时器中断中累加

/*
    标志位，（如果当前油量百分比与上一次的油量百分比不在同一个油量格数下）
    是否要更新上一次的油量百分比
    由定时器置一，软件清零
*/
extern volatile bit flag_update_fuel_gear;
/*
    标志位，是否让定时器累计累计要更新上一次油量百分比的时间
    如果当前油量百分比与上一次的油量百分比不在同一个挡位，由软件置一，定时器开始累计时间（flag_timer_scan_update_fuel_gear_cnt）
*/
extern volatile bit flag_timer_scan_update_fuel_gear;
/*
    定时器扫描计数，累计要更新上一次油量百分比的时间
    由定时器累加
*/
extern volatile u16 timer_scan_update_fuel_gear_cnt;

// extern volatile u16 fuel_adc_val; // DEBUG 测试用
void adc_update_fuel_val(u16 adc_val);
u16 adc_get_fuel_val(void);

void fuel_capacity_scan(void);

#endif // FUEL_CAPACITY_SCAN_ENABLE
#endif
