#include "sys.h"
#include "usart3.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h"
#include "ucos_ii.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����3��ʼ������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2016/9/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved  
////////////////////////////////////////////////////////////////////////////////// 	


//���ڷ��ͻ����� 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA=0;   	 
void USART3_IRQHandler(void)
{
	u8 res;	    
	OSIntEnter();    
	if(USART3->ISR&(1<<5))//���յ�����
	{	 
		res=USART3->RDR; 			 
		if((USART3_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//�����Խ�������
			{
				TIM7->CNT=0;         				//���������
				if(USART3_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
					TIM7->CR1|=1<<0;     			//ʹ�ܶ�ʱ��7
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART3_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
	} 								 
	OSIntExit();  											 
}   
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void usart3_init(u32 pclk1,u32 bound)
{  	 
	u32 temp;   
	temp=(pclk1*1000000+bound/2)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	
	RCC->AHB1ENR|=1<<1;   			//ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  			//ʹ�ܴ���3ʱ�� 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,���ù���,�������
 	GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
	GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7   
 	USART3->BRR=temp; 				//����������	
	USART3->CR1=0;		 			//����CR1�Ĵ���
	USART3->CR1|=0<<28;	 			//����M1=0
	USART3->CR1|=0<<12;	 			//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART3->CR1|=0<<15; 			//����OVER8=0,16��������  
	USART3->CR1|=1<<3;  			//���ڷ���ʹ��  
	USART3->CR1|=1<<2;  			//���ڽ���ʹ��
	USART3->CR1|=1<<5;    			//���ջ������ǿ��ж�ʹ��
	USART3->CR3|=1<<12;    			//��ֹ����	 
	MY_NVIC_Init(1,2,USART3_IRQn,2);//��2�����ȼ�0,2,������ȼ� 
	TIM7_Int_Init(100-1,10800-1);	//10ms�ж�һ��
	TIM7->CR1&=~(1<<0);				//�رն�ʱ��7
	USART3->CR1|=1<<0;  			//����ʹ��
	USART3_RX_STA=0;				//���� 
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
		while((USART3->ISR&0X40)==0);//ѭ������,ֱ���������   
		USART3->TDR=USART3_TX_BUF[j];  
	}
}



































