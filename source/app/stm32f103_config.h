/******************************************************************************
*
*
*
*
*
*
*
*
*
*
******************************************************************************/

#ifndef		_stm32f103_config_h_
#define		_stm32f103_config_h_

/******************************************************************************
                               外部函数头文件
                   应用到不同的外设头文件请在这里修改即可                        
******************************************************************************/

#include "sys.h"
#include "stm32f10x.h"

#include <string.h>	//内存操作相关函数库
#include <math.h>	//数学运算相关函数库
#include <stdio.h>	//printf相关
#include <stdbool.h>//布尔运算相关

#include "delay.h"
#include "hal_usart1.h"
#include "hal_uart4.h"
#include "hal_iic.h"
#include "hal_iic1.h"
#include "hal_key.h"
#include "hal_timer2.h"
#include "hal_buzzer.h"


/*短暂延时 1us */
#define _NOP_()	delay_us(2)

/*******************PCF8563-I2C总线*********************/
#define PCF8563_I2C_SCL_PIN_PORT		GPIOA
#define PCF8563_I2C_SCL_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define PCF8563_I2C_SCL_PIN				GPIO_Pin_7
#define PCF8563_I2C_SCL_Init			PCF8563_I2C_SCL_PIN_PORT_CLK,\
										PCF8563_I2C_SCL_PIN_PORT,\
										PCF8563_I2C_SCL_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define PCF8563_I2C_SCL					PAout(7)

#define PCF8563_I2C_SDA_PIN_PORT		GPIOA
#define PCF8563_I2C_SDA_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define PCF8563_I2C_SDA_PIN				GPIO_Pin_6
#define PCF8563_I2C_SDA_OUT_Init		PCF8563_I2C_SDA_PIN_PORT_CLK,\
										PCF8563_I2C_SDA_PIN_PORT,\
										PCF8563_I2C_SDA_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define PCF8563_I2C_SDA_OUT				PAout(6)
#define PCF8563_I2C_SDA_IN_Init			PCF8563_I2C_SDA_PIN_PORT_CLK,\
										PCF8563_I2C_SDA_PIN_PORT,\
										PCF8563_I2C_SDA_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define PCF8563_I2C_SDA_IN				PAin(6)

/*******************PCF8591-I2C总线*********************/
#define PCF8591_I2C_SDA_PIN_PORT		GPIOA
#define PCF8591_I2C_SDA_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define PCF8591_I2C_SDA_PIN				GPIO_Pin_5
#define PCF8591_I2C_SDA_OUT_Init		PCF8591_I2C_SDA_PIN_PORT_CLK,\
										PCF8591_I2C_SDA_PIN_PORT,\
										PCF8591_I2C_SDA_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define PCF8591_I2C_SDA_OUT				PAout(5)
#define PCF8591_I2C_SDA_IN_Init			PCF8591_I2C_SDA_PIN_PORT_CLK,\
										PCF8591_I2C_SDA_PIN_PORT,\
										PCF8591_I2C_SDA_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define PCF8591_I2C_SDA_IN				PAin(5)

#define PCF8591_I2C_SCL_PIN_PORT		GPIOA
#define PCF8591_I2C_SCL_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define PCF8591_I2C_SCL_PIN				GPIO_Pin_4
#define PCF8591_I2C_SCL_Init			PCF8591_I2C_SCL_PIN_PORT_CLK,\
										PCF8591_I2C_SCL_PIN_PORT,\
										PCF8591_I2C_SCL_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define PCF8591_I2C_SCL					PAout(4)

/*******************键盘模块接线*********************/
//K_PA1
#define K_PA1_PIN_PORT		GPIOA
#define K_PA1_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define K_PA1_PIN			GPIO_Pin_3
#define K_PA1_IN_Init		K_PA1_PIN_PORT_CLK,\
							K_PA1_PIN_PORT,\
							K_PA1_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define K_PA1				PAin(3)
//K_PA3
#define K_PA3_PIN_PORT		GPIOA
#define K_PA3_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define K_PA3_PIN			GPIO_Pin_2
#define K_PA3_IN_Init		K_PA3_PIN_PORT_CLK,\
							K_PA3_PIN_PORT,\
							K_PA3_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define K_PA3				PAin(2)
//K_PA4
#define K_PA4_PIN_PORT		GPIOA
#define K_PA4_PIN_PORT_CLK	RCC_APB2Periph_GPIOA
#define K_PA4_PIN			GPIO_Pin_1
#define K_PA4_IN_Init		K_PA4_PIN_PORT_CLK,\
							K_PA4_PIN_PORT,\
							K_PA4_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define K_PA4				PAin(1)

/*******************继电器模块接线*********************/
//RYLAY1-IN
#define RYLAY1_PIN_PORT		GPIOC
#define RYLAY1_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define RYLAY1_PIN			GPIO_Pin_3
#define RYLAY1_Init			RYLAY1_PIN_PORT_CLK,\
							RYLAY1_PIN_PORT,\
							RYLAY1_PIN,GPIO_Speed_10MHz,GPIO_Mode_Out_PP
#define RYLAY1				PCout(3)

//RYLAY2-IN
#define RYLAY2_PIN_PORT		GPIOC
#define RYLAY2_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define RYLAY2_PIN			GPIO_Pin_2
#define RYLAY2_Init			RYLAY2_PIN_PORT_CLK,\
							RYLAY2_PIN_PORT,\
							RYLAY2_PIN,GPIO_Speed_10MHz,GPIO_Mode_Out_PP
#define RYLAY2				PCout(2)

/*******************LCD的信号接口(逻辑管脚与物理管脚的对应表)*********************/
//BL
#define LCD12864_BL_PIN_PORT		GPIOD
#define LCD12864_BL_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_BL_PIN				GPIO_Pin_14
#define LCD12864_BL_Init			LCD12864_BL_PIN_PORT_CLK,\
									LCD12864_BL_PIN_PORT,\
									LCD12864_BL_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_BL					PDout(14)
//RST
#define LCD12864_RST_PIN_PORT		GPIOD
#define LCD12864_RST_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_RST_PIN			GPIO_Pin_0
#define LCD12864_RST_Init			LCD12864_RST_PIN_PORT_CLK,\
									LCD12864_RST_PIN_PORT,\
									LCD12864_RST_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_RST				PDout(0)
//CS2
#define LCD12864_CS2_PIN_PORT		GPIOE
#define LCD12864_CS2_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_CS2_PIN			GPIO_Pin_7
#define LCD12864_CS2_Init			LCD12864_CS2_PIN_PORT_CLK,\
									LCD12864_CS2_PIN_PORT,\
									LCD12864_CS2_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_CS2				PEout(7)
//CS1
#define LCD12864_CS1_PIN_PORT		GPIOE
#define LCD12864_CS1_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_CS1_PIN			GPIO_Pin_9
#define LCD12864_CS1_Init			LCD12864_CS1_PIN_PORT_CLK,\
									LCD12864_CS1_PIN_PORT,\
									LCD12864_CS1_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_CS1				PEout(9)
//DB7
#define LCD12864_DB7_PIN_PORT		GPIOE
#define LCD12864_DB7_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_DB7_PIN			GPIO_Pin_11
#define LCD12864_DB7_Init			LCD12864_DB7_PIN_PORT_CLK,\
									LCD12864_DB7_PIN_PORT,\
									LCD12864_DB7_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB7				PEout(11)
#define LCD12864_DB7_IN_Init		LCD12864_DB7_PIN_PORT_CLK,\
									LCD12864_DB7_PIN_PORT,\
									LCD12864_DB7_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB7_IN				PEin(11)
//DB6
#define LCD12864_DB6_PIN_PORT		GPIOE
#define LCD12864_DB6_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_DB6_PIN			GPIO_Pin_13
#define LCD12864_DB6_Init			LCD12864_DB6_PIN_PORT_CLK,\
									LCD12864_DB6_PIN_PORT,\
									LCD12864_DB6_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB6				PEout(13)
#define LCD12864_DB6_IN_Init		LCD12864_DB6_PIN_PORT_CLK,\
									LCD12864_DB6_PIN_PORT,\
									LCD12864_DB6_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB6_IN				PEin(13)
//DB5
#define LCD12864_DB5_PIN_PORT		GPIOE
#define LCD12864_DB5_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_DB5_PIN			GPIO_Pin_15
#define LCD12864_DB5_Init			LCD12864_DB5_PIN_PORT_CLK,\
									LCD12864_DB5_PIN_PORT,\
									LCD12864_DB5_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB5				PEout(15)
#define LCD12864_DB5_IN_Init		LCD12864_DB5_PIN_PORT_CLK,\
									LCD12864_DB5_PIN_PORT,\
									LCD12864_DB5_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB5_IN				PEin(15)
//DB4
#define LCD12864_DB4_PIN_PORT		GPIOD
#define LCD12864_DB4_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_DB4_PIN			GPIO_Pin_9
#define LCD12864_DB4_Init			LCD12864_DB4_PIN_PORT_CLK,\
									LCD12864_DB4_PIN_PORT,\
									LCD12864_DB4_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB4				PDout(9)
#define LCD12864_DB4_IN_Init		LCD12864_DB4_PIN_PORT_CLK,\
									LCD12864_DB4_PIN_PORT,\
									LCD12864_DB4_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB4_IN				PDin(9)
//DB3
#define LCD12864_DB3_PIN_PORT		GPIOD
#define LCD12864_DB3_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_DB3_PIN			GPIO_Pin_15
#define LCD12864_DB3_Init			LCD12864_DB3_PIN_PORT_CLK,\
									LCD12864_DB3_PIN_PORT,\
									LCD12864_DB3_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB3				PDout(15)
#define LCD12864_DB3_IN_Init		LCD12864_DB3_PIN_PORT_CLK,\
									LCD12864_DB3_PIN_PORT,\
									LCD12864_DB3_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB3_IN				PDin(15)
//DB2
#define LCD12864_DB2_PIN_PORT		GPIOD
#define LCD12864_DB2_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_DB2_PIN			GPIO_Pin_1
#define LCD12864_DB2_Init			LCD12864_DB2_PIN_PORT_CLK,\
									LCD12864_DB2_PIN_PORT,\
									LCD12864_DB2_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB2				PDout(1)
#define LCD12864_DB2_IN_Init		LCD12864_DB2_PIN_PORT_CLK,\
									LCD12864_DB2_PIN_PORT,\
									LCD12864_DB2_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB2_IN				PDin(1)
//DB1
#define LCD12864_DB1_PIN_PORT		GPIOE
#define LCD12864_DB1_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_DB1_PIN			GPIO_Pin_8
#define LCD12864_DB1_Init			LCD12864_DB1_PIN_PORT_CLK,\
									LCD12864_DB1_PIN_PORT,\
									LCD12864_DB1_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB1				PEout(8)
#define LCD12864_DB1_IN_Init		LCD12864_DB1_PIN_PORT_CLK,\
									LCD12864_DB1_PIN_PORT,\
									LCD12864_DB1_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB1_IN				PEin(8)
//DB0
#define LCD12864_DB0_PIN_PORT		GPIOE
#define LCD12864_DB0_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_DB0_PIN			GPIO_Pin_10
#define LCD12864_DB0_Init			LCD12864_DB0_PIN_PORT_CLK,\
									LCD12864_DB0_PIN_PORT,\
									LCD12864_DB0_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_DB0				PEout(10)
#define LCD12864_DB0_IN_Init		LCD12864_DB0_PIN_PORT_CLK,\
									LCD12864_DB0_PIN_PORT,\
									LCD12864_DB0_PIN,GPIO_Speed_2MHz,GPIO_Mode_IPU
#define LCD12864_DB0_IN				PEin(10)
//EN
#define LCD12864_EN_PIN_PORT		GPIOE
#define LCD12864_EN_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_EN_PIN				GPIO_Pin_12
#define LCD12864_EN_Init			LCD12864_EN_PIN_PORT_CLK,\
									LCD12864_EN_PIN_PORT,\
									LCD12864_EN_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_EN					PEout(12)
//RW
#define LCD12864_RW_PIN_PORT		GPIOE
#define LCD12864_RW_PIN_PORT_CLK	RCC_APB2Periph_GPIOE
#define LCD12864_RW_PIN				GPIO_Pin_14
#define LCD12864_RW_Init			LCD12864_RW_PIN_PORT_CLK,\
									LCD12864_RW_PIN_PORT,\
									LCD12864_RW_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_RW					PEout(14)
//RS
#define LCD12864_RS_PIN_PORT		GPIOD
#define LCD12864_RS_PIN_PORT_CLK	RCC_APB2Periph_GPIOD
#define LCD12864_RS_PIN				GPIO_Pin_8
#define LCD12864_RS_Init			LCD12864_RS_PIN_PORT_CLK,\
									LCD12864_RS_PIN_PORT,\
									LCD12864_RS_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define LCD12864_RS					PDout(8)

/*******************74HC595的信号接口*********************/
#define IC74HC595_DAT_PIN_PORT		GPIOB
#define IC74HC595_DAT_PIN_PORT_CLK	RCC_APB2Periph_GPIOB
#define IC74HC595_DAT_PIN			GPIO_Pin_0
#define IC74HC595_DAT_Init			IC74HC595_DAT_PIN_PORT_CLK,\
									IC74HC595_DAT_PIN_PORT,\
									IC74HC595_DAT_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_DAT				PBout(0)

#define IC74HC595_SCK_PIN_PORT		GPIOC
#define IC74HC595_SCK_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define IC74HC595_SCK_PIN			GPIO_Pin_5
#define IC74HC595_SCK_Init			IC74HC595_SCK_PIN_PORT_CLK,\
									IC74HC595_SCK_PIN_PORT,\
									IC74HC595_SCK_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_SCK				PCout(5)

#define IC74HC595_RCK_PIN_PORT		GPIOC
#define IC74HC595_RCK_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define IC74HC595_RCK_PIN			GPIO_Pin_4
#define IC74HC595_RCK_Init			IC74HC595_RCK_PIN_PORT_CLK,\
									IC74HC595_RCK_PIN_PORT,\
									IC74HC595_RCK_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_RCK				PCout(4)

#endif //_stm32f103_config_h_

