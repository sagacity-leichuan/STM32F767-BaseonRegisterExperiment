#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "mpu.h"   
#include "usart1.h"  
#include "lcd.h"  
#include "ltdc.h" 
#include "sdram.h"   
#include "key.h"
#include "usmart.h"  
#include "malloc.h"     
#include "nand.h"    
#include "ftl.h" 
#include "w25qxx.h"    
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"
#include "hjpgd.h"
#include "jpeg_data_handle.h"
#include "pcf8574.h"
#include "ov5640.h"
#include "dcmi.h"
#include "lwip_comm.h"
#include "rtc.h"
#include "adc.h"
#include "timer.h"
#include "httpd.h"
#include "tcp_server_demo.h" 
#include "tcp_client_demo.h"
#include "udp_demo.h"


void TEXT_test(void)
{
		u8 led0sta=1;
		u32 fontcnt;		  
		u8 i,j;
		u8 fontx[2];//gbk��
		u8 key,t;
		Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
		delay_init(216);			//��ʱ��ʼ��  
		uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
		usmart_dev.init(108);		//��ʼ��USMART
		LED_Init(); 				//��ʼ����LED���ӵ�Ӳ���ӿ�
		MPU_Memory_Protection();	//������ش洢����
		SDRAM_Init();				//��ʼ��SDRAM 
		LCD_Init(); 				//��ʼ��LCD
		KEY_Init(); 				//��ʼ������
		W25QXX_Init();				//��ʼ��W25Q256 
		mymem_init(SRAMIN);			//��ʼ���ڲ��ڴ��
		mymem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��
		mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
		exfuns_init();				//Ϊfatfs��ر��������ڴ�	
		f_mount(fs[0],"0:",1);		//����SD�� 
		f_mount(fs[1],"1:",1);		//����FLASH. 
		while(font_init())			//����ֿ�
		{
UPD:	
			LCD_Clear(WHITE);			//����
			POINT_COLOR=RED;			//��������Ϊ��ɫ 		  
			LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
			while(SD_Init())			//���SD��
			{
				LCD_ShowString(30,50,200,16,16,"SD Card Failed!");
				delay_ms(200);
				LCD_Fill(30,50,200+30,50+16,WHITE);
				delay_ms(200);			
			}															
			LCD_ShowString(30,50,200,16,16,"SD Card OK");
			LCD_ShowString(30,70,200,16,16,"Font Updating...");
			key=update_font(20,90,16,"0:");//�����ֿ�
			while(key)//����ʧ��		
			{					  
				LCD_ShowString(30,90,200,16,16,"Font Update Failed!");
				delay_ms(200);
				LCD_Fill(20,90,200+20,90+16,WHITE);
				delay_ms(200);			   
			}		  
			LCD_ShowString(30,90,200,16,16,"Font Update Success!   ");
			delay_ms(1500); 
			LCD_Clear(WHITE);//����		   
		}  
		POINT_COLOR=RED;	   
		Show_Str(30,30,200,16,"������STM32F4/F7������",16,0); 					 
		Show_Str(30,50,200,16,"GBK�ֿ���Գ���",16,0);						 
		Show_Str(30,70,200,16,"�״�",16,0);						 
		Show_Str(30,90,200,16,"2018��6��21��",16,0);
		Show_Str(30,110,200,16,"��KEY0,�����ֿ�",16,0);
		POINT_COLOR=BLUE;  
		Show_Str(30,130,200,16,"������ֽ�:",16,0);						 
		Show_Str(30,150,200,16,"������ֽ�:",16,0);						 
		Show_Str(30,170,200,16,"���ּ�����:",16,0);
	
		Show_Str(30,200,200,32,"��Ӧ����Ϊ:",32,0); 
		Show_Str(30,232,200,24,"��Ӧ����Ϊ:",24,0); 
		Show_Str(30,256,200,16,"��Ӧ����(16*16)Ϊ:",16,0); 		 
		Show_Str(30,272,200,12,"��Ӧ����(12*12)Ϊ:",12,0); 		 
		while(1)
		{
			fontcnt=0;
			for(i=0x81;i<0xff;i++)
			{		
				fontx[0]=i;
				LCD_ShowNum(118,130,i,3,16);		//��ʾ������ֽ�    
				for(j=0x40;j<0xfe;j++)
				{
					if(j==0x7f)continue;
					fontcnt++;
					LCD_ShowNum(118,150,j,3,16);	//��ʾ������ֽ�  
					LCD_ShowNum(118,170,fontcnt,5,16);//���ּ�����ʾ	 
					fontx[1]=j;
					Show_Font(30+176,200,fontx,32,0);
					Show_Font(30+132,232,fontx,24,0);	  
					Show_Font(30+144,256,fontx,16,0);					 
					Show_Font(30+108,272,fontx,12,0);					 
					t=200;
					while(t--)//��ʱ,ͬʱɨ�谴��
					{
						delay_ms(1);
						key=KEY_Scan(0);
						if(key==KEY0_PRES)goto UPD;
					}
					LED0(led0sta^=1);
				}	
			}	
		} 
}

//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO *tfileinfo;	//��ʱ�ļ���Ϣ	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//�����ڴ�
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼ 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//������/��ĩβ��,�˳�	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//�ͷ��ڴ�
	return rval;
} 

void PICTURE_SoftDecoding_test(void)
{
	u8 led0sta=1;
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO *picfileinfo;//�ļ���Ϣ 
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//ͼƬ�ļ�offset������ 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	usmart_dev.init(108);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	W25QXX_Init();				//��ʼ��W25Q256 
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
 	f_mount(fs[2],"2:",1); 		//����NAND FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"������STM32F4/F7������",16,0);				    	 
	Show_Str(30,70,200,16,"ͼƬ��ʾ����",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY2:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"KEY_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"�״�",16,0);				    	 
	Show_Str(30,150,200,16,"2018��6��21��",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//�����ڴ�
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//Ϊ��·�����ļ��������ڴ�
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(!picfileinfo||!pname||!picoffsettbl)				//�ڴ�������
 	{	    	
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"��ʼ��ʾ...",16,0); 
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//ɨ�谴��
			if(t>250)key=1;			//ģ��һ�ΰ���KEY0    
			if((t%20)==0)LED0(led0sta^=1);//LED0��˸,��ʾ������������.
			if(key==KEY2_PRES)		//��һ��
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1(!pause); 	//��ͣ��ʱ��LED1��.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAMIN,picfileinfo);			//�ͷ��ڴ�						   		    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picoffsettbl);			//�ͷ��ڴ�
}

void PICTURE_HardDecoding_test(void)
{
	u8 led0sta=1;
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO *picfileinfo;//�ļ���Ϣ 
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//ͼƬ�ļ�offset������ 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	usmart_dev.init(108);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	W25QXX_Init();				//��ʼ��W25Q256 
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
 	f_mount(fs[2],"2:",1); 		//����NAND FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"������STM32F4/F7������",16,0);				    	 
	Show_Str(30,70,200,16,"Ӳ��JPEG�������",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY2:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"KEY_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"�״�",16,0);				    	 
	Show_Str(30,150,200,16,"2018��6��21��",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//�����ڴ�
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//Ϊ��·�����ļ��������ڴ�
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(!picfileinfo||!pname||!picoffsettbl)				//�ڴ�������
 	{	    	
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"��ʼ��ʾ...",16,0); 
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//ɨ�谴��
			if(t>250)key=1;			//ģ��һ�ΰ���KEY0    
			if((t%20)==0)LED0(led0sta^=1);//LED0��˸,��ʾ������������.
			if(key==KEY2_PRES)		//��һ��
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1(!pause); 	//��ͣ��ʱ��LED1��.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAMIN,picfileinfo);			//�ͷ��ڴ�						   		    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picoffsettbl);		//�ͷ��ڴ�

}



extern vu8 bmp_request;						//bmp��������:0,��bmp��������;1,��bmp��������,��Ҫ��֡�ж�����,�ر�DCMI�ӿ�.
extern u8 ovx_mode;							//bit0:0,RGB565ģʽ;1,JPEGģʽ 
extern u16 curline;							//����ͷ�������,��ǰ�б��
extern u16 yoffset;							//y�����ƫ����

extern u32 *dcmi_line_buf[2];					//RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л���  
extern u32 *jpeg_data_buf;						//JPEG���ݻ���buf 

extern volatile u32 jpeg_data_len; 			//buf�е�JPEG��Ч���ݳ��� 
extern volatile u8 jpeg_data_ok;				//JPEG���ݲɼ���ɱ�־ 
												//0,����û�вɼ���;
												//1,���ݲɼ�����,���ǻ�û����;
												//2,�����Ѿ����������,���Կ�ʼ��һ֡����
void CAMERA_test(void)
{
	u8 led0sta=1;
	u8 res,fac;							 
	u8 *pname;					//��·�����ļ��� 
	u8 key;						//��ֵ		   
	u8 i;						 
	u8 sd_ok=1;					//0,sd��������;1,SD������. 
 	u8 scale=1;					//Ĭ����ȫ�ߴ�����
	u8 msgbuf[15];				//��Ϣ������ 
	u16 outputheight=0;
	
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	usmart_dev.init(108);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	PCF8574_Init();				//��ʼ��PCF8574
	OV5640_Init();				//��ʼ��OV5640
	sw_sdcard_mode();			//�����л�ΪOV5640ģʽ
	W25QXX_Init();				//��ʼ��W25Q256
 	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD��  
	POINT_COLOR=RED;      
	while(font_init()) 			//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"������STM32F4/F7������",16,0);	 			    	 
	Show_Str(30,70,200,16,"�����ʵ��",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:����(bmp��ʽ)",16,0);			    	 
	Show_Str(30,110,200,16,"KEY1:����(jpg��ʽ)",16,0);			    	 
	Show_Str(30,130,200,16,"KEY2:�Զ��Խ�",16,0);					    	 
	Show_Str(30,150,200,16,"WK_UP:FullSize/Scale",16,0);				    	 
	Show_Str(30,170,200,16,"2018��6��23��",16,0);
	res=f_mkdir("0:/PHOTO");		//����PHOTO�ļ���
	if(res!=FR_EXIST&&res!=FR_OK) 	//�����˴���
	{		
		res=f_mkdir("0:/PHOTO");		//����PHOTO�ļ���		
		Show_Str(30,190,240,16,"SD������!",16,0);
		delay_ms(200);				  
		Show_Str(30,190,240,16,"���չ��ܽ�������!",16,0);
		delay_ms(200);				  
		sd_ok=0;  	
	} 	
	dcmi_line_buf[0]=mymalloc(SRAMIN,jpeg_line_size*4);	//Ϊjpeg dma���������ڴ�	
	dcmi_line_buf[1]=mymalloc(SRAMIN,jpeg_line_size*4);	//Ϊjpeg dma���������ڴ�	
	jpeg_data_buf=mymalloc(SRAMEX,jpeg_buf_size);		//Ϊjpeg�ļ������ڴ�(���4MB)
 	pname=mymalloc(SRAMIN,30);//Ϊ��·�����ļ�������30���ֽڵ��ڴ�	 
 	while(pname==NULL||!dcmi_line_buf[0]||!dcmi_line_buf[1]||!jpeg_data_buf)	//�ڴ�������
 	{	    
		Show_Str(30,190,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}   
	while(OV5640_Init())//��ʼ��OV5640s
	{
		Show_Str(30,190,240,16,"OV5640 ����!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}	
 	Show_Str(30,210,230,16,"OV5640 ����",16,0); 
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
	OV5640_OutSize_Set(16,4,lcddev.width,outputheight);	//����������ʾ
	DCMI_Start(); 			//��������
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//��֧������
		if(key)
		{ 
			if(key!=KEY2_PRES)
			{
				if(key==KEY0_PRES)//�����BMP����,��ȴ�1����,ȥ����,�Ի���ȶ���bmp��Ƭ	
				{
					delay_ms(300);
					bmp_request=1;		//����ر�DCMI
					while(bmp_request);	//�ȴ����������
 				}else DCMI_Stop();
			}
			if(key==WKUP_PRES)		//���Ŵ���
			{
				scale=!scale;  
				if(scale==0)
				{
					fac=800/outputheight;//�õ���������
					OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
					sprintf((char*)msgbuf,"Full Size 1:1");
				}else 
				{
					OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
					sprintf((char*)msgbuf,"Scale");
				}
				delay_ms(800); 	
			}else if(key==KEY2_PRES)OV5640_Focus_Single(); //�ֶ������Զ��Խ�
			else if(sd_ok)//SD�������ſ�������
			{    
				sw_sdcard_mode();	//�л�ΪSD��ģʽ
				if(key==KEY0_PRES)	//BMP����
				{
					camera_new_pathname(pname,0);	//�õ��ļ���	
					res=bmp_encode(pname,0,yoffset,lcddev.width,outputheight,0);
					sw_ov5640_mode();				//�л�ΪOV5640ģʽ
				}else if(key==KEY1_PRES)//JPG����
				{
					camera_new_pathname(pname,1);//�õ��ļ���	
					res=ov5640_jpg_photo(pname);
					if(scale==0)
					{
						fac=800/outputheight;//�õ���������
						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
 					}else 
					{
						OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
 					}	
					if(lcddev.height>800)OV5640_WR_Reg(0x3035,0X51);//�������֡�ʣ�������ܶ���
				}
				if(res)//��������
				{
					Show_Str(30,130,240,16,"д���ļ�����!",16,0);		 
				}else 
				{
					Show_Str(30,130,240,16,"���ճɹ�!",16,0);
					Show_Str(30,150,240,16,"����Ϊ:",16,0);
					Show_Str(30+56,150,240,16,pname,16,0);		    
					PCF8574_WriteBit(BEEP_IO,0);	//�������̽У���ʾ�������
					delay_ms(100);
					PCF8574_WriteBit(BEEP_IO,1);	//�رշ�����
				}  
				delay_ms(1000);		//�ȴ�1����	
				DCMI_Start();		//������ʹ��dcmi,Ȼ�������ر�DCMI,�����ٿ���DCMI,���Է�ֹRGB���Ĳ�������.
				DCMI_Stop();			
			}else //��ʾSD������
			{					    
				Show_Str(30,130,240,16,"SD������!",16,0);
				Show_Str(30,150,240,16,"���չ��ܲ�����!",16,0);			    
			}   		
			if(key!=KEY2_PRES)DCMI_Start();//��ʼ��ʾ  
		} 
		delay_ms(10);
		i++;
		if(i==20)//DS0��˸.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}

}

extern __lwip_dev lwipdev;						//lwip���ƽṹ�� 
//����UI
//mode:
//bit0:0,������;1,����ǰ�벿��UI
//bit1:0,������;1,���غ�벿��UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//�����ʾ
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F7");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"LEICHUAN");
		LCD_ShowString(30,90,200,16,16,"2018/06/25"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//�����ʾ
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//�õ�����
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"KEY0:TCP Server Test");
		LCD_ShowString(30,190,200,16,16,"KEY1:TCP Client Test");
		LCD_ShowString(30,210,200,16,16,"KEY2:UDP Test");
	}
}

void LWIP_test(void)
{
	u8 led0sta=1;
	u8 t;
	u8 key; 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart1_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200  
	usmart_dev.init(108);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	SDRAM_Init();				//��ʼ��SDRAM 
	LCD_Init();					//��ʼ��LCD
	KEY_Init();					//��ʼ������
	RTC_Init();  				//RTC��ʼ��
	Adc_Init();  				//ADC��ʼ�� 
	PCF8574_Init();				//��ʼ��PCF8574
	TIM3_Int_Init(100-1,10800-1);//10khz��Ƶ��,����100Ϊ10ms 
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMTCM);		//��ʼ��TCM�ڴ��
   	POINT_COLOR=RED;
	lwip_test_ui(1);			//����ǰ�벿��UI
	//�ȳ�ʼ��lwIP(����LAN8720��ʼ��),��ʱ�����������,�����ʼ����ʧ��!! 
	LCD_ShowString(30,110,200,16,16,"lwIP Initing...");
	while(lwip_comm_init()!=0)
	{
		LCD_ShowString(30,110,200,16,16,"lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(30,110,230,110+16,WHITE);//�����ʾ
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");

	//�ȴ�DHCP��ȡ 
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();
	}
	lwip_test_ui(2);//���غ�벿��UI 
	httpd_init();	//HTTP��ʼ��(Ĭ�Ͽ���websever)
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://TCP Serverģʽ
				tcp_server_test();
 				lwip_test_ui(3);//���¼���UI  
				break;
			case KEY1_PRES://TCP Clientģʽ
				tcp_client_test();
				lwip_test_ui(3);//���¼���UI
				break; 
			case KEY2_PRES://UDPģʽ
				udp_demo_test();
				lwip_test_ui(3);//���¼���UI
				break; 
		} 
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if(t==100)LCD_ShowString(30,230,200,16,16,"Please choose a mode!");
		if(t==200)
		{ 
			t=0;
			LCD_Fill(30,230,230,230+16,WHITE);//�����ʾ
			LED0(led0sta^=1);
		} 
	}  
}


