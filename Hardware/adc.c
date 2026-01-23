#include "adc.h"

#if (BATTERY_SCAN_ENABLE || AD_KEY_ENABLE || FUEL_CAPACITY_SCAN_ENABLE || TEMP_OF_WATER_SCAN_ENABLE)

static volatile u8 adc_status = ADC_STATUS_NONE;

// adc配置，使用adc时还需要切换到对应的引脚通道
void adc_config(void)
{
#if BATTERY_SCAN_ENABLE
    // P04--测量电池电压的引脚
    P0_MD1 |= GPIO_P04_MODE_SEL(0x3); // 模拟模式
#endif

#if AD_KEY_ENABLE
    // P05-- ad按键检测
    P0_MD1 |= GPIO_P05_MODE_SEL(0x3); // 模拟模式
#endif

#if FUEL_CAPACITY_SCAN_ENABLE
    // 检测油量的引脚：
    P0_MD0 |= GPIO_P01_MODE_SEL(0x3); // 模拟模式
#endif

#if TEMP_OF_WATER_SCAN_ENABLE
// 检测水温的引脚:
// P0_MD0 |= GPIO_P00_MODE_SEL(0x3); // 模拟模式
#endif // TEMP_OF_WATER_SCAN_ENABLE

    ADC_CFG1 |= (0x0F << 3) | // ADC时钟分频为16分频，为系统时钟/16
                (0x01 << 0);  // adc0中断使能
    ADC_CFG2 = 0xFF;          // 通道0采样时间配置为256个采样时钟周期

    // ADC_ACON1 &= ~((0x01 << 6) | (0x01 << 5) | (0x07 << 0)); // 关闭ADC中内部参考能使信号，关闭ADC外部参考选择信号，清空ADC内部参考电压的选择配置
    // ADC_ACON1 |= (0x03 << 3) | (0x06 << 0);                  // 关闭测试信号，选择内部VCCA作为参考电压（使用VCCA作为参考电压，需要关闭内部使能参考和外部使能参考）
    ADC_ACON0 = ADC_CMP_EN(0x1) |  // 打开ADC中的CMP使能信号
                ADC_BIAS_EN(0x1) | // 打开ADC偏置电流能使信号
                ADC_BIAS_SEL(0x1); // 偏置电流选择：1x

    ADC_TRGS0 |= (0x07 << 4); // 通道 0DLY 的 ADC 时钟个数选择，配置为 4n+1，4 * 29 + 1
    ADC_CHS0 |= (0x01 << 6);  // 使能 通道 0DLY 功能

    __EnableIRQ(ADC_IRQn); // 使能ADC中断
    IE_EA = 1;             // 使能总中断
}

// 切换adc扫描的引脚
// adc_pin--adc引脚，在对应的枚举类型中定义
void adc_sel_pin(u8 adc_pin)
{
#if 1
    ADC_CHS0 &= ~((0x01 << 4) | (0x01 << 3) | (0x01 << 2) | (0x01 << 1) | (0x01 << 0)); // 清空选择的adc0通路

    switch (adc_pin)
    {
#if BATTERY_SCAN_ENABLE
    case ADC_PIN_BATTERY:                                     // 检测电池电量
        ADC_ACON1 &= ~((0x01 << 5) | (0x07 << 0));            // 关闭ADC外部参考选择信号，清空ADC内部参考电压的选择配置
        ADC_ACON1 |= (0x01 << 6) | (0x03 << 3) | (0x02 << 0); // 使能ADC内部参考信号，关闭测试信号内部参考电压选择2.4V
        ADC_CHS0 |= ADC_ANALOG_CHAN(0x04);                    // P04通路
        break;
#endif

#if AD_KEY_ENABLE
    // case ADC_PIN_TOUCH:                    // 检测触摸IC传过来的电压
    case ADC_PIN_KEY:                                            // 检测ad按键
        ADC_ACON1 &= ~((0x01 << 6) | (0x01 << 5) | (0x07 << 0)); // 关闭ADC中内部参考能使信号，关闭ADC外部参考选择信号，清空ADC内部参考电压的选择配置
        ADC_ACON1 |= (0x03 << 3) | (0x06 << 0);                  // 关闭测试信号，选择内部VCCA作为参考电压（使用VCCA作为参考电压，需要关闭内部使能参考和外部使能参考）
        ADC_CHS0 |= ADC_ANALOG_CHAN(0x05);                       // P05通路

        break;
#endif
// ==============================================================================================================
#if FUEL_CAPACITY_SCAN_ENABLE
    case ADC_PIN_FUEL: // 检测油量
    {
        ADC_ACON1 &= ~((0x01 << 5) | (0x07 << 0));            // 关闭ADC外部参考选择信号，清空ADC内部参考电压的选择配置
        ADC_ACON1 |= (0x01 << 6) | (0x03 << 3) | (0x02 << 0); // 使能ADC内部参考信号，关闭测试信号，内部参考电压选择 2.4 V
        ADC_CHS0 |= ADC_ANALOG_CHAN(0x01);                    // P01通路
    }
    break;
#endif
    }

    ADC_CFG0 |= ADC_CHAN0_EN(0x1) | // 使能通道0转换
                ADC_EN(0x1);        // 使能A/D转换

    // delay_ms(1); // 等待ADC模块配置稳定，需要等待20us以上
    // delay(500);

#endif
}

#if 0
// 获取adc值，存放到变量adc_val中(adc单次转换)
u16 adc_single_convert(void)
{
    ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
    while (!(ADC_STA & ADC_CHAN0_DONE(0x1)))
        ;                                           // 等待转换完成
    ADC_STA = ADC_CHAN0_DONE(0x1);                  // 清除ADC0转换完成标志位
    return ((ADC_DATAH0 << 4) | (ADC_DATAL0 >> 4)); // 读取ADC0的值
}

// adc采集+滤波
u16 adc_getval(void)
{
    u16 adc_val_tmp = 0;     // 存放单次采集到的ad值
    u32 adc_val_sum = 0;     // 存放所有采集到的ad值的累加
    u16 get_adcmax = 0;      // 存放采集到的最大的ad值
    u16 get_adcmin = 0xFFFF; // 存放采集到的最小的ad值(初始值为最大值)
    u8 i = 0;

    for (i = 0; i < 20; i++)
    {
        adc_val_tmp = adc_single_convert(); // 获取一次ad值
        if (i < 2)
            continue; // 丢弃前两次采样的
        if (adc_val_tmp > get_adcmax)
            get_adcmax = adc_val_tmp; // 更新当前采集到的最大值
        if (adc_val_tmp < get_adcmin)
            get_adcmin = adc_val_tmp; // 更新当前采集到的最小值
        adc_val_sum += adc_val_tmp;
    }

    adc_val_sum -= get_adcmax;        // 去掉一个最大
    adc_val_sum -= get_adcmin;        // 去掉一个最小
    adc_val_tmp = (adc_val_sum >> 4); // 除以16，取平均值

    return adc_val_tmp;
}
#endif

/**
 * @brief 控制adc通道切换的函数（由于adc通道切换后要等待稳定，这里改成非阻塞延时的方式：定时调用通道切换+中断获取ad值）
 *
 * @note 1ms调用一次，每次在函数内切换通道->开启转换->切换通道->开启转换->...
 *
 * @return * void
 */
void adc_channel_handle(void)
{

    if (adc_status == ADC_STATUS_SEL_FUEL_END ||
        adc_status == ADC_STATUS_NONE)
    {
        adc_sel_pin(ADC_PIN_BATTERY);
        adc_status = ADC_STATUS_SEL_BATTERY_BEGIN;
    }
    else if (adc_status == ADC_STATUS_SEL_BATTERY_BEGIN)
    {
        ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
        adc_status = ADC_STATUS_SEL_BATTERY_END;
    }
    else if (adc_status == ADC_STATUS_SEL_BATTERY_END)
    {
        adc_sel_pin(ADC_PIN_KEY);
        adc_status = ADC_STATUS_SEL_AD_KEY_BEGIN;
    }
    else if (adc_status == ADC_STATUS_SEL_AD_KEY_BEGIN)
    {
        ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
        adc_status = ADC_STATUS_SEL_AD_KEY_END;
    }
    else if (adc_status == ADC_STATUS_SEL_AD_KEY_END)
    {
        adc_sel_pin(ADC_PIN_FUEL);
        adc_status = ADC_STATUS_SEL_FUEL_BEGIN;
    }
    else if (adc_status == ADC_STATUS_SEL_FUEL_BEGIN)
    {
        ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
        adc_status = ADC_STATUS_SEL_FUEL_END;
    }
}

void ADC_IRQHandler(void) interrupt ADC_IRQn
{
    u16 adc_val; // 由后续赋值

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(ADC_IRQn);

    // ---------------- 用户函数处理 -------------------

    if (ADC_STA & ADC_CHAN0_DONE(0x01))
    {
        ADC_STA |= ADC_CHAN0_DONE(0x01);                 // 清除ADC0转换完成标志位
        adc_val = (ADC_DATAH0 << 4) | (ADC_DATAL0 >> 4); // 读取ADC0的值

        if (adc_status == ADC_STATUS_SEL_BATTERY_END)
        {
            adc_update_battery_val(adc_val);
        }
        else if (adc_status == ADC_STATUS_SEL_AD_KEY_END)
        {
            adc_update_ad_key_val(adc_val);
        }
        else if (adc_status == ADC_STATUS_SEL_FUEL_END)
        {
            adc_update_fuel_val(adc_val);
        }
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(ADC_IRQn);
}

#endif // #if (BATTERY_SCAN_ENABLE || AD_KEY_ENABLE || FUEL_CAPACITY_SCAN_ENABLE || TEMP_OF_WATER_SCAN_ENABLE)
