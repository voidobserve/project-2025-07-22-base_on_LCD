#ifndef __IIC_ON_CHIP_H
#define __IIC_ON_CHIP_H

#include "include.h" // 包含芯片官方提供的头文件

/*
    IIC设备地址
    例如 IIC_ON_CHIP_DEVICE_ADDR == 0xA0，高四位器件地址为0b-1010，器件引脚的A2~A0为0b-000。
*/
#define IIC_ON_CHIP_DEVICE_ADDR 0xA0

#define EEPROM_PAGE_SIZE (u8)(32) // eeprom 页地址大小，单位：字节
#define EEPROM_PAGE_X_ADDR(x) ((u16)(0x00 + (u16)(x) * EEPROM_PAGE_SIZE))

typedef struct
{
    // 总里程表（单位：m，使用英制单位时，只需要再发送时进行转换）
    // （大计里程，范围：0~ 99999.9 KM）
    u32 total_mileage;
    // 短距离里程表(单位：m，使用英制单位时，只需要再发送时进行转换)
    // （小计里程，范围：0~9999.9 KM）
    u32 subtotal_mileage;
    u32 subtotal_mileage_2; // 小计里程2

    u32 erase_cnt; // 擦写次数

    u8 is_data_valid;
} eeprom_saveinfo_t;

typedef struct
{
    u8 cur_write_page_id; // 当前写入的扇区ID （只在扇区目录中写入/读取时使用）
    u8 is_data_valid;      // DATA_VALID_ID--数据有效，其他--数据无效（只在扇区目录中写入/读取时使用）
} eeprom_menu_t;

u8 iic_eeprom_write(u16 device_addr, u16 addr, u8 *p_buf, u16 len);
u8 iic_eeprom_read(u16 device_addr, u16 addr, u8 *p_buf, u16 len);

void eeprom_24cxx_write(u8 page_id, u8 *p_buf, u16 len);
void eeprom_24cxx_read(u8 page_id, u8 *p_buf, u16 len);

void iic_config(void);



#endif
