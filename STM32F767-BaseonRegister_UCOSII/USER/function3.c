#include "function3.h"
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
#include "touch.h"
#include "tpad.h"
#include "malloc.h"


/////////////////////////UCOSII-2��������///////////////////////////////////

//�����ջ	
OS_STK START2_TASK_STK[START2_STK_SIZE];

//�����ջ
OS_STK LED_TASK_STK[LED_STK_SIZE];

//�����ջ	
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];

//�����ջ	
OS_STK QMSGSHOW_TASK_STK[QMSGSHOW_STK_SIZE];

//�����ջ	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];

//�����ջ	
OS_STK FLAGS_TASK_STK[FLAGS_STK_SIZE];

//�����ջ	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];


OS_EVENT * msg_key;			//���������¼���	  
OS_EVENT * q_msg;			//��Ϣ����
OS_TMR   * tmr1;			//�����ʱ��1
OS_TMR   * tmr2;			//�����ʱ��2
OS_TMR   * tmr3;			//�����ʱ��3
OS_FLAG_GRP * flags_key;	//�����ź�����
void * MsgGrp[256];			//��Ϣ���д洢��ַ,���֧��256����Ϣ	


//�����ʱ��1�Ļص�����	
//ÿ100msִ��һ��,������ʾCPUʹ���ʺ��ڴ�ʹ����		   
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{
 	static u16 cpuusage=0;
	static u8 tcnt=0;	    
	POINT_COLOR=BLUE;
	if(tcnt==5)
	{
 		LCD_ShowxNum(202,10,cpuusage/5,3,16,0);			//��ʾCPUʹ����  
		cpuusage=0;
		tcnt=0; 
	}
	cpuusage+=OSCPUUsage;
	tcnt++;				    
 	LCD_ShowxNum(202,30,mymem_perused(SRAMIN)/10,3,16,0);	//��ʾ�ڴ�ʹ����	 	  		 					    
	LCD_ShowxNum(202,50,((OS_Q*)(q_msg->OSEventPtr))->OSQEntries,3,16,0X80);//��ʾ���е�ǰ�Ĵ�С		   
 }

//�����ʱ��2�Ļص�����				  	   
void tmr2_callback(OS_TMR *ptmr,void *p_arg) 
{	
	static u8 sta=0;
	switch(sta)
	{
		case 0:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,RED);
			break;
		case 1:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,GREEN);
			break;
		case 2:		    
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,BLUE);
			break;
		case 3:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,MAGENTA);
			break;
 		case 4:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,GBLUE);
			break;
		case 5:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,YELLOW);
			break;
		case 6:
			LCD_Fill(131,221,lcddev.width-1,lcddev.height-1,BRRED);
			break;	 
	}
	sta++;
	if(sta>6)sta=0;	 											   
}

//�����ʱ��3�Ļص�����				  	   
void tmr3_callback(OS_TMR *ptmr,void *p_arg) 
{	
	u8* p;	 
	u8 err; 
	static u8 msg_cnt=0;	//msg���	  
	p=mymalloc(SRAMIN,13);	//����13���ֽڵ��ڴ�
	if(p)
	{
	 	sprintf((char*)p,"ALIENTEK %03d",msg_cnt);
		msg_cnt++;
		err=OSQPost(q_msg,p);	//���Ͷ���
		if(err!=OS_ERR_NONE) 	//����ʧ��
		{
			myfree(SRAMIN,p);	//�ͷ��ڴ�
			OSTmrStop(tmr3,OS_TMR_OPT_NONE,0,&err);	//�ر������ʱ��3
 		}
	}
} 
//����������   
void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//����
 	POINT_COLOR=RED;	//��������Ϊ��ɫ 
	LCD_ShowString(10,10,200,16,16,"Apollo STM32");	
	LCD_ShowString(10,30,200,16,16,"UCOSII TEST3");	
	LCD_ShowString(10,50,200,16,16,"LEICHUAN");
   	LCD_ShowString(10,75,240,16,16,"TPAD:TMR2 SW   KEY_UP:ADJUST");	
   	LCD_ShowString(10,95,240,16,16,"KEY0:DS0 KEY1:Q SW KEY2:CLR");	
 	LCD_DrawLine(0,70,lcddev.width-1,70);
	LCD_DrawLine(150,0,150,70);

 	LCD_DrawLine(0,120,lcddev.width-1,120);
 	LCD_DrawLine(0,220,lcddev.width-1,220);
	LCD_DrawLine(130,120,130,lcddev.height-1);
		    
 	LCD_ShowString(5,125,240,16,16,"QUEUE MSG");//������Ϣ
	LCD_ShowString(5,150,240,16,16,"Message:");	 
	LCD_ShowString(5+130,125,240,16,16,"FLAGS");//�ź�����
	LCD_ShowString(5,225,240,16,16,"TOUCH");	//������
	LCD_ShowString(5+130,225,240,16,16,"TMR2");	//������Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
  	LCD_ShowString(170,10,200,16,16,"CPU:   %");	
   	LCD_ShowString(170,30,200,16,16,"MEM:   %");	
   	LCD_ShowString(170,50,200,16,16," Q :000");	

	delay_ms(300);
}	

///////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ����
void start_task2(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
    u8 err;	  
	pdata=pdata;
	msg_key=OSMboxCreate((void*)0);		//������Ϣ����
	q_msg=OSQCreate(&MsgGrp[0],256);	//������Ϣ����
 	flags_key=OSFlagCreate(0,&err); 	//�����ź�����
	OSStatInit();  //����ͳ������
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
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
	//��Ϣ������ʾ����
    OSTaskCreateExt((void(*)(void*) )qmsgshow_task,                 
                    (void*          )0,
                    (OS_STK*        )&QMSGSHOW_TASK_STK[QMSGSHOW_STK_SIZE-1],
                    (INT8U          )QMSGSHOW_TASK_PRIO,          
                    (INT16U         )QMSGSHOW_TASK_PRIO,            
                    (OS_STK*        )&QMSGSHOW_TASK_STK[0],         
                    (INT32U         )QMSGSHOW_STK_SIZE,            
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
    //�ź���������
    OSTaskCreateExt((void(*)(void*) )flags_task,                 
                    (void*          )0,
                    (OS_STK*        )&FLAGS_TASK_STK[FLAGS_STK_SIZE-1],
                    (INT8U          )FLAGS_TASK_PRIO,          
                    (INT16U         )FLAGS_TASK_PRIO,            
                    (OS_STK*        )&FLAGS_TASK_STK[0],         
                    (INT32U         )FLAGS_STK_SIZE,            
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
	OSTaskSuspend(START2_TASK_PRIO); //����ʼ����
}
 
//LED����
void led_task(void *pdata)
{
	u8 t;
	while(1)
	{
		t++;
		delay_ms(10);
		if(t==8)LED0(1);//LED0��
		if(t==100)		//LED0��
		{
			t=0;
			LED0(0);
		}
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
		 	if(tp_dev.x[0]<(130-1)&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>(220+1))
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

//������Ϣ��ʾ����
void qmsgshow_task(void *pdata)
{
	u8 *p;
	u8 err;
	while(1)
	{
		p=OSQPend(q_msg,0,&err);//������Ϣ����
		LCD_ShowString(5,170,240,16,16,p);//��ʾ��Ϣ
 		myfree(SRAMIN,p);	  
		delay_ms(500);	 
	}									 
}

//������
void main_task(void *pdata)
{		
	u8 led1sta=1;
	u32 key=0;	
	u8 err;	
 	u8 tmr2sta=1;	//�����ʱ��2����״̬   
 	u8 tmr3sta=0;	//�����ʱ��3����״̬
	u8 flagsclrt=0;	//�ź�������ʾ���㵹��ʱ   
 	tmr1=OSTmrCreate(10,10,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);		//100msִ��һ��
	tmr2=OSTmrCreate(10,20,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err);		//200msִ��һ��
	tmr3=OSTmrCreate(10,10,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr3_callback,0,"tmr3",&err);		//100msִ��һ��
	OSTmrStart(tmr1,&err);//���������ʱ��1				 
	OSTmrStart(tmr2,&err);//���������ʱ��2				 
 	while(1)
	{
		key=(u32)OSMboxPend(msg_key,10,&err); 
		if(key)
		{
			flagsclrt=51;//500ms�����
			OSFlagPost(flags_key,1<<(key-1),OS_FLAG_SET,&err);//���ö�Ӧ���ź���Ϊ1
		}
		if(flagsclrt)//����ʱ
		{
			flagsclrt--;
			if(flagsclrt==1)LCD_Fill(140,162,239,162+16,WHITE);//�����ʾ
		}
		switch(key)
		{
			case 1://����DS1
				LED1(led1sta^=1);
				break;
			case 2://���������ʱ��3	 
				tmr3sta=!tmr3sta;
				if(tmr3sta)OSTmrStart(tmr3,&err);  
				else OSTmrStop(tmr3,OS_TMR_OPT_NONE,0,&err);		//�ر������ʱ��3
 				break;
			case 3://���
 				LCD_Fill(0,221,129,lcddev.height-1,WHITE);
				break;
			case 4://У׼
				OSTaskSuspend(TOUCH_TASK_PRIO);						//������������		 
				OSTaskSuspend(QMSGSHOW_TASK_PRIO);	 				//���������Ϣ��ʾ����		 
 				OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);				//�ر������ʱ��1
				if(tmr2sta)OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);	//�ر������ʱ��2				 
 				if((tp_dev.touchtype&0X80)==0)TP_Adjust();   
				OSTmrStart(tmr1,&err);				//���¿��������ʱ��1
				if(tmr2sta)OSTmrStart(tmr2,&err);	//���¿��������ʱ��2	 
 				OSTaskResume(TOUCH_TASK_PRIO);		//���
 				OSTaskResume(QMSGSHOW_TASK_PRIO); 	//���
				ucos_load_main_ui();				//���¼���������		 
				break;
			case 5://�����ʱ��2 ����
				tmr2sta=!tmr2sta;
				if(tmr2sta)OSTmrStart(tmr2,&err);			  	//���������ʱ��2
				else 
				{		    		    
  					OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);	//�ر������ʱ��2
 					LCD_ShowString(148,262,240,16,16,"TMR2 STOP");//��ʾ��ʱ��2�ر���	
				}
				break;				 
				
		}  
		delay_ms(10);
	}
}		   

//�ź�������������
void flags_task(void *pdata)
{	
	u16 flags;	
	u8 err;	    						 
	while(1)
	{
		flags=OSFlagPend(flags_key,0X001F,OS_FLAG_WAIT_SET_ANY,0,&err);//�ȴ��ź���
 		if(flags&0X0001)LCD_ShowString(140,162,240,16,16,"KEY0 DOWN  "); 
		if(flags&0X0002)LCD_ShowString(140,162,240,16,16,"KEY1 DOWN  "); 
		if(flags&0X0004)LCD_ShowString(140,162,240,16,16,"KEY2 DOWN  "); 
		if(flags&0X0008)LCD_ShowString(140,162,240,16,16,"KEY_UP DOWN"); 
		if(flags&0X0010)LCD_ShowString(140,162,240,16,16,"TPAD DOWN  "); 

		PCF8574_WriteBit(BEEP_IO,0);
		delay_ms(50);
		PCF8574_WriteBit(BEEP_IO,1);
		OSFlagPost(flags_key,0X001F,OS_FLAG_CLR,&err);//ȫ���ź�������
 	}
}
   		    
//����ɨ������
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		if(key==0)
		{
			if(TPAD_Scan(0))key=5;
		}
		if(key)OSMboxPost(msg_key,(void*)key);//������Ϣ
 		delay_ms(10);
	}
}

void Third_Test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
    PCF8574_Init();				//��ʼ��PCF8574
    TPAD_Init(8);				//��ʼ����������
	tp_dev.init();				//��������ʼ�� 
	ucos_load_main_ui();		//����������	 
    mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	OSInit();					//UCOS��ʼ��
    OSTaskCreateExt((void(*)(void*) )start_task2,                //������
                    (void*          )0,                         //���ݸ��������Ĳ���
                    (OS_STK*        )&START2_TASK_STK[START2_STK_SIZE-1],//�����ջջ��
                    (INT8U          )START2_TASK_PRIO,           //�������ȼ�
                    (INT16U         )START2_TASK_PRIO,           //����ID����������Ϊ�����ȼ�һ��
                    (OS_STK*        )&START2_TASK_STK[0],        //�����ջջ��
                    (INT32U         )START2_STK_SIZE,            //�����ջ��С
                    (void*          )0,                         //�û�����Ĵ洢��
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
	OSStart(); 					//��ʼ���� 

}

