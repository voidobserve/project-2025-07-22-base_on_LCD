// GPIO配置的源文件
#include "my_gpio.h"

// 输出引脚初始化（测试用）
// P1_1
// void output_pin_init(void)
// {
//     P1_MD0 &= ~(GPIO_P11_MODE_SEL(0x1)); // 对应的寄存器先清零
//     P1_MD0 |= GPIO_P11_MODE_SEL(0x1);    // 配置为输出模式

//     FOUT_S11 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

//     // 位操作
//     P11 = 1;
// }

// /**
//  * @brief P11配置为输出模式
//  */
// void p11_output_config(void)
// {
//     P1_MD0 &= ~(GPIO_P11_MODE_SEL(0x1)); // 对应的寄存器先清零
//     P1_MD0 |= GPIO_P11_MODE_SEL(0x1);    // 配置为输出模式

//     FOUT_S11 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

//     // 位操作
//     P11 = 1;
// }

// /**
//  * @brief P12配置为输出模式
//  */
// void p12_output_config(void)
// {
//     P1_MD0 &= ~(GPIO_P12_MODE_SEL(0x1)); // 对应的寄存器先清零
//     P1_MD0 |= GPIO_P12_MODE_SEL(0x1);    // 配置为输出模式

//     FOUT_S12 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

//     // 位操作
//     P12 = 1;
// }

/**
 * @brief P01配置为输出模式
 *        P01在开发板是连接到了LED6的正极
 */
// void p01_output_config(void)
// {
//     P0_MD0 &= ~(GPIO_P01_MODE_SEL(0x1)); // 对应的寄存器先清零
//     P0_MD0 |= GPIO_P01_MODE_SEL(0x1);    // 配置为输出模式

//     FOUT_S01 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

//     P0_PU |= GPIO_P01_PULL_UP(0x01); // 内部的上拉电阻
//     P0_PD |= GPIO_P01_PULL_PD(0x01); // 内部的下拉电阻
//     // 配置完上下拉电阻后，这个引脚就相当于浮空状态，同时能够避免一些电路的干扰

//     // 位操作
//     P01 = 0;
// }

/**
 * @brief P03配置为输出模式
 */
void p03_output_config(void)
{
    P0_MD0 &= ~(GPIO_P03_MODE_SEL(0x1)); // 对应的寄存器先清零
    P0_MD0 |= GPIO_P03_MODE_SEL(0x1);    // 配置为输出模式

    FOUT_S03 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出
}

/**
 * @brief P26配置为输出模式
 */
// void p26_output_config(void)
// {
//     P2_MD1 &= ~(GPIO_P26_MODE_SEL(0x1)); // 对应的寄存器先清零
//     P2_MD1 |= GPIO_P26_MODE_SEL(0x1);    // 配置为输出模式

//     FOUT_S26 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

//     // 位操作
//     P26 = 0;
// }

// p06配置成输入上拉模式
void p06_input_config(void)
{
    // 配置为输入模式
    P0_MD1 &= ~(GPIO_P06_MODE_SEL(0x3));
    // 配置为上拉
    P0_PU |= (GPIO_P06_PULL_UP(0x1));
}

// p07配置成输入上拉模式
void p07_input_config(void)
{
    // 配置为输入模式
    P0_MD1 &= ~(GPIO_P07_MODE_SEL(0x3));
    // 配置为上拉
    P0_PU |= (GPIO_P07_PULL_UP(0x1));
}

// p10配置成输入上拉模式
void p10_input_config(void)
{
    // 配置为输入模式
    P1_MD0 &= ~(GPIO_P10_MODE_SEL(0x3));
    // 配置为上拉
    P1_PU |= (GPIO_P10_PULL_UP(0x1));
}

// p13配置成输入上拉模式
void p13_input_config(void)
{
    // 配置为输入模式
    P1_MD0 &= ~(GPIO_P13_MODE_SEL(0x3));
    // 配置为上拉
    P1_PU |= (GPIO_P13_PULL_UP(0x1));
}

// p14配置成输入上拉模式
void p14_input_config(void)
{
    // 配置为输入模式
    P1_MD1 &= ~(GPIO_P14_MODE_SEL(0x3));
    // 配置为上拉
    P1_PU |= (GPIO_P14_PULL_UP(0x1));
}

// // p17配置成输入上拉模式
// void p17_input_config(void)
// {
//     // 配置为输入模式
//     P1_MD1 &= ~(GPIO_P17_MODE_SEL(0x3));
//     // 配置为上拉
//     P1_PU |= (GPIO_P17_PULL_UP(0x1));
// }

// p21配置成输入上拉模式
void p21_input_config(void)
{
    // 配置为输入模式
    P2_MD0 &= ~(GPIO_P21_MODE_SEL(0x3));
    // 配置为上拉
    P2_PU |= (GPIO_P21_PULL_UP(0x1));
}

// p22配置成输入上拉模式
void p22_input_config(void)
{
    // 配置为输入模式
    P2_MD0 &= ~(GPIO_P22_MODE_SEL(0x3));
    // 配置为上拉
    P2_PU |= (GPIO_P22_PULL_UP(0x1));
}

// p23配置成输入上拉模式
void p23_input_config(void)
{
    // 配置为输入模式
    P2_MD0 &= ~(GPIO_P23_MODE_SEL(0x3));
    // 配置为上拉
    P2_PU |= (GPIO_P23_PULL_UP(0x1));
}

// p25配置成输入上拉模式
void p25_input_config(void)
{
    // 配置为输入模式
    P2_MD1 &= ~(GPIO_P25_MODE_SEL(0x3));
    // 配置为上拉
    P2_PU |= (GPIO_P25_PULL_UP(0x1));
}

// p27配置成输入上拉模式
void p27_input_config(void)
{
    // 配置为输入模式
    P2_MD1 &= ~(GPIO_P27_MODE_SEL(0x3));
    // 配置为上拉
    P2_PU |= (GPIO_P27_PULL_UP(0x1));
}

// P30配置成输入上拉模式
void p30_input_config(void)
{
    // 配置为输入模式
    P3_MD0 &= ~(GPIO_P30_MODE_SEL(0x3));
    // 配置为上拉
    P3_PU |= (GPIO_P30_PULL_UP(0x1));
}

// // P31配置成输入上拉模式
// void p31_input_config(void)
// {
//     // 配置为输入模式
//     P3_MD0 &= ~(GPIO_P31_MODE_SEL(0x3));
//     // 配置为上拉
//     P3_PU |= (GPIO_P31_PULL_UP(0x1));
// }
