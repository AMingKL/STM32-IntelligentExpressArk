/**
  *************************************************************************************************************************
  * @file    myconfig.c
  * @author  AMKL
  * @version V1.0
  * @date    2021-12-28
  * @brief   ���ú���
  *************************************************************************************************************************
  * @attention
  *
  * 
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"


/* �������� -----------------------------------------------------------------------------------------------------------------*/
unsigned char  Timers[TIMERCOUNT];
Para_InitTypeDef Para;	
Ark_InitTypeDef Ark[4]={{0,0,"654321"},
												{0,0,"654321"},
												{0,0,"654321"},
												{0,0,"654321"}};
char TempStr[20];
uint8_t InputPickupCode[6]="000000";//�����ȡ����																	
char* PointToPickupCode;//ָ��ȡ�����ַ				
char* PointToPhone;	//ָ���ֻ���ַ																				

																				
/**																				
 * @brief  �����ʱ��
 * @param  xIndex-�¼�x��xTime-��ʱʱ��
 * @retval ��
 */
unsigned char xDelay(unsigned char xIndex,unsigned char xTime)
{
	unsigned char RetValue=TIMEISNOTUP;

	if(Timers[xIndex]==0)//�ж�ָ�������ʱ���ļ������Ƿ�Ϊ 0
	{
		Timers[xIndex]=xTime;//��װ��������ֵ
		RetValue=TIMEISUP;	 //��ֵ����ֵΪ��ʱ�䵽	
	}
	return(RetValue);
}


/**
 * @brief  ��ѯ/�ȴ�SIM������ע��
 * @param  ��
 * @retval ��
 */
void Waiting_Network(void)
{
  uint8_t i;	
	for(i=0;i<80;i++)
	{
		sprintf(TempStr,"j0.val=%d",i);
		Uart2SendTjcScreen((unsigned char *)TempStr);	
		delay_ms(100);
	}
  Wait_CREG();
	Set_Text_Mode();
	for(i=80;i<100;i++)
	{
		sprintf(TempStr,"j0.val=%d",i);
		Uart2SendTjcScreen((unsigned char *)TempStr);	
		delay_ms(100);
	}	
	
	Uart2SendTjcScreen("page Main");//�л���������
	Para.SendMessageFlag=NON;//��ʼ��Ϊ�ǹ����
	UartRec2Len=0; 
	memset((char*)USART2_RX_BUF,0,100);//��մ���2���ݻ�����
}


/**
 * @brief  ���ڴ�������Ϣ������������
 * @param  ��
 * @retval ��
 */
void AboutUsartScreenProc(void)
{
  if(strstr((const char*)USART2_RX_BUF,"END")!=NULL)	//����һ���������
	{
		if(strstr((const char*)USART2_RX_BUF,"ZCKDY")!=NULL)//����������ע����Ա����
		{	
			Para.InputFingerprintFlag=1;
			Para.FingerprintID=CourierID;//ע����Ա
		}
		else if(strstr((const char*)USART2_RX_BUF,"ZCGLY")!=NULL)//����������ע�����Ա����
		{	
			Para.InputFingerprintFlag=1;
			Para.FingerprintID=AdminerID;//ע�����Ա
		}
		else if(strstr((const char*)USART2_RX_BUF,"1HTD")!=NULL)//����������1�Ź�Ͷ�ݲ���
		{	
			//1HTD15983674750QJM123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
			PointToPhone=strstr((const char*)USART2_RX_BUF,"1HTD");
			strncpy((char *)NO1_phone, PointToPhone+4, 11);
			strncpy((char *)Ark[ONE].PickupCode, PointToPickupCode+3, 6);
			Ark[ONE].UseState=FULL;
			Ark[ONE].UseNum+=1;	
			Para.SendMessageFlag=ONE;			
		}		
		else if(strstr((const char*)USART2_RX_BUF,"2HTD")!=NULL)//����������2�Ź�Ͷ�ݲ���
		{	
			//2HTD15983674750QJM123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
			PointToPhone=strstr((const char*)USART2_RX_BUF,"2HTD");
			strncpy((char *)NO2_phone, PointToPhone+4, 11);
			strncpy((char *)Ark[TWO].PickupCode, PointToPickupCode+3, 6);
			Ark[TWO].UseState=FULL;
			Ark[TWO].UseNum+=1;	
			Para.SendMessageFlag=TWO;	
		}
		else if(strstr((const char*)USART2_RX_BUF,"3HTD")!=NULL)//����������3�Ź�Ͷ�ݲ���
		{	
			//3HTD15983674750QJM123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
			PointToPhone=strstr((const char*)USART2_RX_BUF,"3HTD");
			strncpy((char *)NO3_phone, PointToPhone+4, 11);
			strncpy((char *)Ark[THREE].PickupCode, PointToPickupCode+3, 6);
			Ark[THREE].UseState=FULL;
			Ark[THREE].UseNum+=1;	
			Para.SendMessageFlag=THREE;	
		}
		else if(strstr((const char*)USART2_RX_BUF,"4HTD")!=NULL)//����������4�Ź�Ͷ�ݲ���
		{	
			//4HTD15983674750QJM123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
			PointToPhone=strstr((const char*)USART2_RX_BUF,"4HTD");
			strncpy((char *)NO4_phone, PointToPhone+4, 11);
			strncpy((char *)Ark[FOUR].PickupCode, PointToPickupCode+3, 6);
			Ark[FOUR].UseState=FULL;
			Ark[FOUR].UseNum+=1;	
			Para.SendMessageFlag=FOUR;	
		}
		else if(strstr((const char*)USART2_RX_BUF,"QJ")!=NULL)//����������ȡ������
		{	
			//QJ123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJ");
			strncpy((char *)InputPickupCode, PointToPickupCode+2, 6);
			 if(strstr((char*)InputPickupCode,(char*)Ark[ONE].PickupCode)!=NULL)
			 {
					RELAY1_ON;//ģ��1�Ź�
					Uart2SendTjcScreen("t6.txt=\"�뵽1�Ź�ȡ����ݣ�\""); //��ʾ �뵽1�Ź�ȡ����ݣ�
					Ark[ONE].UseState=EMPTY;//ȡ���󣬿�ݹ�ָ�δʹ��״̬
			 }			
			 else if(strstr((char*)InputPickupCode,(char*)Ark[TWO].PickupCode)!=NULL)
			 {
					RELAY2_ON;//ģ��2�Ź�
					Uart2SendTjcScreen("t6.txt=\"�뵽2�Ź�ȡ����ݣ�\""); //��ʾ �뵽2�Ź�ȡ����ݣ�
					Ark[TWO].UseState=EMPTY;//ȡ���󣬿�ݹ�ָ�δʹ��״̬
			 }
			 else if(strstr((char*)InputPickupCode,(char*)Ark[THREE].PickupCode)!=NULL)
			 {
					RELAY3_ON;//ģ��3�Ź�
					Uart2SendTjcScreen("t6.txt=\"�뵽3�Ź�ȡ����ݣ�\""); //��ʾ �뵽3�Ź�ȡ����ݣ�
					Ark[THREE].UseState=EMPTY;//ȡ���󣬿�ݹ�ָ�δʹ��״̬
			 }
			 else if(strstr((char*)InputPickupCode,(char*)Ark[FOUR].PickupCode)!=NULL)
			 {
					RELAY4_ON;//ģ��4�Ź�
					Uart2SendTjcScreen("t6.txt=\"�뵽4�Ź�ȡ����ݣ�\""); //��ʾ �뵽4�Ź�ȡ����ݣ�
					Ark[FOUR].UseState=EMPTY;//ȡ���󣬿�ݹ�ָ�δʹ��״̬
			 }
			else
			{
				  Uart2SendTjcScreen("t6.txt=\"ȡ�������\""); //��ʾ ȡ�������			
			}				 
		}		
		else if(strstr((const char*)USART2_RX_BUF,"JXTD")!=NULL)//���������м���Ͷ�ݲ���
		{	
			(Ark[ONE].UseState==FULL)?(Uart2SendTjcScreen("t2.txt=\"����\"")):(Uart2SendTjcScreen("t2.txt=\"δ��\""));
			(Ark[TWO].UseState==FULL)?(Uart2SendTjcScreen("t3.txt=\"����\"")):(Uart2SendTjcScreen("t3.txt=\"δ��\""));
			(Ark[THREE].UseState==FULL)?(Uart2SendTjcScreen("t4.txt=\"����\"")):(Uart2SendTjcScreen("t4.txt=\"δ��\""));
			(Ark[FOUR].UseState==FULL)?(Uart2SendTjcScreen("t5.txt=\"����\"")):(Uart2SendTjcScreen("t5.txt=\"δ��\""));			
		}			
		else if(strstr((const char*)USART2_RX_BUF,"YQCKD")!=NULL)//��������ȡ����ݲ���
		{	
			RELAYALL_OFF;//�ر����п�ݹ���
		}	
		UartRec2Len=0; 
		memset((char*)USART2_RX_BUF,0,100);
		Write_Flash(); //�������		
	}
}

/**
 * @brief  ¼ָ�ƣ�ָ�ƿ�����������
 * @param  ��
 * @retval ��
 */
void AboutFingerprintProc(void)
{
	if(Para.InputFingerprintFlag==1)//¼ָ��
	{		
		Add_FR();
		Para.InputFingerprintFlag=0;
	}
	else//����ָ�ƶԱ�
	{
	  press_FR();
	}
}

/**
 * @brief  ���ڷ���ȡ�����ţ���ʱδ��ȡ��ʾ���ſ�ݲ���
 * @param  ��
 * @retval ��
 */
void AboutCourierProc(void)
{
  switch (Para.SendMessageFlag)
	{
	  case ONE:	 RELAY1_ON;//ģ�⿪1�Ź���		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[ONE].PickupCode);	//����ȡ�������
							 Send_Text_Sms(NO1_phone);
							 delay_ms(1000);		
							 RELAY1_OFF;//���ŷ������,ģ���1�Ź���,����Ͷ�����
							 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
							 Para.SendMessageFlag=NON;break;
		case TWO:	 RELAY2_ON;//ģ�⿪2�Ź���		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[TWO].PickupCode);	//����ȡ�������
							 Send_Text_Sms(NO2_phone);
							 delay_ms(1000);		
							 RELAY2_OFF;//���ŷ������,ģ���2�Ź���,����Ͷ�����
							 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
							 Para.SendMessageFlag=NON;break;
		case THREE:RELAY3_ON;//ģ�⿪3�Ź���		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[THREE].PickupCode);	//����ȡ�������
							 Send_Text_Sms(NO3_phone);
							 delay_ms(1000);		
							 RELAY3_OFF;//���ŷ������,ģ���3�Ź���,����Ͷ�����
							 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
							 Para.SendMessageFlag=NON;break;
		case FOUR: RELAY4_ON;//ģ�⿪4�Ź���		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[FOUR].PickupCode);	//����ȡ�������
							 Send_Text_Sms(NO4_phone);
							 delay_ms(1000);		
							 RELAY4_OFF;//���ŷ������,ģ���4�Ź���,����Ͷ�����
							 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
							 Para.SendMessageFlag=NON;break;
		default:break;	
	}
	
	//��ʱδ��ȡ��ʾ����
	if(Para.Sec==180)//6���Ӽ�ʱ����ģ��1��δȡ�أ��������Ѷ���
	{
	   Para.Sec=0;
			if(Ark[ONE].UseState == FULL)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(NO1_phone);
			}
			if(Ark[TWO].UseState == FULL)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(NO2_phone);
			}
			if(Ark[THREE].UseState == FULL)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(NO3_phone);
			}	
			if(Ark[FOUR].UseState == FULL)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(NO4_phone);
			}	
	}
}




/**
 * @brief  Flash��ȡ
 * @param  ��
 * @retval ��
 */
void Read_Flash(void)
{
   uint16_t Data[17]={0};
	 STMFLASH_Read(FLASH_SAVE_ADDR,Data,17);
	 
	 //�ж��Ƿ��һ���ϵ�
	 if(Data[0]!=0x88)//��һ���ϵ� д������
	 {
		 Write_Flash();
		 printf("first");
	 }
	 else//�ָ�����
	 {
		 /*����ݹ��ʹ��״̬�Լ�ʹ�ô���*/
		 Ark[ONE].UseState=Data[1]>>8; 
		 Ark[ONE].UseNum=Data[1]; 	 
		 Ark[TWO].UseState=Data[2]>>8; 
		 Ark[TWO].UseNum=Data[2]; 		 
		 Ark[THREE].UseState=Data[3]>>8; 
		 Ark[THREE].UseNum=Data[3]; 		 
		 Ark[FOUR].UseState=Data[4]>>8; 
		 Ark[FOUR].UseNum=Data[4];
		 /*����ݹ��ȡ����*/
		 Ark[ONE].PickupCode[0]=Data[5]>>8; 
		 Ark[ONE].PickupCode[1]=Data[5]; 
		 Ark[ONE].PickupCode[2]=Data[6]>>8; 
		 Ark[ONE].PickupCode[3]=Data[6]; 
		 Ark[ONE].PickupCode[4]=Data[7]>>8; 
		 Ark[ONE].PickupCode[5]=Data[7]; 
		 
		 Ark[TWO].PickupCode[0]=Data[8]>>8; 
		 Ark[TWO].PickupCode[1]=Data[8]; 
		 Ark[TWO].PickupCode[2]=Data[9]>>8; 
		 Ark[TWO].PickupCode[3]=Data[9]; 
		 Ark[TWO].PickupCode[4]=Data[10]>>8; 
		 Ark[TWO].PickupCode[5]=Data[10]; 
		 
		 Ark[THREE].PickupCode[0]=Data[11]>>8; 
		 Ark[THREE].PickupCode[1]=Data[11]; 
		 Ark[THREE].PickupCode[2]=Data[12]>>8; 
		 Ark[THREE].PickupCode[3]=Data[12]; 
		 Ark[THREE].PickupCode[4]=Data[13]>>8; 
		 Ark[THREE].PickupCode[5]=Data[13]; 
		 
		 Ark[FOUR].PickupCode[0]=Data[14]>>8; 
		 Ark[FOUR].PickupCode[1]=Data[14]; 
		 Ark[FOUR].PickupCode[2]=Data[15]>>8; 
		 Ark[FOUR].PickupCode[3]=Data[15]; 
		 Ark[FOUR].PickupCode[4]=Data[16]>>8; 
		 Ark[FOUR].PickupCode[5]=Data[16]; 
	 }	 
}


/**
 * @brief  Flashд��
 * @param  ��
 * @retval ��
 */
void Write_Flash(void)
{
  uint16_t Data[17]={0};
	
	Data[0]=0x88;//д���־
	
	Data[1]=Ark[ONE].UseState;
	Data[1]=(Data[1]<<8)|Ark[ONE].UseNum;
	Data[2]=Ark[TWO].UseState;
	Data[2]=(Data[2]<<8)|Ark[TWO].UseNum;
	Data[3]=Ark[THREE].UseState;
	Data[3]=(Data[3]<<8)|Ark[THREE].UseNum;
	Data[4]=Ark[FOUR].UseState;
	Data[4]=(Data[4]<<8)|Ark[FOUR].UseNum;
	
	Data[5]=Ark[ONE].PickupCode[0];
	Data[5]=(Data[5]<<8)|Ark[ONE].PickupCode[1];
	Data[6]=Ark[ONE].PickupCode[2];
	Data[6]=(Data[6]<<8)|Ark[ONE].PickupCode[3];
	Data[7]=Ark[ONE].PickupCode[4];
	Data[7]=(Data[7]<<8)|Ark[ONE].PickupCode[5];
	
	Data[8]=Ark[TWO].PickupCode[0];
	Data[8]=(Data[8]<<8)|Ark[TWO].PickupCode[1];
	Data[9]=Ark[TWO].PickupCode[2];
	Data[9]=(Data[9]<<8)|Ark[TWO].PickupCode[3];
	Data[10]=Ark[TWO].PickupCode[4];
	Data[10]=(Data[10]<<8)|Ark[TWO].PickupCode[5];
	
	Data[11]=Ark[THREE].PickupCode[0];
	Data[11]=(Data[11]<<8)|Ark[THREE].PickupCode[1];
	Data[12]=Ark[THREE].PickupCode[2];
	Data[12]=(Data[12]<<8)|Ark[THREE].PickupCode[3];
	Data[13]=Ark[THREE].PickupCode[4];
	Data[13]=(Data[13]<<8)|Ark[THREE].PickupCode[5];

	Data[14]=Ark[FOUR].PickupCode[0];
	Data[14]=(Data[14]<<8)|Ark[FOUR].PickupCode[1];
	Data[15]=Ark[FOUR].PickupCode[2];
	Data[15]=(Data[15]<<8)|Ark[FOUR].PickupCode[3];
	Data[16]=Ark[FOUR].PickupCode[4];
	Data[16]=(Data[16]<<8)|Ark[FOUR].PickupCode[5];	
	
	STMFLASH_Write(FLASH_SAVE_ADDR,Data,17);
}
	
/**
 * @brief  ��ʱ��4�жϷ�����-10ms�ж�
 * @param  ��
 * @retval ��
 */
void TIM4_IRQHandler(void)
{
	static uint8_t 	Index,TimeCnt;	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == 1) 
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);		
		//Ϊÿһ�������ʱ���е��жϴ��������� 1��ֱ����ֵΪ 0
		for(Index=0;Index<TIMERCOUNT;Index++)
		{
			if(Timers[Index]>0)
		 	Timers[Index]--;			
		}
		//���ڿ�ݳ�ʱδȡ��-��ʼ��ʱ
		if(Ark[ONE].UseState||Ark[TWO].UseState||Ark[THREE].UseState||Ark[FOUR].UseState)
		{
			if(++TimeCnt>=200) //2s
			{
				TimeCnt=0;
				Para.Sec++;
			}
		
		}	
		else
			TimeCnt=0;
	}
}


