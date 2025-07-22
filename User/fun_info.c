#include "fun_info.h"
#include <string.h> // memset()

volatile fun_info_t fun_info = {0}; // 存放所有功能状态的结构体变量

// 初始化存放所有信息的结构体变量
void fun_info_init(void)
{
    memset(&fun_info, 0, sizeof(fun_info));
    flash_read(0x00, (u8 *)&fun_info.save_info, sizeof(fun_info.save_info));
}

// 保存 存放了所有信息的结构体变量，写入flash中
void fun_info_save(void)
{
    flash_erase_sector(0x00);
    flash_write(0x00, (u8 *)&fun_info.save_info, sizeof(fun_info.save_info));
}


