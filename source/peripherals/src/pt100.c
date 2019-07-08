#include "pt100.h"

#define AMPLIFY_VALUE ((uint32_t)(10000))  //放大倍数，使数据避免浮点运算，从而提高MCU运行效率
const uint32_t VOLTAGE_PT100_TEMP_TABLE[] = //对应的电压温度关系
{//实际电压				
	0.0000*AMPLIFY_VALUE,//	0	℃
	0.0253*AMPLIFY_VALUE,//	1	℃
	0.0505*AMPLIFY_VALUE,//	2	℃
	0.0757*AMPLIFY_VALUE,//	3	℃
	0.1010*AMPLIFY_VALUE,//	4	℃
	0.1262*AMPLIFY_VALUE,//	5	℃
	0.1514*AMPLIFY_VALUE,//	6	℃
	0.1766*AMPLIFY_VALUE,//	7	℃
	0.2018*AMPLIFY_VALUE,//	8	℃
	0.2269*AMPLIFY_VALUE,//	9	℃
	0.2521*AMPLIFY_VALUE,//	10	℃
	0.2773*AMPLIFY_VALUE,//	11	℃
	0.3024*AMPLIFY_VALUE,//	12	℃
	0.3263*AMPLIFY_VALUE,//	13	℃
	0.3527*AMPLIFY_VALUE,//	14	℃
	0.3778*AMPLIFY_VALUE,//	15	℃
	0.4029*AMPLIFY_VALUE,//	16	℃
	0.4280*AMPLIFY_VALUE,//	17	℃
	0.4531*AMPLIFY_VALUE,//	18	℃
	0.4776*AMPLIFY_VALUE,//	19	℃
	0.5026*AMPLIFY_VALUE,//	20	℃
	0.5277*AMPLIFY_VALUE,//	21	℃
	0.5528*AMPLIFY_VALUE,//	22	℃
	0.5778*AMPLIFY_VALUE,//	23	℃
	0.6028*AMPLIFY_VALUE,//	24	℃
	0.6272*AMPLIFY_VALUE,//	25	℃
	0.6522*AMPLIFY_VALUE,//	26	℃
	0.6773*AMPLIFY_VALUE,//	27	℃
	0.7023*AMPLIFY_VALUE,//	28	℃
	0.7273*AMPLIFY_VALUE,//	29	℃
	0.7516*AMPLIFY_VALUE,//	30	℃
	0.7766*AMPLIFY_VALUE,//	31	℃
	0.8015*AMPLIFY_VALUE,//	32	℃
	0.8258*AMPLIFY_VALUE,//	33	℃
	0.8508*AMPLIFY_VALUE,//	34	℃
	0.8757*AMPLIFY_VALUE,//	35	℃
	0.9007*AMPLIFY_VALUE,//	36	℃
	0.9249*AMPLIFY_VALUE,//	37	℃
	0.9499*AMPLIFY_VALUE,//	38	℃
	0.9741*AMPLIFY_VALUE,//	39	℃
	0.9990*AMPLIFY_VALUE,//	40	℃
	1.0239*AMPLIFY_VALUE,//	41	℃
	1.0481*AMPLIFY_VALUE,//	42	℃
	1.0730*AMPLIFY_VALUE,//	43	℃
	1.1431*AMPLIFY_VALUE,//	44	℃
	1.1221*AMPLIFY_VALUE,//	45	℃
	1.1469*AMPLIFY_VALUE,//	46	℃
	1.1711*AMPLIFY_VALUE,//	47	℃
	1.1959*AMPLIFY_VALUE,//	48	℃
	1.2201*AMPLIFY_VALUE,//	49	℃
	1.2449*AMPLIFY_VALUE,//	50	℃
	1.2690*AMPLIFY_VALUE,//	51	℃
	1.2938*AMPLIFY_VALUE,//	52	℃
	1.3180*AMPLIFY_VALUE,//	53	℃
	1.3427*AMPLIFY_VALUE,//	54	℃
	1.3668*AMPLIFY_VALUE,//	55	℃
	1.3916*AMPLIFY_VALUE,//	56	℃
	1.4157*AMPLIFY_VALUE,//	57	℃
	1.4398*AMPLIFY_VALUE,//	58	℃
	1.4645*AMPLIFY_VALUE,//	59	℃
	1.4886*AMPLIFY_VALUE,//	60	℃
	1.5133*AMPLIFY_VALUE,//	61	℃
	1.5374*AMPLIFY_VALUE,//	62	℃
	1.5614*AMPLIFY_VALUE,//	63	℃
	1.5861*AMPLIFY_VALUE,//	64	℃
	1.6101*AMPLIFY_VALUE,//	65	℃
	1.6341*AMPLIFY_VALUE,//	66	℃
	1.6588*AMPLIFY_VALUE,//	67	℃
	1.6828*AMPLIFY_VALUE,//	68	℃
	1.7068*AMPLIFY_VALUE,//	69	℃
	1.7314*AMPLIFY_VALUE,//	70	℃
	1.7554*AMPLIFY_VALUE,//	71	℃
	1.7794*AMPLIFY_VALUE,//	72	℃
	1.8033*AMPLIFY_VALUE,//	73	℃
	1.8279*AMPLIFY_VALUE,//	74	℃
	1.8519*AMPLIFY_VALUE,//	75	℃
	1.8758*AMPLIFY_VALUE,//	76	℃
	1.8998*AMPLIFY_VALUE,//	77	℃
	1.9237*AMPLIFY_VALUE,//	78	℃
	1.9482*AMPLIFY_VALUE,//	79	℃
	1.9721*AMPLIFY_VALUE,//	80	℃
	1.9960*AMPLIFY_VALUE,//	81	℃
	2.0199*AMPLIFY_VALUE,//	82	℃
	2.0438*AMPLIFY_VALUE,//	83	℃
	2.0677*AMPLIFY_VALUE,//	84	℃
	2.0915*AMPLIFY_VALUE,//	85	℃
	2.1154*AMPLIFY_VALUE,//	86	℃
	2.1399*AMPLIFY_VALUE,//	87	℃
	2.1637*AMPLIFY_VALUE,//	88	℃
	2.1875*AMPLIFY_VALUE,//	89	℃
	2.2113*AMPLIFY_VALUE,//	90	℃
	2.2351*AMPLIFY_VALUE,//	91	℃
	2.2590*AMPLIFY_VALUE,//	92	℃
	2.2827*AMPLIFY_VALUE,//	93	℃
	2.3065*AMPLIFY_VALUE,//	94	℃
	2.3303*AMPLIFY_VALUE,//	95	℃
	2.3541*AMPLIFY_VALUE,//	96	℃
	2.3778*AMPLIFY_VALUE,//	97	℃
	2.4016*AMPLIFY_VALUE,//	98	℃
	2.4253*AMPLIFY_VALUE,//	99	℃
	2.4491*AMPLIFY_VALUE,//	100	℃
	2.4722*AMPLIFY_VALUE,//	101	℃
};

#define Vref 3.28*AMPLIFY_VALUE     //参考电压，单位：V

/********************************************************
*函数名称：uint32_t calculatePt100Voltage(uint8_t ADC_Value)
*功能：输入ADC值，输出对应电压值，单位：V；
*输入：uint8_t ADC_Value，ADC值，
*输出：电压值，单位：V；
*特殊说明：ADC分辨率：8位分辨率，2^8=256。
*		返回的电压值已经被扩大AMPLIFY_VALUE倍
**********************************************************/
uint32_t calculatePt100Voltage(uint8_t ADC_Value)
{
	uint32_t Vrt=0;
	
	Vrt=(Vref/(256))*ADC_Value;//热敏电阻两端电压
	
	return Vrt;
}

/********************************************************
*函数名称：int8_t calculatePt100Temperature(uint32_t Vol)
*功能：输入热电压值，单位：V，输出温度值，单位：℃
*输入：uint32_t Vol，电压值，单位：V
*输出：输出温度值，单位：℃
*特殊说明：温度范围0℃~101℃
**********************************************************/
int8_t calculatePt100Temperature(uint32_t Vol)
{
	uint8_t i=0;
	int8_t temp=0;
	uint32_t mide=0;
	
	for(i=0;i<102;i++)
	{
		if(i >= 101)
		{
			return 101;//101;//超出温度最大测量范围
		}
		else if(i == 0)
		{
			if(Vol <= VOLTAGE_PT100_TEMP_TABLE[0])
			{
				return 0;//超出温度最小测量范围
			}
		}
		
		if(Vol<=VOLTAGE_PT100_TEMP_TABLE[i+1] && Vol>=VOLTAGE_PT100_TEMP_TABLE[i])
		{
			mide = (VOLTAGE_PT100_TEMP_TABLE[i+1]+VOLTAGE_PT100_TEMP_TABLE[i])/2;
			if(Vol>=mide)
			{
				temp=i+1;
			}
			else
			{
				temp=i;
			}
			break;
		}
	}
	return (temp);
}

/********************************************************
*函数名称：int8_t getPt100Temperature(uint8_t ch)
*功能：测算出当前温度
*输入：uint8_t ch，通道号
*输出：输出温度值，有符号，单位：℃
*特殊说明：度范围0℃~101℃
**********************************************************/
int8_t getPt100Temperature(uint8_t ch)
{
	int8_t temp;
	uint8_t adcVale;
	uint32_t voltage;
	
	PCF8591_ReadChannel(&adcVale, ch);
		
	voltage=calculatePt100Voltage(adcVale);
	
	temp=calculatePt100Temperature(voltage);

	return temp;
}
