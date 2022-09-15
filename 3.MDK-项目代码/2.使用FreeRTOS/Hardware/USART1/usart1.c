/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart1.h"
#include "sim800c.h"
#include "stdio.h"
#include "gpio.h" 
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"/* ���� -----------------------------------------------------------------------------------------------------------------*/
u8 USART1_RX_BUF[USART1_REC_LEN]={0};//����1���ݻ����� 


/**
 * @brief  ���ڳ�ʼ������
 * @param  ��
 * @retval ��
 */
void Usart1_Init(u32 bound)  
{
	GPIO_InitTypeDef   GPIO_InitStructure;	
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// ���� GPIOA �� USART1 ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	// PA9-TXD �����������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;        
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// PA10-RXD �������루��λ״̬������ʡ�ԣ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
	// ��ʼ�� USART3�������� BAUDRATE�ɵ���Ĭ�� 8 ������λ�� 1 ��ֹͣλ����У�飬���� Rx �� Tx��
	USART_InitStructure.USART_BaudRate   = bound; 						
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;		
	USART_InitStructure.USART_Parity     = USART_Parity_No;	 		
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USART1, &USART_InitStructure);
	
	// ���� USART1 ʹ��
	USART_Cmd(USART1, ENABLE);
	
	// ����USART1�����ж�
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// ���� NVIC USART1 �ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;        
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						
	NVIC_Init(&NVIC_InitStructure);	
}

void Usart1_SendByte(USART_TypeDef* pUSARTx,uint8_t data)
{
  USART_SendData(pUSARTx, data);
  while( USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE)==RESET);  
}



///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

void UART1_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,*s++);//���͵�ǰ�ַ�
	}
}

/**
 * @brief  ����1�����жϷ������
 * @param  ��
 * @retval ��
 */
void USART1_IRQHandler(void) 
{	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	  USART1_RX_BUF[First_Int]=USART_ReceiveData(USART1);
		First_Int++;
		if(First_Int > USART1_REC_LEN)       		//���������,������ָ��ָ�򻺴���׵�ַ
		{
			First_Int = 0;
		} 	
	 USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
	}
}



/*****************************************************END OF FILE*********************************************************/	


