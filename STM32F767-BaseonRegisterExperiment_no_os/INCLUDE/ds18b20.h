#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32������
//DS18B20��������	   										  
//////////////////////////////////////////////////////////////////////////////////
  
//IO��������
#define DS18B20_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}	//PB12����ģʽ
#define DS18B20_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;} 	//PB12���ģʽ
 
////IO��������						 
#define DS18B20_DQ_OUT(x)		GPIO_Pin_Set(GPIOB,PIN12,x)			//���ݶ˿����	 
#define DS18B20_DQ_IN			GPIO_Pin_Get(GPIOB,PIN12)   		//���ݶ˿�����
    	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif

