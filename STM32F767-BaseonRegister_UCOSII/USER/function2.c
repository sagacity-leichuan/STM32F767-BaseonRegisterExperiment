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

/////////////////////////UCOSII-1��������///////////////////////////////////


//�����ջ	
OS_STK START1_TASK_STK[START1_STK_SIZE];
//�����ջ	
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//�����ջ	
OS_STK LED_TASK_STK[LED_STK_SIZE];
//�����ջ	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//�����ջ	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//�����ջ	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];

OS_EVENT * msg_key;			//���������¼���ָ��
OS_EVENT * sem_beep;		//�������ź���ָ��

void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//����
 	POINT_COLOR=RED;	//��������Ϊ��ɫ 
	LCD_ShowString(30,10,200,16,16,"Apollo STM32");	
	LCD_ShowString(30,30,200,16,16,"UCOSII TEST2");	
	LCD_ShowString(30,50,200,16,16,"LEICHUAN");
   	LCD_ShowString(30,75,200,16,16,"KEY0:DS1 KEY_UP:ADJUST");	
   	LCD_ShowString(30,95,200,16,16,"KEY1:BEEP  KEY2:CLEAR"); 
	LCD_ShowString(80,210,200,16,16,"Touch Area");	
	LCD_DrawLine(0,120,lcddev.width-1,120);
	LCD_DrawLine(0,70,lcddev.width-1,70);
	LCD_DrawLine(150,0,150,70);
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
  	LCD_ShowString(160,30,200,16,16,"CPU:   %");	
   	LCD_ShowString(160,50,200,16,16,"SEM:000");	
}	

void SemaphoreTest(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ�� 
	PCF8574_Init();				//��ʼ��PCF8574
	tp_dev.init();				//��������ʼ�� 
	ucos_load_main_ui();		//����������	 
	OSInit();					//UCOS��ʼ��
    OSTaskCreateExt((void(*)(void*) )start_task1,                //������
                    (void*          )0,                         //���ݸ��������Ĳ���
                    (OS_STK*        )&START1_TASK_STK[START1_STK_SIZE-1],//�����ջջ��
                    (INT8U          )START1_TASK_PRIO,           //�������ȼ�
                    (INT16U         )START1_TASK_PRIO,           //����ID����������Ϊ�����ȼ�һ��
                    (OS_STK*        )&START1_TASK_STK[0],        //�����ջջ��
                    (INT32U         )START1_STK_SIZE,            //�����ջ��С
                    (void*          )0,                         //�û�����Ĵ洢��
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
	OSStart(); 					//��ʼ���� 

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ����
void start_task1(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
    msg_key=OSMboxCreate((void*)0);	//������Ϣ����
	sem_beep=OSSemCreate(0);		//�����ź���	
	OSStatInit();  //����ͳ������
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
    //��������
    OSTaskCreateExt((void(*)(void*) )touch_task,                 
                    (void*          )0,
                    (OS_STK*        )&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],
                    (INT8U          )TOUCH_TASK_PRIO,            
                    (INT16U         )TOUCH_TASK_PRIO,            
                    (OS_STK*        )&TOUCH_TASK_STK[0],         
                    (INT32U         )TOUCH_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED����
    OSTaskCreateExt((void(*)(void*) )led_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED_TASK_STK[LED_STK_SIZE-1],
                    (INT8U          )LED_TASK_PRIO,            
                    (INT16U         )LED_TASK_PRIO,            
                    (OS_STK*        )&LED_TASK_STK[0],         
                    (INT32U         )LED_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//����������
    OSTaskCreateExt((void(*)(void*) )beep_task,                 
                    (void*          )0,
                    (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                    (INT8U          )BEEP_TASK_PRIO,          
                    (INT16U         )BEEP_TASK_PRIO,            
                    (OS_STK*        )&BEEP_TASK_STK[0],         
                    (INT32U         )BEEP_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //������
    OSTaskCreateExt((void(*)(void*) )main_task,                 
                    (void*          )0,
                    (OS_STK*        )&MAIN_TASK_STK[MAIN_STK_SIZE-1],
                    (INT8U          )MAIN_TASK_PRIO,          
                    (INT16U         )MAIN_TASK_PRIO,            
                    (OS_STK*        )&MAIN_TASK_STK[0],         
                    (INT32U         )MAIN_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //��������
    OSTaskCreateExt((void(*)(void*) )key_task,                 
                    (void*          )0,
                    (OS_STK*        )&KEY_TASK_STK[KEY_STK_SIZE-1],
                    (INT8U          )KEY_TASK_PRIO,          
                    (INT16U         )KEY_TASK_PRIO,            
                    (OS_STK*        )&KEY_TASK_STK[0],         
                    (INT32U         )KEY_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
	OSTaskSuspend(START1_TASK_PRIO); //����ʼ����
}
 
//LED����
void led_task(void *pdata)
{
	u8 t;
	while(1)
	{
		t++;
		delay_ms(10);
		if(t==8)LED0(1);	//LED0��
		if(t==100)		//LED0��
		{
			t=0;
			LED0(0);
		}
	}									 
}   

//����������
void beep_task(void *pdata)
{
    u8 err;
	while(1)
	{  
        OSSemPend(sem_beep,0,&err);     //�����ź���            
        PCF8574_WriteBit(BEEP_IO,0);    //�򿪷�����
        delay_ms(60);
    	PCF8574_WriteBit(BEEP_IO,1);    //�رշ�����
        delay_ms(940);
	}									 
}

//����������
void touch_task(void *pdata)
{	  	
	u32 cpu_sr;
 	u16 lastpos[2];		//���һ�ε����� 
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)		//������������
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>120)
			{			
				if(lastpos[0]==0XFFFF)
				{
					lastpos[0]=tp_dev.x[0];
					lastpos[1]=tp_dev.y[0]; 
				}
				OS_ENTER_CRITICAL();//�����ٽ��,��ֹ��������,���LCD����,����Һ������.
				lcd_draw_bline(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],2,RED);//����
				OS_EXIT_CRITICAL();
				lastpos[0]=tp_dev.x[0];
				lastpos[1]=tp_dev.y[0];     
			}
		}else 
		{
			lastpos[0]=0XFFFF;
			delay_ms(10);	//û�а������µ�ʱ��
		}
	}
}

//������
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
			case 1://����DS1
				LED1(led1sta^=1);
				break;
			case 2://�����ź���
				semmask=1;
				OSSemPost(sem_beep);
				break;
			case 3://���
				LCD_Fill(0,121,lcddev.width-1,lcddev.height-1,WHITE);
				break;
			case 4://У׼
				OSTaskSuspend(TOUCH_TASK_PRIO);	//������������		 
 				if((tp_dev.touchtype&0X80)==0)TP_Adjust();   
 				OSTaskResume(TOUCH_TASK_PRIO);	//���
				ucos_load_main_ui();			//���¼���������		 
				break;
		}
   		if(semmask||sem_beep->OSEventCnt)//��Ҫ��ʾsem		
		{
			POINT_COLOR=BLUE;
			LCD_ShowxNum(192,50,sem_beep->OSEventCnt,3,16,0X80);//��ʾ�ź�����ֵ
			if(sem_beep->OSEventCnt==0)semmask=0;	//ֹͣ����
		} 
		if(tcnt==10)//0.6�����һ��CPUʹ����
		{
			tcnt=0;
			POINT_COLOR=BLUE;
 			LCD_ShowxNum(192,30,OSCPUUsage,3,16,0);	//��ʾCPUʹ����   
		}
		tcnt++;
		delay_ms(10);
	}
} 

//����ɨ������
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		if(key)OSMboxPost(msg_key,(void*)key);//������Ϣ
 		delay_ms(10);
	}
}


