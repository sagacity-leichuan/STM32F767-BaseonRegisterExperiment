#ifndef __FUNCTION1_H
#define __FUNCTION1_H	 

#include "includes.h"

/////////////////////////UCOSII-0任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START0_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START0_STK_SIZE  				128
//任务函数
void start_task0(void *pdata);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		128

//任务函数
void led0_task(void *pdata);


//LED1任务
//设置任务优先级
#define LED1_TASK_PRIO       			6 
//设置任务堆栈大小
#define LED1_STK_SIZE  					128

//任务函数
void led1_task(void *pdata);


void TaskManageTest(void);

#endif

