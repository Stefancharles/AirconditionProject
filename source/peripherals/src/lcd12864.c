/****************************************************************************
*
*
*
*
*
*
****************************************************************************/

#include "stm32f103_config.h"
#include "ascii8x16.h"
#include "lcd12864.h"
#include "delay.h"

#define	setbit(p,b) (p|=(1<<b))
#define	clrbit(p,b) (p&=(~(1<<b)))
#define	testbit(p,b) (p&(1<<b))

#define _LCD_NOP_()         delay_us(1)

/************************************************************
*������void HC595_GpioInit(void)
*���ܣ�74HC595�ź����Ŷ�Ӧ��I/O�ڹ��ܳ�ʼ��
*���룺��
*�������
*����˵������
************************************************************/
void LCD_GpioInit(void)
{
	STM32_GPIOx_Init(LCD12864_BL_Init);
	STM32_GPIOx_Init(LCD12864_RST_Init);
	STM32_GPIOx_Init(LCD12864_CS2_Init);
	STM32_GPIOx_Init(LCD12864_CS1_Init);
	
	STM32_GPIOx_Init(LCD12864_DB7_Init);
	STM32_GPIOx_Init(LCD12864_DB6_Init);
	STM32_GPIOx_Init(LCD12864_DB5_Init);
	STM32_GPIOx_Init(LCD12864_DB4_Init);
	STM32_GPIOx_Init(LCD12864_DB3_Init);
	STM32_GPIOx_Init(LCD12864_DB2_Init);
	STM32_GPIOx_Init(LCD12864_DB1_Init);
	STM32_GPIOx_Init(LCD12864_DB0_Init);
	
	STM32_GPIOx_Init(LCD12864_EN_Init);
	STM32_GPIOx_Init(LCD12864_RW_Init);
	STM32_GPIOx_Init(LCD12864_RS_Init);
	
	LCD12864_BL = 1;
	LCD12864_RST = 1;
	LCD12864_CS2 = 1;
	LCD12864_CS1 = 1;
	LCD12864_DB7 = 1;
	LCD12864_DB5 = 1;
	LCD12864_DB4 = 1;
	LCD12864_DB3 = 1;
	LCD12864_DB2 = 1;
	LCD12864_DB1 = 1;
	LCD12864_DB0 = 1;
	LCD12864_EN = 1;
	LCD12864_RW = 1;
	LCD12864_RS = 1;
}

//==========================================================================
// ������unsigned char LCD_ReadDataPort(void)
// �������ܣ�����ʾ�����ݶ˿�DB7~DB0������,�����䷵��
// ���룺��
// ���أ�����ʾ��DB7~DB0������
// ����˵������
/*LCD12864_DB7_Init;
	LCD12864_DB7_IN_Init;
	LCD12864_DB6_Init;
	LCD12864_DB6_IN_Init;
	LCD12864_DB5_Init;
	LCD12864_DB5_IN_Init;
	LCD12864_DB4_Init;
	LCD12864_DB4_IN_Init;
	LCD12864_DB3_Init;
	LCD12864_DB3_IN_Init;
	LCD12864_DB2_Init;
	LCD12864_DB2_IN_Init;
	LCD12864_DB1_Init;
	LCD12864_DB1_IN_Init;
	LCD12864_DB0_Init;
	LCD12864_DB0_IN_Init;*/
//==========================================================================
unsigned char LCD_ReadDataPort(void)
{
	unsigned char temp;
	
	STM32_GPIOx_Init(LCD12864_DB7_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB6_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB5_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB4_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB3_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB2_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB1_IN_Init);
	STM32_GPIOx_Init(LCD12864_DB0_IN_Init);
	
	if(LCD12864_DB7_IN)
	{setbit(temp,7);}
	else
	{clrbit(temp,7);}
	
	if(LCD12864_DB6_IN)
	{setbit(temp,6);}
	else
	{clrbit(temp,6);}
	
	if(LCD12864_DB5_IN)
	{setbit(temp,5);}
	else
	{clrbit(temp,5);}
	
	if(LCD12864_DB4_IN)
	{setbit(temp,4);}
	else
	{clrbit(temp,4);}
	
	if(LCD12864_DB3_IN)
	{setbit(temp,3);}
	else
	{clrbit(temp,3);}
	
	if(LCD12864_DB2_IN)
	{setbit(temp,2);}
	else
	{clrbit(temp,2);}
	
	if(LCD12864_DB1_IN)
	{setbit(temp,1);}
	else
	{clrbit(temp,1);}
	
	if(LCD12864_DB0_IN)
	{setbit(temp,0);}
	else
	{clrbit(temp,0);}
	
	return temp;
}

//==========================================================================
// ������unsigned char LCD_WriteDataPort(void)
// �������ܣ�����ʾ�����ݶ˿�DB7~DB0��������temp����ֵ
// ���룺unsigned char temp����д�������
// ���أ���
// ����˵������
/*LCD12864_DB7_Init;
	LCD12864_DB7_IN_Init;
	LCD12864_DB6_Init;
	LCD12864_DB6_IN_Init;
	LCD12864_DB5_Init;
	LCD12864_DB5_IN_Init;
	LCD12864_DB4_Init;
	LCD12864_DB4_IN_Init;
	LCD12864_DB3_Init;
	LCD12864_DB3_IN_Init;
	LCD12864_DB2_Init;
	LCD12864_DB2_IN_Init;
	LCD12864_DB1_Init;
	LCD12864_DB1_IN_Init;
	LCD12864_DB0_Init;
	LCD12864_DB0_IN_Init;*/
//==========================================================================
void LCD_WriteDataPort(unsigned char temp)
{
	unsigned char i;
	
	STM32_GPIOx_Init(LCD12864_DB7_Init);
	STM32_GPIOx_Init(LCD12864_DB6_Init);
	STM32_GPIOx_Init(LCD12864_DB5_Init);
	STM32_GPIOx_Init(LCD12864_DB4_Init);
	STM32_GPIOx_Init(LCD12864_DB3_Init);
	STM32_GPIOx_Init(LCD12864_DB2_Init);
	STM32_GPIOx_Init(LCD12864_DB1_Init);
	STM32_GPIOx_Init(LCD12864_DB0_Init);
	
	for(i=0; i<8; i++)
	{
		if(testbit(temp,i))
		{
			switch(i)
			{
				case(0):LCD12864_DB0=1;break;
				case(1):LCD12864_DB1=1;break;
				case(2):LCD12864_DB2=1;break;
				case(3):LCD12864_DB3=1;break;
				case(4):LCD12864_DB4=1;break;
				case(5):LCD12864_DB5=1;break;
				case(6):LCD12864_DB6=1;break;
				case(7):LCD12864_DB7=1;break;
				default:break;
			}
		}
		else
		{
			switch(i)
			{
				case(0):LCD12864_DB0=0;break;
				case(1):LCD12864_DB1=0;break;
				case(2):LCD12864_DB2=0;break;
				case(3):LCD12864_DB3=0;break;
				case(4):LCD12864_DB4=0;break;
				case(5):LCD12864_DB5=0;break;
				case(6):LCD12864_DB6=0;break;
				case(7):LCD12864_DB7=0;break;
				default:break;
			}
		}
	}
	
}

//==========================================================================
// ��������: ����Ƿ����
//==========================================================================
void LCD_BusyChk(void)
{
    unsigned int timeout = 0;
    
    LCD12864_EN = 0;
    LCD12864_RS = 0;
    LCD12864_RW = 1;
    LCD12864_EN = 1;
    while((LCD_ReadDataPort() & 0x80) && (timeout < 60))//æ״̬���, �ȴ���ʱʱ��Ϊ60ms
	{	
		timeout++;
		delay_ms(1);
	}
    LCD12864_EN = 0;
}

//==========================================================================
// ��������: ��12864������
//==========================================================================
unsigned char LCD_RdData(void)
{
    unsigned char temp;
    
    LCD_BusyChk();
    LCD12864_EN = 0;
    //LCD_DATA = 0xFF;                            //IO���øߵ�ƽ, �Ա������
    LCD12864_RS = 1;
    LCD12864_RW = 1;
    LCD12864_EN  = 1;
    _LCD_NOP_();                                        //ʹ����ʱ, ����ǽϿ��CPUӦ����ʱ��һЩ
    _LCD_NOP_();
    _LCD_NOP_();
    temp = LCD_ReadDataPort();//temp = LCD_DATA;
    
    return temp;
}
//==========================================================================
// ��������: ��12864д����
//==========================================================================
void LCD_WrCmd(unsigned char cmd)
{
    LCD12864_RS = 0;
    LCD12864_RW = 0;
    LCD_WriteDataPort(cmd);//LCD_DATA = cmd;
    LCD12864_EN = 1;
    _LCD_NOP_();
    LCD12864_EN = 0;
}

//==========================================================================
// ��������: ��12864д����
//==========================================================================
void LCD_WrData(unsigned char wdata)
{
    LCD12864_RS = 1;
    LCD12864_RW = 0;
    LCD_WriteDataPort(wdata);//LCD_DATA = wdata;
    LCD12864_EN = 1;
    _LCD_NOP_();
    LCD12864_EN = 0;
}

//==========================================================================
// ��������: ��ַת��
//==========================================================================
void LCD_Select(unsigned char x, unsigned char y)
{
    if( y>=64 )
    {
        LCD12864_CS2 = 1;                            //ѡ������
        LCD12864_CS1 = 0;
    }
    else
    {
        LCD12864_CS1 = 1;                            //ѡ������
        LCD12864_CS2 = 0;
    }
    
    LCD_WrCmd(0xC0);
    LCD_WrCmd(0x40 + (y&0x3F));                 //�е�ַ
    LCD_WrCmd(0xB8 + (x&7));                    //��
}
//==========================================================================
// ��������: LCD����
// �����ӿ�: void LCD_Full(void);
//==========================================================================
void LCD_Full(void)
{
    unsigned char i, x=0;
    
    for(x=0; x<8; x++)
    {
        LCD_Select(x, 0);                       //ѡ������
        for(i=0; i<64; i++)
        {
            LCD_WrData(0xFF);
        }
        
        LCD_Select(x, 64);                      //ѡ������
        for(i=0; i<64; i++)
        {
            LCD_WrData(0xFF);
        }
    }
}

//==========================================================================
// ��������: LCD��ʾһ��
// �����ӿ�: void LCD_DispOneRow(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len);
// ���ڲ���: unsigned char x, unsigned char y, unsigned char *buf, unsigned char len
//           x-��ʾ��, 0~7(һ�п��Ϊ8����)
//           y-��ʾ��ʼ��, 0~127, ��������ʾ��Ч
//==========================================================================
void LCD_DispOneRow(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len)
{
    x = x&7;
    
    LCD_Select(x, y);
    while(len--)
    {
        LCD_WrData(*buf);
        buf++;
        y++;
        if( y == 64 )
        {
            LCD_Select(x, y);                   //ѡ������
        }
    }
}

//==========================================================================
// ��������: LCD����
// �����ӿ�: void DrawDot_12864(unsigned char x, unsigned char y, unsigned char type);
// ���ڲ���: unsigned char x, unsigned char y, unsigned char type
//           x-��ʾ��, 0~63
//           y-��ʾ��ʼ��, 0~127, ��������ʾ��Ч
//==========================================================================
void DrawDot_12864(unsigned char x, unsigned char y, unsigned char type)
{
    unsigned char X, Y;                         //X�洢�е�ַ, Y�洢�е�ַ
    unsigned char dat, temp;
    
    if(((int)x >= 0) && (x < 64))
    {
        X = x/8;
        Y = y;
    }
    LCD_Select(X, Y);                           //��ַת��
    
    dat = LCD_RdData();                         //�ٶ�
    dat = LCD_RdData();
    
    temp = x % 8;                               //����Ϊ�������е�λ��
    
    switch(type)                                //��������, 0��, 1��, 2ȡ��
    {
    case 0:
        dat &= ~(0x01 << temp);                //�޸ĸõ�ͬʱ��������λ����
        break;
    case 1:
        dat |=  (0x01 << temp);                 //�޸ĸõ�ͬʱ��������λ����
        break;
    case 2:
        dat ^=  (0x01 << temp);                //�޸ĸõ�ͬʱ��������λ����
        break;
    default:
        break;  
    }
    LCD_Select(X, Y);
    LCD_WrData(dat);                            //���ֽ�����д��
}
//==========================================================================
// ��������: LCD����
// �����ӿ�: void LCD_Clr(void);
//==========================================================================
void LCD_Clr(void)
{
    unsigned char i, x=0;
    
    for(x=0; x<8; x++)
    {
        LCD_Select(x, 0);                       //ѡ������
        for(i=0; i<64; i++)
        {
            LCD_WrData(0);
        }
        
        LCD_Select(x, 64);                      //ѡ������
        for(i=0; i<64; i++)
        {
            LCD_WrData(0);
        }
    }
}
//==========================================================================
// ��������: LCD��ʼ��
// �����ӿ�: void LCD_Init(void);
//==========================================================================
void LCD_Init (void)
{
	LCD_GpioInit();
    LCD12864_BL  = 0;                                //�򿪱���
    LCD12864_RST = 0;
    delay_ms(100);
    LCD12864_RST = 1;
    delay_ms(1);
    
    LCD12864_CS1 = 1;
    LCD12864_CS2 = 1;
    LCD_WrCmd(0xC0);                            //������ʾ��ʼ��
    LCD_WrCmd(0x3F);                            //��ʾ��
	LCD_Clr();																	//����
}

/*****************************************************************************
* ��������: ������ʾ128x64����ͼƬ��Ҫ��ͼƬ��128����64�����ص�
* �����ӿ�: void Disp_Img(unsigned char code *img)
* ��ڲ���: unsigned char code *img
*           img-ָ�����ʾ��ͼƬ
*****************************************************************************/
void LCD_DispFullImg(unsigned char *img)			   //��ʾͼƬ
{
	unsigned char i=0,j=0;
	for(i=0;i<8;i++)
	{
		LCD_Select(i, 0);													
		for(j=0;j<128;j++)
		{
			if( j == 64 )
			{
					LCD_Select(i, j);                   //ѡ������
			}
			LCD_WrData(*img);
			img++;
		}
	}
}

/*****************************************************************************
* ��������:ָ��λ����ʾ�ض���С��ͼƬ
* �����ӿ�: void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-��ʾ��ʼ��, 0~7(һ�п��Ϊ8����)
*           y-��ʾ��ʼ��, 0~127, ��������ʾ��Ч
*						wid-ͼƬ��ȣ���λ�����ص㣬���ֵ64
*						lon-ͼƬ���ȣ���λ�����ص㣬���ֵ128
*           img-ָ�룬ָ�����ʾ��ͼƬ
*****************************************************************************/
void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char *img)			   //ָ��λ����ʾ�ض���С��ͼƬ
{
	unsigned char i=0,j=0,k;
	k=wid/8;
	for(i=x;i<(k+x);i++)
	{
		LCD_Select(i, y);
		for(j=y;j<(lon+y);j++)
		{
			if( j == 64 )
			{
					LCD_Select(i, j);                   //ѡ������
			}
			LCD_WrData(*img);
			img++;
		}
	}
}

/*****************************************************************************
* ��������:��x��дһ���ַ��������м�����ֵ�����������м��������ֵ��
*					 ���м�������ȷ��ѡ����OR�Ұ���Ļ
* �����ӿ�: unsigned char LCD_WriteCharData(unsigned char c,unsigned char x, unsigned char lineCount)
* ��ڲ���: unsigned char c,unsigned char x, unsigned char lineCount
*						unsigned char c,Ҫд����ַ�
*						unsigned char x,Ҫд����� 
*						unsigned char lineCount,�м���������ȷ��ѡ������Ұ���Ļ
*****************************************************************************/
void LCD_WriteEnglish(unsigned char x,unsigned char y, unsigned char c)
{
	unsigned char i,line,temp;
	c -= 32;
	for(i=0;i<2;i++)
	{
		LCD_Select(x+i,y);
		temp=i<<3;
		for(line=0; line<8; line++)
		{
			if((y+line)==64)
			{LCD_Select(x+i,y+line);}//ѡ���Ұ���Ļ
			LCD_WrData(font8x16[c][line+temp]);//�˴���temp���ڴ��i*8�Ľ����
		}
	}
}

/*****************************************************************************
* ��������:ָ��λ��дһ��Ӣ���ַ�����ÿ������д21���ַ�
* �����ӿ�: void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
* ��ڲ���: unsigned char x,unsigned char y,unsigned char *s
*						unsigned char x,��ֵ��ȡֵ��Χ0~7
*						unsigned char y,��ֵ��ȡֵ��Χ0~127
*						unsigned char *s,sָ���д����ַ���
*****************************************************************************/
void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
{		
	unsigned char lineCount=0;
	unsigned char dataCount=0;
	while(*s)
	{
		if(lineCount>120)
		{//��ʾ��һ�У�����һ��
			x += 2;
			LCD_WriteEnglish( x, 0, *s);
			lineCount=0+8;
		}
		else
		{
			LCD_WriteEnglish( x, y+lineCount, *s);
			lineCount+=8;//ÿ��Ӣ���ַ�ռ��8��
		}
		s++;
		dataCount++;
		if(dataCount > (16*4))//������Ļ�ַ��Ѿ���ʾ��
		{
			return;
		}
	}
}

/*****************************************************************************
* ��������:ָ��λ����ʾһ������
* �����ӿ�: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-��ʾ��ʼ��, 0~6(һ�и߶�Ϊ8����)
*           y-��ʾ��ʼ��, 0~111, ��������ʾ��Ч
*           img-ָ�룬ָ�����ʾ�ĺ��ֵ���16*16
*****************************************************************************/
void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
{
	LCD_DispImg( x, y, 16, 16, (unsigned char *)img);
}

/*****************************************************************************
* ��������:ָ��λ����ʾһ������
* �����ӿ�: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char *img, unsigned char len
*           x-��ʾ��ʼ��, 0~6(һ�и߶�Ϊ8����)
*           y-��ʾ��ʼ��, 0~111, ��������ʾ��Ч
*           img-ָ�룬ָ�����ʾ�ĺ��ֵ���16*16
*						len-Ҫ��ʾ�ĺ��ָ���
*****************************************************************************/
void LCD_WriteChineseString(unsigned char x, unsigned char y, unsigned char *img, unsigned char len)	
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		LCD_WriteChinese(x, y, (unsigned char *)img);
		y += 16;
		img += 32;
	}
}
