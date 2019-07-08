/************************************************************************************************************
*
*
*
*
*
**************************************************************************************************************/

/* Includes */
#include "delay.h"
#include "hal_iic.h"
#include "pcf8591.h"


/*******************************************************************
*函数名：uint8_t PCF8591_Write(uint8_t DAC_Date)
*功能：PCF8591按DAC_Date的大小进行数模转换输出，输出对应大小电压在
*				DAC引脚上;
*输入：uint8_t DAC_Date：数字量
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
*******************************************************************/
uint8_t PCF8591_Write(uint8_t DAC_Date)
{
	uint8_t ack;
	IIC_Init();
	IIC_Start();
	ack=IIC_WriteByte(Address_PCF8591);//写器件地址
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	ack=IIC_WriteByte(Model_PCF8591);//写第二个字节  控制字
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	ack=IIC_WriteByte(DAC_Date);//写第三个字节  DAC数据
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	IIC_Stop();
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	return(HAL_I2C_OK);
}

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
uint8_t PCF8591_Read(uint8_t *a, uint8_t ch, uint8_t n)
{    
	uint8_t ack,i;
	uint8_t ad[4];
	ch &= 3;
	if(n > 4)
	{
		n = 4;
	}
	IIC_Init();
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591);//写器件地址
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	ack=IIC_WriteByte(Model_PCF8591auto);//写第二个字节  控制字
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	IIC_Start();
	ack= IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	ad[0] = IIC_ReadByte(MASTER_ACK);	   //读一个数据开始ADC转换 这个数据没用哦
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	ad[0] = IIC_ReadByte(MASTER_ACK);	// 读AN0
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	ad[1] = IIC_ReadByte(MASTER_ACK);	// 读AN1
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	ad[2] = IIC_ReadByte(MASTER_ACK);	// 读AN2
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	ad[3] = IIC_ReadByte(MASTER_NO_ACK);	// 读AN3
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	IIC_Stop();
	for(i = 0; i < n; i++)
	{
		a[i]=ad[(i + ch) & 3];				
	}
	return (HAL_I2C_OK);
}


/**************************************************************
*函数名：uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
*功能：从PCF8591中读出ch通道的ADC数据
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
***************************************************************/
uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
{    
	uint8_t ack;
	ch &= 3;
	IIC_Init();
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591);//写器件地址
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	ack=IIC_WriteByte(Model_PCF8591S | ch);//写第二个字节  控制字
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_ReadByte(MASTER_ACK);	   //读一个数据开始ADC转换 这个数据没用哦
	IIC_Stop();
	
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)
	{
		return (HAL_I2C_FAIL);	   //写入失败 从机设备没有相应
	}
	IIC_ReadByte(MASTER_ACK);	   //读一个数据开始ADC转换 这个数据没用哦
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	*a = IIC_ReadByte(MASTER_ACK);	   //读数据,不大稳定,这个数据没用
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	*a = IIC_ReadByte(MASTER_ACK);	   //读数据,不大稳定,这个数据没用
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	*a = IIC_ReadByte(MASTER_NO_ACK);	   //读数据
	IIC_Stop();

	return (HAL_I2C_OK);	
}

/************************************************************
*函数名：uint8_t  PCF8591_Init(uint8_t ch)
*功能：初始化PCF8591和IIC总线，并指定PCF8591的通道和工作模式
*输入： uint8_t *Data, ch通道的ADC数据缓冲区
*				uint8_t ch，通道编号
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_Init(uint8_t ch)
{
	uint8_t ack;
	ch &= 3;
	IIC_Init();
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591);//写器件地址
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	ack=IIC_WriteByte(Model_PCF8591S | ch);//写第二个字节  控制字
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA作为输出口，输出电平信号到IIC总线上
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //写入失败 从机设备没有相应
	{
		return (HAL_I2C_FAIL);
	}
	IIC_ReadByte(MASTER_ACK);	   //读一个数据开始ADC转换 这个数据没用哦
	IIC_Stop();
	return (HAL_I2C_OK);
}

/************************************************************
*函数名：uint8_t  PCF8591_ReadChNd(uint8_t *a)
*功能：从指定通道读出ADC数据，与PCF8591_Init配合使用
*输入： uint8_t *Data ，ADC数据缓冲区
*输出：返回 HAL_I2C_OK 或return HAL_I2C_FAIL;
*特殊说明：无
************************************************************/
uint8_t PCF8591_ReadChNd(uint8_t *a)
{
	IIC_Start();
	if(IIC_WriteByte(Address_PCF8591|0x01) == HAL_I2C_FAIL)
	{
		return (HAL_I2C_FAIL);	   //写入失败 从机设备没有相应
	}
	IIC_ReadByte(MASTER_ACK);	   //读一个数据开始ADC转换 这个数据没用哦
	IIC_SDA_IN_Init;//作为输入口，从IIC总线上读取电平信号
	*a = IIC_ReadByte(MASTER_NO_ACK);	   //读数据
	IIC_Stop();
	printf("adc的值：%d\r\n",*a);
	return (HAL_I2C_OK);	
}
