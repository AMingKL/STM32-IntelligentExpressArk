/**
  *************************************************************************************************************************
  * @file    main.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-06
  * @brief   stm32-���ܿ�ݹ�
  *************************************************************************************************************************
  * @attention								
	�Ķ�Read me Txt�ļ�

  *************************************************************************************************************************
  */
/* Includes -------------------------------------------------------------------------------------------------------------*/
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "limits.h"
#include "semphr.h"
#include "event_groups.h"
/* ������Ӳ��bspͷ�ļ� */
#include "myconfig.h" 
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
TaskHandle_t AppTaskCreate_Handle ;
TaskHandle_t UsartScreen_Task_Handle ;
TaskHandle_t Fingerprint_Task_Handle ;
TaskHandle_t Waiting_Network_Task_Handle ;
TaskHandle_t Message_Task_Handle ;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
TimerHandle_t 				xMyTimerHandle ;
SemaphoreHandle_t 		BinarySem_InputFingerprint_Handle;
QueueHandle_t 				Queue_Ark1UseNum,Queue_Ark2UseNum,Queue_Ark3UseNum,Queue_Ark4UseNum;
QueueHandle_t 				Queue_ArkPickupCode;
QueueHandle_t 				Queue_Phone,Queue_ArkWaitTime;
EventGroupHandle_t 		Event_ArkState_Handle,Event_ArkSMS_Handle;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
EventBits_t R_event_Ark1State,R_event_Ark2State,R_event_Ark3State,R_event_Ark4State;//����һ���¼����ձ��� 														
char* PointToPickupCode;		
char* PointToPhone;																				
char TempStr[20];	
 
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
void BSP_Init(void);
void Task_Message(void* parameter);
void AppTaskCreate(void* parameter);
void Task_UsartScreen(void* parameter);
void Task_Fingerprint(void* parameter);
void Task_Waiting_Network(void* parameter);
void MyTimerCallbackFunction(void* parameter);



/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
	BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS 	
	BSP_Init();//Ӳ����ʼ��
	
	xReturn=xTaskCreate((TaskFunction_t)AppTaskCreate,"AppTaskCreate",512,NULL,1,&AppTaskCreate_Handle);//������ʼ����
	xMyTimerHandle = xTimerCreate("mytimer", 1000, pdTRUE, NULL, MyTimerCallbackFunction);//���������ʱ������ʱ����1s,���ڶ�ʱ��
  if(xMyTimerHandle != NULL) { xTimerStart(xMyTimerHandle, 0);	}//����xTicksToWait
  if(pdPASS == xReturn) vTaskStartScheduler();// �������񣬿������� 
  else return -1;  
	while(1);
}

static void AppTaskCreate(void* parameter)
{
  taskENTER_CRITICAL();//�����ٽ���
			
	BinarySem_InputFingerprint_Handle=xSemaphoreCreateBinary();//������ֵ�ź���
	Queue_Phone=xQueueCreate(QUEUE_Phone_LEN,QUEUE_Phone_SIZE);//����һ����Ϣ����
	Queue_Ark1UseNum=xQueueCreate(QUEUE_Ark1UseNum_LEN,QUEUE_Ark1UseNum_SIZE);//����һ����Ϣ����
	Queue_Ark2UseNum=xQueueCreate(QUEUE_Ark2UseNum_LEN,QUEUE_Ark2UseNum_SIZE);//����һ����Ϣ����
	Queue_Ark3UseNum=xQueueCreate(QUEUE_Ark3UseNum_LEN,QUEUE_Ark3UseNum_SIZE);//����һ����Ϣ����
	Queue_Ark4UseNum=xQueueCreate(QUEUE_Ark4UseNum_LEN,QUEUE_Ark4UseNum_SIZE);//����һ����Ϣ����
	Queue_ArkPickupCode=xQueueCreate(QUEUE_ArkPickupCode_LEN,QUEUE_ArkPickupCode_SIZE);//����һ����Ϣ����
	Queue_ArkWaitTime=xQueueCreate(QUEUE_ArkWaitTime_LEN,QUEUE_ArkWaitTime_SIZE);//����һ����Ϣ����
	
	Event_ArkState_Handle	= xEventGroupCreate();//�����¼���	 
	Event_ArkSMS_Handle	= xEventGroupCreate();//�����¼���
	
	xTaskCreate((TaskFunction_t)Task_Waiting_Network,"Waiting_NetworkCreate",512,NULL,3,&Waiting_Network_Task_Handle); 	
	xTaskCreate((TaskFunction_t)Task_UsartScreen,"UsartScreenCreate",1024,NULL,2,&UsartScreen_Task_Handle);
	xTaskCreate((TaskFunction_t)Task_Fingerprint,"FingerprinCreate",1024,NULL,1,&Fingerprint_Task_Handle);
	xTaskCreate((TaskFunction_t)Task_Message,"MessageTaskCreate",1024,NULL,1,&Message_Task_Handle);
	
	vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate���� 
  taskEXIT_CRITICAL();//�˳��ٽ���
}

static void Task_UsartScreen(void* parameter)
{
	static uint8_t  Ark1_UseNum,Ark2_UseNum,Ark3_UseNum,Ark4_UseNum;
	static uint8_t  Ark1_PickupCode[6],Ark2_PickupCode[6],Ark3_PickupCode[6],Ark4_PickupCode[6];
	static uint8_t  Ark_phone[11];	
	static uint8_t  InputPickupCode[6];//�����ȡ����	
  while(1)
	{
		
	 if(strstr((const char*)USART2_RX_BUF,"END")!=NULL)	//����һ���������
		{
			if(strstr((const char*)USART2_RX_BUF,"ZCKDY")!=NULL)//����������ע����Ա����
			{				
				xSemaphoreGive( BinarySem_InputFingerprint_Handle );//������ֵ�ź���
				xTaskNotify(Fingerprint_Task_Handle,ENTRY_Courier_EVENT,eSetBits);//ָ֪ͨ������¼����Աָ��
			}
			else if(strstr((const char*)USART2_RX_BUF,"ZCGLY")!=NULL)//����������ע�����Ա����
			{	
				xSemaphoreGive( BinarySem_InputFingerprint_Handle );//������ֵ�ź���
				xTaskNotify(Fingerprint_Task_Handle,ENTRY_Adminer_EVENT,eSetBits);//ָ֪ͨ������¼�����Աָ��
			}
			else if(strstr((const char*)USART2_RX_BUF,"1HTD")!=NULL)//����������1�Ź�Ͷ�ݲ���
			{	
				//UsartFormat:1HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");//��ȡ�ַ���QJM����ʼ��ַ
				PointToPhone=strstr((const char*)USART2_RX_BUF,"1HTD");//��ȡ�ַ���1HTD����ʼ��ַ
				strncpy((char *)Ark_phone, PointToPhone+4, 11);//ͨ��ƫ�ƣ���ȡ�ֻ��ŵĵ�ַ���������ݸ��Ƶ���ݹ�1�ֻ��ŵĵ�ַ
				strncpy((char *)Ark1_PickupCode, PointToPickupCode+3, 6);//ͨ��ƫ�ƣ���ȡȡ����ĵ�ַ���������ݸ��Ƶ���ݹ�1ȡ����ĵ�ַ
				xQueueSend( Queue_Phone,&Ark_phone,0 );//���Ϳ�ݹ�1�ֻ�������Ϣ����
				xQueueSend( Queue_ArkPickupCode,&Ark1_PickupCode,0 );//���Ϳ�ݹ�1ȡ��������Ϣ����
				xEventGroupSetBits(Event_ArkState_Handle,Ark1_State_EVENT);//�����¼�_��ݹ�1״̬�����ı�
				Ark1_UseNum++;
				xQueueSend( Queue_Ark1UseNum,&Ark1_UseNum,0 );//���Ϳ�ݹ�1ʹ�ô�������Ϣ����
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark1_SMS_EVENT);//�����¼�_��ݹ�1���Ͷ���			
			}		
			else if(strstr((const char*)USART2_RX_BUF,"2HTD")!=NULL)//����������2�Ź�Ͷ�ݲ���
			{	
				//UsartFormat:2HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"2HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark2_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//���Ϳ�ݹ�1�ֻ�������Ϣ����
				xQueueSend( Queue_ArkPickupCode,&Ark2_PickupCode,0 );//���Ϳ�ݹ�1ȡ��������Ϣ����			
				xEventGroupSetBits(Event_ArkState_Handle,Ark2_State_EVENT);//�����¼�_��ݹ�2״̬�����ı�
				Ark2_UseNum++;
				xQueueSend( Queue_Ark2UseNum,&Ark2_UseNum,0 );//���Ϳ�ݹ�2ʹ�ô�������Ϣ����			
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark2_SMS_EVENT);//�����¼�_��ݹ�2���Ͷ���
			}
			else if(strstr((const char*)USART2_RX_BUF,"3HTD")!=NULL)//����������3�Ź�Ͷ�ݲ���
			{	
				//UsartFormat:3HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"3HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark3_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//���Ϳ�ݹ�1�ֻ�������Ϣ����
				xQueueSend( Queue_ArkPickupCode,&Ark3_PickupCode,0 );//���Ϳ�ݹ�1ȡ��������Ϣ����			
				xEventGroupSetBits(Event_ArkState_Handle,Ark3_State_EVENT);//�����¼�_��ݹ�1״̬�����ı�
				Ark3_UseNum++;
				xQueueSend( Queue_Ark3UseNum,&Ark3_UseNum,0 );//���Ϳ�ݹ�3ʹ�ô�������Ϣ����
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark3_SMS_EVENT);//�����¼�_��ݹ�3���Ͷ���	
			}
			else if(strstr((const char*)USART2_RX_BUF,"4HTD")!=NULL)//����������4�Ź�Ͷ�ݲ���
			{	
				//UsartFormat:4HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"4HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark4_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//���Ϳ�ݹ�1�ֻ�������Ϣ����
				xQueueSend( Queue_ArkPickupCode,&Ark4_PickupCode,0 );//���Ϳ�ݹ�1ȡ��������Ϣ����			
				xEventGroupSetBits(Event_ArkState_Handle,Ark4_State_EVENT);//�����¼�_��ݹ�1״̬�����ı�
				Ark4_UseNum++;
				xQueueSend( Queue_Ark4UseNum,&Ark4_UseNum,0 );//���Ϳ�ݹ�4ʹ�ô�������Ϣ����				
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark4_SMS_EVENT);//�����¼�_��ݹ�4���Ͷ���
			}
			else if(strstr((const char*)USART2_RX_BUF,"QJ")!=NULL)//����������ȡ������
			{	
				//UsartFormat:QJ123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJ");
				strncpy((char *)InputPickupCode, PointToPickupCode+2, 6);
				 if(strstr((char*)InputPickupCode,(char*)Ark1_PickupCode)!=NULL)
				 {
						RELAY1_ON;//ģ��1�Ź�
						Uart2SendTjcScreen("t6.txt=\"�뵽1�Ź�ȡ����ݣ�\""); //��ʾ �뵽1�Ź�ȡ����ݣ�
						xEventGroupClearBits(Event_ArkState_Handle,Ark1_State_EVENT);////ȡ���󣬿�ݹ�ָ�δʹ��״̬
				 }			
				 else if(strstr((char*)InputPickupCode,(char*)Ark2_PickupCode)!=NULL)
				 {
						RELAY2_ON;//ģ��2�Ź�
						Uart2SendTjcScreen("t6.txt=\"�뵽2�Ź�ȡ����ݣ�\""); //��ʾ �뵽2�Ź�ȡ����ݣ�
						xEventGroupClearBits(Event_ArkState_Handle,Ark2_State_EVENT);//ȡ���󣬿�ݹ�ָ�δʹ��״̬
				 }
				 else if(strstr((char*)InputPickupCode,(char*)Ark3_PickupCode)!=NULL)
				 {
						RELAY3_ON;//ģ��3�Ź�
						Uart2SendTjcScreen("t6.txt=\"�뵽3�Ź�ȡ����ݣ�\""); //��ʾ �뵽3�Ź�ȡ����ݣ�
						xEventGroupClearBits(Event_ArkState_Handle,Ark3_State_EVENT);//ȡ���󣬿�ݹ�ָ�δʹ��״̬
				 }
				 else if(strstr((char*)InputPickupCode,(char*)Ark4_PickupCode)!=NULL)
				 {
						RELAY4_ON;//ģ��4�Ź�
						Uart2SendTjcScreen("t6.txt=\"�뵽4�Ź�ȡ����ݣ�\""); //��ʾ �뵽4�Ź�ȡ����ݣ�
						xEventGroupClearBits(Event_ArkState_Handle,Ark4_State_EVENT);//ȡ���󣬿�ݹ�ָ�δʹ��״̬
				 }
				else
				{
						Uart2SendTjcScreen("t6.txt=\"ȡ�������\""); //��ʾ ȡ�������			
				}				 
			}		
			else if(strstr((const char*)USART2_RX_BUF,"JXTD")!=NULL)//���������м���Ͷ�ݲ���
			{	
				R_event_Ark1State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);							
				R_event_Ark2State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
				R_event_Ark3State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
				R_event_Ark4State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								

				(R_event_Ark1State==Ark1_State_EVENT)?(Uart2SendTjcScreen("t2.txt=\"����\"")):(Uart2SendTjcScreen("t2.txt=\"δ��\""));
				(R_event_Ark2State==Ark2_State_EVENT)?(Uart2SendTjcScreen("t3.txt=\"����\"")):(Uart2SendTjcScreen("t3.txt=\"δ��\""));
				(R_event_Ark3State==Ark3_State_EVENT)?(Uart2SendTjcScreen("t4.txt=\"����\"")):(Uart2SendTjcScreen("t4.txt=\"δ��\""));
				(R_event_Ark4State==Ark4_State_EVENT)?(Uart2SendTjcScreen("t5.txt=\"����\"")):(Uart2SendTjcScreen("t5.txt=\"δ��\""));			
			}			
			else if(strstr((const char*)USART2_RX_BUF,"YQCKD")!=NULL)//��������ȡ����ݲ���
			{	
				RELAYALL_OFF;//�ر����п�ݹ���
			}	
			UartRec2Len=0; 
			memset((char*)USART2_RX_BUF,0,100);
		}	
		vTaskDelay(100);
	}
}



static void Task_Fingerprint(void* parameter)
{
	BaseType_t xReturn = pdPASS;// ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  uint32_t r_event = 0;  //����һ���¼����ձ���	
	uint8_t  Ark1_R_UseNum,Ark2_R_UseNum,Ark3_R_UseNum,Ark4_R_UseNum;
	SearchResult seach;
  uint8_t ensure;
 while(1) 
 {
	 xReturn=xSemaphoreTake(BinarySem_InputFingerprint_Handle,0);
	 if(xReturn==pdPASS)//��ȡ�ɹ���¼ָ��
	 {
		//��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		xReturn = xTaskNotifyWait(0x0,ULONG_MAX,&r_event,0);
	  if(xReturn==pdPASS)//��ȡ�ɹ���¼ָ��
		{
			if(r_event==ENTRY_Courier_EVENT)
			{
			 Add_Courier_FR();
			}
			else if(r_event==ENTRY_Adminer_EVENT)
			{
			 Add_Adminer_FR();
			}
		}
	 }
	 else//ˢָ��
	 {
    ensure = PS_GetImage();
    if(ensure == 0x00) //��ȡͼ��ɹ�
    {
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //���������ɹ�
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //�����ɹ�
        {
					switch(seach.pageID)
					{
							case 1:Uart2SendTjcScreen("page KDY_Interface");
										 R_event_Ark1State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);							
										 R_event_Ark2State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
										 R_event_Ark3State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
										 R_event_Ark4State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								

										 (R_event_Ark1State==Ark1_State_EVENT)?(Uart2SendTjcScreen("t2.txt=\"����\"")):(Uart2SendTjcScreen("t2.txt=\"δ��\""));
										 (R_event_Ark2State==Ark2_State_EVENT)?(Uart2SendTjcScreen("t3.txt=\"����\"")):(Uart2SendTjcScreen("t3.txt=\"δ��\""));
										 (R_event_Ark3State==Ark3_State_EVENT)?(Uart2SendTjcScreen("t4.txt=\"����\"")):(Uart2SendTjcScreen("t4.txt=\"δ��\""));
										 (R_event_Ark4State==Ark4_State_EVENT)?(Uart2SendTjcScreen("t5.txt=\"����\"")):(Uart2SendTjcScreen("t5.txt=\"δ��\""));	
										 break;//�л������ԱͶ�ݽ���
						case 2:	 Uart2SendTjcScreen("page AdminInterface");
										 xQueueReceive(Queue_Ark1UseNum,&Ark1_R_UseNum,0);
										 xQueueReceive(Queue_Ark2UseNum,&Ark2_R_UseNum,0);
										 xQueueReceive(Queue_Ark3UseNum,&Ark3_R_UseNum,0);
										 xQueueReceive(Queue_Ark4UseNum,&Ark4_R_UseNum,0);				
										 sprintf(TempStr,"t4.txt=\"%d\"",Ark1_R_UseNum);
										 Uart2SendTjcScreen((unsigned char *)TempStr);		
										 sprintf(TempStr,"t5.txt=\"%d\"",Ark2_R_UseNum);
										 Uart2SendTjcScreen((unsigned char *)TempStr);	
										 sprintf(TempStr,"t7.txt=\"%d\"",Ark3_R_UseNum);
										 Uart2SendTjcScreen((unsigned char *)TempStr);	
										 sprintf(TempStr,"t9.txt=\"%d\"",Ark4_R_UseNum);
										 Uart2SendTjcScreen((unsigned char *)TempStr);							
									 break;//�л�������Ա�������
						default:break;											
					}					
					vTaskDelay(20);
					break;										
        }
			}
		}
	}
 	 vTaskDelay(30);
 }
}




static void Task_Message(void* parameter)
{
	EventBits_t R_event_Ark_SMS;//����һ���¼����ձ���
	uint8_t  Ark1_phone[11],Ark2_phone[11],Ark3_phone[11],Ark4_phone[11];//����һ��������Ϣ�ı��� 
	BaseType_t xReturn = pdTRUE;//����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE
  uint8_t R_queue_PickUpCode[6];//����һ��������Ϣ�ı��� 
	static uint8_t WaitHour;
	
	while(1)
	{
		R_event_Ark_SMS=xEventGroupWaitBits(Event_ArkSMS_Handle,Ark1_SMS_EVENT|Ark2_SMS_EVENT|Ark3_SMS_EVENT|Ark4_SMS_EVENT,pdFALSE, pdFALSE,portMAX_DELAY);	
		if(R_event_Ark_SMS==Ark1_SMS_EVENT)
		{
			xReturn = xQueueReceive( Queue_Phone, &Ark1_phone, portMAX_DELAY);  
			if(pdTRUE == xReturn)
			{
			 xReturn = xQueueReceive( Queue_ArkPickupCode, &R_queue_PickUpCode, portMAX_DELAY);  
				if(pdTRUE == xReturn)
				{
					 RELAY1_ON;//ģ�⿪1�Ź���		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//����ȡ�������
					 Send_Text_Sms(Ark1_phone);
					 delay_ms(1000);		
					 RELAY1_OFF;//���ŷ������,ģ���1�Ź���,����Ͷ�����
					 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
				}
			}		  
		}
		else if(R_event_Ark_SMS==Ark2_SMS_EVENT)
		{
			xReturn = xQueueReceive( Queue_Phone, &Ark2_phone, portMAX_DELAY);  
			if(pdTRUE == xReturn)
			{
			 xReturn = xQueueReceive( Queue_ArkPickupCode, &R_queue_PickUpCode, portMAX_DELAY);  
				if(pdTRUE == xReturn)
				{
					 RELAY2_ON;//ģ�⿪2�Ź���		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//����ȡ�������
					 Send_Text_Sms(Ark2_phone);
					 delay_ms(1000);		
					 RELAY2_OFF;//���ŷ������,ģ���1�Ź���,����Ͷ�����
					 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
				}
			}		  
		}		
		else if(R_event_Ark_SMS==Ark3_SMS_EVENT)
		{
			xReturn = xQueueReceive( Queue_Phone, &Ark3_phone, portMAX_DELAY);  
			if(pdTRUE == xReturn)
			{
			 xReturn = xQueueReceive( Queue_ArkPickupCode, &R_queue_PickUpCode, portMAX_DELAY);  
				if(pdTRUE == xReturn)
				{
					 RELAY3_ON;//ģ�⿪3�Ź���		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//����ȡ�������
					 Send_Text_Sms(Ark3_phone);
					 delay_ms(1000);		
					 RELAY3_OFF;//���ŷ������,ģ���3�Ź���,����Ͷ�����
					 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
				}
			}		  
		}		
		else if(R_event_Ark_SMS==Ark4_SMS_EVENT)
		{
			xReturn = xQueueReceive( Queue_Phone, &Ark4_phone, portMAX_DELAY);  
			if(pdTRUE == xReturn)
			{
			 xReturn = xQueueReceive( Queue_ArkPickupCode, &R_queue_PickUpCode, portMAX_DELAY);  
				if(pdTRUE == xReturn)
				{
					 RELAY4_ON;//ģ�⿪4�Ź���		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//����ȡ�������
					 Send_Text_Sms(Ark4_phone);
					 delay_ms(1000);		
					 RELAY4_OFF;//���ŷ������,ģ���4�Ź���,����Ͷ�����
					 Uart2SendTjcScreen("t6.txt=\"Ͷ�ݳɹ�\""); //��ʾ ��Ͷ�ݳɹ���		
				}
			}		  
		}	
		
		xQueueReceive( Queue_ArkWaitTime, &WaitHour, 0); 
		if(WaitHour>=10)//ģ��10��Сʱδȡ��������ʾ����		
		{
			R_event_Ark1State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);							
			R_event_Ark2State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
			R_event_Ark3State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
			R_event_Ark4State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								

			if(R_event_Ark1State==Ark1_State_EVENT)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(Ark1_phone);
			 }
			if(R_event_Ark2State==Ark2_State_EVENT)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(Ark2_phone);			
			}			
			if(R_event_Ark3State==Ark3_State_EVENT)			
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(Ark3_phone);			
		  }
			if(R_event_Ark4State==Ark4_State_EVENT)
			{
				strcpy((char *)content,"Your courier has not been picked up for more than 24 hours, please pick it up as soon as possible");
		    Send_Text_Sms(Ark4_phone);			
			}
			WaitHour=0;
			xQueueSend( Queue_ArkWaitTime,&WaitHour,0 );	
		}	
		vTaskDelay(60);		
	}
}

void MyTimerCallbackFunction(void* parameter)
{
	BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS 	
	static uint8_t TimeCnt,Sec;
	
	//���ڿ�ݳ�ʱδȡ��-��ʼ��ʱ
	xReturn=xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT|Ark2_State_EVENT|Ark3_State_EVENT|Ark4_State_EVENT,pdFALSE, pdFALSE,0);
	
	if(xReturn==pdPASS)
	{
		if(++TimeCnt>=60) //60s
		{
			TimeCnt=0;
			Sec++;
			xQueueSend( Queue_ArkWaitTime,&Sec,0 );
		}		
	}	
	else
		TimeCnt=0;
}


static void BSP_Init(void)
{
	NVIC_Config();//����NVIC�жϷ���
	delay_init();//��ʱ��ʼ��
	Usart1_Init(9600);//����1��ʼ�� 
	Usart2_Init(115200);//����2��ʼ��
	Usart3_Init(57600);//����3��ʼ��	
}

/**
 * @brief  ��ѯ/�ȴ�SIM������ע��
 * @param  ��
 * @retval ��
 */
void Task_Waiting_Network(void* parameter)
{
	uint8_t i;
	while(1)
	{
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
		UartRec2Len=0; 
		memset((char*)USART2_RX_BUF,0,100);//��մ���2���ݻ�����		
		vTaskDelete(Waiting_Network_Task_Handle); //ɾ���������� 		
	}
}

/*****************************************************END OF FILE*********************************************************/	

