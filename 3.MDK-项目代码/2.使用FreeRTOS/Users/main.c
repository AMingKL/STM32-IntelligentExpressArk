/**
  *************************************************************************************************************************
  * @file    main.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-06
  * @brief   stm32-智能快递柜
  *************************************************************************************************************************
  * @attention								
	阅读Read me Txt文件

  *************************************************************************************************************************
  */
/* Includes -------------------------------------------------------------------------------------------------------------*/
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "limits.h"
#include "semphr.h"
#include "event_groups.h"
/* 开发板硬件bsp头文件 */
#include "myconfig.h" 
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
TaskHandle_t AppTaskCreate_Handle ;
TaskHandle_t UsartScreen_Task_Handle ;
TaskHandle_t Fingerprint_Task_Handle ;
TaskHandle_t Waiting_Network_Task_Handle ;
TaskHandle_t Message_Task_Handle ;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
TimerHandle_t 				xMyTimerHandle ;
SemaphoreHandle_t 		BinarySem_InputFingerprint_Handle;
QueueHandle_t 				Queue_Ark1UseNum,Queue_Ark2UseNum,Queue_Ark3UseNum,Queue_Ark4UseNum;
QueueHandle_t 				Queue_ArkPickupCode;
QueueHandle_t 				Queue_Phone,Queue_ArkWaitTime;
EventGroupHandle_t 		Event_ArkState_Handle,Event_ArkSMS_Handle;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
EventBits_t R_event_Ark1State,R_event_Ark2State,R_event_Ark3State,R_event_Ark4State;//定义一个事件接收变量 														
char* PointToPickupCode;		
char* PointToPhone;																				
char TempStr[20];	
 
/*
*************************************************************************
*                             函数声明
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
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
	BaseType_t xReturn = pdPASS;//定义一个创建信息返回值，默认为pdPASS 	
	BSP_Init();//硬件初始化
	
	xReturn=xTaskCreate((TaskFunction_t)AppTaskCreate,"AppTaskCreate",512,NULL,1,&AppTaskCreate_Handle);//创建起始任务
	xMyTimerHandle = xTimerCreate("mytimer", 1000, pdTRUE, NULL, MyTimerCallbackFunction);//创建软件定时器，定时周期1s,周期定时器
  if(xMyTimerHandle != NULL) { xTimerStart(xMyTimerHandle, 0);	}//忽略xTicksToWait
  if(pdPASS == xReturn) vTaskStartScheduler();// 启动任务，开启调度 
  else return -1;  
	while(1);
}

static void AppTaskCreate(void* parameter)
{
  taskENTER_CRITICAL();//进入临界区
			
	BinarySem_InputFingerprint_Handle=xSemaphoreCreateBinary();//创建二值信号量
	Queue_Phone=xQueueCreate(QUEUE_Phone_LEN,QUEUE_Phone_SIZE);//创建一个消息队列
	Queue_Ark1UseNum=xQueueCreate(QUEUE_Ark1UseNum_LEN,QUEUE_Ark1UseNum_SIZE);//创建一个消息队列
	Queue_Ark2UseNum=xQueueCreate(QUEUE_Ark2UseNum_LEN,QUEUE_Ark2UseNum_SIZE);//创建一个消息队列
	Queue_Ark3UseNum=xQueueCreate(QUEUE_Ark3UseNum_LEN,QUEUE_Ark3UseNum_SIZE);//创建一个消息队列
	Queue_Ark4UseNum=xQueueCreate(QUEUE_Ark4UseNum_LEN,QUEUE_Ark4UseNum_SIZE);//创建一个消息队列
	Queue_ArkPickupCode=xQueueCreate(QUEUE_ArkPickupCode_LEN,QUEUE_ArkPickupCode_SIZE);//创建一个消息队列
	Queue_ArkWaitTime=xQueueCreate(QUEUE_ArkWaitTime_LEN,QUEUE_ArkWaitTime_SIZE);//创建一个消息队列
	
	Event_ArkState_Handle	= xEventGroupCreate();//创建事件组	 
	Event_ArkSMS_Handle	= xEventGroupCreate();//创建事件组
	
	xTaskCreate((TaskFunction_t)Task_Waiting_Network,"Waiting_NetworkCreate",512,NULL,3,&Waiting_Network_Task_Handle); 	
	xTaskCreate((TaskFunction_t)Task_UsartScreen,"UsartScreenCreate",1024,NULL,2,&UsartScreen_Task_Handle);
	xTaskCreate((TaskFunction_t)Task_Fingerprint,"FingerprinCreate",1024,NULL,1,&Fingerprint_Task_Handle);
	xTaskCreate((TaskFunction_t)Task_Message,"MessageTaskCreate",1024,NULL,1,&Message_Task_Handle);
	
	vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务 
  taskEXIT_CRITICAL();//退出临界区
}

static void Task_UsartScreen(void* parameter)
{
	static uint8_t  Ark1_UseNum,Ark2_UseNum,Ark3_UseNum,Ark4_UseNum;
	static uint8_t  Ark1_PickupCode[6],Ark2_PickupCode[6],Ark3_PickupCode[6],Ark4_PickupCode[6];
	static uint8_t  Ark_phone[11];	
	static uint8_t  InputPickupCode[6];//输入的取件码	
  while(1)
	{
		
	 if(strstr((const char*)USART2_RX_BUF,"END")!=NULL)	//接收一组数据完毕
		{
			if(strstr((const char*)USART2_RX_BUF,"ZCKDY")!=NULL)//检索到进行注册快递员操作
			{				
				xSemaphoreGive( BinarySem_InputFingerprint_Handle );//给出二值信号量
				xTaskNotify(Fingerprint_Task_Handle,ENTRY_Courier_EVENT,eSetBits);//通知指纹任务录入快递员指纹
			}
			else if(strstr((const char*)USART2_RX_BUF,"ZCGLY")!=NULL)//检索到进行注册管理员操作
			{	
				xSemaphoreGive( BinarySem_InputFingerprint_Handle );//给出二值信号量
				xTaskNotify(Fingerprint_Task_Handle,ENTRY_Adminer_EVENT,eSetBits);//通知指纹任务录入管理员指纹
			}
			else if(strstr((const char*)USART2_RX_BUF,"1HTD")!=NULL)//检索到进行1号柜投递操作
			{	
				//UsartFormat:1HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");//获取字符串QJM的起始地址
				PointToPhone=strstr((const char*)USART2_RX_BUF,"1HTD");//获取字符串1HTD的起始地址
				strncpy((char *)Ark_phone, PointToPhone+4, 11);//通过偏移，获取手机号的地址，并将数据复制到快递柜1手机号的地址
				strncpy((char *)Ark1_PickupCode, PointToPickupCode+3, 6);//通过偏移，获取取件码的地址，并将数据复制到快递柜1取件码的地址
				xQueueSend( Queue_Phone,&Ark_phone,0 );//发送快递柜1手机号至消息队列
				xQueueSend( Queue_ArkPickupCode,&Ark1_PickupCode,0 );//发送快递柜1取件码至消息队列
				xEventGroupSetBits(Event_ArkState_Handle,Ark1_State_EVENT);//触发事件_快递柜1状态发生改变
				Ark1_UseNum++;
				xQueueSend( Queue_Ark1UseNum,&Ark1_UseNum,0 );//发送快递柜1使用次数至消息队列
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark1_SMS_EVENT);//触发事件_快递柜1发送短信			
			}		
			else if(strstr((const char*)USART2_RX_BUF,"2HTD")!=NULL)//检索到进行2号柜投递操作
			{	
				//UsartFormat:2HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"2HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark2_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//发送快递柜1手机号至消息队列
				xQueueSend( Queue_ArkPickupCode,&Ark2_PickupCode,0 );//发送快递柜1取件码至消息队列			
				xEventGroupSetBits(Event_ArkState_Handle,Ark2_State_EVENT);//触发事件_快递柜2状态发生改变
				Ark2_UseNum++;
				xQueueSend( Queue_Ark2UseNum,&Ark2_UseNum,0 );//发送快递柜2使用次数至消息队列			
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark2_SMS_EVENT);//触发事件_快递柜2发送短信
			}
			else if(strstr((const char*)USART2_RX_BUF,"3HTD")!=NULL)//检索到进行3号柜投递操作
			{	
				//UsartFormat:3HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"3HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark3_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//发送快递柜1手机号至消息队列
				xQueueSend( Queue_ArkPickupCode,&Ark3_PickupCode,0 );//发送快递柜1取件码至消息队列			
				xEventGroupSetBits(Event_ArkState_Handle,Ark3_State_EVENT);//触发事件_快递柜1状态发生改变
				Ark3_UseNum++;
				xQueueSend( Queue_Ark3UseNum,&Ark3_UseNum,0 );//发送快递柜3使用次数至消息队列
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark3_SMS_EVENT);//触发事件_快递柜3发送短信	
			}
			else if(strstr((const char*)USART2_RX_BUF,"4HTD")!=NULL)//检索到进行4号柜投递操作
			{	
				//UsartFormat:4HTD15983674750QJM123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJM");
				PointToPhone=strstr((const char*)USART2_RX_BUF,"4HTD");
				strncpy((char *)Ark_phone, PointToPhone+4, 11);
				strncpy((char *)Ark4_PickupCode, PointToPickupCode+3, 6);
				xQueueSend( Queue_Phone,&Ark_phone,0 );//发送快递柜1手机号至消息队列
				xQueueSend( Queue_ArkPickupCode,&Ark4_PickupCode,0 );//发送快递柜1取件码至消息队列			
				xEventGroupSetBits(Event_ArkState_Handle,Ark4_State_EVENT);//触发事件_快递柜1状态发生改变
				Ark4_UseNum++;
				xQueueSend( Queue_Ark4UseNum,&Ark4_UseNum,0 );//发送快递柜4使用次数至消息队列				
				xEventGroupSetBits(Event_ArkSMS_Handle,Ark4_SMS_EVENT);//触发事件_快递柜4发送短信
			}
			else if(strstr((const char*)USART2_RX_BUF,"QJ")!=NULL)//检索到进行取件操作
			{	
				//UsartFormat:QJ123456END
				PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJ");
				strncpy((char *)InputPickupCode, PointToPickupCode+2, 6);
				 if(strstr((char*)InputPickupCode,(char*)Ark1_PickupCode)!=NULL)
				 {
						RELAY1_ON;//模拟1号柜开
						Uart2SendTjcScreen("t6.txt=\"请到1号柜取出快递！\""); //显示 请到1号柜取出快递！
						xEventGroupClearBits(Event_ArkState_Handle,Ark1_State_EVENT);////取出后，快递柜恢复未使用状态
				 }			
				 else if(strstr((char*)InputPickupCode,(char*)Ark2_PickupCode)!=NULL)
				 {
						RELAY2_ON;//模拟2号柜开
						Uart2SendTjcScreen("t6.txt=\"请到2号柜取出快递！\""); //显示 请到2号柜取出快递！
						xEventGroupClearBits(Event_ArkState_Handle,Ark2_State_EVENT);//取出后，快递柜恢复未使用状态
				 }
				 else if(strstr((char*)InputPickupCode,(char*)Ark3_PickupCode)!=NULL)
				 {
						RELAY3_ON;//模拟3号柜开
						Uart2SendTjcScreen("t6.txt=\"请到3号柜取出快递！\""); //显示 请到3号柜取出快递！
						xEventGroupClearBits(Event_ArkState_Handle,Ark3_State_EVENT);//取出后，快递柜恢复未使用状态
				 }
				 else if(strstr((char*)InputPickupCode,(char*)Ark4_PickupCode)!=NULL)
				 {
						RELAY4_ON;//模拟4号柜开
						Uart2SendTjcScreen("t6.txt=\"请到4号柜取出快递！\""); //显示 请到4号柜取出快递！
						xEventGroupClearBits(Event_ArkState_Handle,Ark4_State_EVENT);//取出后，快递柜恢复未使用状态
				 }
				else
				{
						Uart2SendTjcScreen("t6.txt=\"取件码错误！\""); //显示 取件码错误！			
				}				 
			}		
			else if(strstr((const char*)USART2_RX_BUF,"JXTD")!=NULL)//检索到进行继续投递操作
			{	
				R_event_Ark1State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);							
				R_event_Ark2State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
				R_event_Ark3State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
				R_event_Ark4State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								

				(R_event_Ark1State==Ark1_State_EVENT)?(Uart2SendTjcScreen("t2.txt=\"已用\"")):(Uart2SendTjcScreen("t2.txt=\"未用\""));
				(R_event_Ark2State==Ark2_State_EVENT)?(Uart2SendTjcScreen("t3.txt=\"已用\"")):(Uart2SendTjcScreen("t3.txt=\"未用\""));
				(R_event_Ark3State==Ark3_State_EVENT)?(Uart2SendTjcScreen("t4.txt=\"已用\"")):(Uart2SendTjcScreen("t4.txt=\"未用\""));
				(R_event_Ark4State==Ark4_State_EVENT)?(Uart2SendTjcScreen("t5.txt=\"已用\"")):(Uart2SendTjcScreen("t5.txt=\"未用\""));			
			}			
			else if(strstr((const char*)USART2_RX_BUF,"YQCKD")!=NULL)//检索到已取出快递操作
			{	
				RELAYALL_OFF;//关闭所有快递柜门
			}	
			UartRec2Len=0; 
			memset((char*)USART2_RX_BUF,0,100);
		}	
		vTaskDelay(100);
	}
}



static void Task_Fingerprint(void* parameter)
{
	BaseType_t xReturn = pdPASS;// 定义一个创建信息返回值，默认为pdPASS
  uint32_t r_event = 0;  //定义一个事件接收变量	
	uint8_t  Ark1_R_UseNum,Ark2_R_UseNum,Ark3_R_UseNum,Ark4_R_UseNum;
	SearchResult seach;
  uint8_t ensure;
 while(1) 
 {
	 xReturn=xSemaphoreTake(BinarySem_InputFingerprint_Handle,0);
	 if(xReturn==pdPASS)//获取成功，录指纹
	 {
		//获取任务通知 ,没获取到则一直等待
		xReturn = xTaskNotifyWait(0x0,ULONG_MAX,&r_event,0);
	  if(xReturn==pdPASS)//获取成功，录指纹
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
	 else//刷指纹
	 {
    ensure = PS_GetImage();
    if(ensure == 0x00) //获取图像成功
    {
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //生成特征成功
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //搜索成功
        {
					switch(seach.pageID)
					{
							case 1:Uart2SendTjcScreen("page KDY_Interface");
										 R_event_Ark1State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);							
										 R_event_Ark2State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
										 R_event_Ark3State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								
										 R_event_Ark4State = xEventGroupWaitBits(Event_ArkState_Handle,Ark1_State_EVENT,pdFALSE, pdFALSE,0);								

										 (R_event_Ark1State==Ark1_State_EVENT)?(Uart2SendTjcScreen("t2.txt=\"已用\"")):(Uart2SendTjcScreen("t2.txt=\"未用\""));
										 (R_event_Ark2State==Ark2_State_EVENT)?(Uart2SendTjcScreen("t3.txt=\"已用\"")):(Uart2SendTjcScreen("t3.txt=\"未用\""));
										 (R_event_Ark3State==Ark3_State_EVENT)?(Uart2SendTjcScreen("t4.txt=\"已用\"")):(Uart2SendTjcScreen("t4.txt=\"未用\""));
										 (R_event_Ark4State==Ark4_State_EVENT)?(Uart2SendTjcScreen("t5.txt=\"已用\"")):(Uart2SendTjcScreen("t5.txt=\"未用\""));	
										 break;//切换到快递员投递界面
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
									 break;//切换到管理员管理界面
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
	EventBits_t R_event_Ark_SMS;//定义一个事件接收变量
	uint8_t  Ark1_phone[11],Ark2_phone[11],Ark3_phone[11],Ark4_phone[11];//定义一个接收消息的变量 
	BaseType_t xReturn = pdTRUE;//定义一个创建信息返回值，默认为pdTRUE
  uint8_t R_queue_PickUpCode[6];//定义一个接收消息的变量 
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
					 RELAY1_ON;//模拟开1号柜门		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//发送取件码短信
					 Send_Text_Sms(Ark1_phone);
					 delay_ms(1000);		
					 RELAY1_OFF;//短信发送完毕,模拟关1号柜门,代表投递完毕
					 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
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
					 RELAY2_ON;//模拟开2号柜门		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//发送取件码短信
					 Send_Text_Sms(Ark2_phone);
					 delay_ms(1000);		
					 RELAY2_OFF;//短信发送完毕,模拟关1号柜门,代表投递完毕
					 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
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
					 RELAY3_ON;//模拟开3号柜门		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//发送取件码短信
					 Send_Text_Sms(Ark3_phone);
					 delay_ms(1000);		
					 RELAY3_OFF;//短信发送完毕,模拟关3号柜门,代表投递完毕
					 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
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
					 RELAY4_ON;//模拟开4号柜门		
					 sprintf((char*)content,"Please pick up package with code %s",R_queue_PickUpCode);	//发送取件码短信
					 Send_Text_Sms(Ark4_phone);
					 delay_ms(1000);		
					 RELAY4_OFF;//短信发送完毕,模拟关4号柜门,代表投递完毕
					 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
				}
			}		  
		}	
		
		xQueueReceive( Queue_ArkWaitTime, &WaitHour, 0); 
		if(WaitHour>=10)//模拟10个小时未取，发送提示短信		
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
	BaseType_t xReturn = pdPASS;//定义一个创建信息返回值，默认为pdPASS 	
	static uint8_t TimeCnt,Sec;
	
	//关于快递超时未取回-开始计时
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
	NVIC_Config();//设置NVIC中断分组
	delay_init();//延时初始化
	Usart1_Init(9600);//串口1初始化 
	Usart2_Init(115200);//串口2初始化
	Usart3_Init(57600);//串口3初始化	
}

/**
 * @brief  查询/等待SIM卡网络注册
 * @param  无
 * @retval 无
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
		Uart2SendTjcScreen("page Main");//切换到主界面
		UartRec2Len=0; 
		memset((char*)USART2_RX_BUF,0,100);//清空串口2数据缓冲区		
		vTaskDelete(Waiting_Network_Task_Handle); //删除本身任务 		
	}
}

/*****************************************************END OF FILE*********************************************************/	

