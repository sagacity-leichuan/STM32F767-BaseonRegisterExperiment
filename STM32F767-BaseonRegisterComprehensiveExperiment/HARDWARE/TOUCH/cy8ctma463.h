#ifndef __CY8CTMA463_H
#define __CY8CTMA463_H
#include "sys.h"	
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

//IO操作函数
#define CY_RST(x)		GPIO_Pin_Set(GPIOI,PIN8,x)		//CY8CTMA463复位引脚
#define CY_INT			GPIO_Pin_Get(GPIOH,PIN7)  		//CY8CTMA463中断引脚

//I2C读写命令	
#define CY_CMD_WR		0x48	//写命令
#define CY_CMD_RD		0x49 	//读命令
//CY8CTMA463 部分寄存器定义
#define CY_HST_MODE		0x00	//主机模式寄存器
//操作模式下寄存器
#define CY_TT_STAT		0X0B	//TrueTouch触摸状态寄存器
#define CY_TP_REG		0X0C	//首个触摸点数据地址
//系统信息模式下寄存器
#define CY_FW_VER		0X12	//固件版本寄存器
#define CY_BL_VER		0x1C	//BL版本寄存器
#define CY_ID			0x31	//触摸ID寄存器

//管理TureTouch触摸数据
typedef struct 
{
	u8 xh;
	u8 xl;
	u8 yh;
	u8 yl;
	u8 z;
	u8 t;  
	u8 majorAx;
	u8 minorAx;
	u8 orientation;
} cyttsp4_touch;


u8 CY8CTMA_WR_Reg(u16 reg,u8 *buf,u8 len);
void CY8CTMA_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 CY8CTMA_Init(void);
u8 CY8CTMA_Scan(u8 mode);
#endif






















