/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart1.h"
#include "sim800c.h"
#include "stdio.h"
#include "gpio.h" 
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"/* 定义 -----------------------------------------------------------------------------------------------------------------*/
u8 USART1_RX_BUF[USART1_REC_LEN]={0};//串口1数据缓冲区 


/**
 * @brief  串口初始化函数
 * @param  无
 * @retval 无
 */
void Usart1_Init(u32 bound)  
{
	GPIO_InitTypeDef   GPIO_InitStructure;	
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// 允许 GPIOA 和 USART1 时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	// PA9-TXD 复用推挽输出
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;        
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// PA10-RXD 浮空输入（复位状态，可以省略）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
	// 初始化 USART3（波特率 BAUDRATE可调，默认 8 个数据位， 1 个停止位，无校验，允许 Rx 和 Tx）
	USART_InitStructure.USART_BaudRate   = bound; 						
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;		
	USART_InitStructure.USART_Parity     = USART_Parity_No;	 		
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USART1, &USART_InitStructure);
	
	// 允许 USART1 使能
	USART_Cmd(USART1, ENABLE);
	
	// 允许USART1接收中断
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// 允许 NVIC USART1 中断
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



///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

void UART1_SendString(char* s)
{
	while(*s)//检测字符串结束符
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,*s++);//发送当前字符
	}
}

/**
 * @brief  串口1接收中断服务程序
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void) 
{	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	  USART1_RX_BUF[First_Int]=USART_ReceiveData(USART1);
		First_Int++;
		if(First_Int > USART1_REC_LEN)       		//如果缓存满,将缓存指针指向缓存的首地址
		{
			First_Int = 0;
		} 	
	 USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
	}
}



/*****************************************************END OF FILE*********************************************************/	


