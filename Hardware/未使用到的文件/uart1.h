#ifndef __UART1_H
#define __UART1_H

#include "include.h" // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

// 串口1波特率
#ifndef UART1_BAUDRATE
#define UART1_BAUDRATE  115200
#endif // UART1_BAUDRATE

#if USE_MY_DEBUG

void uart1_config(void);
// void uart1_sendbyte(u8 senddata);
// void uart1_sendbytes(u8 *buf, u32 buf_len);

// 不能使用UART1+DMA来接收不定长的数据，这里的代码不使用
// void uart1_dma_recv_scan(void);

#endif


#endif
