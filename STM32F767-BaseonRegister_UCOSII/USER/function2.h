#ifndef __FUNCTION2_H
#define __FUNCTION2_H

#include "includes.h"

/////////////////////////UCOSII-1��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START1_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START1_STK_SIZE  				128
//������
void start_task1(void *pdata);	
 			   
//����������
//�����������ȼ�
#define TOUCH_TASK_PRIO       		 	7
//���������ջ��С
#define TOUCH_STK_SIZE  				128

//������
void touch_task(void *pdata);


//LED����
//�����������ȼ�
#define LED_TASK_PRIO       			6 
//���������ջ��С
#define LED_STK_SIZE  		    		128

//������
void led_task(void *pdata);


//����������
//�����������ȼ�
#define BEEP_TASK_PRIO       			5 
//���������ջ��С
#define BEEP_STK_SIZE  					128

//������
void beep_task(void *pdata);


//������
//�����������ȼ�
#define MAIN_TASK_PRIO       			4 
//���������ջ��С
#define MAIN_STK_SIZE  					128

//������
void main_task(void *pdata);

//����ɨ������
//�����������ȼ�
#define KEY_TASK_PRIO       			3 
//���������ջ��С
#define KEY_STK_SIZE  					128
//������
void key_task(void *pdata);


void SemaphoreTest(void);

#endif
