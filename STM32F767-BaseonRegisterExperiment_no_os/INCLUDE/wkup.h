#ifndef __WKUP_H
#define __WKUP_H	 
#include "sys.h"
					    
#define WKUP_KD GPIO_Pin_Get(GPIOA,PIN0)		//PA0 ����Ƿ��ⲿWK_UP��������
	 
u8 Check_WKUP(void);  			//���WKUP�ŵ��ź�
void WKUP_Init(void); 			//PA0 WKUP���ѳ�ʼ��
void Sys_Enter_Standby(void);	//ϵͳ�������ģʽ
#endif

