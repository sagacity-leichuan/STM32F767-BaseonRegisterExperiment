#ifndef __HJPGD_H
#define __HJPGD_H
#include "sys.h"
#include "jpegcodec.h" 
//////////////////////////////////////////////////////////////////////////////////	  
//ͼƬ���� ��������-jpegӲ�����벿��	   
//********************************************************************************
//����˵��
//V1.1  20161017
//����hjpgd_decode����rgb565buf�ڴ������һ��bug
//////////////////////////////////////////////////////////////////////////////////
	
extern jpeg_codec_typedef hjpgd;  

 

void jpeg_dma_in_callback(void);
void jpeg_dma_out_callback(void);
void jpeg_endofcovert_callback(void);
void jpeg_hdrover_callback(void);
u8 hjpgd_decode(u8* pname);

#endif

