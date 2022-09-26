/**************************************************
 * @file Usart.h
 * @brief STM32串口初始化函数头文件
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-05   13:46
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-05   13:46 	|
 * <Description>:内容
***************************************************/
#ifndef _MYUSART_H_
#define _MYUSART_H_

#include "stm32f10x.h"

#define USART3_REC_LEN   2*1024     //串口1数据接收最大数量
#define Set_SEND		 GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define Set_RCVE		 GPIO_ResetBits(GPIOB,GPIO_Pin_15)

extern u8 USART3_BUFF[USART3_REC_LEN];  //串口1接收缓存
extern u16 USART3_REC_CNT;      //串口1接收的数据长度


/**************************************************
 * @brief 串口3初始化函数
 * @param  baud             波特率
 * @date 2022-07-05   15:10
***************************************************/
void Usart3_Init(u32 baud);

/**************************************************
 * @brief 串口x的单个字节发送
 * @param  USARTx           串口号
 * @param  Data             8位的数据
 * @date 2022-07-07   09:31
***************************************************/
void USARTx_SendBitData(USART_TypeDef* USARTx, uint8_t Data);

/**************************************************
 * @brief 串口批量发送数据
 * @param  USARTx           串口号
 * @param  Data             数组地址
 * @param  Datalen          发送的字符长度
 * @date 2022-07-07   11:26
***************************************************/
void USARTx_SendData(USART_TypeDef* USARTx, u8* Data, u32 Datalen);

#endif
