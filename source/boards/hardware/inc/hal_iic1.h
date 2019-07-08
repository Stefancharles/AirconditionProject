#ifndef _hal_iic1_h_
#define _hal_iic1_h_

#include "stdint.h"
#include "stm32f103_config.h"

/***************************IIC_SCL、IIC_SDA定义***********************/
#define IIC1_SDA_IN_Init				STM32_GPIOx_Init(PCF8563_I2C_SDA_IN_Init)  //作为输入口，从IIC1总线上读取电平信号
#define IIC1_SDA_OUT_Init				STM32_GPIOx_Init(PCF8563_I2C_SDA_OUT_Init) //作为输出口，输出电平信号到IIC1总线
#define IIC1_SCL_Init					STM32_GPIOx_Init(PCF8563_I2C_SCL_Init)

#define IIC1_SDA_IN						PCF8563_I2C_SDA_IN
#define IIC1_SDA_OUT					PCF8563_I2C_SDA_OUT
#define IIC1_SCL						PCF8563_I2C_SCL

void IIC1_Init(void);
void IIC1_Start(void);
void IIC1_Stop(void);
void IIC1_Ack(void);
void IIC1_NoAck(void);
uint8_t IIC1_ReadByte(void);
void IIC1_WriteByte(uint8_t wdata);
void IIC1_WriteSlaveAddr(uint8_t ucSlaAddr);

#endif /* _hal_iic1_h_ */

