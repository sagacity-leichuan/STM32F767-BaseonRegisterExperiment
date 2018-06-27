#ifndef __FUNCTION2_H
#define __FUNCTION2_H

#include "includes.h"

/////////////////////////UCOSII-1任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START1_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START1_STK_SIZE  				128
//任务函数
void start_task1(void *pdata);	
 			   
//触摸屏任务
//设置任务优先级
#define TOUCH_TASK_PRIO       		 	7
//设置任务堆栈大小
#define TOUCH_STK_SIZE  				128

//任务函数
void touch_task(void *pdata);


//LED任务
//设置任务优先级
#define LED_TASK_PRIO       			6 
//设置任务堆栈大小
#define LED_STK_SIZE  		    		128

//任务函数
void led_task(void *pdata);


//蜂鸣器任务
//设置任务优先级
#define BEEP_TASK_PRIO       			5 
//设置任务堆栈大小
#define BEEP_STK_SIZE  					128

//任务函数
void beep_task(void *pdata);


//主任务
//设置任务优先级
#define MAIN_TASK_PRIO       			4 
//设置任务堆栈大小
#define MAIN_STK_SIZE  					128

//任务函数
void main_task(void *pdata);

//按键扫描任务
//设置任务优先级
#define KEY_TASK_PRIO       			3 
//设置任务堆栈大小
#define KEY_STK_SIZE  					128
//任务函数
void key_task(void *pdata);


void SemaphoreTest(void);

#endif
