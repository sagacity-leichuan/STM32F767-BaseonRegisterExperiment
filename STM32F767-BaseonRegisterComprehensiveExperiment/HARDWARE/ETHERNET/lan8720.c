#include "lan8720.h"
#include "pcf8574.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
#include "string.h"
#include "ucos_ii.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//LAN8720��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/8/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


ETH_HandleTypeDef ETH_Handler;      //��̫�����
ETH_DMADescTypeDef *DMARxDscrTab;	//��̫��DMA�������������ݽṹ��ָ��
ETH_DMADescTypeDef *DMATxDscrTab;	//��̫��DMA�������������ݽṹ��ָ�� 
uint8_t *Rx_Buff; 					//��̫���ײ���������buffersָ�� 
uint8_t *Tx_Buff; 					//��̫���ײ���������buffersָ��
   

//LAN8720��ʼ��
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 LAN8720_Init(void)
{    
    u8 macaddress[6]; 
    PCF8574_WriteBit(ETH_RESET_IO,1);       //Ӳ����λ
    delay_ms(50);
    PCF8574_WriteBit(ETH_RESET_IO,0);       //��λ���� 
    
    macaddress[0]=lwipdev.mac[0]; 			//��ȡmac��ַ
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];
        
	ETH_Handler.State=HAL_ETH_STATE_RESET;	//���³�ʼ��
	ETH_Handler.Instance=ETH;
    ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_ENABLE;//ʹ����Э��ģʽ 
    ETH_Handler.Init.Speed=ETH_SPEED_100M;						//�ٶ�100M,�����������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.DuplexMode=ETH_MODE_FULLDUPLEX;			//ȫ˫��ģʽ�������������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;			//LAN8720��ַ  
    ETH_Handler.Init.MACAddr=macaddress;            			//MAC��ַ  
    ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;  				//�жϽ���ģʽ 
    ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;		//Ӳ��֡У��  
    ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;	//RMII�ӿ�  
    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)return 0;				//�ɹ�
    else return 1;  											//ʧ��  
}

//ETH�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_ETH_Init()����
//heth:��̫�����
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{ 
	//ʹ����̫��ʱ��
	RCC->AHB1ENR|=7<<25;		//ʹ��ETH MAC/MAC_Tx/MAC_Rxʱ�� 
	//ETH IO�ӿڳ�ʼ��
 	RCC->AHB1ENR|=1<<0;     	//ʹ��PORTAʱ�� 
 	RCC->AHB1ENR|=1<<1;     	//ʹ��PORTBʱ�� 
 	RCC->AHB1ENR|=1<<2;     	//ʹ��PORTCʱ��  
	RCC->AHB1ENR|=1<<6;     	//ʹ��PORTGʱ�� 
 	RCC->APB2ENR|=1<<14;   		//ʹ��SYSCFGʱ��
	SYSCFG->PMC|=1<<23;			//ʹ��RMII PHY�ӿ�. 
	GPIO_Set(GPIOA,PIN1|PIN2|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PA1,2,7�������
	GPIO_Set(GPIOC,PIN1|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PC1,4,5�������
	GPIO_Set(GPIOG,PIN13|PIN14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PG13,14�������
	GPIO_Set(GPIOB,PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);				//PB11�������
 
	GPIO_AF_Set(GPIOA,1,11);	//PA1,AF11
 	GPIO_AF_Set(GPIOA,2,11);	//PA2,AF11
 	GPIO_AF_Set(GPIOA,7,11);	//PA7,AF11 
  	GPIO_AF_Set(GPIOB,11,11);	//PB11,AF11 
	GPIO_AF_Set(GPIOC,1,11);	//PC1,AF11
 	GPIO_AF_Set(GPIOC,4,11);	//PC4,AF11
 	GPIO_AF_Set(GPIOC,5,11);	//PC5,AF11 
  	GPIO_AF_Set(GPIOG,13,11);	//PG13,AF11
 	GPIO_AF_Set(GPIOG,14,11);	//PG14,AF11 
 	MY_NVIC_Init(0,0,ETH_IRQn,2);	//����ETH�еķ���   	
}


//��ȡPHY�Ĵ���ֵ
u32 LAN8720_ReadPHY(u16 reg)
{
    u32 regval;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&regval);
    return regval;
}

//��LAN8720ָ���Ĵ���д��ֵ
//reg:Ҫд��ļĴ���
//value:Ҫд���ֵ
void LAN8720_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&temp);
}

//�õ�8720���ٶ�ģʽ
//����ֵ:
//001:10M��˫��
//101:10Mȫ˫��
//010:100M��˫��
//110:100Mȫ˫��
//����:����.
u8 LAN8720_Get_Speed(void)
{
	u8 speed;
	speed=((LAN8720_ReadPHY(31)&0x1C)>>2); 	//��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ
	return speed;
}

extern void lwip_pkt_handle(void);			//��lwip_comm.c���涨�� 
//�жϷ�����
void ETH_IRQHandler(void)
{ 
	OSIntEnter();		//�����ж�  
	lwip_pkt_handle();	//������̫�����ݣ����������ύ��LWIP 
    //����жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS); 
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R); 
	OSIntExit();        //���������л����ж�
}

//��ȡ���յ���֡����
//DMARxDesc:����DMA������
//����ֵ:���յ���֡����
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    u32 frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

//ΪETH�ײ����������ڴ�
//����ֵ:0,����
//    ����,ʧ��
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMTCM,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));	//�����ڴ�
	DMATxDscrTab=mymalloc(SRAMTCM,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));	//�����ڴ�  
	Rx_Buff=mymalloc(SRAMTCM,ETH_RX_BUF_SIZE*ETH_RXBUFNB);					//�����ڴ�
	Tx_Buff=mymalloc(SRAMTCM,ETH_TX_BUF_SIZE*ETH_TXBUFNB);					//�����ڴ�
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//����ʧ��
	}	
	return 0;		//����ɹ�
}

//�ͷ�ETH �ײ�����������ڴ�
void ETH_Mem_Free(void)
{ 
	myfree(SRAMTCM,DMARxDscrTab);	//�ͷ��ڴ�
	myfree(SRAMTCM,DMATxDscrTab);	//�ͷ��ڴ�
	myfree(SRAMTCM,Rx_Buff);		//�ͷ��ڴ�
	myfree(SRAMTCM,Tx_Buff);		//�ͷ��ڴ�  
}



