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
#include "stmflash.h" 
#include "tim4.h"
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
/* ���� -----------------------------------------------------------------------------------------------------------------*/
#define TIMERCOUNT  (3)//软件定时器总数
#define TIMERINDEX1 (0)//事件1    
#define TIMERINDEX2 (1)//事件2   
#define TIMERINDEX3 (2)//事件3 
	
#define TIMEISUP		(1)//时间到
#define TIMEISNOTUP	(0)//时间未到


#define ONE			(0)//1号柜
#define TWO			(1)//2号柜
#define THREE		(2)//3号柜
#define FOUR		(3)//4号柜
#define NON     (4)//非快递柜


#define FULL		(1)//柜已使用
#define EMPTY		(0)//柜未使用

#define CourierID		(1)
#define AdminerID		(2)

#define FLASH_SAVE_ADDR  0X08007f00		//FLASH地址，取值大于代码Flash加0X08000000，必须位偶数

      

typedef struct
{
	uint8_t UseState;//快递柜使用状态
	uint8_t UseNum;//快递柜使用次数
  uint8_t PickupCode[6];//取件码
}Ark_InitTypeDef;

typedef struct
{
	uint8_t InputFingerprintFlag;//录入指纹标志位
	uint8_t SendMessageFlag;//发送消息标志位
	uint8_t FingerprintID;//ָ指纹id号
	uint8_t Sec;//计时-秒
}Para_InitTypeDef;

extern Para_InitTypeDef Para;
extern Ark_InitTypeDef Ark[4];
extern char TempStr[20];
/* 提供给其他C文件调用的函数 --------------------------------------------------------------------------------------------*/
unsigned char xDelay(unsigned char xIndex,unsigned char xTime);
void Read_Flash(void);
void Write_Flash(void);
void Waiting_Network(void);
void AboutFingerprintProc(void);
void AboutUsartScreenProc(void);
void AboutCourierProc(void);




#endif /* __MYCONFIG_H */

/*****************************************************END OF FILE*********************************************************/	
