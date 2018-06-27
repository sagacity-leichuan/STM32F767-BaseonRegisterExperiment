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


extern u8  TIM5CH1_CAPTURE_STA;	//输入捕获状态	
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
u8 mpudata[128] __attribute__((at(0X20002000)));	//定义一个数组
u16 testsram[250000] __attribute__((at(0XC01F4000)));//测试用数组
#define SEND_BUF_SIZE 7800	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
const u8 TEXT_TO_SEND[]={"sagacity_lc@163.com STM32F7 DMA 串口实验"};	
//要写入到24c02的字符串数组
const u8 TEXT_Buffer[]={"Apollo STM32F7 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)
extern _m_tp_dev tp_dev;

const u8 TEXT_Buffer1[]={"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(TEXT_Buffer1)	 		  	//数组长度	
#define SIZE1 TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)

#define FLASH_SAVE_ADDR  0X08010000 	//设置FLASH 保存地址(必须为4的倍数，且所在扇区,要大于本代码所占用到的扇区.
										//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.


void FATFS_test(void)
{
	u8 led0sta=1;
 	u32 total,free;
	u8 t=0;	
	u8 res=0;	
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD 
	W25QXX_Init();				//初始化W25Q256 
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FATFS TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2016/7/22");	 	 
	LCD_ShowString(30,130,200,16,16,"Use USMART for test");	      
 	while(SD_Init())//检测不到SD卡
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0闪烁
	}
	FTL_Init();
 	exfuns_init();				//为fatfs相关变量申请内存				 
  	f_mount(fs[0],"0:",1);		//挂载SD卡 
 	res=f_mount(fs[1],"1:",1);	//挂载FLASH.	
	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",FM_ANY,0,fatbuf,_MAX_SS);	//格式化FLASH,1:,盘符;FM_ANY,自动选择文件系统类型,0,自动选择簇大小
		if(res==0)
		{  
			f_setlabel((const TCHAR *)"1:ALIENTEK");//设置Flash磁盘的名字为：ALIENTEK
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//格式化完成
		}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}	 		
	res=f_mount(fs[2],"2:",1);	//挂载NAND FLASH.	
	if(res==0X0D)				//NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//格式化NAND
		res=f_mkfs("2:",FM_ANY,0,fatbuf,_MAX_SS);	//格式化NAND FLASH,1:,盘符;FM_ANY,自动选择文件系统类型,0,自动选择簇大小
		if(res==0)
		{
			f_setlabel((const TCHAR *)"2:NANDDISK");//设置Flash磁盘的名字为：NANDDISK
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//格式化完成
		}else LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}	 				
	LCD_Fill(30,150,240,150+16,WHITE);		//清除显示			  
	while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//清除显示			  
		delay_ms(200);
		LED0(led0sta^=1);//DS0闪烁
	}													  			    
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
	LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+8*14,170,total>>10,5,16);				//显示SD卡总容量 MB
 	LCD_ShowNum(30+8*14,190,free>>10,5,16);					//显示SD卡剩余容量 MB			    
	while(1)
	{
		t++; 
		delay_ms(200);		
		LED0(led0sta^=1);//DS0闪烁
	} 

}

void NAND_test(void)
{
	u8 led0sta=1;
	u8 key,t=0;
	u16 i;
	u8 *buf;
	u8 *backbuf;
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/22");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");		   
 	while(FTL_Init())			//检测NAND FLASH,并初始化FTL
	{
		LCD_ShowString(30,190,200,16,16,"NAND Error!");
		delay_ms(500);				 
		LCD_ShowString(30,190,200,16,16,"Please Check");
		delay_ms(500);	 
		LED0(led0sta^=1);//DS0闪烁
	}
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//申请一个扇区的缓存
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//申请一个扇区的缓存
 	POINT_COLOR=BLUE;								//设置字体为蓝色 
	sprintf((char*)buf,"NAND Size:%dMB",(nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);
	LCD_ShowString(30,190,200,16,16,buf);			//显示NAND容量  
	FTL_ReadSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//预先读取扇区2到备份区域,防止乱写导致文件系统损坏.
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0按下,读取sector
				key=FTL_ReadSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//读取扇区
				if(key==0)//读取成功
				{
					LCD_ShowString(30,210,200,16,16,"USART1 Sending Data...  ");
					printf("Sector 2 data is:\r\n");
					for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)
					{
						printf("%x ",buf[i]);//输出数据
					}
					printf("\r\ndata end.\r\n");
					LCD_ShowString(30,210,200,16,16,"USART1 Send Data Over!  "); 
				}
				break;
			case KEY1_PRES://KEY1按下,写入sector
				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//填充数据(随机的,根据t的值来确定) 
				LCD_ShowString(30,210,210,16,16,"Writing data to sector..");
				key=FTL_WriteSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Write data successed    ");//写入成功
				else LCD_ShowString(30,210,200,16,16,"Write data failed       ");//写入失败
				break;
			case KEY2_PRES://KEY2按下,恢复sector的数据
				LCD_ShowString(30,210,210,16,16,"Recovering data...      ");
				key=FTL_WriteSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Recovering data OK      ");//恢复成功
				else LCD_ShowString(30,210,200,16,16,"Recovering data failed  ");//恢复失败
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

//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}
//测试SD卡的读取
//从secaddr地址开始,读取seccnt个扇区的数据
//secaddr:扇区地址
//seccnt:扇区数
void sd_test_read(u32 secaddr,u32 seccnt)
{
	u32 i;
	u8 *buf;
	u8 sta=0;
	buf=mymalloc(SRAMEX,seccnt*512);	//申请内存,从SDRAM申请内存
	sta=SD_ReadDisk(buf,secaddr,seccnt);//读取secaddr扇区开始的内容
	if(sta==0)			
	{	 
		printf("SECTOR %d DATA:\r\n",secaddr);
		for(i=0;i<seccnt*512;i++)printf("%x ",buf[i]);//打印secaddr开始的扇区数据    	   
		printf("\r\nDATA ENDED\r\n"); 
	}else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);	//释放内存	   
}

//测试SD卡的写入(慎用,最好写全是0XFF的扇区,否则可能损坏SD卡.)
//从secaddr地址开始,写入seccnt个扇区的数据
//secaddr:扇区地址
//seccnt:扇区数
void sd_test_write(u32 secaddr,u32 seccnt)
{
	u32 i;
	u8 *buf;
	u8 sta=0;
	buf=mymalloc(SRAMEX,seccnt*512);	//从SDRAM申请内存
	for(i=0;i<seccnt*512;i++) 			//初始化写入的数据,是3的倍数.
	{
		buf[i]=i*3;
	}
	sta=SD_WriteDisk(buf,secaddr,seccnt);//从secaddr扇区开始写入seccnt个扇区内容
	if(sta==0)							
	{	 
		printf("Write over!\r\n"); 
	}else printf("err:%d\r\n",sta);
	myfree(SRAMEX,buf);					//释放内存	   
}

void SDCARD_test(void)
{
	u8 led0sta=1;
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	u8 *buf;	
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"SD CARD TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2016/7/21");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 0");	   
 	while(SD_Init())//检测不到SD卡
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0闪烁
	}
	show_sdcard_info();	//打印SD卡相关信息
 	POINT_COLOR=BLUE;	//设置字体为蓝色 
	//检测SD卡成功 											    
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	LCD_ShowNum(30+13*8,170,SDCardInfo.CardCapacity>>20,5,16);//显示SD卡容量
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下了
		{
			buf=mymalloc(0,512);		//申请内存
			key=SD_ReadDisk(buf,0,1);
			if(key==0)	//读取0扇区的内容
			{	
				LCD_ShowString(30,190,200,16,16,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//打印0扇区数据    	   
				printf("\r\nDATA ENDED\r\n");
				LCD_ShowString(30,190,200,16,16,"USART1 Send Data Over!");
			}else printf("err:%d\r\n",key);
			myfree(0,buf);//释放内存	   
		}  
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//DS0闪烁
			t=0;
		}
	} 
}


void MEMEMORY_MANAGEMENT_test(void)
{
	u8 led0sta=1;
	u8 paddr[20];				//存放P Addr:+p地址的ASCII值
	u16 memused=0;
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 sramx=0;					//默认为内部sram    
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMTCM);		//初始化TCM内存池
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/21");	 		
	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Read"); 
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,170,200,16,16,"SRAMIN");
	LCD_ShowString(30,190,200,16,16,"SRAMIN  USED:");
	LCD_ShowString(30,210,200,16,16,"SRAMEX  USED:");
	LCD_ShowString(30,230,200,16,16,"SRAMTCM USED:");
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按	
		switch(key)
		{
			case 0://没有按键按下	
				break;
			case KEY0_PRES:	//KEY0按下
				p=mymalloc(sramx,2048);//申请2K字节
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//向p写入一些内容
				break;
			case KEY1_PRES:	//KEY1按下	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//更新显示内容 	 
					LCD_ShowString(30,270,200,16,16,p);			 //显示P的内容
				}
				break;
			case KEY2_PRES:	//KEY2按下	  
				myfree(sramx,p);//释放内存
				p=0;			//指向空地址
				break;
			case WKUP_PRES:	//KEY UP按下 
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
			LCD_ShowString(30,250,200,16,16,paddr);	//显示p的地址
			if(p)LCD_ShowString(30,270,200,16,16,p);//显示P的内容
		    else LCD_Fill(30,270,239,266,WHITE);	//p=0,清除显示
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0闪烁.
		{
			memused=mymem_perused(SRAMIN);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,190,200,16,16,paddr);	//显示内部内存使用率
			memused=mymem_perused(SRAMEX);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,210,200,16,16,paddr);	//显示外部内存使用率
			memused=mymem_perused(SRAMTCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+104,230,200,16,16,paddr);	//显示TCM内存使用率 
 			LED0(led0sta^=1);
 		}
	}	 
}


////************************************************************OV5640_test**************************************//
u8 ovx_mode=0;							//bit0:0,RGB565模式;1,JPEG模式 
u16 curline=0;							//摄像头输出数据,当前行编号
u16 yoffset=0;							//y方向的偏移量

#define jpeg_buf_size   1*1024*1024		//定义JPEG数据缓存jpeg_buf的大小(1*4M字节)
#define jpeg_line_size	2*1024			//定义DMA接收数据时,一行数据的最大值

u32 dcmi_line_buf[2][jpeg_line_size];	//RGB屏时,摄像头采用一行一行读取,定义行缓存  
u32 jpeg_data_buf[jpeg_buf_size] __attribute__((at(0XC0000000+1280*800*2)));//JPEG数据缓存buf,定义在LCD帧缓存之后

volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
										//JPEG尺寸支持列表
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

const u8*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7种特效 
const u8*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEG图片 12种尺寸 


//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;			//剩余数据长度
	u32 *pbuf;
	curline=yoffset;	//行数复位
	if(ovx_mode&0X01)	//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0) //jpeg数据还未采集完?
		{
			DMA2_Stream1->CR&=~(1<<0);		//停止当前传输
			while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置 
			rlen=jpeg_line_size-DMA2_Stream1->NDTR;//得到剩余数据长度	
			pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾,继续添加
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的剩余数据
			else for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的剩余数据 
			jpeg_data_len+=rlen;			//加上剩余长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2) //上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_line_size;//传输长度为jpeg_buf_size*4字节
			DMA2_Stream1->CR|=1<<0; 		//重新传输
			jpeg_data_ok=0; 				//标记数据未采集
			jpeg_data_len=0;				//数据重新开始
		}
	}else
	{
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare(); 			//开始写入GRAM
	}	
} 

//jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{  
	u16 i;
	u32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;		//偏移到有效数据末尾
	if(DMA2_Stream1->CR&(1<<19))			//buf0已满,正常处理buf1
	{ 
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的数据
		jpeg_data_len+=jpeg_line_size;		//偏移
	}else									//buf1已满,正常处理buf0
	{
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的数据
		jpeg_data_len+=jpeg_line_size;		//偏移 
	} 
}

//JPEG数据,通过串口2发送给电脑.
void OV5640_JPEG_test(void)
{
	u32 i,jpgstart,jpglen; 
	u8 *p;
	u8 key,headok=0;
	u8 effect=0,contrast=2;
	u8 size=2;			//默认是QVGA 320*240尺寸
	u8 msgbuf[15];		//消息缓存区 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//执行自动对焦
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//分辨率设置 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//显示当前JPEG分辨率
	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEG模式
	
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦 
	DCMI_Init();			//DCMI配置 
	dcmi_rx_callback=jpeg_dcmi_rx_callback;//JPEG接收数据回调函数
	DCMI_DMA_Init((u32)&dcmi_line_buf[0],(u32)&dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA配置    
 	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
	while(1)
	{
		if(jpeg_data_ok==1)	//已经采集完一帧图像了
		{  
			p=(u8*)jpeg_data_buf;
			printf("jpeg_data_len:%d\r\n",jpeg_data_len*4);//打印帧率
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //提示正在传输数据 
			jpglen=0;	//设置jpg文件大小为0
			headok=0;	//清除jpg头标记
			for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//找到FF D8
				{
					jpgstart=i;
					headok=1;	//标记找到jpg头(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//找到头以后,再找FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			if(jpglen)	//正常的jpeg数据 
			{
				p+=jpgstart;			//偏移到0XFF,0XD8处 
				for(i=0;i<jpglen;i++)	//发送整个jpg文件
				{
					USART2->TDR=p[i];  
					while((USART2->ISR&0X40)==0);	//循环发送,直到发送完毕   
					key=KEY_Scan(0); 
					if(key)break;
				}  
			}
			if(key)	//有按键按下,需要处理
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//提示退出数据传输
				switch(key)
				{				    
					case KEY0_PRES:	//对比度设置
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
						break; 
					case KEY1_PRES:	//执行一次自动对焦
						OV5640_Focus_Single();
						break;
					case KEY2_PRES:	//特效设置				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//设置特效
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG输出尺寸设置   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//提示传输结束设置 
			jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
		}		
	}    
}

//RGB屏数据接收回调函数
void rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA2_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
	{ 
		pbuf=(u16*)dcmi_line_buf[0]; 
	}else 						//DMA使用buf0,读取buf1
	{
		pbuf=(u16*)dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(0,curline,lcddev.width-1,curline,pbuf);//DM2D填充 
	if(curline<lcddev.height)curline++;
}

//RGB数据直接显示在LCD上面
void OV5640_RGB565_test(void)
{
	u8 key;
	u8 effect=0,contrast=2,fac;
	u8 scale=1;		//默认是全尺寸缩放
	u8 msgbuf[15];	//消息缓存区 
	u16 outputheight=0;
	
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode"); 
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//执行自动对焦
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:FullSize/Scale");	//1:1尺寸(显示真实尺寸)/全尺寸缩放
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
	TIM3->CR1&=~(0x01);		//关闭定时器3,关闭帧率统计，打开的话，RGB屏，在串口打印的时候，会抖
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
	OV5640_OutSize_Set(4,0,lcddev.width,outputheight);		//满屏缩放显示
	DCMI_Start(); 			//启动传输
	LCD_Clear(BLACK);
	while(1)
	{ 
		key=KEY_Scan(0); 
		if(key)
		{ 
			if(key!=KEY1_PRES)DCMI_Stop(); //非KEY1按下,停止显示
			switch(key)
			{				    
				case KEY0_PRES:	//对比度设置
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					break;
				case KEY1_PRES:	//执行一次自动对焦
					OV5640_Focus_Single();
					break;
				case KEY2_PRES:	//特效设置				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//设置特效
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					break;
				case WKUP_PRES:	//1:1尺寸(显示真实尺寸)/缩放	    
					scale=!scale;  
					if(scale==0)
					{
						fac=800/outputheight;	//得到比例因子
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,outputheight);
 						sprintf((char*)msgbuf,"Scale");
					}
					break;
			}
			if(key!=KEY1_PRES)	//非KEY1按下
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 
				DCMI_Start();	//重新开始传输
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	usart2_init(54,921600);		//初始化串口2波特率为921600
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
	TIM3_Int_Init(10000-1,10800-1);//10Khz计数,1秒钟中断一次
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"OV5640 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/20");	 		
	while(OV5640_Init())//初始化OV5640
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
		if(key==KEY0_PRES){ovx_mode=0;break;}		//RGB565模式
	    else if(key==KEY1_PRES){ovx_mode=1;break;}	//JPEG模式 
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //闪烁显示提示信息
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FLASH EEPROM TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/15");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)	//KEY1按下,写入STM32 FLASH
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write FLASH....");
			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer1,SIZE1);
			LCD_ShowString(30,170,200,16,16,"FLASH Write Finished!");//提示传送完成
		}
		if(key==KEY0_PRES)	//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read FLASH.... ");
			STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,SIZE1);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);//显示读到的字符串
		}
		i++;
		delay_ms(10);  
		if(i==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
 	NRF24L01_Init();    		//初始化NRF24L01 
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
			LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息

 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}

		delay_ms(5);	  
	} 
	
 	LCD_Fill(10,150,240,166,WHITE);//清空上面的显示		  
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
 	
	if(mode==0)//RX模式
	{
		LCD_ShowString(30,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(30,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  
		while(1)
		{	  		    		    				 
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{
				tmp_buf[32]=0;//加入字符串结束符
				LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf);    
			}
			else 
				delay_us(100);	   

			t++;
			
			if(t==10000)//大约1s钟改变一次状态
			{
				t=0;
				LED0(led0sta^=1);
			} 				    
		};	
	}
	else//TX模式
	{							    
		LCD_ShowString(30,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		mode=' ';//从空格键开始  
		
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
				
				tmp_buf[32]=0;//加入结束符		   
			}
			else
			{										   	
 				LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//清空显示			   
				LCD_ShowString(30,170,lcddev.width-1,32,16,"Send Failed "); 
			}
			
			LED0(led0sta^=1);
			delay_ms(1500);				    
		};
	}     

}

//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{
	while((USART1->ISR&0X40)==0);	//循环发送,直到发送完毕   
    USART1->TDR=c;  
} 

//传送数据给匿名四轴地面站(V4版本)
//fun:功能字. 0X01~0X1C
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0XAA;	//帧头
	send_buf[1]=0XAA;	//帧头
	send_buf[2]=fun;	//功能字
	send_buf[3]=len;	//数据长度
	for(i=0;i<len;i++)
		send_buf[4+i]=data[i];			//复制数据
	for(i=0;i<len+4;i++)
		send_buf[len+4]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+5;i++)
		usart1_send_char(send_buf[i]);	//发送数据到串口1 
}

//发送加速度传感器数据+陀螺仪数据(传感器帧)
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值 
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
	tbuf[12]=0;//因为开启MPL后,无法直接读取磁力计数据,所以这里直接屏蔽掉.用0替代.
	tbuf[13]=0;
	tbuf[14]=0;
	tbuf[15]=0;
	tbuf[16]=0;
	tbuf[17]=0;
	usart1_niming_report(0X02,tbuf,18);//传感器帧,0X02
}

//通过串口1上报结算后的姿态数据给电脑(状态帧)
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
//csb:超声波高度,单位:cm
//prs:气压计高度,单位:mm
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
	usart1_niming_report(0X01,tbuf,12);//状态帧,0X01
}   

void MPU9250_test(void)
{
	u8 led0sta=1;
	u8 t=0,report=0;	    	//默认开启上报
	u8 key;
	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据 
	short temp;		        	//温度
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,500000);		//初始化串口波特率为500000
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init();					//初始化按键
    MPU_Init();             	//初始化MPU9250
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
		LED0(led0sta^=1);//DS0闪烁 
    }
    
    LCD_ShowString(30,130,200,16,16,"MPU9250 OK");
	LCD_ShowString(30,150,200,16,16,"KEY0:UPLOAD ON/OFF");
    POINT_COLOR=BLUE;     //设置字体为蓝色
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
            temp=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据

  			if(report)mpu9250_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//发送加速度+陀螺仪原始数据
			if(report)usart1_report_imu((int)(roll*100),(int)(pitch*100),(int)(yaw*100),0,0);

			if((t%10)==0)
			{ 
				if(temp<0)
				{
					LCD_ShowChar(30+48,200,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,200,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,200,temp/100,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,200,temp%10,1,16);		//显示小数部分 
				temp=pitch*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,220,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,220,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,220,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,220,temp%10,1,16);		//显示小数部分 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,240,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,240,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,240,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,240,temp%10,1,16);		//显示小数部分 
				temp=yaw*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,260,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,260,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,260,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,260,temp%10,1,16);		//显示小数部分  
				t=0;
				LED0(led0sta^=1);//DS0闪烁 
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	PCF8574_Init();				//初始化PCF8574
	PCF8574_ReadBit(EX_IO);		//读一次,释放INT脚,防止干扰单总线 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DHT11 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
 	while(DHT11_Init())	//DHT11初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	while(1)
	{	    	    
 		if(t%10==0)//每100ms读取一次
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值					    
			LCD_ShowNum(30+40,150,temperature,2,16);		//显示温度	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);			//显示湿度	 	   
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	PCF8574_Init();				//初始化PCF8574
	PCF8574_ReadBit(EX_IO);		//读一次,释放INT脚,防止干扰单总线 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DS18B20 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
 	while(DS18B20_Init())	//DS18B20初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}   
	LCD_ShowString(30,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,150,200,16,16,"Temp:   . C");	 
	while(1)
	{	    	    
 		if(t%10==0)//每100ms读取一次
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(30+40,150,'-',16,0);			//显示负号
				temperature=-temperature;					//转为正数
			}else LCD_ShowChar(30+40,150,' ',16,0);			//去掉负号
			LCD_ShowNum(30+40+8,150,temperature/10,2,16);	//显示正数部分	    
   			LCD_ShowNum(30+40+32,150,temperature%10,1,16);	//显示小数部分 		   
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
 	Remote_Init();				//红外接收初始化		
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"REMOTE TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"KEYVAL:");	
   	LCD_ShowString(30,150,200,16,16,"KEYCNT:");	
   	LCD_ShowString(30,170,200,16,16,"SYMBOL:");	    
 	POINT_COLOR=BLUE;//设置字体为蓝色  														 	  		    							  
	while(1)
	{
		key=Remote_Scan();	
		if(key)
		{	 
			LCD_ShowNum(86,130,key,3,16);		//显示键值
			LCD_ShowNum(86,150,RmtCnt,3,16);	//显示按键次数		  
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
			LCD_Fill(86,170,116+8*8,170+16,WHITE);	//清楚之前的显示
			LCD_ShowString(86,170,200,16,16,str);	//显示SYMBOL
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200  
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化 
	tp_dev.init();				//触摸屏初始化 
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"TOUCH TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		

   	if(tp_dev.touchtype!=0XFF)
   		LCD_ShowString(30,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示

	delay_ms(1500);
 	Load_Drow_Dialog();	 	
	
	if(tp_dev.touchtype&0X80)
		ctp_test();//电容屏测试
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
	u8 mode=1;	//CAN工作模式;0,普通模式;1,环回模式
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化 
 	CAN1_Mode_Init(1,7,10,6,1);	//CAN初始化,波特率500Kbps    
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"LoopBack Mode");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");//显示提示信息		
  	POINT_COLOR=BLUE;//设置字体为蓝色	  
	LCD_ShowString(30,170,200,16,16,"Count:");			//显示当前计数值	
	LCD_ShowString(30,190,200,16,16,"Send Data:");		//提示发送的数据	
	LCD_ShowString(30,250,200,16,16,"Receive Data:");	//提示接收到的数据		
 	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下,发送一次数据
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;//填充发送缓冲区
				if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//显示数据
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//显示数据
 			}
			res=CAN1_Send_Msg(canbuf,8);//发送8个字节 
			if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");		//提示发送失败
			else LCD_ShowString(30+80,190,200,16,16,"OK    ");	 		//提示发送成功								   
		}else if(key==WKUP_PRES)//WK_UP按下，改变CAN的工作模式
		{	   
			mode=!mode;
			CAN1_Mode_Init(1,7,10,6,mode);	//CAN普通模式初始化,普通模式,波特率500Kbps
  			POINT_COLOR=RED;//设置字体为红色 
			if(mode==0)//普通模式，需要2个开发板
			{
				LCD_ShowString(30,130,200,16,16,"Nnormal Mode ");	    
			}else //回环模式,一个开发板就可以测试了.
			{
 				LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;//设置字体为蓝色 
		}		 
		key=CAN1_Receive_Msg(canbuf);
		if(key)//接收到有数据
		{			
			LCD_Fill(30,270,160,310,WHITE);//清除之前的显示
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//显示数据
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//显示数据
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0X80);	//显示数据
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化 
	RS485_Init(54,9600);		//初始化RS485
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RS485 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	LCD_ShowString(30,130,200,16,16,"KEY0:Send");	//显示提示信息		
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
	LCD_ShowString(30,150,200,16,16,"Count:");			//显示当前计数值	
	LCD_ShowString(30,170,200,16,16,"Send Data:");		//提示发送的数据	
	LCD_ShowString(30,210,200,16,16,"Receive Data:");	//提示接收到的数据	 							  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下,发送一次数据
		{
			for(i=0;i<5;i++)
			{
				rs485buf[i]=cnt+i;//填充发送缓冲区
				LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//显示数据
 			}
			RS485_Send_Data(rs485buf,5);//发送5个字节 									   
		}		 
		RS485_Receive_Data(rs485buf,&key);
		
		if(key)//接收到有数据
		{
			if(key>5)key=5;//最大是5个数据.
 			for(i=0;i<key;i++)LCD_ShowxNum(30+i*32,230,rs485buf[i],3,16,0X80);	//显示数据
 		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,150,cnt,3,16,0X80);	//显示数据
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化 
	W25QXX_Init();				//W25QXX初始化
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"QSPI TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/7/18");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
	while(W25QXX_ReadID()!=W25Q256)								//检测不到W25Q256
	{
		LCD_ShowString(30,150,200,16,16,"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q256 Ready!"); 
	FLASH_SIZE=32*1024*1024;	//FLASH 大小为32M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q256
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);					//显示读到的字符串
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);	//DS0闪烁
			i=0;
		}		   
	}	  	
}

void AP3216C_test(void)
{	
	u8 led0sta=1;
 	u16 ir,als,ps;			
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"AP3216C TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/6/2");	 		
	while(AP3216C_Init())		//检测不到AP3216C
	{
		LCD_ShowString(30,130,200,16,16,"AP3216C Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,130,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0闪烁
	}
	LCD_ShowString(30,130,200,16,16,"AP3216C Ready!");  
    LCD_ShowString(30,160,200,16,16," IR:");	 
 	LCD_ShowString(30,180,200,16,16," PS:");	
 	LCD_ShowString(30,200,200,16,16,"ALS:");	 
 	POINT_COLOR=BLUE;			//设置字体为蓝色		  
    while(1)
    {
        AP3216C_ReadData(&ir,&ps,&als);	//读取数据 
        LCD_ShowNum(30+32,160,ir,5,16);	//显示IR数据
        LCD_ShowNum(30+32,180,ps,5,16);	//显示PS数据
        LCD_ShowNum(30+32,200,als,5,16);//显示ALS数据 
		LED0(led0sta^=1);				//提示系统正在运行	
        delay_ms(120); 
	}	  
}

void PCF8574_test(void)
{
 	u8 led0sta=1,led1sta=1;
	u8 key;
	u16 i=0; 
	u8 beepsta=1;				
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
 	KEY_Init(); 				//按键初始化	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"PCF8574 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/29");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:BEEP ON/OFF");	//显示提示信息	
	LCD_ShowString(30,150,200,16,16,"EXIO:DS1 ON/OFF");		//显示提示信息		
	while(PCF8574_Init())		//检测不到PCF8574
	{
		LCD_ShowString(30,170,200,16,16,"PCF8574 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,170,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0闪烁
	}
	LCD_ShowString(30,170,200,16,16,"PCF8574 Ready!");    
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0); 
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{ 
			beepsta=!beepsta;					//蜂鸣器状态取反
			PCF8574_WriteBit(BEEP_IO,beepsta);	//控制蜂鸣器
		}
		if(PCF8574_INT==0)				//PCF8574的中断低电平有效
		{
			key=PCF8574_ReadBit(EX_IO);	//读取EXIO状态,同时清除PCF8574的中断输出(INT恢复高电平)
			if(key==0)LED1(led1sta^=1); //LED1状态取反 
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//DS0闪烁
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
 	KEY_Init(); 				//按键初始化	
	AT24CXX_Init();				//IIC初始化 
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"IIC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/29");	 
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
 	while(AT24CXX_Check())//检测不到24c02
	{
		LCD_ShowString(30,150,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"24C02 Ready!");    
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入24C02
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write 24C02....");
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
			LCD_ShowString(30,170,200,16,16,"24C02 Write Finished!");//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read 24C02.... ");
			AT24CXX_Read(0,datatemp,SIZE);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);//显示读到的字符串
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//DS0闪烁
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
	float pro=0;				//进度
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	KEY_Init(); 				//按键初始化	
 	MYDMA_Config(DMA2_Stream7,4,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	//显示提示信息	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//填充ASCII字符集数据
    {
		if(t>=j)//加入换行符
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
		else//复制TEXT_TO_SEND语句
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0按下
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//显示百分号
		    USART1->CR3=1<<7;           //使能串口1的DMA发送       
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);//开始一次DMA传输！	  
		    //等待DMA传输完成，此时我们来做另外一些事，点灯
		    //实际应用中，传输数据期间，可以执行另外的任务
		    while(1)
		    {
				if(DMA2->HISR&(1<<27))	//等待DMA2_Steam7传输完成
				{
					DMA2->HIFCR|=1<<27;	//清除DMA2_Steam7传输完成标志
					break; 
		        }
				pro=DMA2_Stream7->NDTR;	//得到当前还剩余多少个数据
				pro=1-pro/SEND_BUF_SIZE;//得到百分比	  
				pro*=100;      			//扩大100倍
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//显示100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//提示传送完成
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC 
	KEY_Init(); 				//按键初始化	
	TIM9_CH2_PWM_Init(255,1);	//TIM9 PWM初始化, Fpwm=108M/256=421.9Khz.
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	PWM_DAC_VAL=pwmval;//初始值为0	    	      
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
			PWM_DAC_VAL=pwmval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			PWM_DAC_VAL=pwmval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=PWM_DAC_VAL;
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_CH5,20);  		//得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//显示电压值的小数部分
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC
	KEY_Init(); 				//按键初始化
	Dac1_Init();		 		//DAC通道1初始化	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	DAC->DHR12R1=dacval;		//初始值为0	    		
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(dacval<4000)dacval+=200;
			DAC->DHR12R1=dacval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(dacval>200)dacval-=200;
			else dacval=0;
			DAC->DHR12R1=dacval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=DAC->DHR12R1;
			LCD_ShowxNum(94,150,adcx,4,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/4096);			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0X80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_CH5,20);		//得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0X80); 	//显示电压值的小数部分
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,140,200,16,16,"TEMPERATE: 00.00C");	      
	while(1)
	{
		temp=Get_Temprate();	//得到温度值 
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,140,16,16,16,"-");	//显示负号
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	//无符号
		
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0X80);	//显示小数部分 		 
		LED0(led0sta^=1);
		delay_ms(250);	
	}

}

void ADC_test(void)
{
 	u8 led0sta=1;
 	u16 adcx;
	float temp;  
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/28");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	      
	while(1)
	{
		adcx=Get_Adc_Average(ADC_CH5,20);
		LCD_ShowxNum(134,130,adcx,4,16,0);//显示ADC的值
		temp=(float)adcx*(3.3/4096); 
		adcx=temp;
		LCD_ShowxNum(134,150,adcx,1,16,0);//显示电压值
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
 	WKUP_Init();				//待机唤醒初始化
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	KEY_Init();					//按键初始化
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RNG TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-SAGACITY");
	LCD_ShowString(30,110,200,16,16,"2016/7/13");	 
	while(RNG_Init())	 		//初始化随机数发生器
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
			LED0(led0sta^=1);						//每200ms,翻转一次LED0 
			random=RNG_Get_RandomRange(0,9);		//获取[0,9]区间的随机数
			LCD_ShowNum(30+8*16,210,random,1,16); 	//显示随机数
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	RTC_Init();		 			//初始化RTC
	RTC_Set_WakeUp(4,0);		//配置WAKE UP中断,1秒钟中断一次 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-SAGACITY");
	LCD_ShowString(30,110,200,16,16,"2016/7/13");	
  	while(1) 
	{		
		t++;
		if((t%10)==0)	//每100ms更新一次显示数据
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
			LED0(led0sta^=1);//每200ms,翻转一次LED0 
		delay_ms(10);
	}	 
}
void RGBLCD_test(void)
{
	u8 x=0,led0sta=1;
	u8 lcd_id[12];	
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	KEY_Init();					//初始化LED
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。				 	
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
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2018/2/23");	      					 
	    x++;
		if(x==12)
			x=0;
		LED0(led0sta^=1);	//LED0闪烁	 
		delay_ms(1000);	
	} 
}

//SDRAM内存测试	    
void fmc_sdram_scan(u16 x,u16 y)
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	

	//在地址0读到的数据	  				   
  	LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:    0KB");
  	
	//每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节
	for(i = 0X1F4000; i < 32*1024*1024; i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
	
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
  		
		if(i==0)
			sval=temp;
 		else if
 			(temp<=sval)
 				break;//后面读出的数据一定要比第一次读到的数据大.	
 				
 		//显示内存容量  
		LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*16,5,16,0);
		//打印SDRAM容量
		printf("SDRAM Capacity:%dKB\r\n",(u16)(temp-sval+1)*16);
 	}					 
}	
void SDRAM_test(void)
{
	u8 key;		 
 	u8 i=0,led0sta=1;	     
	u32 ts=0; 
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart1_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	KEY_Init();					//初始化LED
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD
 	POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"APOLLO STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"SDRAM TEST");	
	LCD_ShowString(30,90,200,16,16,"LC-LEICHUAN");
	LCD_ShowString(30,110,200,16,16,"2018/5/23");   
	LCD_ShowString(30,130,200,16,16,"KEY0:Test Sram");
	LCD_ShowString(30,150,200,16,16,"KEY1:TEST Data");
 	POINT_COLOR=BLUE;//设置字体为蓝色  
	for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//预存测试数据	 
  	}
	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		
		if(key==KEY0_PRES)
		{
			fmc_sdram_scan(40,170);//测试SDRAM容量
		}
		else if(key==KEY1_PRES)//打印预存测试数据
		{
			for(ts=0;ts<250000;ts++)
			{
				LCD_ShowxNum(40,190,testsram[ts],6,16,0);//显示测试数据	
				printf("testsram[%d]:%d\r\n",ts,testsram[ts]);
			}
		}else 

		delay_ms(10);   
		i++;
		
		if(i==20)//DS0闪烁.
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
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化  
	uart1_init(108,115200);	//初始化串口波特率为115200
	LED_Init();		  		//初始化与LED连接的硬件接口
	MPU_Memory_Protection();//保护相关存储区域
	LCD_Init();				//初始化LCD
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。				 	
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
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2018/5/22");	      					 
	    x++;
	    
		if(x==12)
			x=0;
			
		LED0(led0sta^=1);	//LED0闪烁
		
		delay_ms(1000);	
	} 	
}

/*
利用 STM32F7 自带的 MPU 功能，对一个特定的内存空间（数组，地址：0X20002000）
进行写访问保护。
开机时，串口调试助手显示：MPU closed，表示默认是没有写保护的。
按 KEY0 可以往数组里面写数据，按 KEY1，可以读取数组里面的数据。
按KEY_UP则开启MPU保护，此时，
如果再按KEY0往数组写数据，就会引起MemManage错误，
进入 MemManage_Handler 中断服务函数，此时 DS1 点亮，
同时打印错误信息，最后软件复位，系统重启。
DS0 用于提示程序正在运行，
所有信息都是通过串口 1 输出(115200)，
*/
void MPU_test(void)
{
	u8 i=0,led0sta=1;
	u8 key;
	Stm32_Clock_Init(432,25,2,9);		//设置时钟,216Mhz
    delay_init(216);					//延时初始化  
	uart1_init(108,115200);				//初始化串口波特率为115200
	LED_Init();		  					//初始化与LED连接的硬件接口  
	KEY_Init();		  					//初始化按键
	printf("\r\n\r\nMPU closed!\r\n");	//提示MPU关闭
	while(1)  
	{
		key=KEY_Scan(0);
        if(key==WKUP_PRES)              //使能MPU保护数组 mpudata;
        {
            MPU_Set_Protection(0X20002000,128,0,MPU_REGION_PRIV_RO_URO,0,0,1);//只读,禁止共用,禁止catch,允许缓冲 
            printf("MPU open!\r\n");	//提示MPU打开
        }else if(key==KEY0_PRES)        //向数组中写入数据，如果开启了MPU保护的话会进入内存访问错误！
        {
			printf("Start Writing data...\r\n");
			sprintf((char*)mpudata,"MPU test array %d",i);
			printf("Data Write finshed!\r\n");
        }else if(key==KEY1_PRES)		//从数组中读取数据，不管有没有开启MPU保护都不会进入内存访问错误！
        {
            printf("Array data is:%s\r\n",mpudata);
        }else delay_ms(10);
		i++;
		if((i%50)==0) LED0(led0sta^=1);	//LED0取反 
	}
}
void OLED_test(void)
{
	u8 t=0,led0sta=1;
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化  
	uart1_init(108,115200);	//初始化串口波特率为115200
	LED_Init();		  		//初始化与LED连接的硬件接口
	OLED_Init();			//初始化OLED
  	OLED_ShowString(0,0,"LEICHUAN",24);  
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
 	OLED_ShowString(0,40,"lc_sagacity 2018/5/22",12);  
 	OLED_ShowString(0,52,"ASCII:",12);  
 	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram();//更新显示到OLED	 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(36,52,t,12,1);//显示ASCII字符	
		OLED_ShowNum(94,52,t,3,12);	//显示ASCII字符的码值    
		OLED_Refresh_Gram();//更新显示到OLED
		
		t++;
		if(t>'~')
			t=' '; 
		
		delay_ms(500);
		LED0(led0sta^=1);	//LED0取反
	}

}

/*需要用跳线帽短接多功能端口（P11）的 TPAD 和 ADC，以实现 TPAD 连接到 PA5。*/
void Timer_TIM2_TPAD_test(void)
{
	u8 t=0,led0sta=1,led1sta=1;
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化  
	uart1_init(108,115200);	//初始化串口波特率为115200
	LED_Init();		  		//初始化与LED连接的硬件接口
 	TPAD_Init(2);			//初始化触摸按键,以108/2=54Mhz频率计数
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0))	//成功捕获到了一次上升沿(此函数执行时间至少15ms)
		{
			LED1(led1sta^=1);	//LED1取反
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0(led0sta^=1);	//LED0取反,提示程序运行
		}
		delay_ms(10);
	}
}

/*
通过 KEY_UP 按键输入高电平，并从串口打印高电平脉宽。
同时我们保留 PWM 输出，
可以通过用杜邦线连接 PB1 和 PA0，
来测量 PWM 输出的高电平脉宽。
*/
void Timer_TIM5_Cap_input_test(void)
{
	long long temp=0;  
	Stm32_Clock_Init(432,25,2,9);	//设置时钟,216Mhz
    delay_init(216);				//延时初始化  
	uart1_init(108,115200);			//初始化串口波特率为115200
	LED_Init();		  				//初始化与LED连接的硬件接口
 	TIM3_PWM_Init(500-1,108-1);		//1Mhz的计数频率,2Khz的PWM.     
	TIM5_CH1_Cap_Init(0XFFFFFFFF,108-1);//以1Mhz的频率计数 
   	while(1)
	{
 		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)
			LED0_PWM_VAL=0;	 		 
 		if(TIM5CH1_CAPTURE_STA&0X80)		//成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		//溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;		//得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp);//打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;			//开启下一次捕获
		}
	}

}

void Timer_TIM3_PWM_test(void)
{
	u16 led0pwmval=0;    
	u8 dir=1; 
	Stm32_Clock_Init(432,25,2,9);	//设置时钟,216Mhz
    delay_init(216);				//延时初始化  
	LED_Init();		  				//初始化与LED连接的硬件接口
 	TIM3_PWM_Init(500-1,108-1);		//1Mhz的计数频率,2Khz的PWM.     
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

/*通过 TIM3 的中断来控制 DS1 的亮灭*/
void Timer_TIM3_test(void)
{
	u8 led0sta=1;
	Stm32_Clock_Init(432,25,2,9);	//设置时钟,216Mhz     
    delay_init(216);				//延时初始化  
	LED_Init();		  				//初始化与LED连接的硬件接口
 	TIM3_Int_Init(5000-1,10800-1);	//10Khz的计数频率，计数5K次为500ms     
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(200);
	};
}

/*通过 DS0 来指示 STM32F767 是否被复位了，
如果被复位了就会点亮 300ms。
DS1 用来指示中断喂狗，
每次中断喂狗翻转一次。*/
void Wwatch_dog_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化  
	LED_Init();		  		//初始化与LED连接的硬件接口
 	LED0(0);				//点亮LED0
	delay_ms(300);			//延时300ms再初始化看门狗,LED0的变化"可见"
	WWDG_Init(0X7F,0X5F,3);	//计数器值为7f,窗口寄存器为5f,分频数为8	   
	while(1)
	{
		LED0(1);			//关闭LED0
	};

}

/*我们将通过按键 KEY_UP 来喂狗，然后通过 DS0 提示复位状态。*/
void Iwatch_dog_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化  
	LED_Init();		  		//初始化与LED连接的硬件接口   
	KEY_Init();		  		//初始化按键
 	delay_ms(100);			//延时100ms再初始化看门狗,LED0的变化"可见"
	IWDG_Init(4,500);    	//预分频数为64,重载值为500,溢出时间为1s	   
	LED0(0);			 	//点亮LED0
	while(1)
	{ 
		if(KEY_Scan(0)==WKUP_PRES)//如果WK_UP按下,则喂狗
		{
			IWDG_Feed();//喂狗
		}
		delay_ms(10);
	};
}

void ext_interrupt_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化 
	uart1_init(108,115200);	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口   
	EXTIX_Init();         	//初始化外部中断输入 
	LED0(0);				//先点亮红灯
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
	u8 led0sta=1;			  //LED0前状态
	Stm32_Clock_Init(432,25,2,9);  //设置时钟,216Mhz
    delay_init(216);		  //延时初始化 
	uart1_init(108,115200);	  //串口初始化为115200
	LED_Init();		  		  //初始化与LED连接的硬件接口   
	while(1)
	{
		if(USART1_RX_STA&0x8000)
		{					   
			len=USART1_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->TDR=USART1_RX_BUF[t];
				while((USART1->ISR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART1_RX_STA=0; 
		}
		else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nlc_sagaciy 串口实验\r\n");
				printf("lc_sagacity@leichuan\r\n\r\n\r\n");
			}
			if(times%200==0)
				printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)
				LED0(led0sta^=1);//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}
}

void key_test(void)
{
	u8 key;
	u8 led0sta=1,led1sta=1;		//LED0,LED1的当前状态
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化
	LED_Init();					//初始化LED时钟  
	KEY_Init();         		//初始化与按键连接的硬件接口
	LED0(0);					//先点亮红灯  
	while(1)
	{
		key=KEY_Scan(0); 		//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制LED0,LED1互斥点亮
					led1sta=!led1sta;
					led0sta=!led1sta;
					break;
				case KEY2_PRES:	//控制LED0翻转
	 				led0sta=!led0sta;
					break;
				case KEY1_PRES:	//控制LED1翻转	 
					led1sta=!led1sta;
					break;
				case KEY0_PRES:	//同时控制LED0,LED1翻转 
					led0sta=!led0sta;
					led1sta=!led1sta;
					break;
			}
			LED0(led0sta);		//控制LED0状态
			LED1(led1sta);		//控制LED1状态
		}
		else 
			delay_ms(10); 
	}

}

void led_test(void)
{
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);		//延时初始化 
	LED_Init();				//初始化LED时钟  
	while(1)
	{
		LED0(0);				//DS0亮
		LED1(1);				//DS1灭
		delay_ms(500);
		LED0(1);				//DS0灭
		LED1(0);				//DS1亮
		delay_ms(500);
	}

}




