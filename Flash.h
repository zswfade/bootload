/**************************************************
 * @file Flash.h
 * @brief Flash驱动最底层头文件
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-04   09:05
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-04   09:05 	|
 * <1.0>:增加最底层驱动相关函数声明，增加存储地址管理预编译相关内容
***************************************************/
#ifndef _FLASH_H_
#define _FLASH_H_


/* 头文件区 */
#include "stm32f10x.h"


/* 预编译区 */
#define FLASH_ADDRESS   (0x08000000)        /* 芯片Flash起始地址 */
#define FLASH_MAX_SIZE  (0x00080000)        /* 芯片Flash大小，可以通过读取，目前先使用手动定义 */
#define FLASH_LAST_ADD  (FLASH_ADDRESS + FLASH_MAX_SIZE)    /* Flash终止地址 */
#define FLASH_PAGE_SIZE 2048                /* 每页FLASH尺寸,1K-1024 2K-2048,单位是bit，每bit 8位 */
#define FLASH_SET_SAVE_SIZE 0               /* 用户自己设置的存储空间数量，单位页，每页2K */
#define FLASH_SET_ADDRESS   (FLASH_LAST_ADD - (FLASH_SET_SAVE_SIZE * FLASH_PAGE_SIZE))
                                            /* 用户自己设置的存储空间，不过这里先采用最大空间的最后几页使用 */
#define FLASH_INIT_FLAG 0xABCD              /* 用于识别是否初次初始化的FLAG */


/* 参数定义区 */
// 用来数据类型转换
typedef union Flash_Data_Union {
    u8 u8_data[4];
    u16 u16_data[2];
    u32 u32_data[1];
}Flash_Data;

// 反馈标志
enum Flash_Return_Flag {
    NO = 0,
    OK = 1,
    E_ERROR = 99,
};


/* 参数声明区 */


/* 函数声明区 */
u16 Flash_ReadHalfWord(u32 HalfWordAdd);
u8 Flash_ReadData(u32 DataAddress, u16* ArrayAddress, u16 DataLen);
u8 Flash_ReadtoWrite(u32 DataAddress, u16 DataLen);
u8 Flash_ErasePage(u32 PageAddress);
u8 Flash_ErasePages(u32 PageAddress, u16 PageNumber);
u8 Flash_WriteHalfWord(u32 HalfWordAdd, u16 HalfWordData);
u8 Flash_WriteData(u32 StartAddress, u16* Data, u16 DataNumberToWrite);
u8 Flash_WritePageData(u32 PageAddress, u16* Data, u16 DataNumberToWrite);

#endif

