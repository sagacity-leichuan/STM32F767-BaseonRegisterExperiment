#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK STM32������
//lwip-����ӿ����� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/8/12
//�汾��V1.0 							  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//����������
#define IFNAME0 'e'
#define IFNAME1 'n'
 

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
#endif
