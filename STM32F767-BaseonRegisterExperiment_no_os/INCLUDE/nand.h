#ifndef _NAND_H
#define _NAND_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32开发板
//NAND FLASH 驱动代码	   		  
//********************************************************************************
//升级说明
//V1.1 20160812
//1,针对STM32F767修改了部分时间参数
//2,定义NAND_TADL_DELAY宏,用于设置tADL的延迟时间
//V1.2 20160822
//修改TCLR和TAR的时间参数为4,以防止接RGB屏出现读数不稳定的情况
//V1.3 20160824
//1,修改TCLR和TAR的时间参数为5,以便-O2优化能正常使用
//2,修改NAND_WaitRB函数,解决-O2优化读IO不稳定的bug
//3,针对F7,在硬件读取ECC之前,加入清D cache函数,解决ECC读取错误的bug
////////////////////////////////////////////////////////////////////////////////// 	
 
#define NAND_MAX_PAGE_SIZE			4096		//定义NAND FLASH的最大的PAGE大小（不包括SPARE区），默认4096字节
#define NAND_ECC_SECTOR_SIZE		512			//执行ECC计算的单元大小，默认512字节

#define NAND_TADL_DELAY				100			//tADL等待延迟,最少70ns

//NAND属性结构体
typedef struct
{
    u16 page_totalsize;     	//每页总大小，main区和spare区总和
    u16 page_mainsize;      	//每页的main区大小
    u16 page_sparesize;     	//每页的spare区大小
    u8  block_pagenum;      	//每个块包含的页数量
    u16 plane_blocknum;     	//每个plane包含的块数量
    u16 block_totalnum;     	//总的块数量
    u16 good_blocknum;      	//好块数量    
    u16 valid_blocknum;     	//有效块数量(供文件系统使用的好块数量)
    u32 id;             		//NAND FLASH ID
    u16 *lut;      			   	//LUT表，用作逻辑块-物理块转换
	u32 ecc_hard;				//硬件计算出来的ECC值
	u32 ecc_hdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];	//ECC硬件计算值缓冲区  	
	u32 ecc_rdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];	//ECC读取的值缓冲区
}nand_attriute;      

extern nand_attriute nand_dev;				//nand重要参数结构体 

#define NAND_RB  				GPIO_Pin_Get(GPIOD,PIN6)	//NAND Flash的闲/忙引脚 



#define NAND_ADDRESS			0X80000000	//nand flash的访问地址,接NCE3,地址为:0X8000 0000
#define NAND_CMD				1<<16		//发送命令
#define NAND_ADDR				1<<17		//发送地址

//NAND FLASH命令
#define NAND_READID         	0X90    	//读ID指令
#define NAND_FEATURE			0XEF    	//设置特性指令
#define NAND_RESET          	0XFF    	//复位NAND
#define NAND_READSTA        	0X70   	 	//读状态
#define NAND_AREA_A         	0X00   
#define NAND_AREA_TRUE1     	0X30  
#define NAND_WRITE0        	 	0X80
#define NAND_WRITE_TURE1    	0X10
#define NAND_ERASE0        	 	0X60
#define NAND_ERASE1         	0XD0
#define NAND_MOVEDATA_CMD0  	0X00
#define NAND_MOVEDATA_CMD1  	0X35
#define NAND_MOVEDATA_CMD2  	0X85
#define NAND_MOVEDATA_CMD3  	0X10

//NAND FLASH状态
#define NSTA_READY       	   	0X40		//nand已经准备好
#define NSTA_ERROR				0X01		//nand错误
#define NSTA_TIMEOUT        	0X02		//超时
#define NSTA_ECC1BITERR       	0X03		//ECC 1bit错误
#define NSTA_ECC2BITERR       	0X04		//ECC 2bit以上错误


//NAND FLASH型号和对应的ID号
#define MT29F4G08ABADA			0XDC909556	//MT29F4G08ABADA
#define MT29F16G08ABABA			0X48002689	//MT29F16G08ABABA

 

u8 NAND_Init(void);
u8 NAND_ModeSet(u8 mode);
u32 NAND_ReadID(void);
u8 NAND_ReadStatus(void);
u8 NAND_WaitForReady(void);
u8 NAND_Reset(void);
u8 NAND_WaitRB(vu8 rb);
void NAND_Delay(vu32 i);
u8 NAND_ReadPage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead);
u8 NAND_ReadPageComp(u32 PageNum,u16 ColNum,u32 CmpVal,u16 NumByteToRead,u16 *NumByteEqual);
u8 NAND_WritePage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToWrite);
u8 NAND_WritePageConst(u32 PageNum,u16 ColNum,u32 cval,u16 NumByteToWrite);
u8 NAND_CopyPageWithoutWrite(u32 Source_PageNum,u32 Dest_PageNum);
u8 NAND_CopyPageWithWrite(u32 Source_PageNum,u32 Dest_PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToWrite);
u8 NAND_ReadSpare(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead);
u8 NAND_WriteSpare(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead);
u8 NAND_EraseBlock(u32 BlockNum);
void NAND_EraseChip(void);

u16 NAND_ECC_Get_OE(u8 oe,u32 eccval);
u8 NAND_ECC_Correction(u8* data_buf,u32 eccrd,u32 ecccl);
#endif

