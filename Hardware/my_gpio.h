// GPIO配置的头文件
#ifndef __MY_GPIO_H
#define __MY_GPIO_H

#include "include.h" // 包含芯片官方提供的头文件

// void p01_output_config(void); // P01连接到了开发板的LED6的正极
// void p26_output_config(void); // P26连接到了开发板的LED7的正极P26配置为输出模式
void p03_output_config(void);

void p06_input_config(void); // p06配置成输入上拉模式
void p07_input_config(void); // p07配置成输入上拉模式

void p10_input_config(void); // p10配置成输入上拉模式
void p13_input_config(void); // p13配置成输入上拉模式
void p14_input_config(void); // p14配置成输入上拉模式
// void p17_input_config(void); // p17配置成输入上拉模式

void p21_input_config(void); // p21配置成输入上拉模式
void p22_input_config(void); // p22配置成输入上拉模式
void p23_input_config(void); // p23配置成输入上拉模式

void p25_input_config(void);// p25配置成输入上拉模式

void p27_input_config(void); // p27配置成输入上拉模式

void p30_input_config(void); // p30配置成输入上拉模式
// void p31_input_config(void); // p31配置成输入上拉模式


#endif
