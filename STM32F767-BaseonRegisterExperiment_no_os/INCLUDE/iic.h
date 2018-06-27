#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
 	   		   
//IO方向设置
#define SDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	//PH5输入模式
#define SDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} //PH5输出模式
//IO操作函数	 
#define IIC_SCL(x)		GPIO_Pin_Set(GPIOH,PIN4,x)		//SCL
#define IIC_SDA(x)		GPIO_Pin_Set(GPIOH,PIN5,x)		//SDA
#define READ_SDA		GPIO_Pin_Get(GPIOH,PIN5)   		//读取SDA
 
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号 
#endif




