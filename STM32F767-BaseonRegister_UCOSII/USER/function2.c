#include "function2.h"
#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart1.h"
#include "lcd.h" 
#include "touch.h" 
#include "mpu.h"
#include "sdram.h"
#include "pcf8574.h"
#include "key.h"
#include "touch_test.h"

/////////////////////////UCOSII-1任务设置///////////////////////////////////


//任务堆栈	
OS_STK START1_TASK_STK[START1_STK_SIZE];
//任务堆栈	
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//任务堆栈	
OS_STK LED_TASK_STK[LED_STK_SIZE];
//任务堆栈	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//任务堆栈	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务堆栈	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];

OS_EVENT * msg_key;			//按键邮箱事件块指针
OS_EVENT * sem_beep;		//蜂鸣器信号量指针

void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//清屏
 	POINT_COLOR=RED;	//设置字体为红色 
	LCD_ShowString(30,10,200,16,16,"Apollo STM32");	
	LCD_ShowString(30,30,200,16,16,"UCOSII TEST2");	
	LCD_ShowString(30,50,200,16,16,"LEICHUAN");
   	LCD_ShowString(30,75,200,16,16,"KEY0:DS1 KEY_UP:ADJUST");	
   	LCD_ShowString(30,95,200,16,16,"KEY1:BEEP  KEY2:CLEAR"); 
	LCD_ShowString(80,210,200,16,16,"Touch Area");	
	LCD_DrawLine(0,120,lcddev.width-1,120);
	LCD_DrawLine(0,70,lcddev.width-1,70);
	LCD_DrawLine(150,0,150,70);
 	POINT_COLOR=BLUE;//设置字体为蓝色 
  	LCD_ShowString(160,30,200,16,16,"CPU:   %");	
   	LCD_ShowString(160,50,200,16,16,"SEM:000");	
}	

void SemaphoreTest(void)
{
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化 
	PCF8574_Init();				//初始化PCF8574
	tp_dev.init();				//触摸屏初始化 
	ucos_load_main_ui();		//加载主界面	 
	OSInit();					//UCOS初始化
    OSTaskCreateExt((void(*)(void*) )start_task1,                //任务函数
                    (void*          )0,                         //传递给任务函数的参数
                    (OS_STK*        )&START1_TASK_STK[START1_STK_SIZE-1],//任务堆栈栈顶
                    (INT8U          )START1_TASK_PRIO,           //任务优先级
                    (INT16U         )START1_TASK_PRIO,           //任务ID，这里设置为和优先级一样
                    (OS_STK*        )&START1_TASK_STK[0],        //任务堆栈栈底
                    (INT32U         )START1_STK_SIZE,            //任务堆栈大小
                    (void*          )0,                         //用户补充的存储区
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//任务选项,为了保险起见，所有任务都保存浮点寄存器的值
	OSStart(); 					//开始任务 

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//开始任务
void start_task1(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
    msg_key=OSMboxCreate((void*)0);	//创建消息邮箱
	sem_beep=OSSemCreate(0);		//创建信号量	
	OSStatInit();  //开启统计任务
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
    //触摸任务
    OSTaskCreateExt((void(*)(void*) )touch_task,                 
                    (void*          )0,
                    (OS_STK*        )&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],
                    (INT8U          )TOUCH_TASK_PRIO,            
                    (INT16U         )TOUCH_TASK_PRIO,            
                    (OS_STK*        )&TOUCH_TASK_STK[0],         
                    (INT32U         )TOUCH_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED任务
    OSTaskCreateExt((void(*)(void*) )led_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED_TASK_STK[LED_STK_SIZE-1],
                    (INT8U          )LED_TASK_PRIO,            
                    (INT16U         )LED_TASK_PRIO,            
                    (OS_STK*        )&LED_TASK_STK[0],         
                    (INT32U         )LED_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//蜂鸣器任务
    OSTaskCreateExt((void(*)(void*) )beep_task,                 
                    (void*          )0,
                    (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                    (INT8U          )BEEP_TASK_PRIO,          
                    (INT16U         )BEEP_TASK_PRIO,            
                    (OS_STK*        )&BEEP_TASK_STK[0],         
                    (INT32U         )BEEP_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //主任务
    OSTaskCreateExt((void(*)(void*) )main_task,                 
                    (void*          )0,
                    (OS_STK*        )&MAIN_TASK_STK[MAIN_STK_SIZE-1],
                    (INT8U          )MAIN_TASK_PRIO,          
                    (INT16U         )MAIN_TASK_PRIO,            
                    (OS_STK*        )&MAIN_TASK_STK[0],         
                    (INT32U         )MAIN_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //按键任务
    OSTaskCreateExt((void(*)(void*) )key_task,                 
                    (void*          )0,
                    (OS_STK*        )&KEY_TASK_STK[KEY_STK_SIZE-1],
                    (INT8U          )KEY_TASK_PRIO,          
                    (INT16U         )KEY_TASK_PRIO,            
                    (OS_STK*        )&KEY_TASK_STK[0],         
                    (INT32U         )KEY_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             //退出临界区(开中断)
	OSTaskSuspend(START1_TASK_PRIO); //挂起开始任务
}
 
//LED任务
void led_task(void *pdata)
{
	u8 t;
	while(1)
	{
		t++;
		delay_ms(10);
		if(t==8)LED0(1);	//LED0灭
		if(t==100)		//LED0亮
		{
			t=0;
			LED0(0);
		}
	}									 
}   

//蜂鸣器任务
void beep_task(void *pdata)
{
    u8 err;
	while(1)
	{  
        OSSemPend(sem_beep,0,&err);     //请求信号量            
        PCF8574_WriteBit(BEEP_IO,0);    //打开蜂鸣器
        delay_ms(60);
    	PCF8574_WriteBit(BEEP_IO,1);    //关闭蜂鸣器
        delay_ms(940);
	}									 
}

//触摸屏任务
void touch_task(void *pdata)
{	  	
	u32 cpu_sr;
 	u16 lastpos[2];		//最后一次的数据 
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)		//触摸屏被按下
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>120)
			{			
				if(lastpos[0]==0XFFFF)
				{
					lastpos[0]=tp_dev.x[0];
					lastpos[1]=tp_dev.y[0]; 
				}
				OS_ENTER_CRITICAL();//进入临界段,防止其他任务,打断LCD操作,导致液晶乱序.
				lcd_draw_bline(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],2,RED);//画线
				OS_EXIT_CRITICAL();
				lastpos[0]=tp_dev.x[0];
				lastpos[1]=tp_dev.y[0];     
			}
		}else 
		{
			lastpos[0]=0XFFFF;
			delay_ms(10);	//没有按键按下的时候
		}
	}
}

//主任务
void main_task(void *pdata)
{				
	u8 led1sta=0;
	u32 key=0;	
	u8 err;	
	u8 semmask=0;
	u8 tcnt=0;		
	while(1)
	{
		key=(u32)OSMboxPend(msg_key,10,&err);   
		switch(key)
		{
			case 1://控制DS1
				LED1(led1sta^=1);
				break;
			case 2://发送信号量
				semmask=1;
				OSSemPost(sem_beep);
				break;
			case 3://清除
				LCD_Fill(0,121,lcddev.width-1,lcddev.height-1,WHITE);
				break;
			case 4://校准
				OSTaskSuspend(TOUCH_TASK_PRIO);	//挂起触摸屏任务		 
 				if((tp_dev.touchtype&0X80)==0)TP_Adjust();   
 				OSTaskResume(TOUCH_TASK_PRIO);	//解挂
				ucos_load_main_ui();			//重新加载主界面		 
				break;
		}
   		if(semmask||sem_beep->OSEventCnt)//需要显示sem		
		{
			POINT_COLOR=BLUE;
			LCD_ShowxNum(192,50,sem_beep->OSEventCnt,3,16,0X80);//显示信号量的值
			if(sem_beep->OSEventCnt==0)semmask=0;	//停止更新
		} 
		if(tcnt==10)//0.6秒更新一次CPU使用率
		{
			tcnt=0;
			POINT_COLOR=BLUE;
 			LCD_ShowxNum(192,30,OSCPUUsage,3,16,0);	//显示CPU使用率   
		}
		tcnt++;
		delay_ms(10);
	}
} 

//按键扫描任务
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		if(key)OSMboxPost(msg_key,(void*)key);//发送消息
 		delay_ms(10);
	}
}


