/**
  *************************************************************************************************************************
  * @file    main.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-06
  * @brief   基于stm32-智能快递柜
  *************************************************************************************************************************
  * @attention								

	实现要求:
	1、设计采用STM32F103作为主控，3.5寸串口触摸屏作为上位机与单片机通信，
		 实现人机交互，通过编辑屏幕显示界面，实现显示快递柜是否占用，快递员、管理员等信息。
		 并添加相应触摸按键功能，通过触控选择操作实现快递员、管理员、客户进行纯取件、查看信息等操作。
	2、另采用串口指纹识别模块，通过串口与单片机进行交互，实现指纹的录入及验证，
		 即可配合串口屏一同实现快递员、管理员、客户的权限控制。
	3、采用SIM800C 短信发送模块，通过串口与单片机交互，
		 单片机可通过发送相应AT指令控制短信模块发送相应的取件码信息给客户手机。
	4、采用四路5V继电器进行驱动，四个继电器分别代表1号2号3号4号快递柜，
		 继电器的开和闭模拟快递柜的打开和关闭。

	
	硬件连接:	
	继电器:				AS608指纹模块:     陶晶驰串口屏:    SIM800C:
	IN1-PB9				TXD-PB11           TXD-PA3					TXD-PA10
	IN2-PB8				RXD-PB10           RXD-PA2  				RXD-PA9
	IN3-PB7				U+-3.3V            VCC-5V						VCC-5V
	IN4-PB6				GND-GND						 GND-GND  				GND-GND
	VCC-5V
	GND-GND
		
	sim800c-usart1   串口屏-uart2   as608-uart3
	
  *************************************************************************************************************************
  */
/* Includes -------------------------------------------------------------------------------------------------------------*/
/* 开发板硬件头文件 */
#include "myconfig.h" 


static void BSP_Init(void);/* 用于初始化板载相关资源 */



int main(void)
{	
	BSP_Init();
	while(1)
	{	
		/*10ms检测一次*/
		if(xDelay(TIMERINDEX1,1)  == TIMEISUP)
		{	
			AboutUsartScreenProc();				
		}	
		/*50ms刷新一次*/
		if(xDelay(TIMERINDEX2,5)  == TIMEISUP)
		{	
			AboutCourierProc();
		}	
		/*100ms刷新一次*/
		if(xDelay(TIMERINDEX2,10)  == TIMEISUP)
		{	
			AboutFingerprintProc();
		}
	}
}


static void BSP_Init(void)
{
	NVIC_Config();//设置NVIC中断分组
	delay_init();//延时初始化
	Usart1_Init(9600);//串口1初始化 
	Usart2_Init(115200);//串口2初始化
	Usart3_Init(57600);//串口3初始化	
	Tim4_Init();//定时器4-10ms中断初始化
	Read_Flash();	//读取Flash
	Waiting_Network();//等待网络注册成功	
}



/*****************************************************END OF FILE*********************************************************/	

