#ifndef __AP3216C_H
#define __AP3216C_H
#include "sys.h"	
 
#define AP3216C_ADDR    0X3C	//AP3216C器件IIC地址(左移了一位)


u8 AP3216C_Init(void); 
u8 AP3216C_WriteOneByte(u8 reg,u8 data);
u8 AP3216C_ReadOneByte(u8 reg);
void AP3216C_ReadData(u16* ir,u16* ps,u16* als);
#endif

