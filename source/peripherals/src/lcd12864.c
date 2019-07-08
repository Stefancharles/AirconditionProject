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
*函数：void HC595_GpioInit(void)
*功能：74HC595信号引脚对应的I/O口功能初始化
*输入：无
*输出：无
*特殊说明：无
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
// 函数：unsigned char LCD_ReadDataPort(void)
// 函数功能：读显示屏数据端口DB7~DB0的数据,并将其返回
// 输入：无
// 返回：从显示屏DB7~DB0的数据
// 特殊说明：无
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
// 函数：unsigned char LCD_WriteDataPort(void)
// 函数功能：从显示屏数据端口DB7~DB0并行输入temp的数值
// 输入：unsigned char temp，待写入的数据
// 返回：无
// 特殊说明：无
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
// 函数功能: 检测是否空闲
//==========================================================================
void LCD_BusyChk(void)
{
    unsigned int timeout = 0;
    
    LCD12864_EN = 0;
    LCD12864_RS = 0;
    LCD12864_RW = 1;
    LCD12864_EN = 1;
    while((LCD_ReadDataPort() & 0x80) && (timeout < 60))//忙状态检测, 等待超时时间为60ms
	{	
		timeout++;
		delay_ms(1);
	}
    LCD12864_EN = 0;
}

//==========================================================================
// 函数功能: 往12864读数据
//==========================================================================
unsigned char LCD_RdData(void)
{
    unsigned char temp;
    
    LCD_BusyChk();
    LCD12864_EN = 0;
    //LCD_DATA = 0xFF;                            //IO口置高电平, 以便读引脚
    LCD12864_RS = 1;
    LCD12864_RW = 1;
    LCD12864_EN  = 1;
    _LCD_NOP_();                                        //使能延时, 如果是较快的CPU应该延时久一些
    _LCD_NOP_();
    _LCD_NOP_();
    temp = LCD_ReadDataPort();//temp = LCD_DATA;
    
    return temp;
}
//==========================================================================
// 函数功能: 往12864写命令
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
// 函数功能: 往12864写数据
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
// 函数功能: 地址转换
//==========================================================================
void LCD_Select(unsigned char x, unsigned char y)
{
    if( y>=64 )
    {
        LCD12864_CS2 = 1;                            //选中左屏
        LCD12864_CS1 = 0;
    }
    else
    {
        LCD12864_CS1 = 1;                            //选中右屏
        LCD12864_CS2 = 0;
    }
    
    LCD_WrCmd(0xC0);
    LCD_WrCmd(0x40 + (y&0x3F));                 //列地址
    LCD_WrCmd(0xB8 + (x&7));                    //行
}
//==========================================================================
// 函数功能: LCD满屏
// 函数接口: void LCD_Full(void);
//==========================================================================
void LCD_Full(void)
{
    unsigned char i, x=0;
    
    for(x=0; x<8; x++)
    {
        LCD_Select(x, 0);                       //选中左屏
        for(i=0; i<64; i++)
        {
            LCD_WrData(0xFF);
        }
        
        LCD_Select(x, 64);                      //选中右屏
        for(i=0; i<64; i++)
        {
            LCD_WrData(0xFF);
        }
    }
}

//==========================================================================
// 函数功能: LCD显示一行
// 函数接口: void LCD_DispOneRow(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len);
// 出口参数: unsigned char x, unsigned char y, unsigned char *buf, unsigned char len
//           x-显示行, 0~7(一行宽度为8个点)
//           y-显示起始列, 0~127, 超过列显示无效
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
            LCD_Select(x, y);                   //选中右屏
        }
    }
}

//==========================================================================
// 函数功能: LCD画点
// 函数接口: void DrawDot_12864(unsigned char x, unsigned char y, unsigned char type);
// 出口参数: unsigned char x, unsigned char y, unsigned char type
//           x-显示行, 0~63
//           y-显示起始列, 0~127, 超过列显示无效
//==========================================================================
void DrawDot_12864(unsigned char x, unsigned char y, unsigned char type)
{
    unsigned char X, Y;                         //X存储行地址, Y存储列地址
    unsigned char dat, temp;
    
    if(((int)x >= 0) && (x < 64))
    {
        X = x/8;
        Y = y;
    }
    LCD_Select(X, Y);                           //地址转换
    
    dat = LCD_RdData();                         //假读
    dat = LCD_RdData();
    
    temp = x % 8;                               //余数为点在字中的位置
    
    switch(type)                                //画点类型, 0白, 1黑, 2取反
    {
    case 0:
        dat &= ~(0x01 << temp);                //修改该点同时保持其他位不变
        break;
    case 1:
        dat |=  (0x01 << temp);                 //修改该点同时保持其他位不变
        break;
    case 2:
        dat ^=  (0x01 << temp);                //修改该点同时保持其他位不变
        break;
    default:
        break;  
    }
    LCD_Select(X, Y);
    LCD_WrData(dat);                            //将字节数据写回
}
//==========================================================================
// 函数功能: LCD清屏
// 函数接口: void LCD_Clr(void);
//==========================================================================
void LCD_Clr(void)
{
    unsigned char i, x=0;
    
    for(x=0; x<8; x++)
    {
        LCD_Select(x, 0);                       //选中左屏
        for(i=0; i<64; i++)
        {
            LCD_WrData(0);
        }
        
        LCD_Select(x, 64);                      //选中右屏
        for(i=0; i<64; i++)
        {
            LCD_WrData(0);
        }
    }
}
//==========================================================================
// 函数功能: LCD初始化
// 函数接口: void LCD_Init(void);
//==========================================================================
void LCD_Init (void)
{
	LCD_GpioInit();
    LCD12864_BL  = 0;                                //打开背光
    LCD12864_RST = 0;
    delay_ms(100);
    LCD12864_RST = 1;
    delay_ms(1);
    
    LCD12864_CS1 = 1;
    LCD12864_CS2 = 1;
    LCD_WrCmd(0xC0);                            //设置显示起始行
    LCD_WrCmd(0x3F);                            //显示打开
	LCD_Clr();																	//清屏
}

/*****************************************************************************
* 函数功能: 整屏显示128x64点阵图片，要求图片长128、宽64个像素点
* 函数接口: void Disp_Img(unsigned char code *img)
* 入口参数: unsigned char code *img
*           img-指向待显示的图片
*****************************************************************************/
void LCD_DispFullImg(unsigned char *img)			   //显示图片
{
	unsigned char i=0,j=0;
	for(i=0;i<8;i++)
	{
		LCD_Select(i, 0);													
		for(j=0;j<128;j++)
		{
			if( j == 64 )
			{
					LCD_Select(i, j);                   //选中右屏
			}
			LCD_WrData(*img);
			img++;
		}
	}
}

/*****************************************************************************
* 函数功能:指定位置显示特定大小的图片
* 函数接口: void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-显示起始行, 0~7(一行宽度为8个点)
*           y-显示起始列, 0~127, 超过列显示无效
*						wid-图片宽度，单位：像素点，最大值64
*						lon-图片长度，单位：像素点，最大值128
*           img-指针，指向待显示的图片
*****************************************************************************/
void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char *img)			   //指定位置显示特定大小的图片
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
					LCD_Select(i, j);                   //选中右屏
			}
			LCD_WrData(*img);
			img++;
		}
	}
}

/*****************************************************************************
* 函数功能:在x行写一个字符，输入列计数初值，返回最终列计数的最后值，
*					 该列计数用于确定选中左OR右半屏幕
* 函数接口: unsigned char LCD_WriteCharData(unsigned char c,unsigned char x, unsigned char lineCount)
* 入口参数: unsigned char c,unsigned char x, unsigned char lineCount
*						unsigned char c,要写入的字符
*						unsigned char x,要写入的行 
*						unsigned char lineCount,列计数，用于确定选中左或右半屏幕
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
			{LCD_Select(x+i,y+line);}//选中右半屏幕
			LCD_WrData(font8x16[c][line+temp]);//此处的temp用于存放i*8的结果，
		}
	}
}

/*****************************************************************************
* 函数功能:指定位置写一串英文字符串，每行最多可写21个字符
* 函数接口: void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
* 入口参数: unsigned char x,unsigned char y,unsigned char *s
*						unsigned char x,行值，取值范围0~7
*						unsigned char y,列值，取值范围0~127
*						unsigned char *s,s指向待写入的字符串
*****************************************************************************/
void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
{		
	unsigned char lineCount=0;
	unsigned char dataCount=0;
	while(*s)
	{
		if(lineCount>120)
		{//显示满一行，另起一行
			x += 2;
			LCD_WriteEnglish( x, 0, *s);
			lineCount=0+8;
		}
		else
		{
			LCD_WriteEnglish( x, y+lineCount, *s);
			lineCount+=8;//每个英文字符占用8列
		}
		s++;
		dataCount++;
		if(dataCount > (16*4))//整个屏幕字符已经显示满
		{
			return;
		}
	}
}

/*****************************************************************************
* 函数功能:指定位置显示一个汉字
* 函数接口: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-显示起始行, 0~6(一行高度为8个点)
*           y-显示起始列, 0~111, 超过列显示无效
*           img-指针，指向待显示的汉字点阵16*16
*****************************************************************************/
void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
{
	LCD_DispImg( x, y, 16, 16, (unsigned char *)img);
}

/*****************************************************************************
* 函数功能:指定位置显示一串汉字
* 函数接口: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char *img, unsigned char len
*           x-显示起始行, 0~6(一行高度为8个点)
*           y-显示起始列, 0~111, 超过列显示无效
*           img-指针，指向待显示的汉字点阵16*16
*						len-要显示的汉字个数
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
