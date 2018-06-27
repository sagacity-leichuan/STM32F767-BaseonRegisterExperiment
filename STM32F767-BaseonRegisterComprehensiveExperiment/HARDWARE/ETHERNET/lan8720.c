#include "lan8720.h"
#include "pcf8574.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
#include "string.h"
#include "ucos_ii.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//LAN8720驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/8/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


ETH_HandleTypeDef ETH_Handler;      //以太网句柄
ETH_DMADescTypeDef *DMARxDscrTab;	//以太网DMA接收描述符数据结构体指针
ETH_DMADescTypeDef *DMATxDscrTab;	//以太网DMA发送描述符数据结构体指针 
uint8_t *Rx_Buff; 					//以太网底层驱动接收buffers指针 
uint8_t *Tx_Buff; 					//以太网底层驱动发送buffers指针
   

//LAN8720初始化
//返回值:0,成功;
//    其他,失败
u8 LAN8720_Init(void)
{    
    u8 macaddress[6]; 
    PCF8574_WriteBit(ETH_RESET_IO,1);       //硬件复位
    delay_ms(50);
    PCF8574_WriteBit(ETH_RESET_IO,0);       //复位结束 
    
    macaddress[0]=lwipdev.mac[0]; 			//获取mac地址
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];
        
	ETH_Handler.State=HAL_ETH_STATE_RESET;	//重新初始化
	ETH_Handler.Instance=ETH;
    ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_ENABLE;//使能自协商模式 
    ETH_Handler.Init.Speed=ETH_SPEED_100M;						//速度100M,如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.DuplexMode=ETH_MODE_FULLDUPLEX;			//全双工模式，如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;			//LAN8720地址  
    ETH_Handler.Init.MACAddr=macaddress;            			//MAC地址  
    ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;  				//中断接收模式 
    ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;		//硬件帧校验  
    ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;	//RMII接口  
    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)return 0;				//成功
    else return 1;  											//失败  
}

//ETH底层驱动，时钟使能，引脚配置
//此函数会被HAL_ETH_Init()调用
//heth:以太网句柄
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{ 
	//使能以太网时钟
	RCC->AHB1ENR|=7<<25;		//使能ETH MAC/MAC_Tx/MAC_Rx时钟 
	//ETH IO接口初始化
 	RCC->AHB1ENR|=1<<0;     	//使能PORTA时钟 
 	RCC->AHB1ENR|=1<<1;     	//使能PORTB时钟 
 	RCC->AHB1ENR|=1<<2;     	//使能PORTC时钟  
	RCC->AHB1ENR|=1<<6;     	//使能PORTG时钟 
 	RCC->APB2ENR|=1<<14;   		//使能SYSCFG时钟
	SYSCFG->PMC|=1<<23;			//使用RMII PHY接口. 
	GPIO_Set(GPIOA,PIN1|PIN2|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PA1,2,7复用输出
	GPIO_Set(GPIOC,PIN1|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PC1,4,5复用输出
	GPIO_Set(GPIOG,PIN13|PIN14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PG13,14复用输出
	GPIO_Set(GPIOB,PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);				//PB11复用输出
 
	GPIO_AF_Set(GPIOA,1,11);	//PA1,AF11
 	GPIO_AF_Set(GPIOA,2,11);	//PA2,AF11
 	GPIO_AF_Set(GPIOA,7,11);	//PA7,AF11 
  	GPIO_AF_Set(GPIOB,11,11);	//PB11,AF11 
	GPIO_AF_Set(GPIOC,1,11);	//PC1,AF11
 	GPIO_AF_Set(GPIOC,4,11);	//PC4,AF11
 	GPIO_AF_Set(GPIOC,5,11);	//PC5,AF11 
  	GPIO_AF_Set(GPIOG,13,11);	//PG13,AF11
 	GPIO_AF_Set(GPIOG,14,11);	//PG14,AF11 
 	MY_NVIC_Init(0,0,ETH_IRQn,2);	//配置ETH中的分组   	
}


//读取PHY寄存器值
u32 LAN8720_ReadPHY(u16 reg)
{
    u32 regval;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&regval);
    return regval;
}

//向LAN8720指定寄存器写入值
//reg:要写入的寄存器
//value:要写入的值
void LAN8720_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&temp);
}

//得到8720的速度模式
//返回值:
//001:10M半双工
//101:10M全双工
//010:100M半双工
//110:100M全双工
//其他:错误.
u8 LAN8720_Get_Speed(void)
{
	u8 speed;
	speed=((LAN8720_ReadPHY(31)&0x1C)>>2); 	//从LAN8720的31号寄存器中读取网络速度和双工模式
	return speed;
}

extern void lwip_pkt_handle(void);			//在lwip_comm.c里面定义 
//中断服务函数
void ETH_IRQHandler(void)
{ 
	OSIntEnter();		//进入中断  
	lwip_pkt_handle();	//处理以太网数据，即将数据提交给LWIP 
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS); 
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R); 
	OSIntExit();        //触发任务切换软中断
}

//获取接收到的帧长度
//DMARxDesc:接收DMA描述符
//返回值:接收到的帧长度
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    u32 frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

//为ETH底层驱动申请内存
//返回值:0,正常
//    其他,失败
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMTCM,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));	//申请内存
	DMATxDscrTab=mymalloc(SRAMTCM,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));	//申请内存  
	Rx_Buff=mymalloc(SRAMTCM,ETH_RX_BUF_SIZE*ETH_RXBUFNB);					//申请内存
	Tx_Buff=mymalloc(SRAMTCM,ETH_TX_BUF_SIZE*ETH_TXBUFNB);					//申请内存
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}

//释放ETH 底层驱动申请的内存
void ETH_Mem_Free(void)
{ 
	myfree(SRAMTCM,DMARxDscrTab);	//释放内存
	myfree(SRAMTCM,DMATxDscrTab);	//释放内存
	myfree(SRAMTCM,Rx_Buff);		//释放内存
	myfree(SRAMTCM,Tx_Buff);		//释放内存  
}



