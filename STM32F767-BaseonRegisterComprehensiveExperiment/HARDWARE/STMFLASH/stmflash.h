#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//STM32�ڲ�FLASH��д ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 32 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08008000) 	//����1��ʼ��ַ, 32 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08010000) 	//����2��ʼ��ַ, 32 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x08018000) 	//����3��ʼ��ַ, 32 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08020000) 	//����4��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08040000) 	//����5��ʼ��ַ, 256 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08080000) 	//����6��ʼ��ַ, 256 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x080C0000) 	//����7��ʼ��ַ, 256 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08100000) 	//����8��ʼ��ַ, 256 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x08140000) 	//����9��ʼ��ַ, 256 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x08180000) 	//����10��ʼ��ַ,256 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x081C0000) 	//����11��ʼ��ַ,256 Kbytes 
 
 
void STMFLASH_Unlock(void);					//FLASH����
void STMFLASH_Lock(void);				 	//FLASH����
u8 STMFLASH_GetStatus(void);				//���״̬
u8 STMFLASH_WaitDone(u32 time);				//�ȴ���������
u8 STMFLASH_ErasePage(u32 sectoraddr);	 	//����ҳ
u8 STMFLASH_WriteWord(u32 faddr, u32 dat);	//д����
u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ����� 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u32 WriteData);								   
#endif

















