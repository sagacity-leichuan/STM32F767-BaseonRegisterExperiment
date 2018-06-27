#include "function.h"  
#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "delay.h" 
#include "led.h"
#include "key.h"
#include "exti.h"
#include "watch_dog.h"
#include "timer.h"
#include "tpad.h"
#include "oled.h"
#include "mpu.h" 
#include "lcd.h"
#include "sdram.h"
#include "ltdc.h" 
#include "wkup.h"
#include "rng.h"
#include "rtc.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "pcf8574.h"
#include "24cxx.h"
#include "can.h"
#include "rs485.h"
#include "w25qxx.h"
#include "ap3216c.h"
#include "remote.h"
#include "ds18b20.h"
#include "dht11.h"
#include "touch_test.h"
#include "touch.h"
#include "stmflash.h"
#include "24l01.h"
#include "mpu9250.h"
#include "inv_mpu.h"
#include "malloc.h"
#include "sdmmc_sdcard.h"
#include "ftl.h"
#include "exfuns.h"
#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of device I/O functions */
#include "nand.h"
#include "ov5640.h"
#include "dcmi.h" 


extern u8  TIM5CH1_CAPTURE_STA;	//���벶��״̬	
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
u8 mpudata[128] __attribute__((at(0X20002000)));	//����һ������
u16 testsram[250000] __attribute__((at(0XC01F4000)));//����������
#define SEND_BUF_SIZE 7800	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.
u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����
const u8 TEXT_TO_SEND[]={"sagacity_lc@163.com STM32F7 DMA ����ʵ��"};	
//Ҫд�뵽24c02���ַ�������
const u8 TEXT_Buffer[]={"Apollo STM32F7 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)
extern _m_tp_dev tp_dev;

const u8 TEXT_Buffer1[]={"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(TEXT_Buffer1)	 		  	//���鳤��	
#define SIZE1 TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)

#define FLASH_SAVE_ADDR  0X08010000 	//����FLASH �����ַ(����Ϊ4�ı���������������,Ҫ���ڱ�������ռ�õ�������.
										//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.


void FATFS_test(void)
{
	u8 led0sta=1;
 	u32 total,free;
	u8 t=0;	
	u8 res=0;	
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD 
	W25QXX_Init();				//��ʼ��W25Q256 
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FATFS TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2016/7/22");	 	 
	LCD_ShowString(30,130,200,16,16,"Use USMART for test");	      
 	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0��˸
	}
	FTL_Init();
 	exfuns_init();				//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1);		//����SD�� 
 	res=f_mount(fs[1],"1:",1);	//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",FM_ANY,0,fatbuf,_MAX_SS);	//��ʽ��FLASH,1:,�̷�;FM_ANY,�Զ�ѡ���ļ�ϵͳ����,0,�Զ�ѡ��ش�С
		if(res==0)
		{  
			f_setlabel((const TCHAR *)"1:ALIENTEK");//����Flash���̵�����Ϊ��ALIENTEK
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
		}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}	 		
	res=f_mount(fs[2],"2:",1);	//����NAND FLASH.	
	if(res==0X0D)				//NAND FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//��ʽ��NAND
		res=f_mkfs("2:",FM_ANY,0,fatbuf,_MAX_SS);	//��ʽ��NAND FLASH,1:,�̷�;FM_ANY,�Զ�ѡ���ļ�ϵͳ����,0,�Զ�ѡ��ش�С
		if(res==0)
		{
			f_setlabel((const TCHAR *)"2:NANDDISK");//����Flash���̵�����Ϊ��NANDDISK
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//��ʽ�����
		}else LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}	 				
	LCD_Fill(30,150,240,150+16,WHITE);		//�����ʾ			  
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//�����ʾ			  
		delay_ms(200);
		LED0(led0sta^=1);//DS0��˸
	}													  			    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
	LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+8*14,170,total>>10,5,16);				//��ʾSD�������� MB
 	LCD_ShowNum(30+8*14,190,free>>10,5,16);					//��ʾSD��ʣ������ MB			    
	while(1)
	{
		t++; 
		delay_ms(200);		
		LED0(led0sta^=1);//DS0��˸
	} 

}

void NAND_test(void)
{
	u8 led0sta=1;
	u8 key,t=0;
	u16 i;
	u8 *buf;
	u8 *backbuf;
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/22");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");		   
 	while(FTL_Init())			//���NAND FLASH,����ʼ��FTL
	{
		LCD_ShowString(30,190,200,16,16,"NAND Error!");
		delay_ms(500);				 
		LCD_ShowString(30,190,200,16,16,"Please Check");
		delay_ms(500);	 
		LED0(led0sta^=1);//DS0��˸
	}
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//����һ�������Ļ���
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//����һ�������Ļ���
 	POINT_COLOR=BLUE;								//��������Ϊ��ɫ 
	sprintf((char*)buf,"NAND Size:%dMB",(nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);
	LCD_ShowString(30,190,200,16,16,buf);			//��ʾNAND����  
	FTL_ReadSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//Ԥ�ȶ�ȡ����2����������,��ֹ��д�����ļ�ϵͳ��.
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0����,��ȡsector
				key=FTL_ReadSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//��ȡ����
				if(key==0)//��ȡ�ɹ�
				{
					LCD_ShowString(30,210,200,16,16,"USART1 Sending Data...  ");
					printf("Sector 2 data is:\r\n");
					for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)
					{
						printf("%x ",buf[i]);//�������
					}
					printf("\r\ndata end.\r\n");
					LCD_ShowString(30,210,200,16,16,"USART1 Send Data Over!  "); 
				}
				break;
			case KEY1_PRES://KEY1����,д��sector
				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//�������(�����,����t��ֵ��ȷ��) 
				LCD_ShowString(30,210,210,16,16,"Writing data to sector..");
				key=FTL_WriteSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//д������
				if(key==0)LCD_ShowString(30,210,200,16,16,"Write data successed    ");//д��ɹ�
				else LCD_ShowString(30,210,200,16,16,"Write data failed       ");//д��ʧ��
				break;
			case KEY2_PRES://KEY2����,�ָ�sector������
				LCD_ShowString(30,210,210,16,16,"Recovering data...      ");
				key=FTL_WriteSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//д������
				if(key==0)LCD_ShowString(30,210,200,16,16,"Recovering data OK      ");//�ָ��ɹ�
				else LCD_ShowString(30,210,200,16,16,"Recovering data failed  ");//�ָ�ʧ��
				break;
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);
			t=0;
		}
	} 

}


////************************************************************SDCARD**************************************//

//ͨ�����ڴ�ӡSD�������Ϣ
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//������ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//����Ե�ַ
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//��ʾ����
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//��ʾ���С
}
//����SD���Ķ�ȡ
//��secaddr��ַ��ʼ,��ȡseccnt������������
//secaddr:������ַ
//seccnt:������
void sd_test_read(u32 secaddr,u32 seccnt)
{
	u32 i;
	u8 *buf;
	u8 sta=0;
	buf=mymalloc(SRAMEX,seccnt*512);	//�����ڴ�,��SDRAM�����ڴ�
	sta=SD_ReadDisk(buf,secaddr,seccnt);//��ȡsecaddr������ʼ������
	if(sta==0)			
	{	 
		printf("SECTOR %d DATA:\r\n",secaddr);
		for(i=0;i<seccnt*512;i++)printf("%x ",buf[i]);//��ӡsecaddr��ʼ����������    	   
		printf("\r\nDATA ENDED\r\n"); 
	}else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);	//�ͷ��ڴ�	   
}

//����SD����д��(����,���дȫ��0XFF������,���������SD��.)
//��secaddr��ַ��ʼ,д��seccnt������������
//secaddr:������ַ
//seccnt:������
void sd_test_write(u32 secaddr,u32 seccnt)
{
	u32 i;
	u8 *buf;
	u8 sta=0;
	buf=mymalloc(SRAMEX,seccnt*512);	//��SDRAM�����ڴ�
	for(i=0;i<seccnt*512;i++) 			//��ʼ��д�������,��3�ı���.
	{
		buf[i]=i*3;
	}
	sta=SD_WriteDisk(buf,secaddr,seccnt);//��secaddr������ʼд��seccnt����������
	if(sta==0)							
	{	 
		printf("Write over!\r\n"); 
	}else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);					//�ͷ��ڴ�	   
}

void SDCARD_test(void)
{
	u8 led0sta=1;
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	u8 *buf;	
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"SD CARD TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2016/7/21");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 0");	   
 	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0��˸
	}
	show_sdcard_info();	//��ӡSD�������Ϣ
 	POINT_COLOR=BLUE;	//��������Ϊ��ɫ 
	//���SD���ɹ� 											    
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	LCD_ShowNum(30+13*8,170,SDCardInfo.CardCapacity>>20,5,16);//��ʾSD������
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0������
		{
			buf=mymalloc(0,512);		//�����ڴ�
			key=SD_ReadDisk(buf,0,1);
			if(key==0)	//��ȡ0����������
			{	
				LCD_ShowString(30,190,200,16,16,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//��ӡ0��������    	   
				printf("\r\nDATA ENDED\r\n");
				LCD_ShowString(30,190,200,16,16,"USART1 Send Data Over!");
			}else printf("err:%d\r\n",key);
			myfree(0,buf);//�ͷ��ڴ�	   
		}  
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//DS0��˸
			t=0;
		}
	} 
}


void MEMEMORY_MANAGEMENT_test(void)
{
	u8 led0sta=1;
	u8 paddr[20];				//���P Addr:+p��ַ��ASCIIֵ
	u16 memused=0;
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 sramx=0;					//Ĭ��Ϊ�ڲ�sram    
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/21");	 		
	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Read"); 
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,170,200,16,16,"SRAMIN");
	LCD_ShowString(30,190,200,16,16,"SRAMIN  USED:");
	LCD_ShowString(30,210,200,16,16,"SRAMEX  USED:");
	LCD_ShowString(30,230,200,16,16,"SRAMTCM USED:");
 	while(1)
	{	
		key=KEY_Scan(0);//��֧������	
		switch(key)
		{
			case 0://û�а�������	
				break;
			case KEY0_PRES:	//KEY0����
				p=mymalloc(sramx,2048);//����2K�ֽ�
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//��pд��һЩ����
				break;
			case KEY1_PRES:	//KEY1����	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//������ʾ���� 	 
					LCD_ShowString(30,270,200,16,16,p);			 //��ʾP������
				}
				break;
			case KEY2_PRES:	//KEY2����	  
				myfree(sramx,p);//�ͷ��ڴ�
				p=0;			//ָ��յ�ַ
				break;
			case WKUP_PRES:	//KEY UP���� 
				sramx++; 
				if(sramx>2)sramx=0;
				if(sramx==0)LCD_ShowString(30,170,200,16,16,"SRAMIN ");
				else if(sramx==1)LCD_ShowString(30,170,200,16,16,"SDRAMEX ");
				else LCD_ShowString(30,170,200,16,16,"SRAMTCM");
				break;
		}
		if(tp!=p&&p!=NULL)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,250,200,16,16,paddr);	//��ʾp�ĵ�ַ
			if(p)LCD_ShowString(30,270,200,16,16,p);//��ʾP������
		    else LCD_Fill(30,270,239,266,WHITE);	//p=0,�����ʾ
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0��˸.
		{
			memused=mymem_perused(SRAMIN);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,190,200,16,16,paddr);	//��ʾ�ڲ��ڴ�ʹ����
			memused=mymem_perused(SRAMEX);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,210,200,16,16,paddr);	//��ʾ�ⲿ�ڴ�ʹ����
			memused=mymem_perused(SRAMTCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,230,200,16,16,paddr);	//��ʾTCM�ڴ�ʹ���� 
 			LED0(led0sta^=1);
 		}
	}	 
}


////************************************************************OV5640_test**************************************//
u8 ovx_mode=0;							//bit0:0,RGB565ģʽ;1,JPEGģʽ 
u16 curline=0;							//����ͷ�������,��ǰ�б��
u16 yoffset=0;							//y�����ƫ����

#define jpeg_buf_size   1*1024*1024		//����JPEG���ݻ���jpeg_buf�Ĵ�С(1*4M�ֽ�)
#define jpeg_line_size	2*1024			//����DMA��������ʱ,һ�����ݵ����ֵ

u32 dcmi_line_buf[2][jpeg_line_size];	//RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л���  
u32 jpeg_data_buf[jpeg_buf_size] __attribute__((at(0XC0000000+1280*800*2)));//JPEG���ݻ���buf,������LCD֡����֮��

volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
										//JPEG�ߴ�֧���б�
const u16 jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA 
	320,240,	//QVGA	
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA 
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
	1920,1080,	//1080P
	2048,1536,	//QXGA
	2592,1944,	//500W 
};

const u8*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7����Ч 
const u8*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEGͼƬ 12�ֳߴ� 


//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;			//ʣ�����ݳ���
	u32 *pbuf;
	curline=yoffset;	//������λ
	if(ovx_mode&0X01)	//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0) //jpeg���ݻ�δ�ɼ���?
		{
			DMA2_Stream1->CR&=~(1<<0);		//ֹͣ��ǰ����
			while(DMA2_Stream1->CR&0X01);	//�ȴ�DMA2_Stream1������ 
			rlen=jpeg_line_size-DMA2_Stream1->NDTR;//�õ�ʣ�����ݳ���	
			pbuf=jpeg_data_buf+jpeg_data_len;//ƫ�Ƶ���Ч����ĩβ,�������
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[1][i];//��ȡbuf1�����ʣ������
			else for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[0][i];//��ȡbuf0�����ʣ������ 
			jpeg_data_len+=rlen;			//����ʣ�೤��
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2) //��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_line_size;//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA2_Stream1->CR|=1<<0; 		//���´���
			jpeg_data_ok=0; 				//�������δ�ɼ�
			jpeg_data_len=0;				//�������¿�ʼ
		}
	}else
	{
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare(); 			//��ʼд��GRAM
	}	
} 

//jpeg���ݽ��ջص�����
void jpeg_dcmi_rx_callback(void)
{  
	u16 i;
	u32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;		//ƫ�Ƶ���Ч����ĩβ
	if(DMA2_Stream1->CR&(1<<19))			//buf0����,��������buf1
	{ 
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[0][i];//��ȡbuf0���������
		jpeg_data_len+=jpeg_line_size;		//ƫ��
	}else									//buf1����,��������buf0
	{
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[1][i];//��ȡbuf1���������
		jpeg_data_len+=jpeg_line_size;		//ƫ�� 
	} 
}

//JPEG����,ͨ������2���͸�����.
void OV5640_JPEG_test(void)
{
	u32 i,jpgstart,jpglen; 
	u8 *p;
	u8 key,headok=0;
	u8 effect=0,contrast=2;
	u8 size=2;			//Ĭ����QVGA 320*240�ߴ�
	u8 msgbuf[15];		//��Ϣ������ 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//ִ���Զ��Խ�
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//�ֱ������� 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//��ʾ��ǰJPEG�ֱ���
	//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEGģʽ
	
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ� 
	DCMI_Init();			//DCMI���� 
	dcmi_rx_callback=jpeg_dcmi_rx_callback;//JPEG�������ݻص�����
	DCMI_DMA_Init((u32)&dcmi_line_buf[0],(u32)&dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA����    
 	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	DCMI_Start(); 		//��������
	while(1)
	{
		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			p=(u8*)jpeg_data_buf;
			printf("jpeg_data_len:%d\r\n",jpeg_data_len*4);//��ӡ֡��
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //��ʾ���ڴ������� 
			jpglen=0;	//����jpg�ļ���СΪ0
			headok=0;	//���jpgͷ���
			for(i=0;i<jpeg_data_len*4;i++)//����0XFF,0XD8��0XFF,0XD9,��ȡjpg�ļ���С
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//�ҵ�FF D8
				{
					jpgstart=i;
					headok=1;	//����ҵ�jpgͷ(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			if(jpglen)	//������jpeg���� 
			{
				p+=jpgstart;			//ƫ�Ƶ�0XFF,0XD8�� 
				for(i=0;i<jpglen;i++)	//��������jpg�ļ�
				{
					USART2->TDR=p[i];  
					while((USART2->ISR&0X40)==0);	//ѭ������,ֱ���������   
					key=KEY_Scan(0); 
					if(key)break;
				}  
			}
			if(key)	//�а�������,��Ҫ����
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//��ʾ�˳����ݴ���
				switch(key)
				{				    
					case KEY0_PRES:	//�Աȶ�����
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
						break; 
					case KEY1_PRES:	//ִ��һ���Զ��Խ�
						OV5640_Focus_Single();
						break;
					case KEY2_PRES:	//��Ч����				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//������Ч
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG����ߴ�����   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//��ʾ����������� 
			jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
		}		
	}    
}

//RGB�����ݽ��ջص�����
void rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
	{ 
		pbuf=(u16*)dcmi_line_buf[0]; 
	}else 						//DMAʹ��buf0,��ȡbuf1
	{
		pbuf=(u16*)dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(0,curline,lcddev.width-1,curline,pbuf);//DM2D��� 
	if(curline<lcddev.height)curline++;
}

//RGB����ֱ����ʾ��LCD����
void OV5640_RGB565_test(void)
{
	u8 key;
	u8 effect=0,contrast=2,fac;
	u8 scale=1;		//Ĭ����ȫ�ߴ�����
	u8 msgbuf[15];	//��Ϣ������ 
	u16 outputheight=0;
	
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode"); 
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//ִ���Զ��Խ�
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:FullSize/Scale");	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/ȫ�ߴ�����
	//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ	
	OV5640_Focus_Init();
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	DCMI_Init();			//DCMI����
	if(lcdltdc.pwidth!=0)	//RGB��
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB���������ݻص�����
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA����  
	}else					//MCU ��
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA����,MCU��,����
	} 
	TIM3->CR1&=~(0x01);		//�رն�ʱ��3,�ر�֡��ͳ�ƣ��򿪵Ļ���RGB�����ڴ��ڴ�ӡ��ʱ�򣬻ᶶ
	if(lcddev.height>800)
	{
		yoffset=(lcddev.height-800)/2;
		outputheight=800;
		OV5640_WR_Reg(0x3035,0X51);//�������֡�ʣ�������ܶ���
	}else 
	{
		yoffset=0;
		outputheight=lcddev.height;
	}
	curline=yoffset;		//������λ
	OV5640_OutSize_Set(4,0,lcddev.width,outputheight);		//����������ʾ
	DCMI_Start(); 			//��������
	LCD_Clear(BLACK);
	while(1)
	{ 
		key=KEY_Scan(0); 
		if(key)
		{ 
			if(key!=KEY1_PRES)DCMI_Stop(); //��KEY1����,ֹͣ��ʾ
			switch(key)
			{				    
				case KEY0_PRES:	//�Աȶ�����
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					break;
				case KEY1_PRES:	//ִ��һ���Զ��Խ�
					OV5640_Focus_Single();
					break;
				case KEY2_PRES:	//��Ч����				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//������Ч
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					break;
				case WKUP_PRES:	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/����	    
					scale=!scale;  
					if(scale==0)
					{
						fac=800/outputheight;	//�õ���������
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,outputheight);
 						sprintf((char*)msgbuf,"Scale");
					}
					break;
			}
			if(key!=KEY1_PRES)	//��KEY1����
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 
				DCMI_Start();	//���¿�ʼ����
			}
		} 
		delay_ms(10);		
	}    
}

void OV5640_test(void)
{
	u8 led0sta=1;
	u8 key; 
	u8 t;     
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	usart2_init(54,921600);		//��ʼ������2������Ϊ921600
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	TIM3_Int_Init(10000-1,10800-1);//10Khz����,1�����ж�һ��
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"OV5640 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/20");	 		
	while(OV5640_Init())//��ʼ��OV5640
	{
		LCD_ShowString(30,130,240,16,16,"OV5640 ERROR");
		delay_ms(200);
	    LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
		LED0(led0sta^=1);
	} 
	LCD_ShowString(30,130,200,16,16,"OV5640 OK");
  	while(1)
	{	
		key=KEY_Scan(0);
		if(key==KEY0_PRES){ovx_mode=0;break;}		//RGB565ģʽ
	    else if(key==KEY1_PRES){ovx_mode=1;break;}	//JPEGģʽ 
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			LCD_Fill(30,150,210,150+16,WHITE);
			t=0; 
			LED0(led0sta^=1);
		}
		delay_ms(5);	  
	}
	if(ovx_mode==1)OV5640_JPEG_test();
	else OV5640_RGB565_test();  
}

void STMFLASH_test(void)
{
	u8 led0sta=1;
	u8 key=0;
	u16 i=0;
	u8 datatemp[SIZE1];	  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FLASH EEPROM TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/15");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)	//KEY1����,д��STM32 FLASH
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,"Start Write FLASH....");
			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer1,SIZE1);
			LCD_ShowString(30,170,200,16,16,"FLASH Write Finished!");//��ʾ�������
		}
		if(key==KEY0_PRES)	//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read FLASH.... ");
			STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,SIZE1);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);//��ʾ�������ַ���
		}
		i++;
		delay_ms(10);  
		if(i==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			i=0;
		}		   
	}    
}

void NRF24L01_test(void)
{
	u8 led0sta=1;
	u8 key,mode;
	u16 t=0;			 
	u8 tmp_buf[33];	 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
 	NRF24L01_Init();    		//��ʼ��NRF24L01 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"NRF24L01 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/15");
	
	while(NRF24L01_Check())
	{
		LCD_ShowString(30,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}
	
	LCD_ShowString(30,130,200,16,16,"NRF24L01 OK");
	
 	while(1)
	{	
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			mode=0;   
			break;
		}else if(key==KEY1_PRES)
		{
			mode=1;
			break;
		}
		
		t++;
		
		if(t==100)
			LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //��˸��ʾ��ʾ��Ϣ

 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}

		delay_ms(5);	  
	} 
	
 	LCD_Fill(10,150,240,166,WHITE);//����������ʾ		  
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
 	
	if(mode==0)//RXģʽ
	{
		LCD_ShowString(30,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(30,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  
		while(1)
		{	  		    		    				 
			if(NRF24L01_RxPacket(tmp_buf)==0)//һ�����յ���Ϣ,����ʾ����.
			{
				tmp_buf[32]=0;//�����ַ���������
				LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf);    
			}
			else 
				delay_us(100);	   

			t++;
			
			if(t==10000)//��Լ1s�Ӹı�һ��״̬
			{
				t=0;
				LED0(led0sta^=1);
			} 				    
		};	
	}
	else//TXģʽ
	{							    
		LCD_ShowString(30,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		mode=' ';//�ӿո����ʼ  
		
		while(1)
		{	  		   				 
			if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
			{
				LCD_ShowString(30,170,239,32,16,"Sended DATA:");	
				LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf); 
				key=mode;
				for(t=0;t<32;t++)
				{
					key++;
					
					if(key>('~'))
						key=' ';
						
					tmp_buf[t]=key;	
				}
				
				mode++;
				
				if(mode>'~')mode=' ';
				
				tmp_buf[32]=0;//���������		   
			}
			else
			{										   	
 				LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//�����ʾ			   
				LCD_ShowString(30,170,lcddev.width-1,32,16,"Send Failed "); 
			}
			
			LED0(led0sta^=1);
			delay_ms(1500);				    
		};
	}     

}

//����1����1���ַ� 
//c:Ҫ���͵��ַ�
void usart1_send_char(u8 c)
{
	while((USART1->ISR&0X40)==0);	//ѭ������,ֱ���������   
    USART1->TDR=c;  
} 

//�������ݸ������������վ(V4�汾)
//fun:������. 0X01~0X1C
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0XAA;	//֡ͷ
	send_buf[1]=0XAA;	//֡ͷ
	send_buf[2]=fun;	//������
	send_buf[3]=len;	//���ݳ���
	for(i=0;i<len;i++)
		send_buf[4+i]=data[i];			//��������
	for(i=0;i<len+4;i++)
		send_buf[len+4]+=send_buf[i];	//����У���	
	for(i=0;i<len+5;i++)
		usart1_send_char(send_buf[i]);	//�������ݵ�����1 
}

//���ͼ��ٶȴ���������+����������(������֡)
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ 
void mpu9250_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[18]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	tbuf[12]=0;//��Ϊ����MPL��,�޷�ֱ�Ӷ�ȡ����������,��������ֱ�����ε�.��0���.
	tbuf[13]=0;
	tbuf[14]=0;
	tbuf[15]=0;
	tbuf[16]=0;
	tbuf[17]=0;
	usart1_niming_report(0X02,tbuf,18);//������֡,0X02
}

//ͨ������1�ϱ���������̬���ݸ�����(״̬֡)
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
//csb:�������߶�,��λ:cm
//prs:��ѹ�Ƹ߶�,��λ:mm
void usart1_report_imu(short roll,short pitch,short yaw,short csb,int prs)
{
	u8 tbuf[12];   	
	tbuf[0]=(roll>>8)&0XFF;
	tbuf[1]=roll&0XFF;
	tbuf[2]=(pitch>>8)&0XFF;
	tbuf[3]=pitch&0XFF;
	tbuf[4]=(yaw>>8)&0XFF;
	tbuf[5]=yaw&0XFF;
	tbuf[6]=(csb>>8)&0XFF;
	tbuf[7]=csb&0XFF;
	tbuf[8]=(prs>>24)&0XFF;
	tbuf[9]=(prs>>16)&0XFF;
	tbuf[10]=(prs>>8)&0XFF;
	tbuf[11]=prs&0XFF;
	usart1_niming_report(0X01,tbuf,12);//״̬֡,0X01
}   

void MPU9250_test(void)
{
	u8 led0sta=1;
	u8 t=0,report=0;	    	//Ĭ�Ͽ����ϱ�
	u8 key;
	float pitch,roll,yaw; 		//ŷ����
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ���� 
	short temp;		        	//�¶�
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,500000);		//��ʼ�����ڲ�����Ϊ500000
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
    MPU_Init();             	//��ʼ��MPU9250
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"MPU9250 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/15");	 		
 	 
	while(mpu_dmp_init())         
    {   
		LCD_ShowString(30,130,200,16,16,"MPU9250 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
		LED0(led0sta^=1);//DS0��˸ 
    }
    
    LCD_ShowString(30,130,200,16,16,"MPU9250 OK");
	LCD_ShowString(30,150,200,16,16,"KEY0:UPLOAD ON/OFF");
    POINT_COLOR=BLUE;     //��������Ϊ��ɫ
    LCD_ShowString(30,170,200,16,16,"UPLOAD ON ");	 
 	LCD_ShowString(30,200,200,16,16," Temp:    . C");	
 	LCD_ShowString(30,220,200,16,16,"Pitch:    . C");	
 	LCD_ShowString(30,240,200,16,16," Roll:    . C");	 
 	LCD_ShowString(30,260,200,16,16," Yaw :    . C");	
    while(1)
    {
        key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
            report=!report;
			if(report)LCD_ShowString(30,170,200,16,16,"UPLOAD ON ");
			else LCD_ShowString(30,170,200,16,16,"UPLOAD OFF");
		}
		
        if(mpu_mpl_get_data(&pitch,&roll,&yaw)==0)
        {
            temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������

  			if(report)mpu9250_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//���ͼ��ٶ�+������ԭʼ����
			if(report)usart1_report_imu((int)(roll*100),(int)(pitch*100),(int)(yaw*100),0,0);

			if((t%10)==0)
			{ 
				if(temp<0)
				{
					LCD_ShowChar(30+48,200,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,200,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,200,temp/100,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,200,temp%10,1,16);		//��ʾС������ 
				temp=pitch*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,220,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,220,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,220,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,220,temp%10,1,16);		//��ʾС������ 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,240,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,240,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,240,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,240,temp%10,1,16);		//��ʾС������ 
				temp=yaw*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,260,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,260,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,260,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,260,temp%10,1,16);		//��ʾС������  
				t=0;
				LED0(led0sta^=1);//DS0��˸ 
			}
		}
        t++;
	} 	
}

void DHT11_test(void)
{
	u8 led0sta=1;
	u8 t=0;			        
	u8 temperature;  	    
	u8 humidity;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	PCF8574_Init();				//��ʼ��PCF8574
	PCF8574_ReadBit(EX_IO);		//��һ��,�ͷ�INT��,��ֹ���ŵ����� 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DHT11 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
 	while(DHT11_Init())	//DHT11��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	while(1)
	{	    	    
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ					    
			LCD_ShowNum(30+40,150,temperature,2,16);		//��ʾ�¶�	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);			//��ʾʪ��	 	   
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0(led0sta^=1);
		}
	}

}

void DS18B20_test(void)
{
	u8 led0sta=1;
	u8 t=0;			    
	short temperature;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	PCF8574_Init();				//��ʼ��PCF8574
	PCF8574_ReadBit(EX_IO);		//��һ��,�ͷ�INT��,��ֹ���ŵ����� 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DS18B20 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
 	while(DS18B20_Init())	//DS18B20��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}   
	LCD_ShowString(30,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,150,200,16,16,"Temp:   . C");	 
	while(1)
	{	    	    
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(30+40,150,'-',16,0);			//��ʾ����
				temperature=-temperature;					//תΪ����
			}else LCD_ShowChar(30+40,150,' ',16,0);			//ȥ������
			LCD_ShowNum(30+40+8,150,temperature/10,2,16);	//��ʾ��������	    
   			LCD_ShowNum(30+40+32,150,temperature%10,1,16);	//��ʾС������ 		   
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0(led0sta^=1);
		}
	}

}

void Remote_test(void)
{
	u8 led0sta=1;
	u8 key;
	u8 t=0;	
	u8 *str=0; 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
 	Remote_Init();				//������ճ�ʼ��		
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"REMOTE TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"KEYVAL:");	
   	LCD_ShowString(30,150,200,16,16,"KEYCNT:");	
   	LCD_ShowString(30,170,200,16,16,"SYMBOL:");	    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ  														 	  		    							  
	while(1)
	{
		key=Remote_Scan();	
		if(key)
		{	 
			LCD_ShowNum(86,130,key,3,16);		//��ʾ��ֵ
			LCD_ShowNum(86,150,RmtCnt,3,16);	//��ʾ��������		  
			switch(key)
			{
				case 0:str="ERROR";break;			   
				case 162:str="POWER";break;	    
				case 98:str="UP";break;	    
				case 2:str="PLAY";break;		 
				case 226:str="ALIENTEK";break;		  
				case 194:str="RIGHT";break;	   
				case 34:str="LEFT";break;		  
				case 224:str="VOL-";break;		  
				case 168:str="DOWN";break;		   
				case 144:str="VOL+";break;		    
				case 104:str="1";break;		  
				case 152:str="2";break;	   
				case 176:str="3";break;	    
				case 48:str="4";break;		    
				case 24:str="5";break;		    
				case 122:str="6";break;		  
				case 16:str="7";break;			   					
				case 56:str="8";break;	 
				case 90:str="9";break;
				case 66:str="0";break;
				case 82:str="DELETE";break;		 
			}
			LCD_Fill(86,170,116+8*8,170+16,WHITE);	//���֮ǰ����ʾ
			LCD_ShowString(86,170,200,16,16,str);	//��ʾSYMBOL
		}else delay_ms(10);	  
		t++;
		if(t==20)
		{
			t=0;
			LED0(led0sta^=1);
		}
	}
}

void TP_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ�� 
	tp_dev.init();				//��������ʼ�� 
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"TOUCH TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		

   	if(tp_dev.touchtype!=0XFF)
   		LCD_ShowString(30,130,200,16,16,"Press KEY0 to Adjust");//����������ʾ

	delay_ms(1500);
 	Load_Drow_Dialog();	 	
	
	if(tp_dev.touchtype&0X80)
		ctp_test();//����������
	else 
		rtp_test(); 

}

void CAN1_test(void)
{
 	u8 led0sta=1; 
	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1;	//CAN����ģʽ;0,��ͨģʽ;1,����ģʽ
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ�� 
 	CAN1_Mode_Init(1,7,10,6,1);	//CAN��ʼ��,������500Kbps    
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"LoopBack Mode");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");//��ʾ��ʾ��Ϣ		
  	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(30,170,200,16,16,"Count:");			//��ʾ��ǰ����ֵ	
	LCD_ShowString(30,190,200,16,16,"Send Data:");		//��ʾ���͵�����	
	LCD_ShowString(30,250,200,16,16,"Receive Data:");	//��ʾ���յ�������		
 	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;//��䷢�ͻ�����
				if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//��ʾ����
 			}
			res=CAN1_Send_Msg(canbuf,8);//����8���ֽ� 
			if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");		//��ʾ����ʧ��
			else LCD_ShowString(30+80,190,200,16,16,"OK    ");	 		//��ʾ���ͳɹ�								   
		}else if(key==WKUP_PRES)//WK_UP���£��ı�CAN�Ĺ���ģʽ
		{	   
			mode=!mode;
			CAN1_Mode_Init(1,7,10,6,mode);	//CAN��ͨģʽ��ʼ��,��ͨģʽ,������500Kbps
  			POINT_COLOR=RED;//��������Ϊ��ɫ 
			if(mode==0)//��ͨģʽ����Ҫ2��������
			{
				LCD_ShowString(30,130,200,16,16,"Nnormal Mode ");	    
			}else //�ػ�ģʽ,һ��������Ϳ��Բ�����.
			{
 				LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;//��������Ϊ��ɫ 
		}		 
		key=CAN1_Receive_Msg(canbuf);
		if(key)//���յ�������
		{			
			LCD_Fill(30,270,160,310,WHITE);//���֮ǰ����ʾ
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//��ʾ����
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0X80);	//��ʾ����
		}		   
	} 

}

void RS485_test(void)
{
 	u8 led0sta=1; 
	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 rs485buf[5]; 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ�� 
	RS485_Init(54,9600);		//��ʼ��RS485
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RS485 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"KEY0:Send");	//��ʾ��ʾ��Ϣ		
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(30,150,200,16,16,"Count:");			//��ʾ��ǰ����ֵ	
	LCD_ShowString(30,170,200,16,16,"Send Data:");		//��ʾ���͵�����	
	LCD_ShowString(30,210,200,16,16,"Receive Data:");	//��ʾ���յ�������	 							  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			for(i=0;i<5;i++)
			{
				rs485buf[i]=cnt+i;//��䷢�ͻ�����
				LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//��ʾ����
 			}
			RS485_Send_Data(rs485buf,5);//����5���ֽ� 									   
		}		 
		RS485_Receive_Data(rs485buf,&key);
		
		if(key)//���յ�������
		{
			if(key>5)key=5;//�����5������.
 			for(i=0;i<key;i++)LCD_ShowxNum(30+i*32,230,rs485buf[i],3,16,0X80);	//��ʾ����
 		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,150,cnt,3,16,0X80);	//��ʾ����
		}		   
	}   

}

void QSPI_test(void)
{
 	u8 led0sta=1; 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;		
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ�� 
	W25QXX_Init();				//W25QXX��ʼ��
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"QSPI TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/18");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ		
	while(W25QXX_ReadID()!=W25Q256)								//��ⲻ��W25Q256
	{
		LCD_ShowString(30,150,200,16,16,"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0��˸
	}
	LCD_ShowString(30,150,200,16,16,"W25Q256 Ready!"); 
	FLASH_SIZE=32*1024*1024;	//FLASH ��СΪ32M�ֽ�
  	POINT_COLOR=BLUE;			//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����,д��W25Q256
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//��ʾ�������
		}
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);					//��ʾ�������ַ���
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);	//DS0��˸
			i=0;
		}		   
	}	  	
}

void AP3216C_test(void)
{	
	u8 led0sta=1;
 	u16 ir,als,ps;			
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"AP3216C TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	while(AP3216C_Init())		//��ⲻ��AP3216C
	{
		LCD_ShowString(30,130,200,16,16,"AP3216C Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,130,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0��˸
	}
	LCD_ShowString(30,130,200,16,16,"AP3216C Ready!");  
    LCD_ShowString(30,160,200,16,16," IR:");	 
 	LCD_ShowString(30,180,200,16,16," PS:");	
 	LCD_ShowString(30,200,200,16,16,"ALS:");	 
 	POINT_COLOR=BLUE;			//��������Ϊ��ɫ		  
    while(1)
    {
        AP3216C_ReadData(&ir,&ps,&als);	//��ȡ���� 
        LCD_ShowNum(30+32,160,ir,5,16);	//��ʾIR����
        LCD_ShowNum(30+32,180,ps,5,16);	//��ʾPS����
        LCD_ShowNum(30+32,200,als,5,16);//��ʾALS���� 
		LED0(led0sta^=1);				//��ʾϵͳ��������	
        delay_ms(120); 
	}	  
}

void PCF8574_test(void)
{
 	u8 led0sta=1,led1sta=1;
	u8 key;
	u16 i=0; 
	u8 beepsta=1;				
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
 	KEY_Init(); 				//������ʼ��	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"PCF8574 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/29");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:BEEP ON/OFF");	//��ʾ��ʾ��Ϣ	
	LCD_ShowString(30,150,200,16,16,"EXIO:DS1 ON/OFF");		//��ʾ��ʾ��Ϣ		
	while(PCF8574_Init())		//��ⲻ��PCF8574
	{
		LCD_ShowString(30,170,200,16,16,"PCF8574 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,170,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0��˸
	}
	LCD_ShowString(30,170,200,16,16,"PCF8574 Ready!");    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan(0); 
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{ 
			beepsta=!beepsta;					//������״̬ȡ��
			PCF8574_WriteBit(BEEP_IO,beepsta);	//���Ʒ�����
		}
		if(PCF8574_INT==0)				//PCF8574���жϵ͵�ƽ��Ч
		{
			key=PCF8574_ReadBit(EX_IO);	//��ȡEXIO״̬,ͬʱ���PCF8574���ж����(INT�ָ��ߵ�ƽ)
			if(key==0)LED1(led1sta^=1); //LED1״̬ȡ�� 
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//DS0��˸
			i=0;
		}		   
	} 	    

}

void AT24CXX_test(void)
{
 	u8 led0sta=1;
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];					
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
 	KEY_Init(); 				//������ʼ��	
	AT24CXX_Init();				//IIC��ʼ�� 
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"IIC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/29");	 
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ		
 	while(AT24CXX_Check())//��ⲻ��24c02
	{
		LCD_ShowString(30,150,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0��˸
	}
	LCD_ShowString(30,150,200,16,16,"24C02 Ready!");    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����,д��24C02
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,"Start Write 24C02....");
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
			LCD_ShowString(30,170,200,16,16,"24C02 Write Finished!");//��ʾ�������
		}
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read 24C02.... ");
			AT24CXX_Read(0,datatemp,SIZE);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);//��ʾ�������ַ���
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//DS0��˸
			i=0;
		}		   
	} 	    

}

void MYDMA_test(void)
{
 	u8 led0sta=1;
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;				//����
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init(); 				//������ʼ��	
 	MYDMA_Config(DMA2_Stream7,4,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      	 
	//��ʾ��ʾ��Ϣ	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//���ASCII�ַ�������
    {
		if(t>=j)//���뻻�з�
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}
			else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}
		else//����TEXT_TO_SEND���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0����
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//��ʾ�ٷֺ�
		    USART1->CR3=1<<7;           //ʹ�ܴ���1��DMA����       
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);//��ʼһ��DMA���䣡	  
		    //�ȴ�DMA������ɣ���ʱ������������һЩ�£����
		    //ʵ��Ӧ���У����������ڼ䣬����ִ�����������
		    while(1)
		    {
				if(DMA2->HISR&(1<<27))	//�ȴ�DMA2_Steam7�������
				{
					DMA2->HIFCR|=1<<27;	//���DMA2_Steam7������ɱ�־
					break; 
		        }
				pro=DMA2_Stream7->NDTR;	//�õ���ǰ��ʣ����ٸ�����
				pro=1-pro/SEND_BUF_SIZE;//�õ��ٷֱ�	  
				pro*=100;      			//����100��
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//��ʾ100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//��ʾ�������
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			i=0;
		}		   
	}		    

}

void PWM_DAC_test(void)
{
	u8 led0sta=1;
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 pwmval=0;
	u8 key;   
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	Adc_Init(); 				//��ʼ��ADC 
	KEY_Init(); 				//������ʼ��	
	TIM9_CH2_PWM_Init(255,1);	//TIM9 PWM��ʼ��, Fpwm=108M/256=421.9Khz.
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	PWM_DAC_VAL=pwmval;//��ʼֵΪ0	    	      
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
			PWM_DAC_VAL=pwmval;		//���	
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			PWM_DAC_VAL=pwmval; 		//���
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1������,���߶�ʱʱ�䵽��
		{	  
 			adcx=PWM_DAC_VAL;
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//��ʾDAC�Ĵ���ֵ
			temp=(float)adcx*(3.3/256);;			//�õ�DAC��ѹֵ
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//��ʾ��ѹֵ��������
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//��ʾ��ѹֵ��С������
 			adcx=Get_Adc_Average(ADC_CH5,20);  		//�õ�ADCת��ֵ	  
			temp=(float)adcx*(3.3/4096);			//�õ�ADC��ѹֵ
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//��ʾ��ѹֵ��������
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//��ʾ��ѹֵ��С������
			t=0;
			LED0(led0sta^=1);	   
		}	    
		delay_ms(10); 
	}		
}
void Dac1_test(void)
{
 	u8 led0sta=1;
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 dacval=0;
	u8 key;	 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	Adc_Init(); 				//��ʼ��ADC
	KEY_Init(); 				//������ʼ��
	Dac1_Init();		 		//DACͨ��1��ʼ��	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	DAC->DHR12R1=dacval;		//��ʼֵΪ0	    		
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(dacval<4000)dacval+=200;
			DAC->DHR12R1=dacval;		//���	
		}else if(key==KEY1_PRES)	
		{
			if(dacval>200)dacval-=200;
			else dacval=0;
			DAC->DHR12R1=dacval; 		//���
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1������,���߶�ʱʱ�䵽��
		{	  
 			adcx=DAC->DHR12R1;
			LCD_ShowxNum(94,150,adcx,4,16,0);     	//��ʾDAC�Ĵ���ֵ
			temp=(float)adcx*(3.3/4096);			//�õ�DAC��ѹֵ
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//��ʾ��ѹֵ��������
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0X80); 	//��ʾ��ѹֵ��С������
 			adcx=Get_Adc_Average(ADC_CH5,20);		//�õ�ADCת��ֵ	  
			temp=(float)adcx*(3.3/4096);			//�õ�ADC��ѹֵ
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//��ʾ��ѹֵ��������
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0X80); 	//��ʾ��ѹֵ��С������
			LED0(led0sta^=1);	   
			t=0;
		}	    
		delay_ms(10);	 
	}	

}

void Temprate_test(void)
{
	u8 led0sta=1;
	short temp;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	Adc_Init(); 				//��ʼ��ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,140,200,16,16,"TEMPERATE: 00.00C");	      
	while(1)
	{
		temp=Get_Temprate();	//�õ��¶�ֵ 
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,140,16,16,16,"-");	//��ʾ����
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	//�޷���
		
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//��ʾ��������
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0X80);	//��ʾС������ 		 
		LED0(led0sta^=1);
		delay_ms(250);	
	}

}

void ADC_test(void)
{
 	u8 led0sta=1;
 	u16 adcx;
	float temp;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	Adc_Init(); 				//��ʼ��ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	      
	while(1)
	{
		adcx=Get_Adc_Average(ADC_CH5,20);
		LCD_ShowxNum(134,130,adcx,4,16,0);//��ʾADC��ֵ
		temp=(float)adcx*(3.3/4096); 
		adcx=temp;
		LCD_ShowxNum(134,150,adcx,1,16,0);//��ʾ��ѹֵ
		temp-=adcx;
		temp*=1000;
		LCD_ShowxNum(150,150,temp,3,16,0X80);
		LED0(led0sta^=1);
		delay_ms(250);	
	}	
}

void WKUP_test(void)
{
 	u8 led0sta=1;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
 	WKUP_Init();				//�������ѳ�ʼ��
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-SAGACITY");
	LCD_ShowString(30,110,200,16,16,"2018/5/24");	
	LCD_ShowString(30,130,200,16,16,"WK_UP:Stanby/WK_UP");	 
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(250);   
	}
}
void RNG_test(void)
{
 	u8 led0sta=1; 
	u32 random;
	u8 t=0,key;
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	KEY_Init();					//������ʼ��
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RNG TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-SAGACITY");
	LCD_ShowString(30,110,200,16,16,"2016/7/13");	 
	while(RNG_Init())	 		//��ʼ�������������
	{
		LCD_ShowString(30,130,200,16,16,"  RNG Error! ");	 
		delay_ms(200);
		LCD_ShowString(30,130,200,16,16,"RNG Trying...");	 
	}                                 
	LCD_ShowString(30,130,200,16,16,"RNG Ready!   ");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Get Random Num");	 
	LCD_ShowString(30,180,200,16,16,"Random Num:");	 
 	LCD_ShowString(30,210,200,16,16,"Random Num[0-9]:");
 	
 	POINT_COLOR=BLUE;
 	
	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			random=RNG_Get_RandomNum(); 
			LCD_ShowNum(30+8*11,180,random,10,16); 
		} 
		if((t%20)==0)
		{
			LED0(led0sta^=1);						//ÿ200ms,��תһ��LED0 
			random=RNG_Get_RandomRange(0,9);		//��ȡ[0,9]����������
			LCD_ShowNum(30+8*16,210,random,1,16); 	//��ʾ�����
		}
		
		delay_ms(10);
		t++;
	}	 


}
void RTC_test(void)
{
	u8 led0sta=1; 
	u8 hour,min,sec,ampm;
	u8 year,month,date,week;
	u8 tbuf[40];
	u8 t=0;  
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	RTC_Init();		 			//��ʼ��RTC
	RTC_Set_WakeUp(4,0);		//����WAKE UP�ж�,1�����ж�һ�� 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-SAGACITY");
	LCD_ShowString(30,110,200,16,16,"2016/7/13");	
  	while(1) 
	{		
		t++;
		if((t%10)==0)	//ÿ100ms����һ����ʾ����
		{
			RTC_Get_Time(&hour,&min,&sec,&ampm);
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",hour,min,sec); 
			LCD_ShowString(30,140,210,16,16,tbuf);	
			RTC_Get_Date(&year,&month,&date,&week);
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",year,month,date); 
			LCD_ShowString(30,160,210,16,16,tbuf);	
			sprintf((char*)tbuf,"Week:%d",week); 
			LCD_ShowString(30,180,210,16,16,tbuf);
		}
		
		if((t%20)==0)
			LED0(led0sta^=1);//ÿ200ms,��תһ��LED0 
		delay_ms(10);
	}	 
}
void RGBLCD_test(void)
{
	u8 x=0,led0sta=1;
	u8 lcd_id[12];	
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	KEY_Init();					//��ʼ��LED
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣				 	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(10,40,240,32,32,"Apollo STM32"); 	
		LCD_ShowString(10,80,240,24,24,"LTDC LCD TEST");
		LCD_ShowString(10,110,240,16,16,"LC-LEICHUAN");
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//��ʾLCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2018/2/23");	      					 
	    x++;
		if(x==12)
			x=0;
		LED0(led0sta^=1);	//LED0��˸	 
		delay_ms(1000);	
	} 
}

//SDRAM�ڴ����	    
void fmc_sdram_scan(u16 x,u16 y)
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	

	//�ڵ�ַ0����������	  				   
  	LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:    0KB");
  	
	//ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�
	for(i = 0X1F4000; i < 32*1024*1024; i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
	
	//���ζ���֮ǰд�������,����У��		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
  		
		if(i==0)
			sval=temp;
 		else if
 			(temp<=sval)
 				break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.	
 				
 		//��ʾ�ڴ�����  
		LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*16,5,16,0);
		//��ӡSDRAM����
		printf("SDRAM Capacity:%dKB\r\n",(u16)(temp-sval+1)*16);
 	}					 
}	
void SDRAM_test(void)
{
	u8 key;		 
 	u8 i=0,led0sta=1;	     
	u32 ts=0; 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	KEY_Init();					//��ʼ��LED
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
 	POINT_COLOR=RED;			//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"APOLLO STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"SDRAM TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/23");   
	LCD_ShowString(30,130,200,16,16,"KEY0:Test Sram");
	LCD_ShowString(30,150,200,16,16,"KEY1:TEST Data");
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ  
	for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//Ԥ���������	 
  	}
	while(1)
	{	
		key=KEY_Scan(0);//��֧������
		
		if(key==KEY0_PRES)
		{
			fmc_sdram_scan(40,170);//����SDRAM����
		}
		else if(key==KEY1_PRES)//��ӡԤ���������
		{
			for(ts=0;ts<250000;ts++)
			{
				LCD_ShowxNum(40,190,testsram[ts],6,16,0);//��ʾ��������	
				printf("testsram[%d]:%d\r\n",ts,testsram[ts]);
			}
		}else 

		delay_ms(10);   
		i++;
		
		if(i==20)//DS0��˸.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}

}
void TFTLCD_test(void)
{
 	u8 x=0,led0sta=1;
	u8 lcd_id[12];	
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ��  
	uart1_init(108,115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();//������ش洢����
	LCD_Init();				//��ʼ��LCD
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣				 	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		
		POINT_COLOR=RED;	  
		LCD_ShowString(10,40,240,32,32,"Apollo STM32"); 	
		LCD_ShowString(10,80,240,24,24,"TFTLCD TEST");
		LCD_ShowString(10,110,240,16,16,"LEICHUAN");
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//��ʾLCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2018/5/22");	      					 
	    x++;
	    
		if(x==12)
			x=0;
			
		LED0(led0sta^=1);	//LED0��˸
		
		delay_ms(1000);	
	} 	
}

/*
���� STM32F7 �Դ��� MPU ���ܣ���һ���ض����ڴ�ռ䣨���飬��ַ��0X20002000��
����д���ʱ�����
����ʱ�����ڵ���������ʾ��MPU closed����ʾĬ����û��д�����ġ�
�� KEY0 ��������������д���ݣ��� KEY1�����Զ�ȡ������������ݡ�
��KEY_UP����MPU��������ʱ��
����ٰ�KEY0������д���ݣ��ͻ�����MemManage����
���� MemManage_Handler �жϷ���������ʱ DS1 ������
ͬʱ��ӡ������Ϣ����������λ��ϵͳ������
DS0 ������ʾ�����������У�
������Ϣ����ͨ������ 1 ���(115200)��
*/
void MPU_test(void)
{
	u8 i=0,led0sta=1;
	u8 key;
	Stm32_Clock_Init(432,25,2,9);		//����ʱ��,216Mhz
    delay_init(216);					//��ʱ��ʼ��  
	uart1_init(108,115200);				//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  					//��ʼ����LED���ӵ�Ӳ���ӿ�  
	KEY_Init();		  					//��ʼ������
	printf("\r\n\r\nMPU closed!\r\n");	//��ʾMPU�ر�
	while(1)  
	{
		key=KEY_Scan(0);
        if(key==WKUP_PRES)              //ʹ��MPU�������� mpudata;
        {
            MPU_Set_Protection(0X20002000,128,0,MPU_REGION_PRIV_RO_URO,0,0,1);//ֻ��,��ֹ����,��ֹcatch,������ 
            printf("MPU open!\r\n");	//��ʾMPU��
        }else if(key==KEY0_PRES)        //��������д�����ݣ����������MPU�����Ļ�������ڴ���ʴ���
        {
			printf("Start Writing data...\r\n");
			sprintf((char*)mpudata,"MPU test array %d",i);
			printf("Data Write finshed!\r\n");
        }else if(key==KEY1_PRES)		//�������ж�ȡ���ݣ�������û�п���MPU��������������ڴ���ʴ���
        {
            printf("Array data is:%s\r\n",mpudata);
        }else delay_ms(10);
		i++;
		if((i%50)==0) LED0(led0sta^=1);	//LED0ȡ�� 
	}
}
void OLED_test(void)
{
	u8 t=0,led0sta=1;
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ��  
	uart1_init(108,115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	OLED_Init();			//��ʼ��OLED
  	OLED_ShowString(0,0,"LEICHUAN",24);  
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
 	OLED_ShowString(0,40,"lc_sagacity 2018/5/22",12);  
 	OLED_ShowString(0,52,"ASCII:",12);  
 	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram();//������ʾ��OLED	 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(36,52,t,12,1);//��ʾASCII�ַ�	
		OLED_ShowNum(94,52,t,3,12);	//��ʾASCII�ַ�����ֵ    
		OLED_Refresh_Gram();//������ʾ��OLED
		
		t++;
		if(t>'~')
			t=' '; 
		
		delay_ms(500);
		LED0(led0sta^=1);	//LED0ȡ��
	}

}

/*��Ҫ������ñ�̽Ӷ๦�ܶ˿ڣ�P11���� TPAD �� ADC����ʵ�� TPAD ���ӵ� PA5��*/
void Timer_TIM2_TPAD_test(void)
{
	u8 t=0,led0sta=1,led1sta=1;
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ��  
	uart1_init(108,115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
 	TPAD_Init(2);			//��ʼ����������,��108/2=54MhzƵ�ʼ���
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0))	//�ɹ�������һ��������(�˺���ִ��ʱ������15ms)
		{
			LED1(led1sta^=1);	//LED1ȡ��
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0(led0sta^=1);	//LED0ȡ��,��ʾ��������
		}
		delay_ms(10);
	}
}

/*
ͨ�� KEY_UP ��������ߵ�ƽ�����Ӵ��ڴ�ӡ�ߵ�ƽ����
ͬʱ���Ǳ��� PWM �����
����ͨ���öŰ������� PB1 �� PA0��
������ PWM ����ĸߵ�ƽ����
*/
void Timer_TIM5_Cap_input_test(void)
{
	long long temp=0;  
	Stm32_Clock_Init(432,25,2,9);	//����ʱ��,216Mhz
    delay_init(216);				//��ʱ��ʼ��  
	uart1_init(108,115200);			//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
 	TIM3_PWM_Init(500-1,108-1);		//1Mhz�ļ���Ƶ��,2Khz��PWM.     
	TIM5_CH1_Cap_Init(0XFFFFFFFF,108-1);//��1Mhz��Ƶ�ʼ��� 
   	while(1)
	{
 		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)
			LED0_PWM_VAL=0;	 		 
 		if(TIM5CH1_CAPTURE_STA&0X80)		//�ɹ�������һ�θߵ�ƽ
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		//���ʱ���ܺ�
			temp+=TIM5CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%lld us\r\n",temp);//��ӡ�ܵĸߵ�ƽʱ��
			TIM5CH1_CAPTURE_STA=0;			//������һ�β���
		}
	}

}

void Timer_TIM3_PWM_test(void)
{
	u16 led0pwmval=0;    
	u8 dir=1; 
	Stm32_Clock_Init(432,25,2,9);	//����ʱ��,216Mhz
    delay_init(216);				//��ʱ��ʼ��  
	LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
 	TIM3_PWM_Init(500-1,108-1);		//1Mhz�ļ���Ƶ��,2Khz��PWM.     
   	while(1)
	{
 		delay_ms(10);	
 		
		if(dir)
			led0pwmval++;
		else 
			led0pwmval--;
			
 		if(led0pwmval>300)
 			dir=0;
		if(led0pwmval==0)
			dir=1;	
			
		LED0_PWM_VAL=led0pwmval;	   
	}
}

/*ͨ�� TIM3 ���ж������� DS1 ������*/
void Timer_TIM3_test(void)
{
	u8 led0sta=1;
	Stm32_Clock_Init(432,25,2,9);	//����ʱ��,216Mhz     
    delay_init(216);				//��ʱ��ʼ��  
	LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
 	TIM3_Int_Init(5000-1,10800-1);	//10Khz�ļ���Ƶ�ʣ�����5K��Ϊ500ms     
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(200);
	};
}

/*ͨ�� DS0 ��ָʾ STM32F767 �Ƿ񱻸�λ�ˣ�
�������λ�˾ͻ���� 300ms��
DS1 ����ָʾ�ж�ι����
ÿ���ж�ι����תһ�Ρ�*/
void Wwatch_dog_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ��  
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
 	LED0(0);				//����LED0
	delay_ms(300);			//��ʱ300ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
	WWDG_Init(0X7F,0X5F,3);	//������ֵΪ7f,���ڼĴ���Ϊ5f,��Ƶ��Ϊ8	   
	while(1)
	{
		LED0(1);			//�ر�LED0
	};

}

/*���ǽ�ͨ������ KEY_UP ��ι����Ȼ��ͨ�� DS0 ��ʾ��λ״̬��*/
void Iwatch_dog_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ��  
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�   
	KEY_Init();		  		//��ʼ������
 	delay_ms(100);			//��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
	IWDG_Init(4,500);    	//Ԥ��Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s	   
	LED0(0);			 	//����LED0
	while(1)
	{ 
		if(KEY_Scan(0)==WKUP_PRES)//���WK_UP����,��ι��
		{
			IWDG_Feed();//ι��
		}
		delay_ms(10);
	};
}

void ext_interrupt_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ�� 
	uart1_init(108,115200);	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�   
	EXTIX_Init();         	//��ʼ���ⲿ�ж����� 
	LED0(0);				//�ȵ������
	while(1)
	{
		printf("lc_sagacity\r\n");	
		delay_ms(1000);	  
	}
}

void uart1_test(void)
{
	u8 t;
	u8 len;	
	u16 times=0;  
	u8 led0sta=1;			  //LED0ǰ״̬
	Stm32_Clock_Init(432,25,2,9);  //����ʱ��,216Mhz
    delay_init(216);		  //��ʱ��ʼ�� 
	uart1_init(108,115200);	  //���ڳ�ʼ��Ϊ115200
	LED_Init();		  		  //��ʼ����LED���ӵ�Ӳ���ӿ�   
	while(1)
	{
		if(USART1_RX_STA&0x8000)
		{					   
			len=USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->TDR=USART1_RX_BUF[t];
				while((USART1->ISR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART1_RX_STA=0; 
		}
		else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nlc_sagaciy ����ʵ��\r\n");
				printf("lc_sagacity@leichuan\r\n\r\n\r\n");
			}
			if(times%200==0)
				printf("����������,�Իس�������\r\n");  
			if(times%30==0)
				LED0(led0sta^=1);//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}
}

void key_test(void)
{
	u8 key;
	u8 led0sta=1,led1sta=1;		//LED0,LED1�ĵ�ǰ״̬
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��
	LED_Init();					//��ʼ��LEDʱ��  
	KEY_Init();         		//��ʼ���밴�����ӵ�Ӳ���ӿ�
	LED0(0);					//�ȵ������  
	while(1)
	{
		key=KEY_Scan(0); 		//�õ���ֵ
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//����LED0,LED1�������
					led1sta=!led1sta;
					led0sta=!led1sta;
					break;
				case KEY2_PRES:	//����LED0��ת
	 				led0sta=!led0sta;
					break;
				case KEY1_PRES:	//����LED1��ת	 
					led1sta=!led1sta;
					break;
				case KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
					led0sta=!led0sta;
					led1sta=!led1sta;
					break;
			}
			LED0(led0sta);		//����LED0״̬
			LED1(led1sta);		//����LED1״̬
		}
		else 
			delay_ms(10); 
	}

}

void led_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);		//��ʱ��ʼ�� 
	LED_Init();				//��ʼ��LEDʱ��  
	while(1)
	{
		LED0(0);				//DS0��
		LED1(1);				//DS1��
		delay_ms(500);
		LED0(1);				//DS0��
		LED1(0);				//DS1��
		delay_ms(500);
	}

}




