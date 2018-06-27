#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//����˵��
//��
////////////////////////////////////////////////////////////////////////////////// 	 

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
void SPI2_Init(void)
{	 
	u16 tempreg=0;
	RCC->AHB1ENR|=1<<1;		//ʹ��PORTBʱ�� 
	RCC->APB1ENR|=1<<14;   	//SPI2ʱ��ʹ�� 
	GPIO_Set(GPIOB,7<<13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB13~15���ù������	
  	GPIO_AF_Set(GPIOB,13,5);//PB13,AF5
 	GPIO_AF_Set(GPIOB,14,5);//PB14,AF5
 	GPIO_AF_Set(GPIOB,15,5);//PB15,AF5 

	//����ֻ���SPI�ڳ�ʼ��
	RCC->APB1RSTR|=1<<14;	//��λSPI2
	RCC->APB1RSTR&=~(1<<14);//ֹͣ��λSPI2
	SPI2->CR2=7<<8;			//8λ���ݸ�ʽ
	SPI2->CR2|=0<<4;		//Ħ������ģʽ
	SPI2->CR2|=1<<12;		//FIFOռ�ó�1�ֽ�,������RXNE�¼�(������ȷ����)
	tempreg|=0<<10;			//ȫ˫��ģʽ	
	tempreg|=1<<9;			//���nss����
	tempreg|=1<<8;			 
	tempreg|=1<<2;			//SPI����   
	tempreg|=1<<1;			//����ģʽ��SCKΪ1 CPOL=1 
	tempreg|=1<<0;			//���ݲ����ӵ�2��ʱ����ؿ�ʼ,CPHA=1  
 	//��SPI2����APB1������.ʱ��Ƶ�����Ϊ54MhzƵ��.
	tempreg|=7<<3;			//Fsck=Fpclk1/256
	tempreg|=0<<7;			//MSB First  
	tempreg|=1<<6;			//SPI���� 
	SPI2->CR1=tempreg; 		//����CR1  
	SPI2->I2SCFGR&=~(1<<11);//ѡ��SPIģʽ
	SPI2_ReadWriteByte(0xff);//��������		 
}   
//SPI2�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=fAPB1/2^(SpeedSet+1)
//fAPB2ʱ��һ��Ϊ54Mhz
void SPI2_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;			//���Ʒ�Χ
 	SPI2->CR1&=~(1<<6); 	//SPI�豸ʧ��
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet<<3;	//����SPI2�ٶ�  
	SPI2->CR1|=1<<6; 		//SPI�豸ʹ��	  
} 
//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		 	   
	vu32 *data_reg=&SPI2->DR;		//����FIFO,�����Դ��䳤�ȷ���
	while((SPI2->SR&1<<1)==0);		//�ȴ��������� 
	*(vu8 *)data_reg=TxData;		//����һ��byte  
	while((SPI2->SR&1<<0)==0);		//�ȴ�������һ��byte  
 	return *(vu8 *)data_reg;		//�����յ�������				    
}






