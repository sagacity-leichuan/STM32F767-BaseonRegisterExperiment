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
		u8 fontx[2];//gbk码
		u8 key,t;
		Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
		delay_init(216);			//延时初始化  
		uart1_init(108,115200);		//初始化串口波特率为115200  
		usmart_dev.init(108);		//初始化USMART
		LED_Init(); 				//初始化与LED连接的硬件接口
		MPU_Memory_Protection();	//保护相关存储区域
		SDRAM_Init();				//初始化SDRAM 
		LCD_Init(); 				//初始化LCD
		KEY_Init(); 				//初始化按键
		W25QXX_Init();				//初始化W25Q256 
		mymem_init(SRAMIN);			//初始化内部内存池
		mymem_init(SRAMEX);			//初始化外部内存池
		mymem_init(SRAMTCM);		//初始化TCM内存池
		exfuns_init();				//为fatfs相关变量申请内存	
		f_mount(fs[0],"0:",1);		//挂载SD卡 
		f_mount(fs[1],"1:",1);		//挂载FLASH. 
		while(font_init())			//检查字库
		{
UPD:	
			LCD_Clear(WHITE);			//清屏
			POINT_COLOR=RED;			//设置字体为红色 		  
			LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
			while(SD_Init())			//检测SD卡
			{
				LCD_ShowString(30,50,200,16,16,"SD Card Failed!");
				delay_ms(200);
				LCD_Fill(30,50,200+30,50+16,WHITE);
				delay_ms(200);			
			}															
			LCD_ShowString(30,50,200,16,16,"SD Card OK");
			LCD_ShowString(30,70,200,16,16,"Font Updating...");
			key=update_font(20,90,16,"0:");//更新字库
			while(key)//更新失败		
			{					  
				LCD_ShowString(30,90,200,16,16,"Font Update Failed!");
				delay_ms(200);
				LCD_Fill(20,90,200+20,90+16,WHITE);
				delay_ms(200);			   
			}		  
			LCD_ShowString(30,90,200,16,16,"Font Update Success!   ");
			delay_ms(1500); 
			LCD_Clear(WHITE);//清屏		   
		}  
		POINT_COLOR=RED;	   
		Show_Str(30,30,200,16,"阿波罗STM32F4/F7开发板",16,0); 					 
		Show_Str(30,50,200,16,"GBK字库测试程序",16,0);						 
		Show_Str(30,70,200,16,"雷川",16,0);						 
		Show_Str(30,90,200,16,"2018年6月21日",16,0);
		Show_Str(30,110,200,16,"按KEY0,更新字库",16,0);
		POINT_COLOR=BLUE;  
		Show_Str(30,130,200,16,"内码高字节:",16,0);						 
		Show_Str(30,150,200,16,"内码低字节:",16,0);						 
		Show_Str(30,170,200,16,"汉字计数器:",16,0);
	
		Show_Str(30,200,200,32,"对应汉字为:",32,0); 
		Show_Str(30,232,200,24,"对应汉字为:",24,0); 
		Show_Str(30,256,200,16,"对应汉字(16*16)为:",16,0); 		 
		Show_Str(30,272,200,12,"对应汉字(12*12)为:",12,0); 		 
		while(1)
		{
			fontcnt=0;
			for(i=0x81;i<0xff;i++)
			{		
				fontx[0]=i;
				LCD_ShowNum(118,130,i,3,16);		//显示内码高字节    
				for(j=0x40;j<0xfe;j++)
				{
					if(j==0x7f)continue;
					fontcnt++;
					LCD_ShowNum(118,150,j,3,16);	//显示内码低字节  
					LCD_ShowNum(118,170,fontcnt,5,16);//汉字计数显示	 
					fontx[1]=j;
					Show_Font(30+176,200,fontx,32,0);
					Show_Font(30+132,232,fontx,24,0);	  
					Show_Font(30+144,256,fontx,16,0);					 
					Show_Font(30+108,272,fontx,12,0);					 
					t=200;
					while(t--)//延时,同时扫描按键
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

//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//申请内存
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//释放内存
	return rval;
} 

void PICTURE_SoftDecoding_test(void)
{
	u8 led0sta=1;
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO *picfileinfo;//文件信息 
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//图片文件offset索引表 
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	usmart_dev.init(108);		//初始化USMART
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	W25QXX_Init();				//初始化W25Q256 
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
 	f_mount(fs[2],"2:",1); 		//挂载NAND FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7开发板",16,0);				    	 
	Show_Str(30,70,200,16,"图片显示程序",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY2:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"KEY_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"雷川",16,0);				    	 
	Show_Str(30,150,200,16,"2018年6月21日",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
 	while(!picfileinfo||!pname||!picoffsettbl)				//内存分配出错
 	{	    	
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
			if((t%20)==0)LED0(led0sta^=1);//LED0闪烁,提示程序正在运行.
			if(key==KEY2_PRES)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1(!pause); 	//暂停的时候LED1亮.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);			//释放内存
}

void PICTURE_HardDecoding_test(void)
{
	u8 led0sta=1;
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO *picfileinfo;//文件信息 
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//图片文件offset索引表 
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	usmart_dev.init(108);		//初始化USMART
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	W25QXX_Init();				//初始化W25Q256 
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
 	f_mount(fs[2],"2:",1); 		//挂载NAND FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7开发板",16,0);				    	 
	Show_Str(30,70,200,16,"硬件JPEG解码程序",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY2:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"KEY_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"雷川",16,0);				    	 
	Show_Str(30,150,200,16,"2018年6月21日",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
 	while(!picfileinfo||!pname||!picoffsettbl)				//内存分配出错
 	{	    	
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
			if((t%20)==0)LED0(led0sta^=1);//LED0闪烁,提示程序正在运行.
			if(key==KEY2_PRES)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1(!pause); 	//暂停的时候LED1亮.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);		//释放内存

}



extern vu8 bmp_request;						//bmp拍照请求:0,无bmp拍照请求;1,有bmp拍照请求,需要在帧中断里面,关闭DCMI接口.
extern u8 ovx_mode;							//bit0:0,RGB565模式;1,JPEG模式 
extern u16 curline;							//摄像头输出数据,当前行编号
extern u16 yoffset;							//y方向的偏移量

extern u32 *dcmi_line_buf[2];					//RGB屏时,摄像头采用一行一行读取,定义行缓存  
extern u32 *jpeg_data_buf;						//JPEG数据缓存buf 

extern volatile u32 jpeg_data_len; 			//buf中的JPEG有效数据长度 
extern volatile u8 jpeg_data_ok;				//JPEG数据采集完成标志 
												//0,数据没有采集完;
												//1,数据采集完了,但是还没处理;
												//2,数据已经处理完成了,可以开始下一帧接收
void CAMERA_test(void)
{
	u8 led0sta=1;
	u8 res,fac;							 
	u8 *pname;					//带路径的文件名 
	u8 key;						//键值		   
	u8 i;						 
	u8 sd_ok=1;					//0,sd卡不正常;1,SD卡正常. 
 	u8 scale=1;					//默认是全尺寸缩放
	u8 msgbuf[15];				//消息缓存区 
	u16 outputheight=0;
	
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	usmart_dev.init(108);		//初始化USMART
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	PCF8574_Init();				//初始化PCF8574
	OV5640_Init();				//初始化OV5640
	sw_sdcard_mode();			//首先切换为OV5640模式
	W25QXX_Init();				//初始化W25Q256
 	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7开发板",16,0);	 			    	 
	Show_Str(30,70,200,16,"照相机实验",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:拍照(bmp格式)",16,0);			    	 
	Show_Str(30,110,200,16,"KEY1:拍照(jpg格式)",16,0);			    	 
	Show_Str(30,130,200,16,"KEY2:自动对焦",16,0);					    	 
	Show_Str(30,150,200,16,"WK_UP:FullSize/Scale",16,0);				    	 
	Show_Str(30,170,200,16,"2018年6月23日",16,0);
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	if(res!=FR_EXIST&&res!=FR_OK) 	//发生了错误
	{		
		res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹		
		Show_Str(30,190,240,16,"SD卡错误!",16,0);
		delay_ms(200);				  
		Show_Str(30,190,240,16,"拍照功能将不可用!",16,0);
		delay_ms(200);				  
		sd_ok=0;  	
	} 	
	dcmi_line_buf[0]=mymalloc(SRAMIN,jpeg_line_size*4);	//为jpeg dma接收申请内存	
	dcmi_line_buf[1]=mymalloc(SRAMIN,jpeg_line_size*4);	//为jpeg dma接收申请内存	
	jpeg_data_buf=mymalloc(SRAMEX,jpeg_buf_size);		//为jpeg文件申请内存(最大4MB)
 	pname=mymalloc(SRAMIN,30);//为带路径的文件名分配30个字节的内存	 
 	while(pname==NULL||!dcmi_line_buf[0]||!dcmi_line_buf[1]||!jpeg_data_buf)	//内存分配出错
 	{	    
		Show_Str(30,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}   
	while(OV5640_Init())//初始化OV5640s
	{
		Show_Str(30,190,240,16,"OV5640 错误!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}	
 	Show_Str(30,210,230,16,"OV5640 正常",16,0); 
	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init(); 
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦
	DCMI_Init();			//DCMI配置
	if(lcdltdc.pwidth!=0)	//RGB屏
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB屏接收数据回调函数
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA配置  
	}else					//MCU 屏
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA配置,MCU屏,竖屏
	}
	if(lcddev.height>800)
	{
		yoffset=(lcddev.height-800)/2;
		outputheight=800; 
		OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动
	}else 
	{
		yoffset=0;
		outputheight=lcddev.height;
	}
	curline=yoffset;		//行数复位
	OV5640_OutSize_Set(16,4,lcddev.width,outputheight);	//满屏缩放显示
	DCMI_Start(); 			//启动传输
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		if(key)
		{ 
			if(key!=KEY2_PRES)
			{
				if(key==KEY0_PRES)//如果是BMP拍照,则等待1秒钟,去抖动,以获得稳定的bmp照片	
				{
					delay_ms(300);
					bmp_request=1;		//请求关闭DCMI
					while(bmp_request);	//等带请求处理完成
 				}else DCMI_Stop();
			}
			if(key==WKUP_PRES)		//缩放处理
			{
				scale=!scale;  
				if(scale==0)
				{
					fac=800/outputheight;//得到比例因子
					OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
					sprintf((char*)msgbuf,"Full Size 1:1");
				}else 
				{
					OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
					sprintf((char*)msgbuf,"Scale");
				}
				delay_ms(800); 	
			}else if(key==KEY2_PRES)OV5640_Focus_Single(); //手动单次自动对焦
			else if(sd_ok)//SD卡正常才可以拍照
			{    
				sw_sdcard_mode();	//切换为SD卡模式
				if(key==KEY0_PRES)	//BMP拍照
				{
					camera_new_pathname(pname,0);	//得到文件名	
					res=bmp_encode(pname,0,yoffset,lcddev.width,outputheight,0);
					sw_ov5640_mode();				//切换为OV5640模式
				}else if(key==KEY1_PRES)//JPG拍照
				{
					camera_new_pathname(pname,1);//得到文件名	
					res=ov5640_jpg_photo(pname);
					if(scale==0)
					{
						fac=800/outputheight;//得到比例因子
						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
 					}else 
					{
						OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
 					}	
					if(lcddev.height>800)OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动
				}
				if(res)//拍照有误
				{
					Show_Str(30,130,240,16,"写入文件错误!",16,0);		 
				}else 
				{
					Show_Str(30,130,240,16,"拍照成功!",16,0);
					Show_Str(30,150,240,16,"保存为:",16,0);
					Show_Str(30+56,150,240,16,pname,16,0);		    
					PCF8574_WriteBit(BEEP_IO,0);	//蜂鸣器短叫，提示拍照完成
					delay_ms(100);
					PCF8574_WriteBit(BEEP_IO,1);	//关闭蜂鸣器
				}  
				delay_ms(1000);		//等待1秒钟	
				DCMI_Start();		//这里先使能dcmi,然后立即关闭DCMI,后面再开启DCMI,可以防止RGB屏的侧移问题.
				DCMI_Stop();			
			}else //提示SD卡错误
			{					    
				Show_Str(30,130,240,16,"SD卡错误!",16,0);
				Show_Str(30,150,240,16,"拍照功能不可用!",16,0);			    
			}   		
			if(key!=KEY2_PRES)DCMI_Start();//开始显示  
		} 
		delay_ms(10);
		i++;
		if(i==20)//DS0闪烁.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}

}

extern __lwip_dev lwipdev;						//lwip控制结构体 
//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F7");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"LEICHUAN");
		LCD_ShowString(30,90,200,16,16,"2018/06/25"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	usmart_dev.init(108);		//初始化USMART
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	RTC_Init();  				//RTC初始化
	Adc_Init();  				//ADC初始化 
	PCF8574_Init();				//初始化PCF8574
	TIM3_Int_Init(100-1,10800-1);//10khz的频率,计数100为10ms 
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
   	POINT_COLOR=RED;
	lwip_test_ui(1);			//加载前半部分UI
	//先初始化lwIP(包括LAN8720初始化),此时必须插上网线,否则初始化会失败!! 
	LCD_ShowString(30,110,200,16,16,"lwIP Initing...");
	while(lwip_comm_init()!=0)
	{
		LCD_ShowString(30,110,200,16,16,"lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(30,110,230,110+16,WHITE);//清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");

	//等待DHCP获取 
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();
	}
	lwip_test_ui(2);//加载后半部分UI 
	httpd_init();	//HTTP初始化(默认开启websever)
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://TCP Server模式
				tcp_server_test();
 				lwip_test_ui(3);//重新加载UI  
				break;
			case KEY1_PRES://TCP Client模式
				tcp_client_test();
				lwip_test_ui(3);//重新加载UI
				break; 
			case KEY2_PRES://UDP模式
				udp_demo_test();
				lwip_test_ui(3);//重新加载UI
				break; 
		} 
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if(t==100)LCD_ShowString(30,230,200,16,16,"Please choose a mode!");
		if(t==200)
		{ 
			t=0;
			LCD_Fill(30,230,230,230+16,WHITE);//清除显示
			LED0(led0sta^=1);
		} 
	}  
}


