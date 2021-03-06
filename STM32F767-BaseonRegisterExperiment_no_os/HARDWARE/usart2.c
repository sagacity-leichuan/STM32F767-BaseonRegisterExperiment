#include "sys.h"
#include "usart2.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//ALIENTEK STM32开发板
//串口2初始化 
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void usart2_init(u32 pclk1,u32 bound)
{  	 
	u32 temp;   
	temp=(pclk1*1000000+bound/2)/bound;		//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->AHB1ENR|=1<<0;   					//使能PORTA口时钟  
	RCC->APB1ENR|=1<<17;  					//使能串口2时钟 
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA2,PA3,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,2,7);					//PA2,AF7
	GPIO_AF_Set(GPIOA,3,7);		 			//PA3,AF7  	   
	//波特率设置
 	USART2->BRR=temp; 		// 波特率设置	
	USART2->CR1=0;			//清零CR1寄存器 
	USART2->CR1|=1<<3;  	//串口发送使能  
	USART2->CR1|=1<<13;  	//串口使能
	USART2->CR1|=1<<0;		//串口使能
}







