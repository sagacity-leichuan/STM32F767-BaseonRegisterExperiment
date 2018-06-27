#include "sys.h"  
#include "usart1.h"  
#include "delay.h" 

int main(void)
{
	u8 t=0; 
	Stm32_Clock_Init(432,25,2,9);     
	delay_init(216);
	uart1_init(108,115200);  
	while(t < 100) 
	{ 
		printf("t:%d\r\n",t); 
		delay_ms(500); 
		t++; 
	}
	
	return 1;
}
