/******************************************************************************
*
*
*
*
*
*
******************************************************************************/

#ifndef _hal_iic_h_
#define _hal_iic_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif
/*===========================================================================*/

#include "stm32f103_config.h"
		
#define HAL_I2C_OK						0
#define HAL_I2C_FAIL					1   
#define MASTER_ACK						1
#define MASTER_NO_ACK					0
		
/***************************IIC_SCL、IIC_SDA定义***********************/
#define IIC_SDA_IN_Init					STM32_GPIOx_Init(PCF8591_I2C_SDA_IN_Init)  //作为输入口，从IIC总线上读取电平信号
#define IIC_SDA_OUT_Init				STM32_GPIOx_Init(PCF8591_I2C_SDA_OUT_Init) //作为输出口，输出电平信号到IIC总线
#define IIC_SCL_Init					STM32_GPIOx_Init(PCF8591_I2C_SCL_Init)

#define IIC_SDA_IN						PCF8591_I2C_SDA_IN
#define IIC_SDA_OUT						PCF8591_I2C_SDA_OUT
#define IIC_SCL							PCF8591_I2C_SCL

/* IIC初始化子程序 */
void IIC_Init(void);       

/* 停止I2C总线 */
void IIC_Stop(void);

/* 启动I2C总线 */
void IIC_Start(void);

/*IIC写一个字节 返回应答信号 返回HAL_I2C_OK设备正常*/
uint8_t IIC_WriteByte(uint8_t date);

/************************************************************************
*IIC读一个字节  主机应答信号Master_ack为1是应答 0时非应答
*#define MASTER_ACK		1
*#define MASTER_NO_ACK	0
************************************************************************/
uint8_t IIC_ReadByte(uint8_t Master_ack);

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/


#endif





