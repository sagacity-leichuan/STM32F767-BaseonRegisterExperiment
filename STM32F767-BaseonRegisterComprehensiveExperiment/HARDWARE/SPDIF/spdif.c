#include "spdif.h"
#include "delay.h"
#include "ucos_ii.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板 
//SPDIF 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/8/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//升级说明
//无
////////////////////////////////////////////////////////////////////////////////// 	 

spdif_rx_dev spdif_dev;				//SPDIF控制结构体

//SPDIF RX初始化 
void SPDIF_RX_Init(void)
{	
	u32 tempreg;
	RCC->APB1ENR|=1<<16;			//使能SPDIF RX时钟 
	RCC->AHB1ENR|=1<<6;				//使能PORTG时钟 
	GPIO_Set(GPIOG,1<<12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PG12复用功能 	
   	GPIO_AF_Set(GPIOG,12,7);		//PG12,AF7 
	SPIDIF_RX_Setclk(300);			//先设置SPDIF RX CLK时钟频率为150Mhz
	SPDIF_RX_Mode(SPDIF_RX_IDLE);	//先进入IDLE模式
	tempreg=1<<16;					//选择SPDIF RX通道1
	tempreg|=1<<14;					//等待SPDIF RX线路上的活动
	tempreg|=2<<12;					//同步阶段,允许最大重试15次
	tempreg|=0<<11;					//从通道A获取通道状态
	tempreg|=0<<10;					//SPDIF传输通道状态和用户信息不使用DMA接收
	tempreg|=1<<9;					//报头类型不写入DR
	tempreg|=1<<8;					//通道状态和用户位不写入DR
	tempreg|=1<<7;					//有效性位不写入DR
	tempreg|=1<<6;					//奇偶校验错误位不写入DR
	tempreg|=0<<4;					//数据采用右对齐(LSB),最高支持24位音频数据流
	tempreg|=1<<3;					//立体声模式  
	tempreg|=1<<2;					//SPDIF音频数据使用DMA来接收 
	SPDIFRX->CR=tempreg;
	SPDIFRX->IMR|=(1<<6)|(1<<2);	//使能奇偶校验错误中断和串行接口错误中断
	MY_NVIC_Init(1,0,SPDIF_RX_IRQn,2);//设置SPDIF RX中断,抢占1，子优先级0，组2  
}   

//设置SPDIF RX CLK的时钟频率
//plln:倍频系数,即PLLI2SN的值
//SPDIF RX CLK的频率=(HSE/pllm)*PLLI2SN/PLLI2SP
//其中HSE为外部晶振频率(25M),pllm在初始化时钟时设置为25
//PLLI2SN由plln设置,PLLI2SP固定为2,所以,公式可以简化为:
//SPDIF RX CLK=plln/2; (单位为Mhz)
void SPIDIF_RX_Setclk(u32 plln)
{
	u32 tempreg;
	spdif_dev.clock=plln/2;
	spdif_dev.clock*=1000000;				//得到SPDIF CLK的频率
	RCC->CR&=~(1<<26);						//先关闭PLLI2S  
	tempreg=RCC->PLLI2SCFGR;				//读取PLLI2SCFGR原来的配置
	tempreg&=~(0X1FF<<6);					//清除PLLI2SN的设置
	tempreg&=~(0X3<<16);					//清除PLLI2SP的设置 
	tempreg|=plln<<6;						//设置PLLI2SN
	tempreg|=0<<16;							//设置PLLI2SP=2
	RCC->PLLI2SCFGR=tempreg;				//设置SPDIF RX的频率 
	RCC->CR|=1<<26;							//开启I2S时钟
	while((RCC->CR&1<<27)==0);				//等待I2S时钟开启成功. 
}

//设置SPDIF的工作模式
//mode:0,IDLE模式
//     1,RX同步模式
//     2,保留
//     3,正常工作模式
void SPDIF_RX_Mode(u8 mode)
{
	u32 tempreg=0;
	tempreg=SPDIFRX->CR;
	tempreg&=~(0X03);
	tempreg|=mode&0X03;	//设置模式
	SPDIFRX->CR=tempreg;
}

//SPDIF RX数据DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//width:位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;
void SPDIF_RXDATA_DMA_Init(u32* buf0,u32 *buf1,u16 num,u8 width)
{
	RCC->AHB1ENR|=1<<21;		//DMA1时钟使能   
	while(DMA1_Stream1->CR&0X01);//等待DMA1_Stream1可配置 
	DMA1->LIFCR|=0X3D<<6*0;		//清空通道1上所有中断标志
	DMA1_Stream1->FCR=0X0000021;//设置为默认值	
	
	DMA1_Stream1->PAR=(u32)&SPDIFRX->DR;//外设地址为:SPDIFRX->DR
	DMA1_Stream1->M0AR=(u32)buf0;//内存1地址
	DMA1_Stream1->M1AR=(u32)buf1;//内存2地址
	DMA1_Stream1->NDTR=num;		//设置长度
	DMA1_Stream1->CR=0;			//先全部复位CR寄存器值  
	DMA1_Stream1->CR|=0<<6;		//外设到存储器模式 
	DMA1_Stream1->CR|=1<<8;		//循环模式
	DMA1_Stream1->CR|=0<<9;		//外设非增量模式
	DMA1_Stream1->CR|=1<<10;	//存储器增量模式
	DMA1_Stream1->CR|=(u16)width<<11;//外设数据长度:16位/32位
	DMA1_Stream1->CR|=(u16)width<<13;//存储器数据长度:16位/32位
	DMA1_Stream1->CR|=2<<16;	//高优先级
	DMA1_Stream1->CR|=1<<18;	//双缓冲模式
	DMA1_Stream1->CR|=0<<21;	//外设突发单次传输
	DMA1_Stream1->CR|=0<<23;	//存储器突发单次传输
	DMA1_Stream1->CR|=0<<25;	//选择通道0 SPDIF RX DR通道 

	DMA1_Stream1->FCR&=~(1<<2);	//不使用FIFO模式
	DMA1_Stream1->FCR&=~(3<<0);	//无FIFO 设置 	
}
 
//等待进入同步状态,同步完成以后自动进入接收状态
//返回值:0,未同步;1,已同步
u8 SPDIF_RX_WaitSync(void)
{
    u8 res=0;
    u8 timeout=0;
	SPDIF_RX_Mode(SPDIF_RX_SYNC);	//设置为同步模式
	while(1)
	{
        timeout++;
        delay_ms(5);
		if(timeout>100)break;
		if(SPDIFRX->SR&(1<<5))	//同步完成?
		{
			res=1;				//标记同步完成
			SPDIF_RX_Mode(SPDIF_RX_RCV);//进入接收模式
			break;	  
		}
	}
	return res;
}

//获取SPDIF RX收到的音频采样率
//返回值:0,错误的采样率
//  其他值,音频采样率
u32 SPDIF_RX_GetSampleRate(void)
{
    u16 spdif_w5;
    u32 samplerate; 
    spdif_w5=SPDIFRX->SR>>16;
    samplerate=(spdif_dev.clock*5)/(spdif_w5&0X7FFF);
    samplerate>>=6;     									//除以64 
    if((8000-1500<=samplerate)&&(samplerate<=8000+1500))samplerate=8000;			//8K的采样率
    else if((11025-1500<=samplerate)&&(samplerate<=11025+1500))samplerate=11025;	//11.025K的采样率
    else if((16000-1500<=samplerate)&&(samplerate<=16000+1500))samplerate=16000;	//16K的采样率
    else if((22050-1500<=samplerate)&&(samplerate<=22050+1500))samplerate=22050;	//22.05K的采样率
    else if((32000-1500<=samplerate)&&(samplerate<=32000+1500))samplerate=32000;	//32K的采样率
    else if((44100-1500<=samplerate)&&(samplerate<=44100+1500))samplerate=44100;	//44.1K的采样率
    else if((48000-1500<=samplerate)&&(samplerate<=48000+1500))samplerate=48000;	//48K的采样率
    else if((88200-1500<=samplerate)&&(samplerate<=88200+1500))samplerate=88200;	//88.2K的采样率
    else if((96000-1500<=samplerate)&&(samplerate<=96000+1500))samplerate=96000;	//96K的采样率
    else if((176400-6000<=samplerate)&&(samplerate<=176400+6000))samplerate=176400;	//176.4K的采
    else if((192000-6000<=samplerate)&&(samplerate<=192000+6000))samplerate=192000;	//192K的采
    else samplerate=0; 
	return samplerate;
}
//SAI DMA回调函数指针
void (*spdif_rx_stop_callback)(void);	//TX回调函数 

//SPDIF接收中断服务函数
void SPDIF_RX_IRQHandler(void)
{
    //发生超时、同步和帧错误中断,这三个中断一定要处理！
	u32 sr=SPDIFRX->SR;
	OSIntEnter();   
	if(sr&((1<<8)|(1<<7)|(1<<6)))		//超时错误/同步错误/帧错误
	{  
        SPDIF_RX_Stop();				//发生错误，关闭SPDIF播放
		spdif_rx_stop_callback();		//调用回调函数
		SPDIF_RX_Mode(SPDIF_RX_IDLE);	//当发生超时、同步和帧错误的时候要将SPDIFRXEN写0来清除中断
	}
	if(sr&(1<<3))						//发生了上溢错误
	{
		SPDIFRX->IFCR|=1<<3;			//清除上溢错误
	}
	if(sr&(1<<2))						//发生了奇偶校验错误
	{
		SPDIFRX->IFCR|=1<<2;			//清除奇偶校验错误
	} 
	OSIntExit(); 		 
} 

//SPDIF开始播放
void SPDIF_RX_Start(void)
{ 
    spdif_dev.consta=1;				//标记已经打开SPDIF
	DMA1_Stream1->CR|=1<<0;			//开启SPDIF DR RX传输   
}

//SPDIF关闭
void SPDIF_RX_Stop(void)
{
    spdif_dev.consta=0;				//标记已经关闭SPDIF 
	spdif_dev.saisync=0;			//清空同步状态 
	DMA1_Stream1->CR&=~(1<<0);		//结束SPDIF DMA传输  
} 

//获取设置SAI采样率以后,PLLN的值.
//因为SAI和SPDIF共用同一个PLL,SAI的采样率设置时,会修改plln的值,
//所以,在设置完SAI采样率以后,要重新获取SPDIF的时钟倍频数(plln).
//此时的PLLN才决定最终SPDIF RX CLK的频率
u32 SPDIF_RX_GetSaiPlln(void)
{
	u32 tempreg;
	tempreg=RCC->PLLI2SCFGR;		//读取PLLI2SCFGR原来的配置
	tempreg>>=6;					//偏移到PLLI2SN的设置
	tempreg&=0X1FF;					//得到PLLI2SN的值
	return tempreg;
}












