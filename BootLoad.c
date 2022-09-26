/**************************************************
 * @file BootLoad.c
 * @brief 引导程序，测试版，简易功能，通过串口发送bin文件
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-05   11:11
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-05   11:11 	|
 * <Description>:内容
***************************************************/
#include "BootLoad/BootLoad.h"


/////////////////////////////////////////////////////////////
/*                      Flash管理功能                      */
/////////////////////////////////////////////////////////////

void StartAddr_Write(void* YmodenFlash)
{
    YmodenFlash_Stu *This_YmodenFlash = (YmodenFlash_Stu*)YmodenFlash;

    This_YmodenFlash->ymoden_size = &This_YmodenFlash->addr1 - &This_YmodenFlash->addr0;
    This_YmodenFlash->ymoden_u16_size = This_YmodenFlash->ymoden_size / 2;

    USART_Cmd(USART3, DISABLE);
    Flash_ErasePage(BOOTLOAD_STARTMESSAGE_ADDR);
    Flash_WriteHalfWord(BOOTLOAD_STARTMESSAGE_ADDR, 0xABCD);
    Flash_WriteData(BOOTLOAD_STARTMESSAGE_ADDR + 2, (u16*)This_YmodenFlash, This_YmodenFlash->ymoden_u16_size);
    USART_Cmd(USART3, ENABLE);
}

void StartAddr_Read(void* YmodenFlash, void* Ymoden)
{
    Ymoden_Stu *This_Ymoden = (Ymoden_Stu*)Ymoden;
    YmodenFlash_Stu *This_YmodenFlash = (YmodenFlash_Stu*)YmodenFlash;

    This_YmodenFlash->ymoden_size = &This_YmodenFlash->addr1 - &This_YmodenFlash->addr0;
    This_YmodenFlash->ymoden_u16_size = This_YmodenFlash->ymoden_size / 2;

    if (Flash_ReadHalfWord(BOOTLOAD_STARTMESSAGE_ADDR) != 0xABCD)   // 确认是否新烧录的程序
    {
        StartAddr_Write(YmodenFlash);
    }
    else
    {
        Flash_ReadData(BOOTLOAD_STARTMESSAGE_ADDR + 2, (u16*)This_YmodenFlash, This_YmodenFlash->ymoden_u16_size);
    }

    This_Ymoden->startaddr = This_YmodenFlash->startaddr;     // 获取Flash内部启动地址
}

/**************************************************
 * @brief  写入更新标志位，表示正在更新程序
 * @date   2022-09-13   10:57
***************************************************/
void UPDATA_Flag_Write(void)
{
    USART_Cmd(USART3, DISABLE);
    Flash_WriteHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR, BOOTLOAD_UPDATA_FLAG_DATA);
    Flash_WriteHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 2, Ymoden.startaddr >> 16);
    Flash_WriteHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 4, Ymoden.startaddr&0xFFFF);
    USART_Cmd(USART3, ENABLE);
}

/**************************************************
 * @brief  擦除更新标志位，表示取消更新或者更新成功
 * @date   2022-09-13   10:58
***************************************************/
void UPDATA_Flag_Erase(void)
{
    USART_Cmd(USART3, DISABLE);
    Flash_ErasePage(BOOTLOAD_UPDATA_FLAG_ADDR);
    USART_Cmd(USART3, ENABLE);
}

/**************************************************
 * @brief  更新成功标志，只有这个标志在，才能在更新之后跳转，否则如果有更新信号也不会跳转，防止卡死
 * @date   2022-09-15   10:32
***************************************************/
void UPDATA_SUCCESS(void)
{
    USART_Cmd(USART3, DISABLE);
    Flash_WriteHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 6, BOOTLOAD_UPDATA_SUCCESS);
    USART_Cmd(USART3, ENABLE);
}

/**************************************************
 * @brief  检测是否升级失败，如果升级失败，就启动之前的版本，如果之前的版本为空，则报错
 * @date   2022-09-13   16:27
***************************************************/
void UPDATA_Flag_Check(void)
{
    u32 temp;
    if (Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR) == BOOTLOAD_UPDATA_FLAG_DATA)
    {
        printf("Previous upgrade failed!\r\n");
        printf("Please upgrade again, or start the previous version!\r\n");
        temp = Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 2) << 16;
        temp = (temp&0xFFFF0000) + Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 4);
        if (YmodenFlash.startaddr == temp)
        {
            if (YmodenFlash.last_startaddr == 0)
                printf("No Program!\r\n");
            else
                printf("Prigran error!\r\n");
        }
        else if (YmodenFlash.startaddr != temp)
        {
            printf("Failed program is:\t0x%.8x\r\n", temp);
            if (Flash_ReadHalfWord(YmodenFlash.startaddr) == 0xFFFF)
            {
                printf("No Program!\r\n");
            }
        }
            
    }
}

/**************************************************
 * @brief  程序跳转功能语句,跳转到指定地点开始运行程序
 * @param  APP_Address      
 * @return u8               
 * @date   2022-09-13   09:04
***************************************************/
IAP_Fun jump2app;
u8 JumpToAPP(u32 APP_Address)
{
    u32 temp;
    // 检测上次更新是否完成，如果是没完成，则不跳转
    if (Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR) == BOOTLOAD_UPDATA_FLAG_DATA)     // 检测到更新
    {
        // 读取本次更新的地址
        temp = Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 2) << 16;
        temp = (temp&0xFFFF0000) + Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 4);
        // 检测本次更新是否完成
        if (Flash_ReadHalfWord(BOOTLOAD_UPDATA_FLAG_ADDR + 6) != BOOTLOAD_UPDATA_SUCCESS    // 更新失败了
        &&  Ymoden.startaddr == temp)   // 当前启动地址和更新失败的地址一样
            return 0;
    }

    // 开始跳转
    if(((*(vu32*)APP_Address)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
        jump2app=(IAP_Fun)*(vu32*)(APP_Address+4);		//用户代码区第二个字为程序开始地址(复位地址)
        __set_MSP(*(vu32*)APP_Address);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
    return 0;
}

/**************************************************
 * @brief  系统中断控制器产生一次复位信号            
 * @date   2022-09-15   09:23
***************************************************/
void System_Reset(void) 
{
    SCB->AIRCR = 0x05FA0000 | (u32)0x04;
}

/**************************************************
 * @brief  系统中断控制器产生一次复位信号,并保留中断分组            
 * @date   2022-09-15   09:23
***************************************************/
void System_Soft_Reset(void)
{
    // ____set_FAULTMASK(1);    // 关闭所有中断
    __disable_fault_irq();  // 关闭所有中断,就是硬fault会被强制执行，所以需要关掉
    NVIC_SystemReset();     // 保留中断分组并产生复位信号
}










