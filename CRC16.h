/**************************************************
 * @file CRC16.c
 * @brief CRC16校验功能驱动
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-09-07   09:05
***************************************************/
#ifndef _CRC16_H_
#define _CRC16_H_

#include "stm32f10x.h"

/**************************************************
 * @brief  CRC16校验码获取
 * @param  puchMsg          数据数组的首地址
 * @param  usDataLen        除了CRC校验数据的数据长度
 * @return u16              2位8字节数据组成
 * @date 2022-09-07   09:04
***************************************************/
u16 CRC16(u8 *puchMsg, u16 usDataLen);

/**************************************************
 * @brief  检测接受到的数据CRC校验是否正确
 * @param  puchMsg          
 * @param  usDataLen        
 * @return u8 
 * @date 2022-09-07   09:26
***************************************************/
u8 CRC16_Check(u8 *puchMsg, u16 usDataLen);

#endif
