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
#ifdef __cplusplus  /* C++֧�� */
	extern "C"{
#endif
/*===========================================================================*/

#include "stm32f103_config.h"
		
#define HAL_I2C_OK						0
#define HAL_I2C_FAIL					1   
#define MASTER_ACK						1
#define MASTER_NO_ACK					0
		
/***************************IIC_SCL��IIC_SDA����***********************/
#define IIC_SDA_IN_Init					STM32_GPIOx_Init(PCF8591_I2C_SDA_IN_Init)  //��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
#define IIC_SDA_OUT_Init				STM32_GPIOx_Init(PCF8591_I2C_SDA_OUT_Init) //��Ϊ����ڣ������ƽ�źŵ�IIC����
#define IIC_SCL_Init					STM32_GPIOx_Init(PCF8591_I2C_SCL_Init)

#define IIC_SDA_IN						PCF8591_I2C_SDA_IN
#define IIC_SDA_OUT						PCF8591_I2C_SDA_OUT
#define IIC_SCL							PCF8591_I2C_SCL

/* IIC��ʼ���ӳ��� */
void IIC_Init(void);       

/* ֹͣI2C���� */
void IIC_Stop(void);

/* ����I2C���� */
void IIC_Start(void);

/*IICдһ���ֽ� ����Ӧ���ź� ����HAL_I2C_OK�豸����*/
uint8_t IIC_WriteByte(uint8_t date);

/************************************************************************
*IIC��һ���ֽ�  ����Ӧ���ź�Master_ackΪ1��Ӧ�� 0ʱ��Ӧ��
*#define MASTER_ACK		1
*#define MASTER_NO_ACK	0
************************************************************************/
uint8_t IIC_ReadByte(uint8_t Master_ack);

/*===========================================================================*/
#ifdef __cplusplus  /* C++֧�� */
	}
#endif
/*===========================================================================*/


#endif





