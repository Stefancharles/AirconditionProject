/******************************************************************************
*
*
*
*
*
******************************************************************************/

#include "delay.h"
#include "hal_iic.h"

///*短暂延时 1us */
//#define _NOP_()	delay_us(2)

/* IIC初始化子程序 */
void IIC_Init(void)
{
	IIC_SDA_OUT_Init;
	IIC_SCL_Init;

	IIC_SDA_OUT = 1 ;
	IIC_SCL = 1 ;
}        

/* 停止I2C总线 */
void IIC_Stop(void)
{
	IIC_SDA_OUT = 0;
	_NOP_();
	IIC_SCL = 1;
	_NOP_();
	_NOP_();
	IIC_SDA_OUT = 1;  //总线置于空闲状态
	_NOP_();
}

/* 启动I2C总线 */
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

/*IIC写一个字节 返回应答信号 返回HAL_I2C_OK设备正常*/
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
	IIC_SDA_IN_Init;//作为输入口，8位发送完后释放数据线，准备接收应答位
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
	if(IIC_SDA_IN == 0)//从机设备应答 
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
*IIC读一个字节  主机应答信号Master_ack为1是应答 0时非应答
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
	IIC_SDA_OUT_Init;//作为输出口，输出应答信号
	if(Master_ack != MASTER_NO_ACK)
	{
		IIC_SDA_OUT = 0;	//主机应答信号
	}
	else
	{
		IIC_SDA_OUT = 1;	//no 主机应答信号
	}
	_NOP_();
	IIC_SCL = 1;
	_NOP_();
	IIC_SCL = 0;
	_NOP_();
	return vaule;
}

