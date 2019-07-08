#ifndef _LCD12864_H_
#define _LCD12864_H_

void LCD_Init(void);
void LCD_Clr(void);
void LCD_Full(void);
void LCD_DispOneRow(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len);
void DrawDot_12864(unsigned char y,unsigned char x,unsigned char type);

/**************************************************************************************************************************************
* 函数功能: 整屏显示128x64点阵图片，要求图片长128、宽64个像素点
* 函数接口: void Disp_Img(unsigned char code *img)
* 入口参数: unsigned char code *img
*           img-指向待显示的图片
**************************************************************************************************************************************/
void LCD_DispFullImg(unsigned char *img);

/**************************************************************************************************************************************
* 函数功能:指定位置显示特定大小的图片
* 函数接口: void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-显示起始行, 0~7(一行宽度为8个点)
*           y-显示起始列, 0~127, 超过列显示无效
*						wid-图片宽度，单位：像素点，最大值64
*						lon-图片长度，单位：像素点，最大值128
*           img-指针，指向待显示的图片
**************************************************************************************************************************************/
void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char *img);

/**************************************************************************************************************************************
* 函数功能:在x行写一个字符，输入列计数初值，返回最终列计数的最后值，
*					 该列计数用于确定选中左OR右半屏幕
* 函数接口: unsigned char LCD_WriteCharData(unsigned char c,unsigned char x, unsigned char lineCount)
* 入口参数: unsigned char c,unsigned char x, unsigned char lineCount
*						unsigned char c,要写入的字符
*						unsigned char x,要写入的行 
*						unsigned char lineCount,列计数，用于确定选中左或右半屏幕
**************************************************************************************************************************************/
void LCD_WriteEnglish(unsigned char x,unsigned char y, unsigned char c);

/**************************************************************************************************************************************
* 函数功能:指定位置写一串英文字符串，每行最多可写21个字符
* 函数接口: void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
* 入口参数: unsigned char x,unsigned char y,unsigned char *s
*						unsigned char x,行值，取值范围0~7
*						unsigned char y,列值，取值范围0~127
*						unsigned char *s,s指向待写入的字符串
**************************************************************************************************************************************/
void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s);

/**************************************************************************************************************************************
* 函数功能:指定位置显示一个汉字
* 函数接口: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-显示起始行, 0~6(一行高度为8个点)
*           y-显示起始列, 0~111, 超过列显示无效
*           img-指针，指向待显示的汉字点阵16*16
**************************************************************************************************************************************/
void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img);

/**************************************************************************************************************************************
* 函数功能:指定位置显示一串汉字
* 函数接口: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* 入口参数: unsigned char x, unsigned char y, unsigned char *img, unsigned char len
*           x-显示起始行, 0~6(一行高度为8个点)
*           y-显示起始列, 0~111, 超过列显示无效
*           img-指针，指向待显示的汉字点阵16*16
*						len-要显示的汉字个数
**************************************************************************************************************************************/
void LCD_WriteChineseString(unsigned char x, unsigned char y, unsigned char *img, unsigned char len);

#endif //_LCD12864_H_
