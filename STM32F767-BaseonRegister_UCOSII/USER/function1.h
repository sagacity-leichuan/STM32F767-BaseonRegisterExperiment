#ifndef __FUNCTION1_H
#define __FUNCTION1_H	 

#include "includes.h"

/////////////////////////UCOSII-0��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START0_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START0_STK_SIZE  				128
//������
void start_task0(void *pdata);	
 			   
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			7 
//���������ջ��С
#define LED0_STK_SIZE  		    		128

//������
void led0_task(void *pdata);


//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO       			6 
//���������ջ��С
#define LED1_STK_SIZE  					128

//������
void led1_task(void *pdata);


void TaskManageTest(void);

#endif

