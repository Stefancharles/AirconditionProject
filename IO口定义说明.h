
/*1.PCF8563-I2C总线IO口定义*/
#define PCF8563_I2C_SCL				PA7
#define PCF8563_I2C_SDA				PA6

/*2.PCF8591的I2C总线IO口定义*/
#define PCF8591_I2C_SDA				PA5
#define PCF8591_I2C_SCL				PA4

/*3.键盘模块接线*/
#define K_PA1				PA3
#define K_PA3				PA2
#define K_PA4				PA1

/*4.继电器模块接线*/
#define RYLAY1				PC3
#define RYLAY2				PC2

/*5.LCD12864的信号接口*/
#define LCD12864_BL					PD14
#define LCD12864_RST				PD0
#define LCD12864_CS2				PE7
#define LCD12864_CS1				PE9
#define LCD12864_DB7				PE11
#define LCD12864_DB6_IN				PE13
#define LCD12864_DB5_IN				PE15
#define LCD12864_DB4_IN				PD9
#define LCD12864_DB3_IN				PD15
#define LCD12864_DB2_IN				PD1
#define LCD12864_DB1_IN				PE8
#define LCD12864_DB0_IN				PE10
#define LCD12864_EN					PE12
#define LCD12864_RW					PE14
#define LCD12864_RS					PD8

/*6.74HC595的信号接口*/
#define IC74HC595_DAT				PB0
#define IC74HC595_SCK				PC5
#define IC74HC595_RCK				PC4

/*7.USART1*/
#define USART1_TX					PA9
#define USART1_RX					PA10

/*8.蜂鸣器BUZZER*/
#define BUZZER0						PA8

/*9.红外数据输入脚（NEC红外遥控器，红外接收头信号输出）*/
#define RDATA 	PA0	 	//

