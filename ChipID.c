/**************************************************
 * @file ChipID.c
 * @brief 
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-07   13:51
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-07   13:51 	|
 * <Description>:内容
***************************************************/
#include "ChipID/ChipID.h"

/**************************************************
 * @brief Get the Chip I D object
 * @param  chipid           
 * @date 2022-07-07   13:51
***************************************************/
void GetChipID(CHIPIDMSG* chipid)
{
    CHIPIDMSG* this_chipid;
    this_chipid = chipid;

    this_chipid->chip_flash_size = *(__IO u16 *)CHIP_FLASH_SIZE_ADD;
    this_chipid->chip_flash_id_key1 = *(__IO u16 *)CHIP_FLASH_ID_ADD1;
    this_chipid->chip_flash_id_key2 = *(__IO u16 *)CHIP_FLASH_ID_ADD2;
    this_chipid->chip_flash_id_msg1 = *(__IO u32 *)CHIP_FLASH_ID_ADD3;
    this_chipid->chip_flash_id_msg2 = *(__IO u32 *)CHIP_FLASH_ID_ADD4;

    #if MCU_ID_OPEN
    this_chipid->chip_mcu_id = *(__IO u32 *)CHIP_MCU_ID_ADD;
    #else
    this_chipid->chip_mcu_id = 0x0000;
    #endif /* END OF MCU_ID_OPEN */
}




