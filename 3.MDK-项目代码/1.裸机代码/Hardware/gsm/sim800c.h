#ifndef __SIM800C_H
#define __SIM800C_H	

#include "stm32f10x.h"


extern unsigned char content[50];//���Ͷ������� 
extern unsigned char NO1_phone[12];//1�Ź���Ž��շ��ֻ���
extern unsigned char NO2_phone[12];//2�Ź���Ž��շ��ֻ���
extern unsigned char NO3_phone[12];//3�Ź���Ž��շ��ֻ���
extern unsigned char NO4_phone[12];//4�Ź���Ž��շ��ֻ���


extern void CLR_Buf1(void);//�崮�ڽ��ջ���
extern u8 Find(u8 *a);//�����ַ���
extern void Second_AT_Command(u8 *b,u8 *a,u8 wait_time);//����ATָ��
extern void Wait_CREG(void);   //��ѯ�ȴ�ģ��ע��ɹ�
extern void Set_Text_Mode(void); //���ö���ΪTEXTģʽ
extern void Send_Text_Sms(unsigned char* phone); //����һ��TEXT����
 

extern u8 First_Int ;
extern u8 Timer0_start;
extern u8 Times;
extern u8 shijian; 
 
 
#endif /*__SIM800C_H*/
