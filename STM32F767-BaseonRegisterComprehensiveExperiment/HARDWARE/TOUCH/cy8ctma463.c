#include "cy8ctma463.h"
#include "touch.h"
#include "ctiic.h"
#include "lcd.h"
#include "usart.h"
#include "delay.h" 
#include "string.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//8寸电容触摸屏-CY8CTMA463 驱动代码(支持10点触摸)	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/9/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved		 
////////////////////////////////////////////////////////////////////////////////// 


//向CY8CTMA463写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 CY8CTMA_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	 
	CT_IIC_Send_Byte(CY_CMD_WR);	//发送写命令 	 
	ret = CT_IIC_Wait_Ack(); 
	CT_IIC_Send_Byte(reg&0XFF);   	//发送8位地址
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//发数据
		ret=CT_IIC_Wait_Ack(); 
		if(ret)break;  
	}
    CT_IIC_Stop();					//产生一个停止条件	    
	return ret; 
} 
//从CY8CTMA463读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void CY8CTMA_RD_Reg(u16 reg,u8 *buf,u8 len)
{   
	u8 i;

 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(CY_CMD_WR);   	//发送写命令 	 	
    CT_IIC_Wait_Ack();	
 	CT_IIC_Send_Byte(reg&0XFF);   	//发送8位地址
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(CY_CMD_RD);   	//发送读命令		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    CT_IIC_Stop();//产生一个停止条件  
	
}

//CY8CTMA463 退出Bootload命令
const u8 CY8_Exit_Bootload[]={0xFF,0x01,0x3B,0x00,0x00,0x4F,0x6D,0x17};

//初始化CY8CTMA463触摸屏
//返回值:0,初始化成功;1,初始化失败 
u8 CY8CTMA_Init(void)
{   

	u16 FW;		//固件版本号
	u16 BL;		//Bootload版本号
	u16 CYID;	//触摸版本号
	u8 Mode;
	u8 ret;
	u8 i;
	u8 temp[2];  
	RCC->AHB1ENR|=1<<7;    	//使能PORTH时钟 
	RCC->AHB1ENR|=1<<8;    	//使能PORTI时钟  
	GPIO_Set(GPIOH,PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//PH7设置为上拉输入
	GPIO_Set(GPIOI,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PI8设置为推挽输出
	CT_IIC_Init();      	//初始化电容屏的I2C总线   
	CY_RST(0);				//复位
	delay_ms(10);
 	CY_RST(1);				//释放复位		    
	delay_ms(10);
	temp[0]=0;
    CY8CTMA_RD_Reg(CY_HST_MODE,&temp[0],1);//读取HST_MODE
	Mode=temp[0];
	if(Mode==0x00)//当前为操作模式
	{
		 temp[0]=0x18;
		 ret=CY8CTMA_WR_Reg(CY_HST_MODE,&temp[0],1);	//进入系统信息模式
		 if(!ret){printf("进入系统信息模式成功..\r\n");}
		 else printf("进入系统信息模式失败...\r\n");		 
		 CY8CTMA_RD_Reg(CY_HST_MODE,&Mode,1);			//重新读一次当前模式
		 delay_ms(100);
	}
	if(Mode!=0x00)			//当前为非操作模式
	{
		 i=5;
		 while(i--)
		 {
			 ret=CY8CTMA_WR_Reg(CY_HST_MODE,(u8*)CY8_Exit_Bootload,sizeof(CY8_Exit_Bootload));//退出bootload模式
			 if(!ret){printf("退出bootloader成功...\r\n");break;}
			 else printf("退出bootloader失败...\r\n");
			 delay_ms(100);
		 }
		 delay_ms(500);		//这里必须加延时,退出bootload时需要点时间 		
		 CY8CTMA_RD_Reg(CY_FW_VER,&temp[0],2);  
		 FW = ((u16)temp[0]<<8) + temp[1];printf("FW_VER: 0x%X\r\n",FW);	//读取固件版本号
		 CY8CTMA_RD_Reg(CY_BL_VER,temp,2);
		 BL = ((u16)temp[0]<<8) + temp[1];printf("BL_VER: 0x%X\r\n",BL);	//读取bootload版本号
		 CY8CTMA_RD_Reg(CY_ID,temp,2);
		 CYID = ((u16)temp[0]<<8) + temp[1];printf("CP_ID: 0x%x\r\n",CYID);	//读取CY ID
		 i=5;
		 while(i--)
		 {
			temp[0]=0x08;
			ret = CY8CTMA_WR_Reg(CY_HST_MODE,&temp[0],1);//进入操作模式
			if(!ret) { printf("进入操作模式成功..\r\n"); break;}
			else      printf("进入操作模式失败...\r\n");
			delay_ms(100);
		 }
		delay_ms(100);//这里必须加延时,运行命令需要时间  
		temp[0]=0;
		CY8CTMA_RD_Reg(CY_HST_MODE,&temp[0],1);	//重新读一次当前模式
		if(temp[0]==0x00) return 0;				//初始化成功 
	}
	return 1;	
} 
//存储触摸数据
cyttsp4_touch cy_tch[CT_MAX_TOUCH];
//扫描触摸屏(采用查询模式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 CY8CTMA_Scan(u8 mode)
{
	u8 i=0;
	u8 res=0;
	u16 temp;
	u16 tempsta;
 	static u8 t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)
	{
		CY8CTMA_RD_Reg(CY_TT_STAT,&mode,1);	//读取触摸点的状态
		if((mode&0xF)&&((mode&0x1F)<11))	//读取触摸点的状态  
		{
			temp=0XFFFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta=tp_dev.sta;				//保存当前的tp_dev.sta值
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES;
			tp_dev.x[CT_MAX_TOUCH-1]=tp_dev.x[0];	//保存触点0的数据
			tp_dev.y[CT_MAX_TOUCH-1]=tp_dev.y[0];
			CY8CTMA_RD_Reg(CY_TP_REG,(u8*)&cy_tch,sizeof(cyttsp4_touch)*mode);//读取触摸数据
			for(i=0;i<CT_MAX_TOUCH;i++)
			{   
				if(tp_dev.sta&(1<<i))		//触摸有效?
				{
					if(tp_dev.touchtype&0X01)//横屏
					{
						tp_dev.x[i]=((u16)(cy_tch[i].xh&0x3f)<<8)+cy_tch[i].xl;
						tp_dev.y[i]=((u16)(cy_tch[i].yh&0x3f)<<8)+cy_tch[i].yl;
						if(tp_dev.x[i]>1023)tp_dev.x[i]=1023; 
					}else
					{
						tp_dev.y[i]=((u16)(cy_tch[i].xh&0x3f)<<8)+cy_tch[i].xl;
						tp_dev.x[i]=600-(((u16)(cy_tch[i].yh&0x3f)<<8)+cy_tch[i].yl);
						if(tp_dev.x[i]>599)tp_dev.x[i]=599;
						if(tp_dev.y[i]>1023)tp_dev.y[i]=1023;
					}
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
			    }					
			}
			res=1;
			if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//非法数据(坐标超出了)
			{ 
				if((mode&0X1F)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}else					//非法数据,则忽略此次数据(还原原来的)  
				{
					tp_dev.x[0]=tp_dev.x[CT_MAX_TOUCH-1];
					tp_dev.y[0]=tp_dev.y[CT_MAX_TOUCH-1];
					mode=0X80;		
					tp_dev.sta=tempsta;	//恢复tp_dev.sta
				}
			}else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率 
		} 
	}
	if((mode&0X1F)==0X00)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)		//之前是被按下的
		{
			tp_dev.sta&=~TP_PRES_DOWN;	//标记按键松开
		}else							//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=(TP_PRES_DOWN|TP_CATH_PRES);//清除点有效标记	
		}	 
	} 	
	if(t>240)t=10;//重新从10开始计数
	return res; 
}
		

























								 
