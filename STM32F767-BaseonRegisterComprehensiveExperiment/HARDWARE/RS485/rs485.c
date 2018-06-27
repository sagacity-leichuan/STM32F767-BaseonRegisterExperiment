#include "sys.h"		    
#include "rs485.h"	 
#include "delay.h"
#include "pcf8574.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//RS485���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/19
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


#if EN_USART2_RX   		//���ʹ���˽���   	  
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;   
void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART2->ISR&(1<<5))//���յ�����
	{	 
		res=USART2->RDR; 			 
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
		} 
	}  											 
} 
#endif										 
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz),APB1һ��Ϊ54Mhz
//bound:������	  
void RS485_Init(u32 pclk1,u32 bound)
{  	 
	u32 temp;   
	temp=(pclk1*1000000+bound/2)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	PCF8574_Init();				//��ʼ��PCF8574,���ڿ���RE��	
	RCC->AHB1ENR|=1<<0;   		//ʹ��PORTA��ʱ��   
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PA2,PA3,���ù���,���� 
 	GPIO_AF_Set(GPIOA,2,7);		//PA2,AF7
	GPIO_AF_Set(GPIOA,3,7);		//PA3,AF7  	   
 
	RCC->APB1ENR|=1<<17;  		//ʹ�ܴ���2ʱ��  
	RCC->APB1RSTR|=1<<17;   	//��λ����2
	RCC->APB1RSTR&=~(1<<17);	//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=temp; 			//����������	
	USART2->CR1=0;		 		//����CR1�Ĵ���
	USART2->CR1|=0<<28;	 		//����M1=0
	USART2->CR1|=0<<12;	 		//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART2->CR1|=0<<15; 		//����OVER8=0,16�������� 
	USART2->CR1|=1<<3;  		//���ڷ���ʹ�� 
#if EN_USART2_RX		  		//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<2;  		//���ڽ���ʹ��
	USART2->CR1|=1<<5;    		//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
	USART2->CR1|=1<<0;  		//����ʹ��
	RS485_TX_Set(0);			//Ĭ������Ϊ����ģʽ	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_Set(1);			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)			//ѭ����������
	{
		while((USART2->ISR&0X40)==0);//�ȴ����ͽ���		  
		USART2->TDR=buf[t];
	}	 
	while((USART2->ISR&0X40)==0);//�ȴ����ͽ���	
	RS485_RX_CNT=0;	  
	RS485_TX_Set(0);			//����Ϊ����ģʽ	
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
} 
//RS485ģʽ����.
//en:0,����;1,����.
void RS485_TX_Set(u8 en)
{
	PCF8574_WriteBit(RS485_RE_IO,en);
}




