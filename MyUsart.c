/**************************************************
 * @file Usart.c
 * @brief STM32串口初始化程序
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-05   13:43
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-05   13:43 	|
 * <1.0>:基于STM32F103的串口初始化程序，目前增加串口1、串口2、串口3一个3个串口初始化函数
***************************************************/
#include "MyUsart/MyUsart.h"
#include <stdio.h>

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    Set_SEND;
    USART3->DR = (u8) ch; 
    while((USART3->SR&0X40)==0);//循环发送,直到发送完毕
    Set_RCVE;
	return ch;
}
#endif 

/**************************************************
 * @brief 串口3初始化函数
 * @param  baud             波特率
 * @date 2022-07-05   15:10
***************************************************/
void Usart3_Init(u32 baud)
{
    // 定义相关函数体
    GPIO_InitTypeDef GPIO_InitStructure;   //串口端口
    USART_InitTypeDef USART_InitTypeStruct; //串口
    NVIC_InitTypeDef NVIC_InitTypeStruct;   //串口中断优先级
    
    // 初始化串口1
    USART_DeInit(USART3);
    USART_Cmd(USART3, DISABLE);
    
    // 打开串口和IO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能外设时钟
    
    // 串口端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* USART3 使用IO端口配置 */    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);   //初始化GPIOA
    
    // 初始化串口
    USART_InitTypeStruct.USART_BaudRate =   baud;   //波特率
    USART_InitTypeStruct.USART_Mode     =   USART_Mode_Rx | USART_Mode_Tx;  //全双工
    USART_InitTypeStruct.USART_Parity   =   USART_Parity_No;    //无校验
    USART_InitTypeStruct.USART_StopBits =   USART_StopBits_1;   //1停止位
    USART_InitTypeStruct.USART_WordLength   =   USART_WordLength_8b;    //8位数据
    USART_InitTypeStruct.USART_HardwareFlowControl  =   USART_HardwareFlowControl_None; //无硬件流
    USART_Init(USART3, &USART_InitTypeStruct);
    
    // 串口中断优先级设置
    NVIC_InitTypeStruct.NVIC_IRQChannel     =   USART3_IRQn;
    NVIC_InitTypeStruct.NVIC_IRQChannelCmd  =   ENABLE;
    NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority   =   3;   //抢占
    NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority          =   3;   //响应
    NVIC_Init(&NVIC_InitTypeStruct);
    
    // 开启串口中断、开启串口接收
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
    Set_RCVE;   // 开启485接收
}

/**************************************************
 * @brief 串口3中断处理函数
 * @date 2022-07-05   15:14
***************************************************/
u8 USART3_BUFF[USART3_REC_LEN];
u16 USART3_REC_CNT = 0;
void USART3_IRQHandler(void)
{
    u8 res;
    
    if(USART3->SR&(1<<5))//接收到数据
	{	 
		res=USART3->DR; 
		if(USART3_REC_CNT<USART3_REC_LEN)
		{
			USART3_BUFF[USART3_REC_CNT++]=res;
            TIM3->CNT = 0;  // 定时器3计数值清零
            TIM_Cmd(TIM3, ENABLE);  // 使能定时器3
		}
	}
}

/**************************************************
 * @brief 串口x的单个字节发送
 * @param  USARTx           串口号
 * @param  Data             8位的数据
 * @date 2022-07-07   09:31
***************************************************/
void USARTx_SendBitData(USART_TypeDef* USARTx, uint8_t Data)
{
    Set_SEND;
    USART_SendData(USARTx, Data);
    while(!(USARTx->SR&(1<<7)));
    Set_RCVE;
}

/**************************************************
 * @brief 串口批量发送数据
 * @param  USARTx           串口号
 * @param  Data             数组地址
 * @param  Datalen          发送的字符长度
 * @date 2022-07-07   11:26
***************************************************/
void USARTx_SendData(USART_TypeDef* USARTx, u8* Data, u32 Datalen)
{
    u32 i;

    for(i = 0; i < Datalen; i++)
    {
        USARTx_SendBitData(USARTx, Data[i]);
    }
}











