#include "fun_info.h"
#include <string.h> // memset()

volatile fun_info_t fun_info; // 存放所有功能状态的结构体变量

// 初始化存放所有信息的结构体变量
void fun_info_init(void)
{
    u8 page_id;
    // memset(&fun_info, 0, sizeof(fun_info));

    // flash_read(0x00, (u8 *)&fun_info.save_info, sizeof(fun_info.save_info));

    eeprom_menu_init();

    

    page_id = eeprom_menu_prev.cur_write_page_id;

    // 根据从目录读取到的页面id，找到对应的数据
    eeprom_24cxx_read(page_id, (u8 *)&eeprom_saveinfo, sizeof(eeprom_saveinfo_t));
    if (eeprom_saveinfo.is_data_valid == EEPROM_DATA_VALID_VAL)
    {
        fun_info.save_info.total_mileage = eeprom_saveinfo.total_mileage;
        fun_info.save_info.subtotal_mileage = eeprom_saveinfo.subtotal_mileage;
        fun_info.save_info.subtotal_mileage_2 = eeprom_saveinfo.subtotal_mileage_2;
    }
    else
    {
        // 如果对应的页面的数据无效，找到它同一组的另一个页面，看看另一个页面的数据是否有效

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
            fun_info.save_info.total_mileage = eeprom_saveinfo.total_mileage;
            fun_info.save_info.subtotal_mileage = eeprom_saveinfo.subtotal_mileage;
            fun_info.save_info.subtotal_mileage_2 = eeprom_saveinfo.subtotal_mileage_2;
        }
        else
        {
            // 如果页面的数据无效，说明eeprom中还没有存放数据
            fun_info.save_info.total_mileage = 0;
            fun_info.save_info.subtotal_mileage = 0;
            fun_info.save_info.subtotal_mileage_2 = 0;
        }
    }
}

// 保存 存放了所有信息的结构体变量，写入flash中
void fun_info_save(void)
{
    // flash_erase_sector(0x00);
    // flash_write(0x00, (u8 *)&fun_info.save_info, sizeof(fun_info.save_info));

    eeprom_data_save();

    printf("eeprom save\n");
}
