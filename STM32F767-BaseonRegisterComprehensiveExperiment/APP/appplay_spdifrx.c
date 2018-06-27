#include "appplay_spdifrx.h"  
#include "audioplay.h"
#include "sai.h" 
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

u32 *spdif_audiobuf[2]; 						//SPDIF��Ƶ���ݽ��ջ�����,��2��(˫����)
u32 *spdif_vubuf=0;

#define SPDIF_BACK_COLOR    	0X2945			//�����汳��ɫ	

#define SPDIF_VU_BKCOLOR    	0X39C7			//VU Meter����ɫ	
#define SPDIF_VU_L1COLOR    	0X07FF			//VU Meter L1ɫ	
#define SPDIF_VU_L2COLOR    	0xFFE0			//VU Meter L2ɫ	
#define SPDIF_VU_L3COLOR    	0xF800			//VU Meter L3ɫ	
#define SPDIF_VU_LEVEL			12				//�ܼ���,������ż��

u8*const SPDIF_RECORD_PIC[3]=
{ 
	"1:/SYSTEM/APP/APPS/SPDIF/record_180.bmp",	//demoͼƬ·�� 
	"1:/SYSTEM/APP/APPS/SPDIF/record_220.bmp",	//demoͼƬ·�� 	 
	"1:/SYSTEM/APP/APPS/SPDIF/record_320.bmp",	//demoͼƬ·�� 	 
};	
 
//��ʾ������
//y:y����(x�����Զ�����)
//samplerate:��Ƶ������(��λ:Hz)
//fsr:��������ʾ������
void spdif_show_samplerate(u16 y,u32 samplerate,u8 fsr)
{  
	u8 *buf;
	u16 x;
	u16 lenth;
	float rate=(float)samplerate/1000; 
	buf=mymalloc(SRAMIN,100);	//�����ڴ�
	if(buf)						//����ɹ�
	{	
		if(samplerate)
		{
			if(samplerate%1000)sprintf((char*)buf,"%2.1fKHz",rate);	//��ӡ������ 
			else sprintf((char*)buf,"%dKHz",samplerate/1000);		//��ӡ������ 
		}else sprintf((char*)buf,"Detecting...");					//��ʾ�����
		
		lenth=strlen((char*)buf);
		lenth=fsr*lenth/2;
		
		x=(lcddev.width-lenth)/2; 
		gui_fill_rectangle((lcddev.width-fsr*6)/2,y,fsr*6,fsr,SPDIF_BACK_COLOR); 
		gui_show_string(buf,x,y,200,fsr,fsr,WHITE);//��ʾ������
	}
	myfree(SRAMIN,buf);			//�ͷ��ڴ�
}
//SAI DMA��������жϻص����� 
void sai_dma_tx_callback(void)
{ 
	if(DMA2_Stream3->CR&(1<<19))		//buf1�ѿ�,��������buf0
	{ 
		spdif_vubuf=spdif_audiobuf[0];	//����buf0
		
	}else	 							//buf0�ѿ�,��������buf1
	{
		spdif_vubuf=spdif_audiobuf[1];	//����buf1
	} 	
}	
//SPDIF RX����ʱ�Ļص����� 
void spdif_rx_stopplay_callback(void)
{
	SAI_Play_Stop();
	SPDIFRX->IFCR|=1<<5;//���ͬ����ɱ�־	 
	spdif_dev.samplerate=0;
    memset((u8*)spdif_audiobuf[0],0,SPDIF_DBUF_SIZE*4);   
    memset((u8*)spdif_audiobuf[1],0,SPDIF_DBUF_SIZE*4);
}	
//��ƽ��ֵ��
const u16 spdif_vu_val_tbl[SPDIF_VU_LEVEL]={600,1200,2400,3600,4800,6000,8000,11000,13000,16000,21000,28000};
//���źŵ�ƽ�õ�vu����ֵ
//signallevel:�źŵ�ƽ
//����ֵ:vuֵ
u8 spdif_vu_get(u16 signallevel)
{
	u8 i;
	for(i=SPDIF_VU_LEVEL;i>0;i--)
	{
		if(signallevel>=spdif_vu_val_tbl[i-1])break;
	}
	return i; 
}
//��ʾVU Meter
//width:���,�߶�=���/2,���=���/8
//x,y:����
//level:0~10;
//��ʾ�ܿ��=width
//��ʾ�ܸ߶�=
void spdif_vu_meter(u8 width,u16 x,u16 y,u8 level)
{
	u8 i; 
	u16 pitch=0;	//2��level֮��ļ�϶
	u16 height=0;
	u16 vucolor=SPDIF_VU_L1COLOR;
	
 	if(level>SPDIF_VU_LEVEL)return ;	 
	pitch=width/8;
	height=pitch*(SPDIF_VU_LEVEL-1)+width*SPDIF_VU_LEVEL/2;
	if(level==0)
	{
		gui_fill_rectangle(x,y,width,height,SPDIF_VU_BKCOLOR);//��䱳��ɫ
		return;
	}   
	for(i=0;i<level;i++)
	{
		if(i==(SPDIF_VU_LEVEL-1))vucolor=SPDIF_VU_L3COLOR;
		else if(i>SPDIF_VU_LEVEL/2)vucolor=SPDIF_VU_L2COLOR;
		gui_fill_rectangle(x,y+height-width/2-(width/2+pitch)*i,width,width/2,vucolor);	//��䱳��ɫ
	}
	if(level<10)gui_fill_rectangle(x,y,width,height-level*(width/2+pitch),SPDIF_VU_BKCOLOR);	//��䱳��ɫ	 
} 


//SPDIF RXӦ��
//ͨ��SPDIF RX�ӿ�,���չ�����Ƶ����,���������.
u8 appplay_spdifrx(u8* caption)
{  
	u8 res;
 	u16 lastvolpos; 
 	u32 lastsamplerate=0; 
	
	u16 i;
	u8 timecnt=0;
	u8 vulevel[2];	//�������� 
	short tempval;
	u16 maxval=0;
	
	
	u8 fsr;							//����������,12/16/24
	u16 sry;						//�����ʵ�y����
	
	u16 bmpx,bmpy,bmpw;				//��ƬͼƬ��x,y����͸߶�
	u8 bmpidx;						//��ƬͼƬ����
	
	u16 vpbh,vpbw,vpbx,vpby;		//�����������ĸ߶�/���/x,y����
	u16 vbmpx,vbmpy;				//����ͼ���x,y����
	
	u16 vux,vuy,vuwidth,vuheight;	//vu x,y����,vu���Ŀ��/�߶�
	
	u16 ydis1,ydis2,ydis3;			//3��������
	u16 vuoffx;						//vu x�����ƫ�ƺ��볪Ƭͼ��ļ�϶
	u16 vbmpoffx,vbmpdis;			//����ͼ���xƫ�ƺ��������������ļ�϶

 	_progressbar_obj*volprgb;		//����������
	
	if(lcddev.width==240)
	{
		fsr=12;
		bmpidx=0;
		bmpw=180;
		vuoffx=4; 
		vuwidth=16;
		vbmpoffx=20;
		vbmpdis=1;
		ydis1=30;ydis2=12;ydis3=20;
		vpbh=12;
	}else if(lcddev.width==272)
	{
		fsr=16;
		bmpidx=1;
		bmpw=220;
		vuoffx=3; 
		vuwidth=16;
		vbmpoffx=26;
		vbmpdis=2;
		ydis1=70;ydis2=10;ydis3=54;
		vpbh=16;
	}else if(lcddev.width==320)
	{
		fsr=16;
		bmpidx=1;
		bmpw=220;
		vuoffx=10; 
		vuwidth=24;
		vbmpoffx=30;
		vbmpdis=2;
		ydis1=70;ydis2=10;ydis3=54;
		vpbh=16;
	}else if(lcddev.width==480)
	{
		fsr=24;
		bmpidx=2;
		bmpw=320;
		vuoffx=20; 
		vuwidth=40;
		vbmpoffx=50;
		vbmpdis=2;
		ydis1=130;ydis2=18;ydis3=130;
		vpbh=16;
	}else if(lcddev.width==600)
	{
		fsr=24;
		bmpidx=2;
		bmpw=320;
		vuoffx=40; 
		vuwidth=40;
		vbmpoffx=80;
		vbmpdis=2;
		ydis1=220;ydis2=20;ydis3=172;
		vpbh=16;
	}
	bmpx=(lcddev.width-bmpw)/2;
	bmpy=ydis1+gui_phy.tbheight;
	sry=bmpy+bmpw+ydis2;
	
	vbmpx=vbmpoffx;
	vbmpy=sry+fsr+ydis3;
	
	vpbw=lcddev.width-vbmpoffx*2-16-vbmpdis;
	vpbx=vbmpx+16+vbmpdis;
	if(vpbh>16)vpby=vbmpy-(vpbh-16)/2;
	else vpby=vbmpy+(16-vpbh)/2;
	 
	vuheight=(vuwidth*(SPDIF_VU_LEVEL-1))/8+vuwidth*SPDIF_VU_LEVEL/2; 
	vux=vuoffx;
	if(vuheight>bmpw)vuy=bmpy-(vuheight-bmpw)/2;
	else vuy=bmpy+(bmpw-vuheight)/2;
	 

	vulevel[0]=0;
	vulevel[1]=0;
	volprgb=progressbar_creat(vpbx,vpby,vpbw,vpbh,0X20);	//������С������
	spdif_audiobuf[0]=gui_memin_malloc(SPDIF_DBUF_SIZE*4);
	spdif_audiobuf[1]=gui_memin_malloc(SPDIF_DBUF_SIZE*4);
	if(!spdif_audiobuf[1]||!volprgb)
	{
		if(volprgb)progressbar_delete(volprgb);	//ɾ��������
		gui_memin_free(spdif_audiobuf[0]);		//�ͷ��ڴ�
		return 1;								//����ʧ��!
	}	
    memset((u8*)spdif_audiobuf[0],0,SPDIF_DBUF_SIZE*4);   
    memset((u8*)spdif_audiobuf[1],0,SPDIF_DBUF_SIZE*4);	
	if(audiodev.status&(1<<7))			//��ǰ�ڷŸ�??
	{	
		audio_stop_req(&audiodev);		//ֹͣ��Ƶ����
		audio_task_delete();			//ɾ�����ֲ�������.
	}
	LCD_Clear(SPDIF_BACK_COLOR);
	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//�·ֽ���	 
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,caption);	//��ʾ���� 
	
	SCB_CleanInvalidateDCache();	//�����MCU��,�ر�D cache
	minibmp_decode((u8*)SPDIF_RECORD_PIC[bmpidx],bmpx,bmpy,bmpw,bmpw,0,0);	//��������ͼ��
	SCB_CleanInvalidateDCache();	//�����MCU��,ʹ��D cache.
	minibmp_decode((u8*)APP_VOL_PIC,vbmpx,vbmpy,16,16,0,0);	//��������ͼ��
	
	volprgb->totallen=63;
	if(wm8978set.mvol<=63)volprgb->curpos=wm8978set.mvol;
	else	//��������� 
	{
		wm8978set.mvol=0;
		volprgb->curpos=0;
	}	  
	lastvolpos=volprgb->curpos;				//�趨�����λ��	
	volprgb->inbkcolora=AUDIO_INFO_COLOR;	//Ĭ��ɫ
	volprgb->inbkcolorb=AUDIO_INFO_COLOR;	//Ĭ��ɫ 
	volprgb->infcolora=0X75D;				//Ĭ��ɫ
	volprgb->infcolorb=0X596;				//Ĭ��ɫ     
	progressbar_draw_progressbar(volprgb);	//�������� 
	spdif_show_samplerate(sry,0,fsr);		//��ʾdetecting...
	app_wm8978_volset(wm8978set.mvol);		//����������	
	SPDIF_RX_Init();						//SPDIF��ʼ��
	WM8978_ADDA_Cfg(1,0);					//����DAC
	WM8978_Input_Cfg(0,0,0);				//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);					//����DAC��� 	
	spdif_rx_stop_callback=spdif_rx_stopplay_callback;	//SPDIF ��������ʱ�Ļص����� 
	sai_tx_callback=sai_dma_tx_callback;
	SPDIF_RXDATA_DMA_Init((u32*)spdif_audiobuf[0],(u32*)spdif_audiobuf[1],SPDIF_DBUF_SIZE,2);	//����SPDIF RX DMA,32λ
	SAIA_TX_DMA_Init((u8*)spdif_audiobuf[1],(u8*)spdif_audiobuf[0],SPDIF_DBUF_SIZE,2);			//����TX DMA,32λ
	while(1)
	{ 
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ  
		if(system_task_return)break;			//TPAD����
		res=progressbar_check(volprgb,&in_obj);	//�������������
		if(res&&lastvolpos!=volprgb->curpos)	//��������,��λ�ñ仯��.ִ����������
		{		  
			lastvolpos=volprgb->curpos;
			if(volprgb->curpos)wm8978set.mvol=volprgb->curpos;//��������
			else wm8978set.mvol=0;	  
			app_wm8978_volset(wm8978set.mvol);	    
		}	  		
        if(spdif_dev.consta==0)				//δ����
        {
            if(SPDIF_RX_WaitSync())			//�ȴ�ͬ��
            {
                spdif_dev.samplerate=SPDIF_RX_GetSampleRate();	//��ò�����
				if(spdif_dev.saisync==0)	//SAIʱ�Ӻ�SPDIFʱ�ӻ�δͬ��,���Ƚ���ͬ��
				{
					if(spdif_dev.samplerate)//��������Ч,ͬ��
					{
						WM8978_I2S_Cfg(2,2);						//�����ֱ�׼,24λ���ݳ���
						SAIA_Init(0,1,6);							//����SAI,������,24λ����
						SAIA_SampleRate_Set(spdif_dev.samplerate);	//���ò�����,���޸�plln��ֵ 
						spdif_dev.clock=SPDIF_RX_GetSaiPlln();		//�õ��µ�SPDIF CLKʱ��
						spdif_dev.clock*=500000;					//�õ�SPDIF CLK��Ƶ��  
						spdif_dev.saisync=1;						//���SAI��SPDIFʱ��ͬ�����
						spdif_dev.samplerate=0;
					}
				}else if(spdif_dev.samplerate)						//SAI��SPDIFʱ���Ѿ�ͬ�����Ҳ���������,SPDIF��������������
				{ 
					SPDIF_RX_Start();								//ͬ����ɣ���SPDIF
					SAI_Play_Start();								//����SAI����						
				}else SPDIF_RX_Stop();								//�����ʴ���,ֹͣSPDIF���� 
            }
        }
		if(lastsamplerate!=spdif_dev.samplerate)					//�����ʸı���
		{
			lastsamplerate=spdif_dev.samplerate;
			spdif_vu_meter(vuwidth,vux,vuy,0);						//��ʾvu meter;
			spdif_vu_meter(vuwidth,lcddev.width-vux-vuwidth,vuy,0);	//��ʾvu meter;
			spdif_show_samplerate(sry,spdif_dev.samplerate,fsr);	//��ʾ������.
		}
		delay_ms(1000/OS_TICKS_PER_SEC);	//��ʱһ��ʱ�ӽ��� 
		timecnt++; 
		if((timecnt%20)==0&&spdif_dev.samplerate)
		{	 
			for(i=0;i<512;i++)//ȡǰ512��������������ֵ
			{
				tempval=0.0039061*spdif_vubuf[i*2];//ת��Ϊshort���� 
				if(tempval<0)tempval=-tempval;
				if(maxval<tempval)maxval=tempval;//ȡ���ֵ	  
			}
			tempval=spdif_vu_get(maxval);
			if(tempval>vulevel[0])vulevel[0]=tempval;
			else if(vulevel[0])vulevel[0]--;
			spdif_vu_meter(vuwidth,vux,vuy,vulevel[0]);//��ʾvu meter;
			maxval=0;		

			for(i=0;i<512;i++)//ȡǰ512��������������ֵ
			{
				tempval=0.0039061*spdif_vubuf[i*2+1];//ת��Ϊshort���� 
				if(tempval<0)tempval=-tempval;
				if(maxval<tempval)maxval=tempval;//ȡ���ֵ	  
			}
			tempval=spdif_vu_get(maxval);
			if(tempval>vulevel[1])vulevel[1]=tempval;
			else if(vulevel[1])vulevel[1]--;
			spdif_vu_meter(vuwidth,lcddev.width-vux-vuwidth,vuy,vulevel[1]);//��ʾvu meter;
			maxval=0;				
		} 
	} 
	SPDIF_RX_Stop();					//ֹͣSPDIF���� 
	SAI_Play_Stop();					//�ر���Ƶ
	SPDIF_RX_Mode(SPDIF_RX_IDLE);		//SPDIFRX IDLE״̬
	WM8978_ADDA_Cfg(0,0);				//�ر�DAC&ADC
	WM8978_Input_Cfg(0,0,0);			//�ر�����ͨ��
	WM8978_Output_Cfg(0,0);				//�ر�DAC���   	
	app_wm8978_volset(0);				//�ر�WM8978�������
	gui_memin_free(spdif_audiobuf[0]);	//�ͷ��ڴ�
	gui_memin_free(spdif_audiobuf[1]);
	progressbar_delete(volprgb);		//ɾ��������
	return 0;
}









