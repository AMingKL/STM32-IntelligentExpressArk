#ifndef __USART2_H
#define	__USART2_H


#include "stm32f10x.h"
 
extern vu8 UartRec2Len;


#define USART2_REC_LEN 100


extern vu8 USART2_RX_BUF[USART2_REC_LEN];
 
void Usart2_Init(u32 band);//串口2 初始化函数
void usart2_SendStringByLen(unsigned char * data,u8 len);//
void usart2_SendString(unsigned char * data);
void USART2SendByte(unsigned char SendData);

 //向淘晶驰串口屏发送数据格式
void Uart2SendTjcScreen(unsigned char * data);
	 
	
	
#endif
