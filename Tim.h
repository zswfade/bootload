/**************************************************
 * @file Tim.h
 * @brief 定时器及其相关外设初始化头文件
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
#ifndef _TIM_H_
#define _TIM_H_

#include "stm32f10x.h"

void TIM3_Init(u16 psc, u16 arr);

#endif
