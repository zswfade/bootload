/**************************************************
 * @file Tim.c
 * @brief 定时器及其相关外设初始化
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-19   09:20
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-19   09:20 	|
 * <Description>:内容
***************************************************/
#include "Tim/Tim.h"
#include "Ymoden/Ymoden.h"


void TIM3_Init(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; //定时器参数设置
    NVIC_InitTypeDef NVIC_InitStruct;               //中断优先级设置

    // 初始化定时器3
    TIM_DeInit(TIM3);
    TIM_Cmd(TIM3, DISABLE);
    
    // 开启定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 初始化定时器参数
    TIM_TimeBaseInitStruct.TIM_Period           =   arr;    //重装载值
    TIM_TimeBaseInitStruct.TIM_Prescaler        =   psc;    //预分频值
    TIM_TimeBaseInitStruct.TIM_CounterMode      =   TIM_CounterMode_Up;    //计数模式
    TIM_TimeBaseInitStruct.TIM_ClockDivision    =   TIM_CKD_DIV1;           //时钟分频设置
    //TIM_TimeBaseInitStruct.TIM_RepetitionCounter=   ; //TIM1和TIM8的设置
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // 设置定时器中断优先级
    NVIC_InitStruct.NVIC_IRQChannel=   TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd               =   ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority       =   2;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=   2;
    NVIC_Init(&NVIC_InitStruct);
    
    // 开启定时器
    TIM_Cmd(TIM3, ENABLE);
    // 开启定时器中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}


void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)    // 检测到进定时器中断，代表10ms没收到串口信号了
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        TIM_Cmd(TIM3, DISABLE);     // 失能定时器，等待下次进入串口再开启
        Ymoden_Recv(USART3_BUFF, &USART3_REC_CNT);   // 处理单次Ymoden指令/数据
    }
}


