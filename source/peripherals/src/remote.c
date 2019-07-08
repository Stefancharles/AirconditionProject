//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//����ң�ؽ������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
#include "remote.h"
 
//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
uint8_t RmtSta=0;	  	
uint8_t RmtCnt=0;	//�������µĴ���	  
uint16_t Dval;		//�½���ʱ��������ֵ
uint32_t RmtRec=0;	//������յ�������	
 
/*******************************************************************
*������void Remote_Init(void)
*���ܣ�����ң�س�ʼ��,����PA0��ΪTIM5��CH1�����벶��
*���룺��
*�������
*����˵������
*******************************************************************/
void Remote_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	//TIM5 ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PA0 ���� 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		//�������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);	//��ʼ��GPIOA0


	TIM_TimeBaseStructure.TIM_Period = 10000; //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;  // ѡ������� IC1ӳ�䵽TI5��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM5, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��

	TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	

	TIM_ITConfig( TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC2IE�����ж�	
}

/*******************************************************************
*������void TIM5_IRQHandler(void)
*���ܣ���ʱ��5�жϷ������
*���룺��
*�������
*����˵������
*******************************************************************/
void TIM5_IRQHandler(void)
{ 		    	 
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			}
			if((RmtSta&0X0F)<14)
			{
				RmtSta++;
			}
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���				
			TIM_SetCounter(TIM5,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}
		else //�½��ز���
		{			
			Dval=TIM_GetCapture1(TIM5);//��ȡCCR1Ҳ������CC1IF��־λ
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //CC1P=0	����Ϊ�����ز���
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���� 
			{
				if(RmtSta&0X80)//���յ���������
				{
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	   
					}
					else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
					}
					else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 		//������������1��
						RmtSta&=0XF0;	//��ռ�ʱ��		
					}
				}
				else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtCnt=0;		//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearFlag(TIM5,TIM_IT_Update|TIM_IT_CC1);	    
}

/*******************************************************************
*������uint8_t Remote_Scan(void)
*���ܣ����������̷����ļ�ֵ
*���룺��
*�����0,û���κΰ������£�����,���µİ�����ֵ.
*����˵������
*******************************************************************/
uint8_t Remote_Scan(void)
{        
	uint8_t sta=0;       
	uint8_t t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
		t1=RmtRec>>24;			//�õ���ַ��
		t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
		if((t1==(uint8_t)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
		{ 
			t1=RmtRec>>8;
			t2=RmtRec; 	
			if(t1==(uint8_t)~t2)
			{
				sta=t1;//��ֵ��ȷ	
			}
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
			RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
	return sta;
}

 /*********************************************************************
*������uint8_t IR_RemoteScan(void)
*���ܣ����������̷����ļ�ֵ���˳��ظ���
*���룺��
*�����0,û���κΰ������£�����,���µİ�����ֵ.
*����˵������ң������������ʱ���������ţ�ң�����������ظ��룬�����˳��ظ���
**********************************************************************/
uint8_t IR_RemoteScan(void)
{        
	static uint8_t IrCount=0; 
	static uint8_t IrDataOld=0; 
	uint8_t IrData=0; 	
	
	IrData = Remote_Scan();
	
	if(IrData != 0)
	{
		if(IrCount>3)
		{
			if(IrDataOld != IrData)
			{
				IrDataOld = IrData;
				RmtRec = 0x00;//��պ����ֵ����
				return IrData;
			}
			else
			{
				return 0;
			}
		}
		if(IrCount<=3)
		{
			IrCount++;
		}
	}
	else
	{
		IrDataOld = 0;
		IrCount = 0;
		return 0;
	}
	
	return 0;
}

























