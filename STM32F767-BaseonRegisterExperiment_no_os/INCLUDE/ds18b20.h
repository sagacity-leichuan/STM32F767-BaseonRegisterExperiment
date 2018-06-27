#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32开发板
//DS18B20驱动代码	   										  
//////////////////////////////////////////////////////////////////////////////////
  
//IO方向设置
#define DS18B20_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}	//PB12输入模式
#define DS18B20_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;} 	//PB12输出模式
 
////IO操作函数						 
#define DS18B20_DQ_OUT(x)		GPIO_Pin_Set(GPIOB,PIN12,x)			//数据端口输出	 
#define DS18B20_DQ_IN			GPIO_Pin_Get(GPIOB,PIN12)   		//数据端口输入
    	
u8 DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20    
#endif

