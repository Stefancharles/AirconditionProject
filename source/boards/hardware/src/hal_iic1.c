//==========================================================================
//
//==========================================================================

#include "hal_iic1.h"
#include "delay.h"


#define IIC1_Delay()         delay_us(2)

/* IIC初始化子程序 */
void IIC1_Init(void)
{
	IIC1_SDA_OUT_Init;
	IIC1_SCL_Init;

	IIC1_SDA_OUT = 1 ;
	IIC1_SCL = 1 ;
}        


//==========================================================================
// 函数功能: 启动IIC1总线
// 函数接口: 
//==========================================================================
void IIC1_Start(void)
{    
	IIC1_SDA_OUT_Init;
	IIC1_SDA_OUT = 1;
	IIC1_SCL = 1;
	IIC1_Delay();
	IIC1_Delay();
	IIC1_SDA_OUT = 0;
	IIC1_Delay();
	IIC1_Delay();
	IIC1_SCL = 0;
}

//==========================================================================
// 函数功能: 停止IIC1总线
// 函数接口: 
//==========================================================================
void IIC1_Stop(void)
{
	IIC1_SDA_OUT_Init;
	IIC1_SCL = 0;
	IIC1_SDA_OUT = 0;
	IIC1_Delay();
	IIC1_Delay();
	IIC1_SCL = 1;
	IIC1_Delay();
	IIC1_Delay();
	IIC1_SDA_OUT = 1;
	IIC1_Delay();
	IIC1_Delay();
}

//==========================================================================
// 函数功能: 对IIC1总线产生应答
// 函数接口: 
//==========================================================================
void IIC1_Ack(void)
{
	IIC1_SDA_OUT_Init;
	IIC1_SCL = 0;
	IIC1_SDA_OUT = 0;
	IIC1_Delay();
	IIC1_SCL = 1;
	IIC1_Delay();
	IIC1_SCL = 0;
}

//==========================================================================
// 函数功能: 不对IIC1总线产生应答
// 函数接口: void IIC1_NoAck(void);
//==========================================================================
void IIC1_NoAck(void)
{
	IIC1_SDA_OUT_Init;
	IIC1_SCL = 0;
	IIC1_SDA_OUT = 1;
	IIC1_Delay();
	IIC1_SCL = 1;
	IIC1_Delay();
	IIC1_SCL = 0;
}

//==========================================================================
// 函数功能: 检查应答位
// 函数接口: uint8_t IIC1_RecAck(void);
//==========================================================================
uint8_t IIC1_RecAck(void)
{
    uint8_t errtime;
    IIC1_SDA_IN_Init;
    IIC1_Delay();
    IIC1_SCL = 1;
	errtime=50;
    while(IIC1_SDA_IN)
    {
        IIC1_Delay();
        errtime--;
        if(!errtime)
        {
            return 0;
        }
    }
    IIC1_SCL = 0;
    IIC1_Delay();
    return 1;
}

//==========================================================================
// 函数功能: 从IIC1总线读取1Byte数据.
// 函数接口: uint8_t IIC1_ReadByte(void);
//==========================================================================
uint8_t IIC1_ReadByte(void)
{
    uint8_t i, dat=0xff;
    IIC1_SDA_IN_Init;
    for(i=0;i<8;i++)
    {
        IIC1_SCL = 1;
        IIC1_Delay();
        dat <<= 1;
        dat |= IIC1_SDA_IN;
        IIC1_SCL = 0;
        IIC1_Delay();
    }
    
    return dat;
}

//==========================================================================
// 函数功能: 往IIC1总线写入1Byte数据
// 函数接口: void IIC1_WriteByte(uint8_t wdata);
//==========================================================================
void IIC1_WriteByte(uint8_t wdata)
{
    uint8_t i;
    IIC1_SDA_OUT_Init;
    for(i=0;i<8;i++)
    {
        if( wdata&0x80 )
            IIC1_SDA_OUT = 1;
        else 
            IIC1_SDA_OUT = 0;
        wdata <<= 1;
        IIC1_Delay();
        IIC1_SCL = 1;
        IIC1_Delay();
        IIC1_SCL = 0;
    }
    
    IIC1_RecAck();
}

//==========================================================================
// 函数功能: 往IIC1总线写入从机地址
// 函数接口: void IIC1_WriteSlaveAddr(uint8_t ucSlaAddr);
//==========================================================================
void IIC1_WriteSlaveAddr(uint8_t ucSlaAddr)
{
    uint8_t i, j, wdata;
    
    for(j=0;j<30;j++)
    {
        IIC1_Start();
        wdata = ucSlaAddr;
        for(i=0;i<8;i++)
        {
            if( wdata&0x80 )
                IIC1_SDA_OUT=1;
            else
                IIC1_SDA_OUT=0;
            wdata <<= 1;
            IIC1_Delay();
            IIC1_SCL = 1;
            IIC1_Delay();
            IIC1_SCL = 0;
        }
        
        if( IIC1_RecAck())
        {
            return;
        }
    }
}
