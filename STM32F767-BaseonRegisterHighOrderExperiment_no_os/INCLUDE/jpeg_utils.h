#ifndef __JPEG_UTILS_H
#define __JPEG_UTILS_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////  
//ALIENTEK STM32������
//�ṩSTM32F7 JPEGӲ������  MCU(Minimum Coded Unit)�鵽RGB����ɫת������  
//////////////////////////////////////////////////////////////////////////////////  

//�����Լ�������,�޸�JPEG_BYTES_PER_PIXEL��ֵ
#define JPEG_BYTES_PER_PIXEL 2		//������ش�С(��λ:�ֽ�)
									//2:RGB565
									//3:RGB888
									//4:ARGB8888
															
									
//////////////////////////////////////////////////////////////////////////////////  									
#if	JPEG_BYTES_PER_PIXEL>2
#define JPEG_RED_OFFSET      16		//Offset of the RED color in a pixel        
#define JPEG_GREEN_OFFSET    8		//Offset of the GREEN color in a pixel          
#define JPEG_BLUE_OFFSET     0		//Offset of the BLUE color in a pixel        
#define JPEG_ALPHA_OFFSET    24 	//Offset of the Transparency Alpha in a pixel
#endif

//JPEG�ļ���Ϣ�ṹ��
typedef struct
{
	u8  ColorSpace;			//ͼ�����ɫ�ռ�: gray-scale/YCBCR/RGB/CMYK 
	u8  ChromaSubsampling;	//YCBCR/CMYK��ɫ�ռ��ɫ�ȳ������:0,4:4:4;1,4:2:2;2,4:1:1;3,4:2:0
	u32 ImageHeight;		//ͼ��߶� 
	u32 ImageWidth;			//ͼ����  
	u8  ImageQuality;		//ͼ���������:1~100
}JPEG_ConfTypeDef;

//��������ֵ����
#define JPEG_OK		0X00	//����
#define JPEG_ERROR	0X01	//����

//JPEG MCU��RGBת����Ҫʹ�õĲ��ֲ����ṹ��
typedef struct __JPEG_MCU_RGB_ConvertorTypeDef
{
	u32 ColorSpace;
	u32 ChromaSubsampling; 
	u32 ImageWidth;
	u32 ImageHeight;
	u32 ImageSize_Bytes; 
	u32 LineOffset; 
	u32 H_factor;
	u32 V_factor; 
	u32 WidthExtend;
	u32 ScaledWidth; 
	u32 MCU_Total_Nb; 
}JPEG_MCU_RGB_ConvertorTypeDef;


#define JPEG_GRAYSCALE_COLORSPACE	((u32)0x00000000U)
#define JPEG_YCBCR_COLORSPACE		JPEG_CONFR1_COLORSPACE_0
#define JPEG_CMYK_COLORSPACE		JPEG_CONFR1_COLORSPACE

#define JPEG_444_SUBSAMPLING	((u32)0x00000000U)	//Chroma Subsampling 4:4:4
#define JPEG_420_SUBSAMPLING	((u32)0x00000001U)	//Chroma Subsampling 4:2:0
#define JPEG_422_SUBSAMPLING	((u32)0x00000002U) 	//Chroma Subsampling 4:2:2 

#define YCBCR_420_BLOCK_SIZE		384				//YCbCr 4:2:0 MCU : 4 8x8 blocks of Y + 1 8x8 block of Cb + 1 8x8 block of Cr   
#define YCBCR_422_BLOCK_SIZE 		256				//YCbCr 4:2:2 MCU : 2 8x8 blocks of Y + 1 8x8 block of Cb + 1 8x8 block of Cr   
#define YCBCR_444_BLOCK_SIZE 		192				//YCbCr 4:4:4 MCU : 1 8x8 block of Y + 1 8x8 block of Cb + 1 8x8 block of Cr   
#define GRAY_444_BLOCK_SIZE 		64				//GrayScale MCU : 1 8x8 block of Y 
#define CMYK_444_BLOCK_SIZE 		256				//CMYK MCU : 1 8x8 blocks of Cyan + 1 8x8 block Magenta + 1 8x8 block of Yellow and 1 8x8 block of BlacK


typedef u32 (* JPEG_YCbCrToRGB_Convert_Function)(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);


u32 JPEG_MCU_YCbCr420_ARGB_ConvertBlocks(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);
u32 JPEG_MCU_YCbCr422_ARGB_ConvertBlocks(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);
u32 JPEG_MCU_YCbCr444_ARGB_ConvertBlocks(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);
u32 JPEG_MCU_Gray_ARGB_ConvertBlocks(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);
u32 JPEG_MCU_YCCK_ARGB_ConvertBlocks(u8 *pInBuffer,u8 *pOutBuffer,u32 BlockIndex,u32 DataCount);
u8 JPEG_GetDecodeColorConvertFunc(JPEG_ConfTypeDef *pJpegInfo, JPEG_YCbCrToRGB_Convert_Function *pFunction, u32 *ImageNbMCUs);

#endif 

