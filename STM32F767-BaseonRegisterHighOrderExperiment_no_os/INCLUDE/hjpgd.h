#ifndef __HJPGD_H
#define __HJPGD_H
#include "sys.h"
#include "jpegcodec.h" 
//////////////////////////////////////////////////////////////////////////////////	  
//图片解码 驱动代码-jpeg硬件解码部分	   
//********************************************************************************
//升级说明
//V1.1  20161017
//修正hjpgd_decode函数rgb565buf内存申请的一个bug
//////////////////////////////////////////////////////////////////////////////////
	
extern jpeg_codec_typedef hjpgd;  

 

void jpeg_dma_in_callback(void);
void jpeg_dma_out_callback(void);
void jpeg_endofcovert_callback(void);
void jpeg_hdrover_callback(void);
u8 hjpgd_decode(u8* pname);

#endif

