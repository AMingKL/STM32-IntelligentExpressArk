#include "gpio.h" 




void Relay_GPIO_Config(void)
{		
 GPIO_InitTypeDef GPIO_InitStructure;	
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
 GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP; 
 GPIO_Init(GPIOB,&GPIO_InitStructure);
	
 RELAYALL_OFF;
}




