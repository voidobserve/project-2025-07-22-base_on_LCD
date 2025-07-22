// 里程表的源程序
#include "mileage.h"

volatile u32 mileage_save_time_cnt = 0; // 里程扫描所需的计数值,每隔一定时间将里程写入flash
volatile u32 distance = 0;              // 存放每次扫描时走过的路程（单位：毫米）-->用于里程表的计数

volatile u16 mileage_update_time_cnt = 0; // 里程更新的时间计数,每隔一段时间更新一次当前里程（负责控制发送里程的周期）

// 总里程扫描
void mileage_scan(void)
{
    static u32 old_total_mileage = 0;    // 用来记录旧的大计里程的变量
    static u32 old_subtotal_mileage = 0; // 用来记录旧的小计里程的变量

    // static u16 mileage_update_time_cnt = 0; // 里程更新的时间计数,每隔一段时间更新一次当前里程（负责控制发送里程的周期）
    // mileage_update_time_cnt += ONE_CYCLE_TIME_MS;

    if (mileage_save_time_cnt >= 30000) // 30 000 ms -- 30s
    {
        mileage_save_time_cnt = 0;
        fun_info_save(); // 将 fun_info 写回flash
                         // printf("fun_info_save()");

#ifdef USE_MY_DEBUG
#if USE_MY_DEBUG
        // printf("fun_info_save()");
#endif // #if USE_MY_DEBUG
#endif // #ifdef USE_MY_DEBUG
    }

    if (distance >= 1000) // 1000mm -- 1m
    {
        // 如果走过的距离超过了1m，再进行保存（保存到变量）

        // // 限制大计里程在 1000000000000 mm(1000,000km)以内
        // if (fun_info.save_info.total_mileage < (u32)1000000000000 - 1001)
        // {
        //     fun_info.save_info.total_mileage += distance / 1000; // 将毫米转换成米，再保存
        // }

        // 限制大计里程在 1000,000km(1,000,000,000m)以内
        // if (fun_info.save_info.total_mileage < (u32)1000000000)
        // if (fun_info.save_info.total_mileage < (u32)(0xFFFFFFFF))
        if (fun_info.save_info.total_mileage < (u32)(999999 * 1000)) // 99 9999 KM
        {
            fun_info.save_info.total_mileage++; // +1m
        }

        // // 限制小计里程在 1,000,000,000 mm(1000km)以内
        // if (fun_info.save_info.subtotal_mileage < (u32)1000000000 - 1001)
        // {
        //     fun_info.save_info.subtotal_mileage += distance / 1000; // 将毫米转换成米，再保存
        // }

        // 限制小计里程在 1000km(1,000,000m)以内
        // if (fun_info.save_info.subtotal_mileage < (u32)1000000)
        // if (fun_info.save_info.subtotal_mileage < (u32)65535000) // 发送出去最大是 0xFFFF，单位百米
        if (fun_info.save_info.subtotal_mileage < (u32)(9999999)) // 9999.9KM， 9999 999 m
        {
            fun_info.save_info.subtotal_mileage++; // +1m
        }

        if (fun_info.save_info.subtotal_mileage_2 < (u32)(9999999)) // 9999.9KM， 9999 999 m
        {
            fun_info.save_info.subtotal_mileage_2++; // +1m
        }

        distance -= 1000; // 剩下的、未保存的、不满1m的数据留到下一次再保存
    }

#ifdef USE_INTERNATIONAL /* 公制单位 */
                         // 如果大计里程有变化且超过了100m(不能满1000m再发送，在显示上，会先更新大计里程，过几百ms才更新小计里程)
    if ((fun_info.save_info.total_mileage - old_total_mileage) > 100)
#else /* USE_IMPERIAL // 英制单位 */
    // 如果大计里程有变化且超过了161m(0.1英里)
    if ((fun_info.save_info.total_mileage - old_total_mileage) > 161)
#endif
    {
        old_total_mileage = fun_info.save_info.total_mileage; // 记录旧的里程

        // printf("total mileage: %lu m\n", fun_info.save_info.total_mileage);

        // 发送数据的操作，可以先置标志位
        flag_get_total_mileage = 1;
    }

#ifdef USE_INTERNATIONAL /* 公制单位 */
                         // 如果小计里程有变化且超过了100m
    if ((fun_info.save_info.subtotal_mileage - old_subtotal_mileage) > 100)
#else /* USE_IMPERIAL // 英制单位 */
    // 如果小计里程有变化且超过了161m(0.1英里)
    if ((fun_info.save_info.subtotal_mileage - old_subtotal_mileage) > 161)
#endif
    {
        old_subtotal_mileage = fun_info.save_info.subtotal_mileage; // 记录旧的里程

        // printf("subtotal mileage: %lu m\n", fun_info.save_info.subtotal_mileage);

        // 发送数据的操作，可以先置标志位
        flag_get_sub_total_mileage = 1;
    }

    if (mileage_update_time_cnt >= MILEAGE_UPDATE_TIME_MS)
    {
        // 每隔一段时间，发送大小里程，
        // 因为最后大计里程在999999km,小计里程在999.9km之后，就不更新了，
        // 要在刷新一次，才会发送1000000km和1000.0km的大小里程
        mileage_update_time_cnt = 0;
        flag_get_total_mileage = 1; //
        flag_get_sub_total_mileage = 1;
        flag_get_sub_total_mileage_2 = 1;
    }
}
