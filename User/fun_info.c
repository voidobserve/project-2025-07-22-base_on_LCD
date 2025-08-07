#include "fun_info.h"
#include <string.h> // memset()

volatile fun_info_t fun_info; // 存放所有功能状态的结构体变量

// 初始化存放所有信息的结构体变量
void fun_info_init(void)
{
    u8 page_id;
    u32 temp_total_mileage; // 临时存放从eeprom读出的总里程数据

    /* 全局变量，上电应该默认为0 */
    // fun_info.save_info.total_mileage = 0;
    // fun_info.save_info.subtotal_mileage = 0;
    // fun_info.save_info.subtotal_mileage_2 = 0;

    eeprom_menu_init();
    page_id = eeprom_menu_prev.cur_write_page_id; // 获取目录的id

    /*
        应该将同一组的数据都读出来，比较总里程的大小，以较大的为最新的数据。
        较小的可能是因为写入eeprom时突然掉电，中断了写入
    */
    // 根据从目录读取到的页面id，找到对应的数据
    eeprom_24cxx_read(page_id, (u8 *)&eeprom_saveinfo, sizeof(eeprom_saveinfo_t));
    if (eeprom_saveinfo.is_data_valid == EEPROM_DATA_VALID_VAL)
    {
        fun_info.save_info.total_mileage = eeprom_saveinfo.total_mileage;
        fun_info.save_info.subtotal_mileage = eeprom_saveinfo.subtotal_mileage;
        fun_info.save_info.subtotal_mileage_2 = eeprom_saveinfo.subtotal_mileage_2;

        temp_total_mileage = fun_info.save_info.total_mileage;
    }

    // 找到它同一组的另一个页面，看看另一个页面的数据是否有效
    if (page_id % 2 == 0)
    {
        /*
            如果是偶数页面id
            得到的偶数，作为同一组页面的第一个页面 prev
            得到的偶数 + 1，作为同一组页面的第二个页面 next
        */
        page_id += 1;
    }
    else
    {
        /*
            如果是奇数页面id，
            得到的奇数 - 1，作为同一组页面的第一个页面 prev
            得到的奇数，作为同一组页面的第二个页面 next
        */
        page_id -= 1;
    }

    eeprom_24cxx_read(page_id, (u8 *)&eeprom_saveinfo, sizeof(eeprom_saveinfo_t));
    if (eeprom_saveinfo.is_data_valid == EEPROM_DATA_VALID_VAL)
    {
        // 如果该页面的数据有效，将数据读出
        if (temp_total_mileage >= eeprom_saveinfo.total_mileage)
        {
        }
        else
        {
            fun_info.save_info.total_mileage = eeprom_saveinfo.total_mileage;
            fun_info.save_info.subtotal_mileage = eeprom_saveinfo.subtotal_mileage;
            fun_info.save_info.subtotal_mileage_2 = eeprom_saveinfo.subtotal_mileage_2;
        }
    }

    /*
        运行到这里，如果两个页面的数据都无效
        is_data_valid 都不等于 EEPROM_DATA_VALID_VAL，
        那么 fun_info.save_info 应该没有被赋值，都为0
    */
}

// 保存 存放了所有信息的结构体变量，写入flash中
void fun_info_save(void)
{
    // DEBUG ：
    // printf("eeprom save begin\n"); // 打印这句话时，可以试试突然断电，测试eeprom突然断电后，会不会影响整个里程记录的数据
    eeprom_data_save();

    // DEBUG ：
    // printf("eeprom save\n");
}
