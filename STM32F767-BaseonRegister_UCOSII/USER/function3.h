#ifndef __FUNCTION3_H
#define __FUNCTION3_H

#include "includes.h"

/////////////////////////UCOSII-2��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START2_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START2_STK_SIZE  				128

//������
void start_task2(void *pdata);	
 			   
//LED����
//�����������ȼ�
#define LED_TASK_PRIO       			7 
//���������ջ��С
#define LED_STK_SIZE  		    		128

//������
void led_task(void *pdata);

//����������
//�����������ȼ�
#define TOUCH_TASK_PRIO       		 	6
//���������ջ��С
#define TOUCH_STK_SIZE  				128

//������
void touch_task(void *pdata);

//������Ϣ��ʾ����
//�����������ȼ�
#define QMSGSHOW_TASK_PRIO    			5 
//���������ջ��С
#define QMSGSHOW_STK_SIZE  		 		128

//������
void qmsgshow_task(void *pdata);


//������
//�����������ȼ�
#define MAIN_TASK_PRIO       			4 
//���������ջ��С
#define MAIN_STK_SIZE  					128

//������
void main_task(void *pdata);

//�ź���������
//�����������ȼ�
#define FLAGS_TASK_PRIO       			3 
//���������ջ��С
#define FLAGS_STK_SIZE  		 		128

//������
void flags_task(void *pdata);


//����ɨ������
//�����������ȼ�
#define KEY_TASK_PRIO       			2 
//���������ջ��С
#define KEY_STK_SIZE  					128

//������
void key_task(void *pdata);
//////////////////////////////////////////////////////////////////////////////

void Third_Test(void);

#endif
