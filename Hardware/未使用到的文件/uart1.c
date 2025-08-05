// 不能使用UART1+DMA来接收不定长的数据，这里的代码不能用
#include "uart1.h"

// 设置的波特率需要适配单片机的时钟，这里直接使用了官方的代码
#define USER_UART1_BAUD ((SYSCLK - UART1_BAUDRATE) / (UART1_BAUDRATE))

#if USE_MY_DEBUG

// 重写putchar()函数
extern void uart1_sendbyte(u8 senddata); // 函数声明
// char putchar(char c)
// {
//     uart1_sendbyte(c);
//     return c;
// }

void uart1_config(void)
{
    // P20为发送引脚
    P2_MD0 &= ~GPIO_P20_MODE_SEL(0x3);
    P2_MD0 |= GPIO_P20_MODE_SEL(0x1); // 输出模式
    FOUT_S20 |= GPIO_FOUT_UART1_TX;   // 配置为UART1_TX

    UART1_BAUD1 = (USER_UART1_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART1_BAUD0 = USER_UART1_BAUD & 0xFF;        // 配置波特率低八位
    UART1_CON0 = UART_STOP_BIT(0x0) |            // 8bit数据，1bit停止位
                 UART_EN(0x1);
}

// UART1发送一个字节数据的函数
void uart1_sendbyte(u8 senddata)
{
    while (!(UART1_STA & UART_TX_DONE(0x01))) // 等待前一次发送完成
        ;
    UART1_DATA = senddata;
    while (!(UART1_STA & UART_TX_DONE(0x01))) // 等待这次发送完成
        ;
}

#endif

// // UART1发送若干字节数据的函数
// void uart1_sendbytes(u8 *buf, u32 buf_len)
// {
//     u32 i = 0;
//     for (i = 0; i < buf_len; i++)
//     {
//         uart1_sendbyte(buf[i]);
//     }
// }
