#include "cy8ctma463.h"
#include "touch.h"
#include "ctiic.h"
#include "lcd.h"
#include "usart.h"
#include "delay.h" 
#include "string.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//8����ݴ�����-CY8CTMA463 ��������(֧��10�㴥��)	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/9/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved		 
////////////////////////////////////////////////////////////////////////////////// 


//��CY8CTMA463д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 CY8CTMA_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	 
	CT_IIC_Send_Byte(CY_CMD_WR);	//����д���� 	 
	ret = CT_IIC_Wait_Ack(); 
	CT_IIC_Send_Byte(reg&0XFF);   	//����8λ��ַ
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//������
		ret=CT_IIC_Wait_Ack(); 
		if(ret)break;  
	}
    CT_IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
} 
//��CY8CTMA463����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
void CY8CTMA_RD_Reg(u16 reg,u8 *buf,u8 len)
{   
	u8 i;

 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(CY_CMD_WR);   	//����д���� 	 	
    CT_IIC_Wait_Ack();	
 	CT_IIC_Send_Byte(reg&0XFF);   	//����8λ��ַ
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(CY_CMD_RD);   	//���Ͷ�����		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
    CT_IIC_Stop();//����һ��ֹͣ����  
	
}

//CY8CTMA463 �˳�Bootload����
const u8 CY8_Exit_Bootload[]={0xFF,0x01,0x3B,0x00,0x00,0x4F,0x6D,0x17};

//��ʼ��CY8CTMA463������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ�� 
u8 CY8CTMA_Init(void)
{   

	u16 FW;		//�̼��汾��
	u16 BL;		//Bootload�汾��
	u16 CYID;	//�����汾��
	u8 Mode;
	u8 ret;
	u8 i;
	u8 temp[2];  
	RCC->AHB1ENR|=1<<7;    	//ʹ��PORTHʱ�� 
	RCC->AHB1ENR|=1<<8;    	//ʹ��PORTIʱ��  
	GPIO_Set(GPIOH,PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//PH7����Ϊ��������
	GPIO_Set(GPIOI,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PI8����Ϊ�������
	CT_IIC_Init();      	//��ʼ����������I2C����   
	CY_RST(0);				//��λ
	delay_ms(10);
 	CY_RST(1);				//�ͷŸ�λ		    
	delay_ms(10);
	temp[0]=0;
    CY8CTMA_RD_Reg(CY_HST_MODE,&temp[0],1);//��ȡHST_MODE
	Mode=temp[0];
	if(Mode==0x00)//��ǰΪ����ģʽ
	{
		 temp[0]=0x18;
		 ret=CY8CTMA_WR_Reg(CY_HST_MODE,&temp[0],1);	//����ϵͳ��Ϣģʽ
		 if(!ret){printf("����ϵͳ��Ϣģʽ�ɹ�..\r\n");}
		 else printf("����ϵͳ��Ϣģʽʧ��...\r\n");		 
		 CY8CTMA_RD_Reg(CY_HST_MODE,&Mode,1);			//���¶�һ�ε�ǰģʽ
		 delay_ms(100);
	}
	if(Mode!=0x00)			//��ǰΪ�ǲ���ģʽ
	{
		 i=5;
		 while(i--)
		 {
			 ret=CY8CTMA_WR_Reg(CY_HST_MODE,(u8*)CY8_Exit_Bootload,sizeof(CY8_Exit_Bootload));//�˳�bootloadģʽ
			 if(!ret){printf("�˳�bootloader�ɹ�...\r\n");break;}
			 else printf("�˳�bootloaderʧ��...\r\n");
			 delay_ms(100);
		 }
		 delay_ms(500);		//����������ʱ,�˳�bootloadʱ��Ҫ��ʱ�� 		
		 CY8CTMA_RD_Reg(CY_FW_VER,&temp[0],2);  
		 FW = ((u16)temp[0]<<8) + temp[1];printf("FW_VER: 0x%X\r\n",FW);	//��ȡ�̼��汾��
		 CY8CTMA_RD_Reg(CY_BL_VER,temp,2);
		 BL = ((u16)temp[0]<<8) + temp[1];printf("BL_VER: 0x%X\r\n",BL);	//��ȡbootload�汾��
		 CY8CTMA_RD_Reg(CY_ID,temp,2);
		 CYID = ((u16)temp[0]<<8) + temp[1];printf("CP_ID: 0x%x\r\n",CYID);	//��ȡCY ID
		 i=5;
		 while(i--)
		 {
			temp[0]=0x08;
			ret = CY8CTMA_WR_Reg(CY_HST_MODE,&temp[0],1);//�������ģʽ
			if(!ret) { printf("�������ģʽ�ɹ�..\r\n"); break;}
			else      printf("�������ģʽʧ��...\r\n");
			delay_ms(100);
		 }
		delay_ms(100);//����������ʱ,����������Ҫʱ��  
		temp[0]=0;
		CY8CTMA_RD_Reg(CY_HST_MODE,&temp[0],1);	//���¶�һ�ε�ǰģʽ
		if(temp[0]==0x00) return 0;				//��ʼ���ɹ� 
	}
	return 1;	
} 
//�洢��������
cyttsp4_touch cy_tch[CT_MAX_TOUCH];
//ɨ�败����(���ò�ѯģʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 CY8CTMA_Scan(u8 mode)
{
	u8 i=0;
	u8 res=0;
	u16 temp;
	u16 tempsta;
 	static u8 t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%10)==0||t<10)
	{
		CY8CTMA_RD_Reg(CY_TT_STAT,&mode,1);	//��ȡ�������״̬
		if((mode&0xF)&&((mode&0x1F)<11))	//��ȡ�������״̬  
		{
			temp=0XFFFF<<(mode&0XF);		//����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta���� 
			tempsta=tp_dev.sta;				//���浱ǰ��tp_dev.staֵ
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES;
			tp_dev.x[CT_MAX_TOUCH-1]=tp_dev.x[0];	//���津��0������
			tp_dev.y[CT_MAX_TOUCH-1]=tp_dev.y[0];
			CY8CTMA_RD_Reg(CY_TP_REG,(u8*)&cy_tch,sizeof(cyttsp4_touch)*mode);//��ȡ��������
			for(i=0;i<CT_MAX_TOUCH;i++)
			{   
				if(tp_dev.sta&(1<<i))		//������Ч?
				{
					if(tp_dev.touchtype&0X01)//����
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
			if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//�Ƿ�����(���곬����)
			{ 
				if((mode&0X1F)>1)		//��������������,�򸴵ڶ�����������ݵ���һ������.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//����һ��,��������������10��,�Ӷ����������
				}else					//�Ƿ�����,����Դ˴�����(��ԭԭ����)  
				{
					tp_dev.x[0]=tp_dev.x[CT_MAX_TOUCH-1];
					tp_dev.y[0]=tp_dev.y[CT_MAX_TOUCH-1];
					mode=0X80;		
					tp_dev.sta=tempsta;	//�ָ�tp_dev.sta
				}
			}else t=0;					//����һ��,��������������10��,�Ӷ���������� 
		} 
	}
	if((mode&0X1F)==0X00)//�޴����㰴��
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)		//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~TP_PRES_DOWN;	//��ǰ����ɿ�
		}else							//֮ǰ��û�б�����
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=(TP_PRES_DOWN|TP_CATH_PRES);//�������Ч���	
		}	 
	} 	
	if(t>240)t=10;//���´�10��ʼ����
	return res; 
}
		

























								 
