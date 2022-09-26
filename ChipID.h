#ifndef _CHIPID_H_
#define _CHIPID_H_

#include "stm32f10x.h"

#define MCU_ID_OPEN 1   //是否开启读取MCU_ID

/************************************************
 *  MCU系列   |  Unique_ID地址  |  Flash容量地址  |
*************************************************
|   STM32F0   |   0x1FFFF7AC   |   0x1FFFF7CC   |
|   STM32F1   |   0x1FFFF7E8   |   0x1FFFF7E0   |
|   STM32F2   |   0x1FFF7A10   |   0x1FFF7A22   |
|   STM32F3   |   0x1FFFF7AC   |   0x1FFFF7CC   |
|   STM32F4   |   0x1FFF7A10   |   0x1FFFFA22   |
|   STM32F7   |   0x1FF0F420   |   0x1FF0F442   |
|   STM32L0   |   0x1FF80050   |   0x1FF8007C   |
|   STM32L1   |   0x1FF80050   |0x1FF8004C /00CC|
|   STM32L4   |   0x1FFF7590   |   0x1FFF75E0   |
|   STM32H7   |   0x1FF0F420   |   0x1FF0F442   |
************************************************/
#define CHIP_FLASH_SIZE_ADD ( (uint32_t)0x1FFFF7E0 )        //闪存容量地址 15：0
#define CHIP_FLASH_ID_ADD   ( (uint32_t)0x1FFFF7E8 )        //Flash ID起始地址
#define CHIP_FLASH_ID_ADD1  ( CHIP_FLASH_ID_ADD + 0x00 )    //Flash ID 15：0
#define CHIP_FLASH_ID_ADD2  ( CHIP_FLASH_ID_ADD + 0x02 )    //Flash ID 31：16
#define CHIP_FLASH_ID_ADD3  ( CHIP_FLASH_ID_ADD + 0x04 )    //Flash ID 63：32
#define CHIP_FLASH_ID_ADD4  ( CHIP_FLASH_ID_ADD + 0x08 )    //Flash ID 95：64
#if MCU_ID_OPEN
#define CHIP_MCU_ID_ADD     ( (uint32_t)0xE0042000 )        //MCU   ID 31：0    这个特别注意，不同芯片需要不同计算，如果不知道可以屏蔽掉，反正用到的也不多
#endif /* END OF MCU_ID_OPEN */

typedef struct CHIPIDMSG_STRU
{
    u16 chip_flash_size;        //闪存存储器容量15:0位
    u16 chip_flash_id_key1;     //唯一身份标志15:0位,产品唯一身份标识寄存器(96位)
    u16 chip_flash_id_key2;     //唯一身份标志31:16位
    u32 chip_flash_id_msg1;     //唯一身份标志63:32位
    u32 chip_flash_id_msg2;     //唯一身份标志95:64位
    u32 chip_mcu_id;            //芯片MCU ID
}CHIPIDMSG;

/**************************************************
 * @brief Get the Chip I D object
 * @param  chipid           
 * @date 2022-07-07   13:51
***************************************************/
void GetChipID(CHIPIDMSG* chipid);

#endif
