#include "function1.h"
#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart1.h"


/////////////////////////UCOSII-0��������///////////////////////////////////

//�����ջ	
OS_STK START0_TASK_STK[START0_STK_SIZE];
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];


void TaskManageTest(void)
{
	Stm32_Clock_Init(432,25,2,9);	//����ʱ��,216Mhz
    delay_init(216);				//��ʱ��ʼ�� 
	uart1_init(108,115200);
	LED_Init();						//��ʼ��LEDʱ��  
	OSInit();                       //UCOS��ʼ��
    OSTaskCreateExt((void(*)(void*) )start_task0,                //������
                    (void*          )0,                         //���ݸ��������Ĳ���
                    (OS_STK*        )&START0_TASK_STK[START0_STK_SIZE-1],//�����ջջ��
                    (INT8U          )START0_TASK_PRIO,           //�������ȼ�
                    (INT16U         )START0_TASK_PRIO,           //����ID����������Ϊ�����ȼ�һ��
                    (OS_STK*        )&START0_TASK_STK[0],        //�����ջջ��
                    (INT32U         )START0_STK_SIZE,            //�����ջ��С
                    (void*          )0,                         //�û�����Ĵ洢��
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
	OSStart(); 						//��ʼ����

}

//��ʼ����
void start_task0(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit(); 		 	//����ͳ������ 
	OS_ENTER_CRITICAL();  	//�����ٽ���(�ر��ж�)
    //LED0����
    OSTaskCreateExt((void(*)(void*) )led0_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED0_TASK_STK[LED0_STK_SIZE-1],
                    (INT8U          )LED0_TASK_PRIO,            
                    (INT16U         )LED0_TASK_PRIO,            
                    (OS_STK*        )&LED0_TASK_STK[0],         
                    (INT32U         )LED0_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED1����
    OSTaskCreateExt((void(*)(void*) )led1_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED1_TASK_STK[LED1_STK_SIZE-1],
                    (INT8U          )LED1_TASK_PRIO,            
                    (INT16U         )LED1_TASK_PRIO,            
                    (OS_STK*        )&LED1_TASK_STK[0],         
                    (INT32U         )LED1_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP); 

    OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
	OSTaskSuspend(START0_TASK_PRIO); //����ʼ����
}
 
//LED0����
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

//LED1����
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

