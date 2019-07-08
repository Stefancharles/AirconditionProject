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
*��������uint8_t PCF8591_Write(uint8_t DAC_Date)
*���ܣ�PCF8591��DAC_Date�Ĵ�С������ģת������������Ӧ��С��ѹ��
*				DAC������;
*���룺uint8_t DAC_Date��������
*��������� HAL_I2C_OK ��return HAL_I2C_FAIL;
*����˵������
*******************************************************************/
uint8_t PCF8591_Write(uint8_t DAC_Date)
{
	uint8_t ack;
	IIC_Init();
	IIC_Start();
	ack=IIC_WriteByte(Address_PCF8591);//д������ַ
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	ack=IIC_WriteByte(Model_PCF8591);//д�ڶ����ֽ�  ������
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	ack=IIC_WriteByte(DAC_Date);//д�������ֽ�  DAC����
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	IIC_Stop();
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	return(HAL_I2C_OK);
}

/**********************************************************
*��������uint8_t PCF8591_Read(uint8_t *a, uint8_t ch, uint8_t n)
*���ܣ���PCF8591�ж�����chͨ����ʼ��n��ADCͨ��������
*���룺 uint8_t *Data, ADC���ݻ�������
*								Data[0]���ͨ��ch��ADC����,
*								Data[1]���ͨ��ch+1��ADC����,
*								......,
*								Data[n-1]���ͨ��ch+n-1��ADC���ݣ�
*				uint8_t ch,��ʼͨ����ţ�
*				uint8_t n,ͨ��������
*��������� HAL_I2C_OK ��return HAL_I2C_FAIL;
*����˵������
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
	ack = IIC_WriteByte(Address_PCF8591);//д������ַ
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	ack=IIC_WriteByte(Model_PCF8591auto);//д�ڶ����ֽ�  ������
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	IIC_Start();
	ack= IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	ad[0] = IIC_ReadByte(MASTER_ACK);	   //��һ�����ݿ�ʼADCת�� �������û��Ŷ
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	ad[0] = IIC_ReadByte(MASTER_ACK);	// ��AN0
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	ad[1] = IIC_ReadByte(MASTER_ACK);	// ��AN1
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	ad[2] = IIC_ReadByte(MASTER_ACK);	// ��AN2
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	ad[3] = IIC_ReadByte(MASTER_NO_ACK);	// ��AN3
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	IIC_Stop();
	for(i = 0; i < n; i++)
	{
		a[i]=ad[(i + ch) & 3];				
	}
	return (HAL_I2C_OK);
}


/**************************************************************
*��������uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
*���ܣ���PCF8591�ж���chͨ����ADC����
*���룺 uint8_t *Data, chͨ����ADC���ݻ�����
*				uint8_t ch��ͨ�����
*��������� HAL_I2C_OK ��return HAL_I2C_FAIL;
*����˵������
***************************************************************/
uint8_t PCF8591_ReadChannel(uint8_t *a, uint8_t ch)
{    
	uint8_t ack;
	ch &= 3;
	IIC_Init();
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591);//д������ַ
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	ack=IIC_WriteByte(Model_PCF8591S | ch);//д�ڶ����ֽ�  ������
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_ReadByte(MASTER_ACK);	   //��һ�����ݿ�ʼADCת�� �������û��Ŷ
	IIC_Stop();
	
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)
	{
		return (HAL_I2C_FAIL);	   //д��ʧ�� �ӻ��豸û����Ӧ
	}
	IIC_ReadByte(MASTER_ACK);	   //��һ�����ݿ�ʼADCת�� �������û��Ŷ
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	*a = IIC_ReadByte(MASTER_ACK);	   //������,�����ȶ�,�������û��
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	*a = IIC_ReadByte(MASTER_ACK);	   //������,�����ȶ�,�������û��
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	*a = IIC_ReadByte(MASTER_NO_ACK);	   //������
	IIC_Stop();

	return (HAL_I2C_OK);	
}

/************************************************************
*��������uint8_t  PCF8591_Init(uint8_t ch)
*���ܣ���ʼ��PCF8591��IIC���ߣ���ָ��PCF8591��ͨ���͹���ģʽ
*���룺 uint8_t *Data, chͨ����ADC���ݻ�����
*				uint8_t ch��ͨ�����
*��������� HAL_I2C_OK ��return HAL_I2C_FAIL;
*����˵������
************************************************************/
uint8_t PCF8591_Init(uint8_t ch)
{
	uint8_t ack;
	ch &= 3;
	IIC_Init();
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591);//д������ַ
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	ack=IIC_WriteByte(Model_PCF8591S | ch);//д�ڶ����ֽ�  ������
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_SDA_OUT_Init;//SDA��Ϊ����ڣ������ƽ�źŵ�IIC������
	IIC_Start();
	ack = IIC_WriteByte(Address_PCF8591|0x01);
	if(ack == HAL_I2C_FAIL)                   //д��ʧ�� �ӻ��豸û����Ӧ
	{
		return (HAL_I2C_FAIL);
	}
	IIC_ReadByte(MASTER_ACK);	   //��һ�����ݿ�ʼADCת�� �������û��Ŷ
	IIC_Stop();
	return (HAL_I2C_OK);
}

/************************************************************
*��������uint8_t  PCF8591_ReadChNd(uint8_t *a)
*���ܣ���ָ��ͨ������ADC���ݣ���PCF8591_Init���ʹ��
*���룺 uint8_t *Data ��ADC���ݻ�����
*��������� HAL_I2C_OK ��return HAL_I2C_FAIL;
*����˵������
************************************************************/
uint8_t PCF8591_ReadChNd(uint8_t *a)
{
	IIC_Start();
	if(IIC_WriteByte(Address_PCF8591|0x01) == HAL_I2C_FAIL)
	{
		return (HAL_I2C_FAIL);	   //д��ʧ�� �ӻ��豸û����Ӧ
	}
	IIC_ReadByte(MASTER_ACK);	   //��һ�����ݿ�ʼADCת�� �������û��Ŷ
	IIC_SDA_IN_Init;//��Ϊ����ڣ���IIC�����϶�ȡ��ƽ�ź�
	*a = IIC_ReadByte(MASTER_NO_ACK);	   //������
	IIC_Stop();
	printf("adc��ֵ��%d\r\n",*a);
	return (HAL_I2C_OK);	
}
