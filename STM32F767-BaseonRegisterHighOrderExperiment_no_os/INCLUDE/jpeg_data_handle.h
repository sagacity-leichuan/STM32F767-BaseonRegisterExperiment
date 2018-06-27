#ifndef __JPEG_DATA_HANDLE_H
#define __JPEG_DATA_HANDLE_H	

#include "sys.h"

#define jpeg_buf_size   4*1024*1024		//定义JPEG数据缓存jpeg_buf的大小(4M字节)
#define jpeg_line_size	2*1024			//定义DMA接收数据时,一行数据的最大值

void jpeg_data_process(void);
void jpeg_dcmi_rx_callback(void);
void rgblcd_dcmi_rx_callback(void);
void sw_ov5640_mode(void);
void sw_sdcard_mode(void);
void camera_new_pathname(u8 *pname,u8 mode);
u8 ov5640_jpg_photo(u8 *pname);
#endif

