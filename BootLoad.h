/**************************************************
 * @file BootLoad.h
 * @brief 
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-05   11:12
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-05   11:12 	|
 * <Description>:内容
***************************************************/
#ifndef _BOOTLOAD_H_
#define _BOOTLOAD_H_

/* 头文件区 */
#include "head.h"
#include "type.h"


/* 预编译区 */
// 增加更新的数据存储区，相存储区域放置应好好规划。同时需明白存储后半部分的Flash是否和前半相同
#define BOOTLOAD_UPDATA_FLAG_ADDR   0x807E000   /* 程序更新标志位，若启动之后未被刷掉，代表升级失败需要重新升级，或者采用之前的地址 */  // 2048是800 1024是400
#define BOOTLOAD_STARTMESSAGE_ADDR	(BOOTLOAD_UPDATA_FLAG_ADDR + FLASH_PAGE_SIZE)	// 启动信息页面，和信号一样，进入APP才更新，如果地址不对，则选择默认的

#define BOOTLOAD_UPDATA_FLAG_DATA	0xACBD	// 更新标志位信息，不易出错
#define BOOTLOAD_UPDATA_SUCCESS     0x1111


/* 参数定义区 */
typedef void (*IAP_Fun)(void);				//定义一个函数类型的参数.

/* 参数声明区 */


/* 函数声明区 */
void StartAddr_Write(void* YmodenFlash);
void StartAddr_Read(void* YmodenFlash, void* Ymoden);
void UPDATA_Flag_Write(void);
void UPDATA_Flag_Erase(void);
void UPDATA_SUCCESS(void);
void UPDATA_Flag_Check(void);
u8 JumpToAPP(u32 APP_Address);
void System_Reset(void);
void System_Soft_Reset(void);


#endif
