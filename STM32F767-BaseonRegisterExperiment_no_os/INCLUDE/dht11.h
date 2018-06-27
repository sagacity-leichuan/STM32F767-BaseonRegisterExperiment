#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32开发板
//DHT11 驱动代码	   										  
//////////////////////////////////////////////////////////////////////////////////
 
//IO方向设置
#define DHT11_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}	//PB12输入模式
#define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;} 	//PB12输出模式 
////IO操作函数							 
#define DHT11_DQ_OUT(x)		GPIO_Pin_Set(GPIOB,PIN12,x)			//数据端口输出	 
#define DHT11_DQ_IN			GPIO_Pin_Get(GPIOB,PIN12)   		//数据端口输入
 

u8 DHT11_Init(void);	//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);	//检测是否存在DHT11
void DHT11_Rst(void);	//复位DHT11    
#endif

