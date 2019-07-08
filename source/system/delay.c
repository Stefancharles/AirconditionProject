/******************************************************************************
* @ File name --> delay.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 10 - 20 - 2012
* @ Brief     --> ϵͳ��ʱ��صĺ���
*
* @               ��ϸ˵����ο���Cortex-M3Ȩ��ָ��(����)����133 ~ 134ҳ ��8�� SysTick��ʱ������
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.0.1
* @ Author    --> By@ Sam Chan
* @ Date      --> 02 - 26 - 2014
* @ Revise    --> ��������һ����ʱ���㷽��
*
* @ Version   --> V1.0.2
* @ Author    --> By@ Sam Chan
* @ Date      --> 05 - 10 - 2014
* @ Revise    --> ���Ӷ�C++����֧��
*
* @ Version   --> V1.1
* @ Author    --> By@ Sam Chan
* @ Date      --> 05 - 24 - 2014
* @ Revise    --> �޸�����ucosʱ��ʼ����શ�ʱ������ʱus��ms����
*
******************************************************************************/

/******************************************************************************

* @ SysTick��ʱ�� ��ؿ��ƼĴ���˵��

@ 1��SysTick���Ƽ�״̬�Ĵ�������ַ��0xE000_E010����λֵΪ0

	bit16 COUNTFLAG(R��  -> ������ϴζ�ȡ���Ĵ�����SysTick�Ѿ�������0�����λΪ1�������ȡ��λ����λ���Զ�����
	bit2  CLKSOURCE(R/W) -> 0=�ⲿʱ��Դ(STCLK)��1=�ں�ʱ��(FCLK)
	bit1  TICKINT(R/W)   -> 1=SysTick������0ʱ����SysTick�쳣����,0=����0ʱ�޶��� 
	bit0  ENABLE(R/W)    -> SysTick��ʱ����ʹ��λ

@ 2��SysTick��װ����ֵ�Ĵ�������ַ��0xE000_E014����λֵΪ0

	[23:0] RELOAD(R/W) -> ����������ʱ��������װ�ص�ֵ

@ 3��SysTick��ǰ��ֵ�Ĵ�������ַ��0xE000_E018�� ��λֵΪ0

	[23:0] CURRENT(R/Wc) -> ��ȡʱ���ص�ǰ��������ֵ��д����ʹ֮���㣬ͬʱ���������SysTick ���Ƽ�״̬�Ĵ����е�COUNTFLAG��־

@ 4��SysTickУ׼��ֵ�Ĵ�������ַ��0xE000_E01C����λֵ: bit31δ֪��bit30δ֪��[23:0]Ϊ0

	bit32 NOREF(R)    -> 1=û���ⲿ�ο�ʱ�ӣ�STCLK�����ã���0=�ⲿ�ο�ʱ�ӿ���
	bit30 SKEW(R)     -> 1=У׼ֵ����׼ȷ��10ms��0=У׼ֵ��׼ȷ��10ms
	[23:0] TENMS(R/W) -> 10ms��ʱ���ڵ������ĸ�����оƬ�����Ӧ��ͨ��Cortex�\M3�������ź��ṩ����ֵ������ֵ�����㣬���ʾ�޷�ʹ��У׼����
	
******************************************************************************/ 

#include "delay.h"

/******************************************************************************
                   ʹ��Ƕ��ʽ����ϵͳʱ��ʼ������������
******************************************************************************/

#if _SYSTEM_SUPPORT_ROTS	//��������֧��ʵʱǶ��ʽ����ϵͳ
	#include "includes.h"	//���Ӳ���ϵͳ��Ҫͷ�ļ�

/******************************************************************************
* Function Name --> SysTick��ʱ��������ʼ��
* Description   --> ��Ҫ�ǳ�ʼ��SysTick�Ĵ��� 
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void OS_Heart_Init(void)
{
	delay_init();
}
/******************************************************************************
* Function Name --> SysTick��ʱ���жϷ�����
* Description   --> �ڴ˱�д�ˣ���stm32f10x_it.c�оͲ���Ҫ��д������stm32f10x_it.c�о�Ҫ��д
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void SysTick_Handler(void)
{
	/* ��д��SysTick��ʱ���жϲ�����ص�API�������� */

	OSIntEnter();  //ucos�����ж�
	OSTimeTick();  //����ucos��ʱ�ӷ�����
	OSIntExit();  //ucos�˳��ж�
} 

#endif  /* end _SYSTEM_SUPPORT_ROTS */

/******************************************************************************
                          ����Ƕ��ʽ����ϵͳ��������
******************************************************************************/



//=========================================================
#if _USER_SysTick==1	//��������ʹ��SysTick��ʱ������ʱ��������
//=========================================================

/******************************************************************************
                               ����������
******************************************************************************/

static uint8_t  fac_us=0;	//us��ʱ������
static uint16_t fac_ms=0;	//ms��ʱ������

/******************************************************************************
* Function Name --> ��ʼ����ʱ����
* Description   --> ��Ҫ�ǳ�ʼ��SysTick�Ĵ��� 
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void delay_init(void)
{
#if _SYSTEM_SUPPORT_ROTS  //������ucos��
	uint32_t RELOAD=0;	//��������������0ʱ����װֵ����Чλ��0 ~ 23
#endif

	/* ����SysTick��ʱ����ʱ�ӷ�Ƶ��ȷ����װֵ */
	/* 8��Ƶʱ����8000��000��1��Ƶʱ����1000��000 */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  //ѡ���ⲿʱ�� HCLK / 8

	fac_us = SystemCoreClock / 8000000;  //ϵͳʱ�ӵ� 1/8

#if _SYSTEM_SUPPORT_ROTS  //������ucos��
	
	RELOAD = SystemCoreClock / 8000000;	//ÿ���ӵļ�����������λHz
	RELOAD *= 1000000 / OS_TICKS_PER_SEC;	//���ݲ���ϵͳ������ʱ�����������ʱ�䣬��λ��KHz
	                                    //RELOADΪ24λ�����������ֵΪ:16777216 

	fac_ms = 1000 / OS_TICKS_PER_SEC;

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	//����SysTick��ʱ���ж�����
	SysTick->LOAD = RELOAD;	//�������ֵ��ÿ1/TICKINT_CNT���ж�һ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ����

#else	    
	fac_ms = (uint16_t)fac_us*1000;	//ms��Ҫ��SysTickʱ����
#endif  /* end _SYSTEM_SUPPORT_ROTS */
}
/******************************************************************************
* Function Name --> ��ʱn��us
* Description   --> none
* Input         --> nus��Ҫ��ʱ��us��
* Output        --> none
* Reaturn       --> none 
******************************************************************************/	    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp=0;

#if _SYSTEM_SUPPORT_ROTS  //������ucos��

	uint32_t VAL_Prev=0;  //��ʼ��ʱ֮ǰ��ֵ
	uint32_t VAL_Now=0;   //��ǰ��ʱֵ
	uint32_t VAL_cnt=0;   //����
	uint32_t Reload=SysTick->LOAD;  //��ȡ��LOAD��ֵ

	temp = nus*fac_us;  //�õ���ʱ�Ľ�����
	VAL_Prev = SysTick->VAL;  //���浱ǰ�ļ���ֵ

	while(1)
	{
		VAL_Now = SysTick->VAL;  //��ȡ��ֵ
		if(VAL_Now != VAL_Prev)
		{
			if(VAL_Now < VAL_Prev)  VAL_cnt += VAL_Prev-VAL_Now;  //��ΪSysTick��һ���ݼ��Ķ�ʱ��
			else                      VAL_cnt += Reload - VAL_Now + VAL_Prev;

			VAL_Prev = VAL_Now;  //ˢ��
			if(VAL_cnt >= temp)  break;  //����/������Ҫ����ʱֵ�ˣ����˳�ѭ��
		}
	};

#else
		    	 
	SysTick->LOAD = nus*fac_us; //ʱ�����	  		 
	SysTick->VAL = 0x00;        //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ���� 	 
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL = 0x00;	//��ռ�����	

#endif  /* end _SYSTEM_SUPPORT_ROTS */
}
/******************************************************************************
* Function Name --> ��ʱn��ms
* Description   --> SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
*                   nms <= 0xffffff*8*1000/SYSCLK
*                   SYSCLK��λΪHz,nms��λΪms
*                   ע��nms�ķ�Χ 0 ~ 1864(72M�����)
* Input         --> nms��Ҫ��ʱ��ms��
* Output        --> none
* Reaturn       --> none 
******************************************************************************/	
void delay_ms(uint16_t nms)
{
#if _SYSTEM_SUPPORT_ROTS  //ʹ��ucos��

	if(OSRunning == 1)  //ucos�Ѿ�������
	{
		if(nms > fac_ms)  //��ʱ����ucos����
		{
			OSTimeDly(nms/fac_ms);  //����ucos��ʱ
		}
		nms %= fac_ms;  //ucos�޷��ṩС�ڽ��ĵ���ʱ��
	}
	delay_us((uint32_t)(nms*1000));  //������ͨ����ʱ

#else

	uint32_t temp;
			   
	SysTick->LOAD = (uint32_t)nms*fac_ms;	//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;           //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ����  
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL = 0x00;	//��ռ�����	 

#endif  /* end _SYSTEM_SUPPORT_ROTS */
}


//=========================================================
#else	//ʹ��������Դ������ʱ����
//=========================================================


/******************************************************************************
* Function Name --> ��ʼ����ʱ����
* Description   --> ��ҪCortex-M3�ں˶�ϵͳʱ�Ӽ�����Ԫ
*                   ��ϸ����Cortex-M3Ȩ��ָ��(����)����216ҳ a)  ʱ�����ڣ�CYCCNT�� ������
*                   ��������CM3�����ο��ֲᡷ��28��29ҳ����110��125ҳ
* Input         --> SYSCLK��ϵͳ������ߵ�Ƶ�ʡ���λMHz
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void delay_init(void)
{
	DEM_CTRL |= 1<<24;	//��λ����Ϊ1��ʹ�ܸ��ٺ͵���ģ���ʹ�á���ϸ������������CM3�����ο��ֲᡷ��115ҳ����
						//��û��ʹ�ø���ʱ����λʹ�ܶԹ���ʹ�õĿ��ơ����ܹ���Ӧ�ó���������ʹ�ܣ���ITMʹ��
						//��DWT�ܹ�ʹ��֮ǰ�������쳣�ͼ�ؿ��ƼĴ�����TRCENA(bit24)λ������λ

	DWT_CTRL |= 1<<0;	//ʹ��DWT_CYCCNT��������
						//�����ʹ�ܣ����������ִ�м�����������˲������PC������CYCCNTENA�¼���
						//������ʹ��ʱ��CYCCNT������Ӧ�ɵ�������ʼ��Ϊ0��
}
/******************************************************************************
* Function Name --> ��ʱn��us
* Description   --> none
* Input         --> nus��Ҫ��ʱ��us��
* Output        --> none
* Reaturn       --> none 
******************************************************************************/	
void delay_us(uint32_t nus)
{
	uint32_t savecount,endcnt,CPU_cnt;

	savecount = DWT_CYCCNT;	//�����������ǰ��ֵ
	CPU_cnt = nus*(SystemCoreClock/(1000*1000));	//����ﵽ������ʱֵ��CPUʱ�����������ٸ�ϵͳʱ�Ӽ���
											//�õ�����ȷ��ʱʱ�䣬��ȥǰ��������е�ʱ�伴��

	endcnt = savecount + CPU_cnt;	//����������ʱʱ��DWT_CYCCNT�ļ���ֵ�������ʱ���ص�0
	
	if(endcnt > savecount)	//������ʱֵ���ڵ�ǰ����ֵ
	{
		while(DWT_CYCCNT < endcnt);	//ѭ���ȴ�����Ҫ����ʱʱ���CPUʱ�Ӽ���ֵ
	}
	else	//С�ڵ�ǰ����ֵ
	{
		while(DWT_CYCCNT > endcnt);	//�ȴ������������ת
		while(DWT_CYCCNT < endcnt);	//�ȴ�������ʱʱ�䵽��
	}
}

void sdelay(uint32_t s)
{
	uint8_t i;
	while(s--)
	{
		i=1;
		while(i--);
	}
}

//=========================================================
#endif
//=========================================================





