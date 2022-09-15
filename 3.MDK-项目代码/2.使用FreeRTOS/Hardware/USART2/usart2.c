#include "usart2.h" 
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h" 
 
vu8 UartRec2Len=0; 
vu8 USART2_RX_BUF[USART2_REC_LEN]={0};//����2���ݻ����� 
  
/*
��ʼ������ 

*/
void Usart2_Init(u32 band)
{
	//�����Ӧ�ṹ�嶨��
  GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// ʹ��GPIOʱ�� 
	
	//���¶���ܽ�
	//GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
	//USART2_TX    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ�� 
   
  //USART2_RX	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��   

  //Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
  //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = band;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
}

/*����һ���ֽ�����*/
void USART2SendByte(unsigned char SendData)
{	   
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
        USART_SendData(USART2,SendData);
} 

/*
���ݳ����򴮿�2 ��������
*/
void usart2_SendStringByLen(unsigned char * data,u8 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
  	USART_SendData(USART2, data[i]);         //�򴮿�2��������
	}
}
/*����2 �������ݵ���λ�� data ��\0��β*/
void usart2_SendString(unsigned char * data)
{
	u16 i=0;
	while(data[i]!='\0')
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART2, data[i]);         //�򴮿�1��������
		
		i++;
	}
	i++;
}


//���Ծ��۴������������ݸ�ʽ
void Uart2SendTjcScreen(unsigned char * data)
{
	 
  usart2_SendString(data);
	
	//����ָ��(0xff)
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART2, 0xff); 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
	USART_SendData(USART2, 0xff);  
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
  USART_SendData(USART2, 0xff); 
}

/*
����2�жϺ���
*/
void USART2_IRQHandler(void)              
	{
		if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
		{	    
			USART2_RX_BUF[UartRec2Len] = USART_ReceiveData(USART2);	
			UartRec2Len++;                			      //����ָ������ƶ�
			if(UartRec2Len > USART2_REC_LEN)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				UartRec2Len = 0;
			} 
		}	  
 }


