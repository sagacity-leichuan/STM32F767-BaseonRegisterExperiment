#ifndef __APPPLAY_SPDIFRX_H
#define __APPPLAY_SPDIFRX_H 	
#include "common.h"  
#include "spdif.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-SPDIF RX���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/9/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	  

#define	SPDIF_DBUF_SIZE		1024			//����SPDIF���ݽ��ջ������Ĵ�С,1K*4�ֽ�

extern u8*const SPDIF_RECORD_PIC[3];

void spdif_show_samplerate(u16 y,u32 samplerate,u8 fsr);
void sai_dma_tx_callback(void);
void spdif_rx_stopplay_callback(void);
u8 spdif_vu_get(u16 signallevel);
void spdif_vu_meter(u8 width,u16 x,u16 y,u8 level); 
u8 appplay_spdifrx(u8* caption);
#endif



