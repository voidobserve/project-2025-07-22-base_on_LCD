#include "iic_eeprom.h"

volatile eeprom_menu_t eeprom_menu_prev;
volatile eeprom_menu_t eeprom_menu_next;
volatile eeprom_saveinfo_t eeprom_saveinfo;

// 更新eeprom中目录相关的数据，需要提前写好 eeprom_menu_prev 和 eeprom_menu_next
void eeprom_menu_write(void)
{
    eeprom_24cxx_write(0, (u8 *)&eeprom_menu_prev, sizeof(eeprom_menu_t));
    eeprom_24cxx_write(1, (u8 *)&eeprom_menu_next, sizeof(eeprom_menu_t));
}

void eeprom_menu_read(void)
{
    eeprom_24cxx_read(0, (u8 *)&eeprom_menu_prev, sizeof(eeprom_menu_t));
    eeprom_24cxx_read(1, (u8 *)&eeprom_menu_next, sizeof(eeprom_menu_t));
}

void eeprom_menu_init(void)
{
    eeprom_menu_read();

    if (eeprom_menu_prev.is_data_valid != EEPROM_DATA_VALID_VAL && eeprom_menu_next.is_data_valid != EEPROM_DATA_VALID_VAL)
    {
        // 如果两个页 的目录读到的数据都是无效的，说明是第一次使用eeprom，进行初始化
        eeprom_menu_prev.cur_write_page_id = 2; // 0为目录prev，1为目录next，真正的数据从2开始
        eeprom_menu_prev.is_data_valid = EEPROM_DATA_VALID_VAL;

        eeprom_menu_next = eeprom_menu_prev;
        eeprom_menu_write();

        printf("all menus are invalid\n");
    }
    else if ((eeprom_menu_prev.is_data_valid == EEPROM_DATA_VALID_VAL && eeprom_menu_next.is_data_valid != EEPROM_DATA_VALID_VAL) ||
             (eeprom_menu_prev.is_data_valid != EEPROM_DATA_VALID_VAL && eeprom_menu_next.is_data_valid == EEPROM_DATA_VALID_VAL))
    {
        // 如果两个页的目录读到的数据有一个页是有效的，有一个页是无效的
        // 用有效的那个页的数据进行初始化

        if (eeprom_menu_prev.is_data_valid == EEPROM_DATA_VALID_VAL)
        {
            eeprom_menu_next = eeprom_menu_prev;
            eeprom_24cxx_write(1, (u8 *)&eeprom_menu_next, sizeof(eeprom_menu_t));
        }
        else if (eeprom_menu_next.is_data_valid == EEPROM_DATA_VALID_VAL)
        {
            eeprom_menu_prev = eeprom_menu_next;
            eeprom_24cxx_write(0, (u8 *)&eeprom_menu_prev, sizeof(eeprom_menu_t));
        }

        printf("one of menus is valid\n");
    }
    else
    {
        // 如果两个页的目录中的数据都有效
        printf("menus are valid\n");
    }
}

#if USE_MY_DEBUG
volatile bit flag_is_printf_eeprom_data;

volatile eeprom_menu_t eeprom_menu;

void eeprom_printf_all(void)
{
    u16 i;

    // eeprom_menu_t eeprom_menu = {0};
    eeprom_menu.cur_write_page_id = 0;
    eeprom_menu.is_data_valid = 0;
    eeprom_24cxx_read(0, (u8 *)&eeprom_menu, sizeof(eeprom_menu_t));
    printf("eeprom_menu_prev.cur_write_page_id = %bu\n", eeprom_menu.cur_write_page_id);
    printf("eeprom_menu_prev.is_data_valid = 0x %x\n", (u16)eeprom_menu.is_data_valid);

    eeprom_menu.cur_write_page_id = 0;
    eeprom_menu.is_data_valid = 0;
    eeprom_24cxx_read(1, (u8 *)&eeprom_menu, sizeof(eeprom_menu_t));
    printf("eeprom_menu_next.cur_write_page_id = %bu\n", eeprom_menu.cur_write_page_id);
    printf("eeprom_menu_next.is_data_valid = 0x %x\n", (u16)eeprom_menu.is_data_valid);

    printf("====================================================\n");

    // for (i = 2; i < 128; i++)
    for (i = 2; i < 128; i++)
    {
        eeprom_24cxx_read(i, (u8 *)&eeprom_saveinfo, sizeof(eeprom_saveinfo_t));
        printf("cur_page_id %u\n", i);
        printf("eeprom_saveinfo.erase_cnt = %lu\n", eeprom_saveinfo.erase_cnt);
        printf("eeprom_saveinfo.toal_mileage = %lu\n", eeprom_saveinfo.total_mileage);
        printf("eeprom_saveinfo.subtotal_mileage = %lu\n", eeprom_saveinfo.subtotal_mileage);
        printf("eeprom_saveinfo.subtotal_mileage_2 = %lu\n", eeprom_saveinfo.subtotal_mileage_2);
        printf("====================================================\n");
    }
}

#endif

// 向eeprom写入需要保存的数据
volatile eeprom_saveinfo_t eeprom_saveinfo_prev; // 存放从eeprom读出的数据，prev，同一组数据的前一页数据
volatile eeprom_saveinfo_t eeprom_saveinfo_next; // 存放从eeprom读出的数据，next，同一组数据的后一页数据
/*
    函数内部会把 fun_info.save_info 的数据写入eeprom
*/
void eeprom_data_save(void)
{
    u8 cur_save_info_id_prev; // 当前保存数据的页id，prev，同一组数据的前一页（为了节省程序空间，不进行初始化，由函数内部赋值）
    u8 cur_save_info_id_next; // 当前保存数据的页id，next，同一组数据的后一页

    u8 cur_write_page_id = 0; // 存放待写入数据的地址对应id
    u32 cur_erase_cnt;        // 存放读取到的擦写次数

    /*
        写入前，读取目录，根据目录找到要写入的地址

        如果目录的数据不一致，以最后写入的地址作为新写入地址（目录中的两个地址，最后写入的地址应该大于另一个地址）
    */

    eeprom_menu_read();

    // if (eeprom_menu_prev.is_data_valid == EEPROM_DATA_VALID_VAL && eeprom_menu_next.is_data_valid == EEPROM_DATA_VALID_VAL)
    if (eeprom_menu_prev.cur_write_page_id == eeprom_menu_next.cur_write_page_id)
    {
        cur_write_page_id = eeprom_menu_prev.cur_write_page_id;
    }
    else
    {
        // 如果目录中的数据不一致，判断哪个地址更大，哪个就作为待写入的地址的id
        if (eeprom_menu_prev.cur_write_page_id > eeprom_menu_next.cur_write_page_id)
        {
            cur_write_page_id = eeprom_menu_prev.cur_write_page_id;
        }
        else
        {
            cur_write_page_id = eeprom_menu_next.cur_write_page_id;
        }
    }

    /*
        page[0]和page[1]已经作为目录使用（id==0和id==1），
        剩下的页面，每两个页面作为一组数据，
        那么偶数页面作为同一组页面的前一个页面，奇数页面作为同一组页面的后一个页面

        例如 id == 2 作为第 1 组页面的前一个页面 prev，
            id == 3 作为第 1 组页面的后一个页面 next
    */
    if (cur_write_page_id % 2 == 0)
    {
        /*
            如果得到的是偶数页面，
            得到的偶数，作为同一组页面的第一个页面 prev
            得到的偶数 + 1，作为同一组页面的第二个页面 next
        */
        cur_save_info_id_prev = cur_write_page_id;
        cur_save_info_id_next = cur_write_page_id + 1;
    }
    else
    {
        /*
            如果得到的是奇数页面，

            得到的奇数 - 1，作为同一组页面的第一个页面 prev
            得到的奇数，作为同一组页面的第二个页面 next
        */
        cur_save_info_id_prev = cur_write_page_id - 1;
        cur_save_info_id_next = cur_write_page_id;
    }

    /*
        得到同一组页面的前、后两个页面的id后，读取这两个页面对应的数据

        将两个页面的数据进行比较， 用总里程较大的数据 覆盖 总里程较小的数据

        如果超过了限制的擦写次数，要换下一组页面，并且更新到目录中
        如果写完之后，记录的页面id与目录中的id不一致，要更新目录中记录的页面id
    */
    eeprom_24cxx_read((cur_save_info_id_prev), (u8 *)&eeprom_saveinfo_prev, sizeof(eeprom_saveinfo_t));
    eeprom_24cxx_read((cur_save_info_id_next), (u8 *)&eeprom_saveinfo_next, sizeof(eeprom_saveinfo_t));

    // 数据无效，可能是第一次读取，或者由于写入时掉电导致数据丢失
    if (eeprom_saveinfo_prev.is_data_valid != EEPROM_DATA_VALID_VAL)
    {
        eeprom_saveinfo_prev.erase_cnt = 0;
        eeprom_saveinfo_prev.total_mileage = 0;
        eeprom_saveinfo_prev.subtotal_mileage = 0;
        eeprom_saveinfo_prev.subtotal_mileage_2 = 0;
        eeprom_saveinfo_prev.is_data_valid = EEPROM_DATA_VALID_VAL;
    }

    // 数据无效，可能是第一次读取，或者由于写入时掉电导致数据丢失
    if (eeprom_saveinfo_next.is_data_valid != EEPROM_DATA_VALID_VAL)
    {
        eeprom_saveinfo_next = eeprom_saveinfo_prev;
    }

    if (eeprom_saveinfo_prev.total_mileage < eeprom_saveinfo_next.total_mileage)
    {
        /*
            如果前一个页面的总里程要小于后一个页面的总里程，
            后一个页面的总里程 加上 新增的总里程增量，得到的总里程保存到前一个页面中，
            后一个页面作为备份，也保存下来
        */
        eeprom_saveinfo_next.total_mileage = fun_info.save_info.total_mileage;
        eeprom_saveinfo_next.subtotal_mileage = fun_info.save_info.subtotal_mileage;
        eeprom_saveinfo_next.subtotal_mileage_2 = fun_info.save_info.subtotal_mileage_2;

        cur_erase_cnt = eeprom_saveinfo_next.erase_cnt; // 存放擦写次数
        eeprom_saveinfo_prev = eeprom_saveinfo_next;
    }
    else
    {
        /*
            如果前一个页面的总里程要 大于或等于 后一个页面的总里程，
            前一个页面的总里程 加上 新增的总里程增量，得到的总里程保存到后一个页面中，
            前一个页面作为备份，也保存下来
        */

        eeprom_saveinfo_prev.total_mileage = fun_info.save_info.total_mileage;
        eeprom_saveinfo_prev.subtotal_mileage = fun_info.save_info.subtotal_mileage;
        eeprom_saveinfo_prev.subtotal_mileage_2 = fun_info.save_info.subtotal_mileage_2;

        cur_erase_cnt = eeprom_saveinfo_prev.erase_cnt; // 存放擦写次数
        eeprom_saveinfo_next = eeprom_saveinfo_prev;
    }

    cur_erase_cnt++; // 擦写次数累加
    eeprom_saveinfo_prev.erase_cnt = cur_erase_cnt;
    eeprom_saveinfo_next.erase_cnt = cur_erase_cnt;
    if (cur_erase_cnt >= EEPROM_MAX_ERASE_COUNTS_PER_PAGE)
    {
        // 如果超过了一组页面的最大擦写次数，更换下一组页面
        cur_save_info_id_prev = cur_save_info_id_next + 1; // 当前为奇数id，加一，为偶数id，是下一组页面的第一个页面
        cur_save_info_id_next = cur_save_info_id_prev + 1;

        // 擦写次数清零
        eeprom_saveinfo_prev.erase_cnt = 0;
        eeprom_saveinfo_next.erase_cnt = 0;
    }

    eeprom_24cxx_write(cur_save_info_id_prev, (u8 *)&eeprom_saveinfo_prev, sizeof(eeprom_saveinfo_t));
    eeprom_24cxx_write(cur_save_info_id_next, (u8 *)&eeprom_saveinfo_next, sizeof(eeprom_saveinfo_t));

    /*
        如果写入的页面id与目录记录的id不一样，则更新目录记录的id
    */
    if (cur_save_info_id_prev != eeprom_menu_prev.cur_write_page_id)
    {
        eeprom_menu_prev.cur_write_page_id = cur_save_info_id_prev;
        eeprom_menu_prev.is_data_valid = EEPROM_DATA_VALID_VAL;

        // eeprom_menu_next.cur_write_page_id = cur_save_info_id_prev;
        // eeprom_menu_next.is_data_valid = EEPROM_DATA_VALID_VAL;

        eeprom_menu_next = eeprom_menu_prev;

        eeprom_menu_write();
    }
}

void eeprom_24cxx_clear(void)
{
    u16 i;
    const u8 clear_data = 0x00;
    for (i = 0; i < ((u16)128 * 32); i++)
    {
        while (0 == iic_eeprom_write(IIC_ON_CHIP_DEVICE_ADDR, i, (u8 *)&clear_data, 1))
            ;
    }
}