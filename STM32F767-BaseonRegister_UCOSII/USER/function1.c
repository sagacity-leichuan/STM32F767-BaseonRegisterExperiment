#include "function1.h"
#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart1.h"


/////////////////////////UCOSII-0任务设置///////////////////////////////////

//任务堆栈	
OS_STK START0_TASK_STK[START0_STK_SIZE];
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];


void TaskManageTest(void)
{
	Stm32_Clock_Init(432,25,2,9);	//设置时钟,216Mhz
    delay_init(216);				//延时初始化 
	uart1_init(108,115200);
	LED_Init();						//初始化LED时钟  
	OSInit();                       //UCOS初始化
    OSTaskCreateExt((void(*)(void*) )start_task0,                //任务函数
                    (void*          )0,                         //传递给任务函数的参数
                    (OS_STK*        )&START0_TASK_STK[START0_STK_SIZE-1],//任务堆栈栈顶
                    (INT8U          )START0_TASK_PRIO,           //任务优先级
                    (INT16U         )START0_TASK_PRIO,           //任务ID，这里设置为和优先级一样
                    (OS_STK*        )&START0_TASK_STK[0],        //任务堆栈栈底
                    (INT32U         )START0_STK_SIZE,            //任务堆栈大小
                    (void*          )0,                         //用户补充的存储区
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//任务选项,为了保险起见，所有任务都保存浮点寄存器的值
	OSStart(); 						//开始任务

}

//开始任务
void start_task0(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit(); 		 	//开启统计任务 
	OS_ENTER_CRITICAL();  	//进入临界区(关闭中断)
    //LED0任务
    OSTaskCreateExt((void(*)(void*) )led0_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED0_TASK_STK[LED0_STK_SIZE-1],
                    (INT8U          )LED0_TASK_PRIO,            
                    (INT16U         )LED0_TASK_PRIO,            
                    (OS_STK*        )&LED0_TASK_STK[0],         
                    (INT32U         )LED0_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED1任务
    OSTaskCreateExt((void(*)(void*) )led1_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED1_TASK_STK[LED1_STK_SIZE-1],
                    (INT8U          )LED1_TASK_PRIO,            
                    (INT16U         )LED1_TASK_PRIO,            
                    (OS_STK*        )&LED1_TASK_STK[0],         
                    (INT32U         )LED1_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP); 

    OS_EXIT_CRITICAL();             //退出临界区(开中断)
	OSTaskSuspend(START0_TASK_PRIO); //挂起开始任务
}
 
//LED0任务
void led0_task(void *pdata)
{	 	 
	while(1)
	{
		LED0(0);
		delay_ms(80);
		LED0(1);
		delay_ms(920);
	};
}

//LED1任务
void led1_task(void *pdata)
{	  
	while(1)
	{ 
		LED1(0);
		delay_ms(300);
		LED1(1);
		delay_ms(300);
	};
}

