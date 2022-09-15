#ifndef __GPIO_H
#define	__GPIO_H

#include "stm32f10x.h"




#define RELAY1_OFF				 			 GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define RELAY1_ON				 			 	 GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define RELAY2_OFF				 			 GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define RELAY2_ON				 			 	 GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define RELAY3_OFF				 			 GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define RELAY3_ON				 			 	 GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define RELAY4_OFF				 			 GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define RELAY4_ON				 			 	 GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define RELAYALL_OFF				 		 GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9)


void Relay_GPIO_Config(void);


#endif /*__GPIO_H*/
