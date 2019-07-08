#ifndef _LCD12864_H_
#define _LCD12864_H_

void LCD_Init(void);
void LCD_Clr(void);
void LCD_Full(void);
void LCD_DispOneRow(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len);
void DrawDot_12864(unsigned char y,unsigned char x,unsigned char type);

/**************************************************************************************************************************************
* ��������: ������ʾ128x64����ͼƬ��Ҫ��ͼƬ��128����64�����ص�
* �����ӿ�: void Disp_Img(unsigned char code *img)
* ��ڲ���: unsigned char code *img
*           img-ָ�����ʾ��ͼƬ
**************************************************************************************************************************************/
void LCD_DispFullImg(unsigned char *img);

/**************************************************************************************************************************************
* ��������:ָ��λ����ʾ�ض���С��ͼƬ
* �����ӿ�: void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-��ʾ��ʼ��, 0~7(һ�п��Ϊ8����)
*           y-��ʾ��ʼ��, 0~127, ��������ʾ��Ч
*						wid-ͼƬ��ȣ���λ�����ص㣬���ֵ64
*						lon-ͼƬ���ȣ���λ�����ص㣬���ֵ128
*           img-ָ�룬ָ�����ʾ��ͼƬ
**************************************************************************************************************************************/
void LCD_DispImg(unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char *img);

/**************************************************************************************************************************************
* ��������:��x��дһ���ַ��������м�����ֵ�����������м��������ֵ��
*					 ���м�������ȷ��ѡ����OR�Ұ���Ļ
* �����ӿ�: unsigned char LCD_WriteCharData(unsigned char c,unsigned char x, unsigned char lineCount)
* ��ڲ���: unsigned char c,unsigned char x, unsigned char lineCount
*						unsigned char c,Ҫд����ַ�
*						unsigned char x,Ҫд����� 
*						unsigned char lineCount,�м���������ȷ��ѡ������Ұ���Ļ
**************************************************************************************************************************************/
void LCD_WriteEnglish(unsigned char x,unsigned char y, unsigned char c);

/**************************************************************************************************************************************
* ��������:ָ��λ��дһ��Ӣ���ַ�����ÿ������д21���ַ�
* �����ӿ�: void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s)
* ��ڲ���: unsigned char x,unsigned char y,unsigned char *s
*						unsigned char x,��ֵ��ȡֵ��Χ0~7
*						unsigned char y,��ֵ��ȡֵ��Χ0~127
*						unsigned char *s,sָ���д����ַ���
**************************************************************************************************************************************/
void LCD_WriteEnglishString(unsigned char x,unsigned char y,unsigned char *s);

/**************************************************************************************************************************************
* ��������:ָ��λ����ʾһ������
* �����ӿ�: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char wid, unsigned char lon, unsigned char code *img
*           x-��ʾ��ʼ��, 0~6(һ�и߶�Ϊ8����)
*           y-��ʾ��ʼ��, 0~111, ��������ʾ��Ч
*           img-ָ�룬ָ�����ʾ�ĺ��ֵ���16*16
**************************************************************************************************************************************/
void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img);

/**************************************************************************************************************************************
* ��������:ָ��λ����ʾһ������
* �����ӿ�: void LCD_WriteChinese(unsigned char x, unsigned char y, unsigned char *img)	
* ��ڲ���: unsigned char x, unsigned char y, unsigned char *img, unsigned char len
*           x-��ʾ��ʼ��, 0~6(һ�и߶�Ϊ8����)
*           y-��ʾ��ʼ��, 0~111, ��������ʾ��Ч
*           img-ָ�룬ָ�����ʾ�ĺ��ֵ���16*16
*						len-Ҫ��ʾ�ĺ��ָ���
**************************************************************************************************************************************/
void LCD_WriteChineseString(unsigned char x, unsigned char y, unsigned char *img, unsigned char len);

#endif //_LCD12864_H_
