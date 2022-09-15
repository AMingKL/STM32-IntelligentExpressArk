#ifndef __SIM800C_H
#define __SIM800C_H	

#include "stm32f10x.h"


extern unsigned char content[50];//发送短信内容 
extern unsigned char NO1_phone[12];//1号柜短信接收方手机号
extern unsigned char NO2_phone[12];//2号柜短信接收方手机号
extern unsigned char NO3_phone[12];//3号柜短信接收方手机号
extern unsigned char NO4_phone[12];//4号柜短信接收方手机号


extern void CLR_Buf1(void);//清串口接收缓存
extern u8 Find(u8 *a);//查找字符串
extern void Second_AT_Command(u8 *b,u8 *a,u8 wait_time);//发送AT指令
extern void Wait_CREG(void);   //查询等待模块注册成功
extern void Set_Text_Mode(void); //设置短信为TEXT模式
extern void Send_Text_Sms(unsigned char* phone); //发送一条TEXT短信
 

extern u8 First_Int ;
extern u8 Timer0_start;
extern u8 Times;
extern u8 shijian; 
 
 
#endif /*__SIM800C_H*/
