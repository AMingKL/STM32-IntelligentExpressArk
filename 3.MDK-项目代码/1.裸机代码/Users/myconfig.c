/**
  *************************************************************************************************************************
  * @file    myconfig.c
  * @author  AMKL
  * @version V1.0
  * @date    2021-12-28
  * @brief   配置函数
  *************************************************************************************************************************
  * @attention
  *
  * 
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"


/* 变量定义 -----------------------------------------------------------------------------------------------------------------*/
unsigned char  Timers[TIMERCOUNT];
Para_InitTypeDef Para;	
Ark_InitTypeDef Ark[4]={{0,0,"654321"},
												{0,0,"654321"},
												{0,0,"654321"},
												{0,0,"654321"}};
char TempStr[20];
uint8_t InputPickupCode[6]="000000";//输入的取件码																	
char* PointToPickupCode;//指向取件码地址				
char* PointToPhone;	//指向手机地址																				

																				
/**																				
 * @brief  软件定时器
 * @param  xIndex-事件x，xTime-延时时间
 * @retval 无
 */
unsigned char xDelay(unsigned char xIndex,unsigned char xTime)
{
	unsigned char RetValue=TIMEISNOTUP;

	if(Timers[xIndex]==0)//判断指定软件定时器的计数器是否为 0
	{
		Timers[xIndex]=xTime;//得装计数器的值
		RetValue=TIMEISUP;	 //赋值返回值为：时间到	
	}
	return(RetValue);
}


/**
 * @brief  查询/等待SIM卡网络注册
 * @param  无
 * @retval 无
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
	
	Uart2SendTjcScreen("page Main");//切换到主界面
	Para.SendMessageFlag=NON;//初始化为非柜号数
	UartRec2Len=0; 
	memset((char*)USART2_RX_BUF,0,100);//清空串口2数据缓冲区
}


/**
 * @brief  关于串口屏消息处理，解析部分
 * @param  无
 * @retval 无
 */
void AboutUsartScreenProc(void)
{
  if(strstr((const char*)USART2_RX_BUF,"END")!=NULL)	//接收一组数据完毕
	{
		if(strstr((const char*)USART2_RX_BUF,"ZCKDY")!=NULL)//检索到进行注册快递员操作
		{	
			Para.InputFingerprintFlag=1;
			Para.FingerprintID=CourierID;//注册快递员
		}
		else if(strstr((const char*)USART2_RX_BUF,"ZCGLY")!=NULL)//检索到进行注册管理员操作
		{	
			Para.InputFingerprintFlag=1;
			Para.FingerprintID=AdminerID;//注册管理员
		}
		else if(strstr((const char*)USART2_RX_BUF,"1HTD")!=NULL)//检索到进行1号柜投递操作
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
		else if(strstr((const char*)USART2_RX_BUF,"2HTD")!=NULL)//检索到进行2号柜投递操作
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
		else if(strstr((const char*)USART2_RX_BUF,"3HTD")!=NULL)//检索到进行3号柜投递操作
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
		else if(strstr((const char*)USART2_RX_BUF,"4HTD")!=NULL)//检索到进行4号柜投递操作
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
		else if(strstr((const char*)USART2_RX_BUF,"QJ")!=NULL)//检索到进行取件操作
		{	
			//QJ123456END
			PointToPickupCode=strstr((const char*)USART2_RX_BUF,"QJ");
			strncpy((char *)InputPickupCode, PointToPickupCode+2, 6);
			 if(strstr((char*)InputPickupCode,(char*)Ark[ONE].PickupCode)!=NULL)
			 {
					RELAY1_ON;//模拟1号柜开
					Uart2SendTjcScreen("t6.txt=\"请到1号柜取出快递！\""); //显示 请到1号柜取出快递！
					Ark[ONE].UseState=EMPTY;//取出后，快递柜恢复未使用状态
			 }			
			 else if(strstr((char*)InputPickupCode,(char*)Ark[TWO].PickupCode)!=NULL)
			 {
					RELAY2_ON;//模拟2号柜开
					Uart2SendTjcScreen("t6.txt=\"请到2号柜取出快递！\""); //显示 请到2号柜取出快递！
					Ark[TWO].UseState=EMPTY;//取出后，快递柜恢复未使用状态
			 }
			 else if(strstr((char*)InputPickupCode,(char*)Ark[THREE].PickupCode)!=NULL)
			 {
					RELAY3_ON;//模拟3号柜开
					Uart2SendTjcScreen("t6.txt=\"请到3号柜取出快递！\""); //显示 请到3号柜取出快递！
					Ark[THREE].UseState=EMPTY;//取出后，快递柜恢复未使用状态
			 }
			 else if(strstr((char*)InputPickupCode,(char*)Ark[FOUR].PickupCode)!=NULL)
			 {
					RELAY4_ON;//模拟4号柜开
					Uart2SendTjcScreen("t6.txt=\"请到4号柜取出快递！\""); //显示 请到4号柜取出快递！
					Ark[FOUR].UseState=EMPTY;//取出后，快递柜恢复未使用状态
			 }
			else
			{
				  Uart2SendTjcScreen("t6.txt=\"取件码错误！\""); //显示 取件码错误！			
			}				 
		}		
		else if(strstr((const char*)USART2_RX_BUF,"JXTD")!=NULL)//检索到进行继续投递操作
		{	
			(Ark[ONE].UseState==FULL)?(Uart2SendTjcScreen("t2.txt=\"已用\"")):(Uart2SendTjcScreen("t2.txt=\"未用\""));
			(Ark[TWO].UseState==FULL)?(Uart2SendTjcScreen("t3.txt=\"已用\"")):(Uart2SendTjcScreen("t3.txt=\"未用\""));
			(Ark[THREE].UseState==FULL)?(Uart2SendTjcScreen("t4.txt=\"已用\"")):(Uart2SendTjcScreen("t4.txt=\"未用\""));
			(Ark[FOUR].UseState==FULL)?(Uart2SendTjcScreen("t5.txt=\"已用\"")):(Uart2SendTjcScreen("t5.txt=\"未用\""));			
		}			
		else if(strstr((const char*)USART2_RX_BUF,"YQCKD")!=NULL)//检索到已取出快递操作
		{	
			RELAYALL_OFF;//关闭所有快递柜门
		}	
		UartRec2Len=0; 
		memset((char*)USART2_RX_BUF,0,100);
		Write_Flash(); //保存参数		
	}
}

/**
 * @brief  录指纹，指纹开锁操作部分
 * @param  无
 * @retval 无
 */
void AboutFingerprintProc(void)
{
	if(Para.InputFingerprintFlag==1)//录指纹
	{		
		Add_FR();
		Para.InputFingerprintFlag=0;
	}
	else//进行指纹对比
	{
	  press_FR();
	}
}

/**
 * @brief  关于发送取件短信，超时未领取提示短信快递部分
 * @param  无
 * @retval 无
 */
void AboutCourierProc(void)
{
  switch (Para.SendMessageFlag)
	{
	  case ONE:	 RELAY1_ON;//模拟开1号柜门		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[ONE].PickupCode);	//发送取件码短信
							 Send_Text_Sms(NO1_phone);
							 delay_ms(1000);		
							 RELAY1_OFF;//短信发送完毕,模拟关1号柜门,代表投递完毕
							 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
							 Para.SendMessageFlag=NON;break;
		case TWO:	 RELAY2_ON;//模拟开2号柜门		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[TWO].PickupCode);	//发送取件码短信
							 Send_Text_Sms(NO2_phone);
							 delay_ms(1000);		
							 RELAY2_OFF;//短信发送完毕,模拟关2号柜门,代表投递完毕
							 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
							 Para.SendMessageFlag=NON;break;
		case THREE:RELAY3_ON;//模拟开3号柜门		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[THREE].PickupCode);	//发送取件码短信
							 Send_Text_Sms(NO3_phone);
							 delay_ms(1000);		
							 RELAY3_OFF;//短信发送完毕,模拟关3号柜门,代表投递完毕
							 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
							 Para.SendMessageFlag=NON;break;
		case FOUR: RELAY4_ON;//模拟开4号柜门		
							 sprintf((char*)content,"Please pick up package with code %s",Ark[FOUR].PickupCode);	//发送取件码短信
							 Send_Text_Sms(NO4_phone);
							 delay_ms(1000);		
							 RELAY4_OFF;//短信发送完毕,模拟关4号柜门,代表投递完毕
							 Uart2SendTjcScreen("t6.txt=\"投递成功\""); //显示 “投递成功”		
							 Para.SendMessageFlag=NON;break;
		default:break;	
	}
	
	//超时未领取提示短信
	if(Para.Sec==180)//6分钟计时到，模拟1天未取回，发送提醒短信
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
 * @brief  Flash读取
 * @param  无
 * @retval 无
 */
void Read_Flash(void)
{
   uint16_t Data[17]={0};
	 STMFLASH_Read(FLASH_SAVE_ADDR,Data,17);
	 
	 //判断是否第一次上电
	 if(Data[0]!=0x88)//第一次上电 写入数据
	 {
		 Write_Flash();
		 printf("first");
	 }
	 else//恢复数据
	 {
		 /*各快递柜的使用状态以及使用次数*/
		 Ark[ONE].UseState=Data[1]>>8; 
		 Ark[ONE].UseNum=Data[1]; 	 
		 Ark[TWO].UseState=Data[2]>>8; 
		 Ark[TWO].UseNum=Data[2]; 		 
		 Ark[THREE].UseState=Data[3]>>8; 
		 Ark[THREE].UseNum=Data[3]; 		 
		 Ark[FOUR].UseState=Data[4]>>8; 
		 Ark[FOUR].UseNum=Data[4];
		 /*各快递柜的取件码*/
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
 * @brief  Flash写入
 * @param  无
 * @retval 无
 */
void Write_Flash(void)
{
  uint16_t Data[17]={0};
	
	Data[0]=0x88;//写入标志
	
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
 * @brief  定时器4中断服务函数-10ms中断
 * @param  无
 * @retval 无
 */
void TIM4_IRQHandler(void)
{
	static uint8_t 	Index,TimeCnt;	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == 1) 
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);		
		//为每一个软件定时器中的中断次数计数减 1，直到其值为 0
		for(Index=0;Index<TIMERCOUNT;Index++)
		{
			if(Timers[Index]>0)
		 	Timers[Index]--;			
		}
		//关于快递超时未取回-开始计时
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


