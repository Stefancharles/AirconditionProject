/********************************************************************************************************
*
*
*
*
*********************************************************************************************************/
#ifndef _PCF8591_H_
#define _PCF8591_H_

#include "hal_iic.h"
#include "stm32f103_config.h"

#define Address_PCF8591 		0x90 //PCF8591的硬件地址 1001 a2a1a0 rw
#define Model_PCF8591auto   	0x44 //模式是四个ADC单端输入 允许DAC输出 允许自动增量
#define Model_PCF8591   		0x40 //模式是四个ADC单端输入 允许DAC输出 
#define Model_PCF8591S  		0x00 //模式是四个ADC单端输入 不允许DAC输出

/************************************************************
*函数名：uint8_t  PCF8591_Init(uint8_t ch)
*功能：初始化PCF8591和IIC总线，并指定PCF8591的通道和工作模式
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_Init(uint8_t ch);

/*******************************************************************
*函数名：uint8_t PCF8591_Write(uint8_t DAC_Date)
*功能：PCF8591按DAC_Date的大小进行数模转换输出，输出对应大小电压在
*				DAC引脚上;
*输入：uint8_t DAC_Date：数字量
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
*******************************************************************/
uint8_t PCF8591_Write(uint8_t DAC_Date);

/**********************************************************
*函数名：uint8_t PCF8591_Read(uint8_t *a, uint8_t ch, uint8_t n)
*功能：从PCF8591中读出自ch通道开始的n个ADC通道的数据
*输入： uint8_t *Data, ADC数据缓存区，
*								Data[0]存放通道ch的ADC数据,
*								Data[1]存放通道ch+1的ADC数据,
*								......,
*								Data[n-1]存放通道ch+n-1的ADC数据；
*				uint8_t ch,开始通道编号；
*				uint8_t n,通道数量；
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
**********************************************************/
uint8_t PCF8591_Read(uint8_t *Data, uint8_t ch, uint8_t n);

/**************************************************************
*函数名：uint8_t PCF8591_ReadChannel(uint8_t *Data, uint8_t ch)
*功能：从PCF8591中读出ch通道的ADC数据
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
***************************************************************/
uint8_t PCF8591_ReadChannel(uint8_t *Data, uint8_t ch);

/************************************************************
*函数名：uint8_t  PCF8591_ReadChNd(uint8_t *a)
*功能：从指定通道读出ADC数据，与PCF8591_Init配合使用
*输入： uint8_t *Data ，ADC数据缓冲区
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_ReadChNd(uint8_t *a);


#endif //_PCF8591_H_
