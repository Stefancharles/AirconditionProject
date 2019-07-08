/******************************************************************************
*
*
*
*
*
******************************************************************************/

#include "delay.h"
#include "hal_iic.h"

///*������ʱ 1us */
//#define _NOP_()	delay_us(2)

/* IIC��ʼ���ӳ��� */
void IIC_Init(void)
{
	IIC_SDA_OUT_Init;
	IIC_SCL_Init;

	IIC_SDA_OUT = 1 ;
	IIC_SCL = 1 ;
}        

/* ֹͣI2C���� */
void IIC_Stop(void)
{
	IIC_SDA_OUT = 0;
	_NOP_();
	IIC_SCL = 1;
	_NOP_();
	_NOP_();
	IIC_SDA_OUT = 1;  //�������ڿ���״̬
	_NOP_();
}

/* ����I2C���� */
void IIC_Start(void)
{
	IIC_SDA_OUT = 1;
	IIC_SDA_OUT = 1;
	_NOP_();
	_NOP_();
	IIC_SDA_OUT = 0;
	_NOP_();
	_NOP_();
	IIC_SCL = 0;
}

/*IICдһ���ֽ� ����Ӧ���ź� ����HAL_I2C_OK�豸����*/
uint8_t IIC_WriteByte(uint8_t date)
{
	uint8_t i;
	uint8_t ack;
	for(i=0;i<8;i++)
	{
		IIC_SCL = 0;
		if(date&0x80)
		IIC_SDA_OUT = 1;
		else
		IIC_SDA_OUT = 0;
		_NOP_(); 
		IIC_SCL = 1;
		_NOP_();
		date  <<= 1;
	}
	IIC_SCL = 0;
	IIC_SDA_IN_Init;//��Ϊ����ڣ�8λ��������ͷ������ߣ�׼������Ӧ��λ
	_NOP_();
	_NOP_();
	_NOP_();
	IIC_SCL = 1;
	i=0;
	while((i < 200) && (IIC_SDA_IN == 1))
	{
		_NOP_();
		i++;
	}
	_NOP_();
	if(IIC_SDA_IN == 0)//�ӻ��豸Ӧ�� 
	{
		ack = HAL_I2C_OK;     
	}
	else 
	{
		ack = HAL_I2C_FAIL;
	}
	IIC_SCL = 0;
	_NOP_();
	return(ack);     
}

/************************************************************************
*IIC��һ���ֽ�  ����Ӧ���ź�Master_ackΪ1��Ӧ�� 0ʱ��Ӧ��
*#define MASTER_ACK										1
*#define MASTER_NO_ACK								0
************************************************************************/
uint8_t IIC_ReadByte(uint8_t Master_ack)
{
	uint8_t i,vaule=0;
	for(i = 0;i <8;i++)
	{
		vaule <<= 1;
		IIC_SCL = 1;
		_NOP_();
		if(IIC_SDA_IN)
		{
			vaule |=0x01;
		}
		IIC_SCL = 0;
		_NOP_();
	}
	IIC_SDA_OUT_Init;//��Ϊ����ڣ����Ӧ���ź�
	if(Master_ack != MASTER_NO_ACK)
	{
		IIC_SDA_OUT = 0;	//����Ӧ���ź�
	}
	else
	{
		IIC_SDA_OUT = 1;	//no ����Ӧ���ź�
	}
	_NOP_();
	IIC_SCL = 1;
	_NOP_();
	IIC_SCL = 0;
	_NOP_();
	return vaule;
}

