#ifndef __EEPROM_H
#define __EEPROM_H

#include "my_config.h" 

// volatile bit flag_is_printf_eeprom_data = 0;

#define EEPROM_DATA_VALID_VAL ((u8)0xC5)
// #define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)100000) // 程序限制的，每组页面最大的擦写次数
#define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)100) // 程序限制的，每组页面最大的擦写次数 -- 测试用
#define MILEAGE_SAVE_PER_METERS (100)                  // 每经过 xx m，保存一次里程



extern volatile eeprom_menu_t eeprom_menu_prev;
extern volatile eeprom_menu_t eeprom_menu_next;
extern volatile eeprom_saveinfo_t eeprom_saveinfo;

#if USE_MY_DEBUG

void eeprom_printf_all(void);

#endif // #if USE_MY_DEBUG

// void eeprom_menu_write(void);
// void eeprom_menu_read(void);
void eeprom_menu_init(void); // 上电使用eeprom前，先初始化它的目录
void eeprom_data_save(void);

void eeprom_24cxx_clear(void);

#endif 
 
 