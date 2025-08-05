// 芯片片上外设iic驱动源程序

#include "iic_on_chip.h"
#include "my_config.h"

/* The direction of the write E2prom. */
#define WRITE_E2PROM_DIRECTION (0)
/* The direction of the read E2prom. */
#define READ_E2PROM_DIRECTION (1)

/**
 * @brief  IIC sends 1 byte data function
 * @param  iic_data: IIC data
 * @param  flag: 0: None  1:send start   2:send stop
 * @retval Returns 1 and receives an ACK
 */
u8 iic_master_tx(u8 iic_data, u8 flag)
{
#define START_FLAG (1)
#define STOP_FLAG (2)

    u8 ack_flag = 0;

    if (flag == START_FLAG)
    {
        // 配置发送起始位
        I2C_CON |= (I2C_START_BIT(0x1) | I2C_SI_CLEAR(0x1));
        // 等待 SI 置 1， 起始位发送成功时会置 1， 其它情况也可能会置 1， 需要注意
        while (!(I2C_STA & I2C_SI_STA(0x1)))
            ;
        I2C_CON &= ~I2C_START_BIT(0x1);

        // 判断模块状态
        if (!(((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x08) ||
              ((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x10)))
        {
            return 0;
        }
    }

    // 写入目标从机地址
    I2C_DATA = iic_data;
    // 等待地址发送完成
    while (!(I2C_STA & I2C_SI_STA(0x1)))
        ;

    // 判断是否收到 ACK
    if (((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x18) ||
        ((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x28) ||
        ((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x40))
    {
        ack_flag = 1;
    }
    else
    {
        ack_flag = 0;
    }

    if (flag == STOP_FLAG)
    {
        // 配置发送停止位
        I2C_CON |= (I2C_STOP_BIT(0x1) | I2C_SI_CLEAR(0x1));
    }

    return ack_flag;
}

/**
 * @brief  IIC receive 1 byte data function
 * @param  ack_en: 0:Send NACK  1:Send ACK   2:Send stop
 * @retval Returns data
 */
u8 iic_master_rx(u8 flag)
{
#define NACK_FLAG (0)
#define ACK_FLAG (1)
#define STOP_FLAG (2)

    u8 ack_flag = 0;
    u8 iic_data = 0;

    if (flag == ACK_FLAG)
    {
        // 接收完1byte数据发送ACK
        I2C_CON |= I2C_ACK_NACK_BIT(0x1);
        if (I2C_STA & I2C_SI_STA(0x1))
        {
            I2C_CON |= I2C_SI_CLEAR(0x1);
        }
    }

    // 等待接收完成
    while (!(I2C_STA & I2C_SI_STA(0x1)))
        ;

    if (flag == NACK_FLAG)
    {
        // 接收完1byte数据发送NACK
        I2C_CON &= ~I2C_ACK_NACK_BIT(0x1);
    }
    if (flag == STOP_FLAG)
    {
        // 配置发送停止位
        I2C_CON |= (I2C_STOP_BIT(0x1) | I2C_SI_CLEAR(0x1));
    }

    // 开发时此处的处理可注释掉
    if ((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x50)
    {
        // 成功发送 ACK
        ack_flag = 1;
    }
    else if ((I2C_STA & I2C_STA_FLAG(0x1F)) == 0x58)
    {
        // 成功发送 NACK
        ack_flag = 0;
    }

    return I2C_DATA;
}

#if 1
/**
 * @brief  E2prom iic write data function
 * @param  device_addr : Iic target device address. 目标器件地址
 * @param  addr        : the addr of IIC slave where you want to write. 想要写入的IIC从设备的存储地址
 * @param  p_buf       : The data buf to be sent. 待发送的数据缓冲区
 * @param  len         : The amount of data to sent. The unit is byte. 待发送的数据长度，单位：字节
 * @retval Success/failure reads data from the IIC. 操作结果：0--失败，1--成功
 */
u8 iic_eeprom_write(u16 device_addr, u16 addr, u8 *p_buf, u16 len)
{
#define NONE_FLAG (0)
#define START_FLAG (1)
#define STOP_FLAG (2)

    u16 i = 0;

    // 发送开始信号和设备地址
    if (!iic_master_tx(device_addr | WRITE_E2PROM_DIRECTION, START_FLAG))
    {
        // user_printf("IIC send device addr failed\n");
        return 0;
    }
    // 发送memory地址
    if (!iic_master_tx(addr >> 8, NONE_FLAG))
    {
        // user_printf("IIC send high addr failed\n");
        return 0;
    }
    if (!iic_master_tx(addr & 0xFF, NONE_FLAG))
    {
        // user_printf("IIC send low addr failed\n");
        return 0;
    }

    // 发送数据
    for (i = 0; i < (len - 1); i++)
    {
        if (!iic_master_tx(*p_buf++, NONE_FLAG))
        {
            // user_printf("IIC send dada failed\n");
            return 0;
        }
    }
    // 发送最后1byte数据并发送停止位
    if (!iic_master_tx(*p_buf, STOP_FLAG))
    {
        // user_printf("IIC send dada and failed\n");
        return 0;
    }

    return 1;
}

/**
 * @brief  E2prom iic read data function
 * @param  device_addr : Iic target device address. 目标器件地址
 * @param  addr        : the addr of IIC slave where you want to read. 想要读取的IIC从设备的存储地址
 * @param  p_buf       : The data buf to be received. 待接收的数据缓冲区
 * @param  len         : The amount of data to received. The unit is byte. 待接收的数据长度，单位：字节
 * @retval Success/failure reads data from the IIC. 操作结果：0--失败，1--成功
 */
u8 iic_eeprom_read(u16 device_addr, u16 addr, u8 *p_buf, u16 len)
{
#define NACK_FLAG (0)
#define ACK_FLAG (1)
#define NONE_FLAG (0)
#define START_FLAG (1)
#define STOP_FLAG (2)

    u16 i = 0;

    // 发送开始信号和设备地址
    if (!iic_master_tx(device_addr | WRITE_E2PROM_DIRECTION, START_FLAG))
    {
        // user_printf("IIC send device addr failed\n");
        return 0;
    }
    // 发送memory地址
    if (!iic_master_tx(addr >> 8, NONE_FLAG))
    {
        // user_printf("IIC send haig addr failed\n");
        return 0;
    }
    if (!iic_master_tx(addr & 0xFF, NONE_FLAG))
    {
        // user_printf("IIC send low addr failed\n");
        return 0;
    }

    // 发送开始信号和设备地址
    if (!iic_master_tx(device_addr | READ_E2PROM_DIRECTION, START_FLAG))
    {
        // user_printf("IIC send device addr failed\n");
        return 0;
    }

    // 接收数据并发送ACK
    for (i = 0; i < (len - 2); i++)
    {
        *p_buf++ = iic_master_rx(ACK_FLAG);
    }
    // 接收数据并发送NACK
    *p_buf++ = iic_master_rx(NONE_FLAG);
    // 接收最后1byte数据，并发送停止位
    *p_buf = iic_master_rx(STOP_FLAG);

    return 1;
}
#endif

/**
 * @brief 24cxx iic eeprom 读写入数据函数
 *          器件地址在宏 IIC_ON_CHIP_DEVICE_ADDR 中定义，例如 IIC_ON_CHIP_DEVICE_ADDR == 0xA0，高四位器件地址为0b-1010，器件引脚的A2~A0为0b-000。
 *
 * @param page_id 页面id，例如，0~127页，page_addr = 0~127
 * @param p_buf 待发送的数据缓冲区
 * @param len 待发送的数据长度，单位：字节
 */
void eeprom_24cxx_write(u8 page_id, u8 *p_buf, u16 len)
{
    u16 addr = EEPROM_PAGE_X_ADDR(page_id); // 计算eeprom地址，将传入的id转换成对应的地址
    while (0 == iic_eeprom_write(IIC_ON_CHIP_DEVICE_ADDR, addr, p_buf, len))
        ;
}

void eeprom_24cxx_read(u8 page_id, u8 *p_buf, u16 len)
{
    u16 addr = EEPROM_PAGE_X_ADDR(page_id); // 计算eeprom地址，将传入的id转换成对应的地址

    while (0 == iic_eeprom_read(IIC_ON_CHIP_DEVICE_ADDR, addr, p_buf, len))
        ;
}

void iic_config(void)
{
    // IIC_CLK -- P21，IIC_SDA -- P22
    P2_MD0 &= ~(GPIO_P21_MODE_SEL(0x03)); // 清空对应的寄存器配置
    P2_MD0 |= (GPIO_P21_MODE_SEL(0x02));  // 数字复用模式
    P2_PU |= (GPIO_P21_PULL_UP(0x01));    // 上拉
    P2_ODN |= (GPIO_P21_ODN_EN(0x01));    // 开漏使能
    P2_AF0 |= GPIO_P21_FUNC_SEL(0x01);    // 0x1--I2C_CLK 功能选择

    P2_MD0 &= ~(GPIO_P22_MODE_SEL(0x03)); // 清空对应的寄存器配置
    P2_MD0 |= (GPIO_P22_MODE_SEL(0x02));  // 数字复用模式
    P2_PU |= (GPIO_P22_PULL_UP(0x01));    // 上拉
    P2_ODN |= (GPIO_P22_ODN_EN(0x01));    // 开漏使能
    P2_AF0 |= GPIO_P22_FUNC_SEL(0x01);    // 0x1--I2C_SDA 功能选择

    I2C_CON = I2C_CR_SEL(0x02); // 配置波特率，可根据表格自行选择
    I2C_CON |= I2C_EN(0x1);     // 使能模块
}
