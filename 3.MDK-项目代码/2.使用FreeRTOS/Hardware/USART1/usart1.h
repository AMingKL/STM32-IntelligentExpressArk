#ifndef __USART1_H
#define __USART1_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"

/* ���� -----------------------------------------------------------------------------------------------------------------*/
#define USART1_REC_LEN 100

#define USART1_SendLR() Usart1_SendByte(USART1,0X0D);\
												Usart1_SendByte(USART1,0X0A)
/* ������ */


/* �ṩ������C�ļ����õĺ��� --------------------------------------------------------------------------------------------*/
void Usart1_Init(u32 bound); 		                 /* ���ڳ�ʼ������ */
void Usart1_SendByte(USART_TypeDef* pUSARTx,uint8_t data);
uint8_t Usart_Scan(uint8_t mode);
void UART1_SendString(char* s);


extern u8 USART1_RX_BUF[USART1_REC_LEN];//����1�������ݻ�����
#endif /* __USART1_H */

/*****************************************************END OF FILE*********************************************************/	
