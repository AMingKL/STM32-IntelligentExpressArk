/**
  *************************************************************************************************************************
  * @file    myconfig.h
  * @author  AMKL
  * @version V1.0
  * @date    2022-01-15
  * @brief   �������ţ����ò���,ͷ�ļ�
  *************************************************************************************************************************
  * @attention
  * 
  * 
  * 
  *************************************************************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------------------------------------------------*/
#ifndef __MYCONFIG_H
#define	__MYCONFIG_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "tim3.h"
#include "gpio.h"
#include "nvic.h"
#include "as608.h"
#include "usart1.h"
#include "usart2.h" 
#include "usart3.h"
#include "sim800c.h"
#include "stdio.h"
#include "string.h"
/* 宏定义 -----------------------------------------------------------------------------------------------------------------*/
#define ONE			(0)//1号柜
#define TWO			(1)//2号柜
#define THREE		(2)//3号柜
#define FOUR		(3)//4号柜
#define NON     (4)//非快递柜


#define FULL		(1)//柜已使用
#define EMPTY		(0)//柜未使用

#define ENTRY_Courier_EVENT		(0x01 << 0)//设置事件录入快递员掩码的位0
#define ENTRY_Adminer_EVENT		(0x01 << 1)//设置事件录入管理员掩码的位1

#define Ark1_State_EVENT		(0x01 << 0)//设置事件1号柜子使用状态掩码的位0
#define Ark2_State_EVENT		(0x01 << 1)//设置事件2号柜子使用状态掩码的位1
#define Ark3_State_EVENT		(0x01 << 2)//设置事件3号柜子使用状态掩码的位2
#define Ark4_State_EVENT		(0x01 << 3)//设置事件4号柜子使用状态掩码的位3

#define Ark1_SMS_EVENT		(0x01 << 0)//设置事件1号柜子发送短信掩码的位0
#define Ark2_SMS_EVENT		(0x01 << 1)//设置事件1号柜子发送短信掩码的位1
#define Ark3_SMS_EVENT		(0x01 << 2)//设置事件1号柜子发送短信掩码的位2
#define Ark4_SMS_EVENT		(0x01 << 3)//设置事件1号柜子发送短信掩码的位3

#define  QUEUE_ArkPickupCode_LEN    1   // 队列(1号柜子取件码)的长度，最大可包含1个消息 
#define  QUEUE_ArkPickupCode_SIZE   6   // 队列大小


#define  QUEUE_Ark1UseNum_LEN    1   // 队列(1号柜子使用次数)的长度，最大可包含1个消息 
#define  QUEUE_Ark1UseNum_SIZE   1   // 队列大小
#define  QUEUE_Ark2UseNum_LEN    1   // 队列(2号柜子使用次数)的长度，最大可包含1个消息 
#define  QUEUE_Ark2UseNum_SIZE   1   // 队列大小
#define  QUEUE_Ark3UseNum_LEN    1   // 队列(3号柜子使用次数)的长度，最大可包含1个消息 
#define  QUEUE_Ark3UseNum_SIZE   1   // 队列大小
#define  QUEUE_Ark4UseNum_LEN    1   // 队列(4号柜子使用次数)的长度，最大可包含1个消息 
#define  QUEUE_Ark4UseNum_SIZE   1   // 队列大小

#define  QUEUE_Phone_LEN    1   		 // 队列(手机号)的长度，最大可包含1个消息 
#define  QUEUE_Phone_SIZE   11   		 // 队列大小

#define  QUEUE_ArkWaitTime_LEN    1  // 队列(1号柜子等待事件)的长度，最大可包含多少个消息
#define  QUEUE_ArkWaitTime_SIZE   4  // 队列大小


#define CourierID		(1)
#define AdminerID		(2)



#endif /* __MYCONFIG_H */

/*****************************************************END OF FILE*********************************************************/	
