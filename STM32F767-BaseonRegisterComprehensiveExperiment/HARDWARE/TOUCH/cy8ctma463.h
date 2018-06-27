#ifndef __CY8CTMA463_H
#define __CY8CTMA463_H
#include "sys.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//8����ݴ�����-CY8CTMA463 ��������(֧��10�㴥��)	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/9/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved		 
////////////////////////////////////////////////////////////////////////////////// 

//IO��������
#define CY_RST(x)		GPIO_Pin_Set(GPIOI,PIN8,x)		//CY8CTMA463��λ����
#define CY_INT			GPIO_Pin_Get(GPIOH,PIN7)  		//CY8CTMA463�ж�����

//I2C��д����	
#define CY_CMD_WR		0x48	//д����
#define CY_CMD_RD		0x49 	//������
//CY8CTMA463 ���ּĴ�������
#define CY_HST_MODE		0x00	//����ģʽ�Ĵ���
//����ģʽ�¼Ĵ���
#define CY_TT_STAT		0X0B	//TrueTouch����״̬�Ĵ���
#define CY_TP_REG		0X0C	//�׸����������ݵ�ַ
//ϵͳ��Ϣģʽ�¼Ĵ���
#define CY_FW_VER		0X12	//�̼��汾�Ĵ���
#define CY_BL_VER		0x1C	//BL�汾�Ĵ���
#define CY_ID			0x31	//����ID�Ĵ���

//����TureTouch��������
typedef struct 
{
	u8 xh;
	u8 xl;
	u8 yh;
	u8 yl;
	u8 z;
	u8 t;  
	u8 majorAx;
	u8 minorAx;
	u8 orientation;
} cyttsp4_touch;


u8 CY8CTMA_WR_Reg(u16 reg,u8 *buf,u8 len);
void CY8CTMA_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 CY8CTMA_Init(void);
u8 CY8CTMA_Scan(u8 mode);
#endif






















