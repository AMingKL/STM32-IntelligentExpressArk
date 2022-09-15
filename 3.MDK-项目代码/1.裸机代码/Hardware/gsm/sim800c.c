#include "sim800c.h"
#include "delay.h"
#include "usart1.h"
#include <string.h>
 

unsigned char NO1_phone[12]="15983674750";     //1号柜短信接收方手机号
unsigned char NO2_phone[12]="15983674750";     //2号柜短信接收方手机号
unsigned char NO3_phone[12]="15983674750";     //3号柜短信接收方手机号
unsigned char NO4_phone[12]="15983674750";     //4号柜短信接收方手机号
unsigned char content[50]="ce shi";//发送短信内容 

u8 First_Int = 0;
u8 Timer0_start=0;
u8 Times=0;
u8 shijian=0;


/*******************************************************************************
* 函数名 : CLR_Buf1
* 描述   : 清除串口1缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf1(void)
{
	u16 k;
	for(k=0;k<USART1_REC_LEN;k++)      //将缓存内容清零
	{
		USART1_RX_BUF[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}
/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/

u8 Find(u8 *a) 
{   
  if(strstr((const char *)USART1_RX_BUF,(char*)a)!=NULL)  
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、希望收到的应答、发送等待时间(单位：S)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

void Second_AT_Command(u8 *b,u8 *a,u8 wait_time)         
{
	u8 i;
	u8 *c;
	c = b;										//保存字符串地址到c
	CLR_Buf1(); 
  i = 0;
	while(i == 0)                    
	{
				if(!Find(a))           //查找需要应答的字符
				{
								if(Timer0_start == 0)//超时重新发送命令
								{
											b = c;//将字符串地址给b
											for (b; *b!='\0';b++)
											{
												Usart1_SendByte(USART1,*b);
											}
											USART1_SendLR();	
											Times = 0;
											shijian = wait_time;
											Timer0_start = 1;  //开始计时
									}
				 }
						else
				  {
					  i = 1;
					  Timer0_start = 0;  
				  }
	}
	CLR_Buf1(); 
}
  
/*******************************************************************************
* 函数名 : Set_Text_Mode
* 描述   : 设置短信为TEXT文本模式
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Set_Text_Mode(void)
{
	 Second_AT_Command("ATE0","OK",3);										//取消回显	
	 Second_AT_Command("AT+CMGF=1","OK",3);								//TEXT模式	
	 Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);//所有操作都在SIM卡中进行	   
}



/*******************************************************************************
* 函数名 : Send_Text_Sms
* 描述   : 发送TEXT文本短信
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Send_Text_Sms(unsigned char* phone)
{
	u8 tem[14],i=0;
	unsigned char temp[50]="AT+CMGS=";
	tem[0]='"';
	for(i=0;i<11;i++)
	{
		tem[i+1]=phone[i];
	}
	tem[12]='"';
	tem[13]=0; //字符串结束符
	strcat((char *)temp,(char *)tem); //字符串拼接函数（库函数）
	Second_AT_Command(temp,">",3); //等待接收到“>”才发送短信内容

	UART1_SendString((char *)content);     //发送短信内容
	Usart1_SendByte(USART1,0X1A);          //发送结束符
}
/*******************************************************************************
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Wait_CREG(void)
{
	u8 i;
	u8 k;
	i = 0;
	CLR_Buf1();
  while(i == 0)        			
	{
		CLR_Buf1();        
		UART1_SendString("AT+CREG?");//查询模块网络注册状态
		USART1_SendLR();  
		delay_ms(250);  	    					 
	    for(k=0;k<USART1_REC_LEN;k++)      			
    	{
			if(USART1_RX_BUF[k] == ':')
			{
				if((USART1_RX_BUF[k+4] == '1')||(USART1_RX_BUF[k+4] == '5')) //表明网络注册成功
				{
					i = 1;
				  break;
				}
			}
		}
	}
}


