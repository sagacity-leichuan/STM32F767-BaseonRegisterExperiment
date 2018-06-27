#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "sys.h"   	 

//修改说明 
//V1.1 20161012
//修正STMFLASH_Write函数OTP部分的注释			  
////////////////////////////////////////////////////////////////////////////////// 


//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//FLASH解锁键值
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 32 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08008000) 	//扇区1起始地址, 32 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08010000) 	//扇区2起始地址, 32 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x08018000) 	//扇区3起始地址, 32 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08020000) 	//扇区4起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08040000) 	//扇区5起始地址, 256 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08080000) 	//扇区6起始地址, 256 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x080C0000) 	//扇区7起始地址, 256 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08100000) 	//扇区8起始地址, 256 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x08140000) 	//扇区9起始地址, 256 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x08180000) 	//扇区10起始地址,256 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x081C0000) 	//扇区11起始地址,256 Kbytes 
 
 
void STMFLASH_Unlock(void);					//FLASH解锁
void STMFLASH_Lock(void);				 	//FLASH上锁
u8 STMFLASH_GetStatus(void);				//获得状态
u8 STMFLASH_WaitDone(u32 time);				//等待操作结束
u8 STMFLASH_ErasePage(u32 sectoraddr);	 	//擦除页
u8 STMFLASH_WriteWord(u32 faddr, u32 dat);	//写入字
u32 STMFLASH_ReadWord(u32 faddr);		  	//读出字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(u32 WriteAddr,u32 WriteData);								   
#endif

