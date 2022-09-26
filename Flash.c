/**************************************************
 * @file Flash.c
 * @brief Flash驱动最底层执行文件
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-07-04   09:29
 * 
 * @copyright Copyright (c) 2022  YSDZ
 * 
 * @par 修改日志:
 * | Version | Author 	| Date 					|
 * | 1.0     | LTC 		| 2022-07-04   09:29 	|
 * <1.0>:编辑相关初始Flash驱动，增加相关底层驱动函数
***************************************************/
#include "Flash/Flash.h"

Flash_Data Flash_Temp;

/**************************************************
 * @brief 读该地址的无符号半字(u16)数据
 * @param  HalfWordAdd      半字地址
 * @return u16 
 * @date 2022-07-04   09:30
***************************************************/
u16 Flash_ReadHalfWord(u32 HalfWordAdd)
{
    return *(vu16*)HalfWordAdd;
}

/**************************************************
 * @brief 数据读取，从开始地址读取，需要判断是否为一页，可以选择读取的数据类型
 * @param  DataAddress      读取数据的起始地址
 * @param  ArrayAddress     存储数据的数组指针
 * @param  DataLen          数据长度（半字）
 * @return u8 
 * @date 2022-07-04   10:23
***************************************************/
u8 Flash_ReadData(u32 DataAddress, u16* ArrayAddress, u16 DataLen)
{
    u16 i;
    //检测是否为偶数
    //本函数均为16位数据读取，地址不能为奇数
    if(DataAddress % 2 != 0)return NO;
    
    //判断读取范围是否超出界限
    if((DataAddress + DataLen * 2) > FLASH_LAST_ADD)return NO;

    //开始读取数据
    for(i = 0; i < DataLen; i++)
    {
        ArrayAddress[i] = Flash_ReadHalfWord(DataAddress);
        DataAddress += 2;   //偏移2个字节
    }

    return OK;
}

/**************************************************
 * @brief 检测要写入的Flash是否需要擦除
 * @param  DataAddress      要读取的数据地址
 * @param  DataLen          要检测的数据长度
 * @return u8 
 * @date 2022-07-04   13:44
***************************************************/
u8 Flash_ReadtoWrite(u32 DataAddress, u16 DataLen)
{
    u16 i;
    for(i = 0; i < DataLen; i++)
    {
        if(Flash_ReadHalfWord(DataAddress) != 0xFFFF)return NO;
        DataAddress += 2;
    }

    return OK;
}

/**************************************************
 * @brief 用来擦除某一页的Flash的内容
 * @param  PageAddress      擦除的页地址，若擦除最后一页则需要最后一页的首地址
 * @return u8               返回值为Flash相关寄存器状态
 * @date 2022-07-04   11:17
***************************************************/
u8 Flash_ErasePage(u32 PageAddress)
{
    //先判断是否越界
    if(PageAddress < FLASH_ADDRESS || PageAddress > (FLASH_LAST_ADD - FLASH_PAGE_SIZE))return E_ERROR;

    //擦除顺便检测是否完成
    FLASH_Unlock();
    if(FLASH_ErasePage(PageAddress) != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return E_ERROR;
    }
    FLASH_Lock();

    return FLASH_COMPLETE;
}

/**************************************************
 * @brief 多页Flash擦除
 * @param  PageAddress      擦除地址
 * @param  PageNumber       擦除页数
 * @return u8               返回值为Flash相关寄存器状态
 * @date 2022-07-04   11:41
***************************************************/
u8 Flash_ErasePages(u32 PageAddress, u16 PageNumber)
{
    u16 i;
    u32 sector;

    //计算扇区首地址
    sector = PageAddress - FLASH_ADDRESS;
    sector = sector - (sector % FLASH_PAGE_SIZE);

    //判断目前Flash是否可以操作
    if(FLASH_GetStatus() != FLASH_COMPLETE)return E_ERROR;

    //开始擦除
    for(i = 0; i < PageNumber; i++)
    {
        if(Flash_ErasePage(FLASH_ADDRESS + sector + (i * FLASH_PAGE_SIZE)) != FLASH_COMPLETE)
            return E_ERROR;
    }

    return FLASH_COMPLETE;
}

/**************************************************
 * @brief 在用户输入的地址写入一个u16的数据
 * @param  HalfWordAdd      
 * @param  HalfWordData     
 * @return u8 
 * @date 2022-07-04   11:55
***************************************************/
u8 Flash_WriteHalfWord(u32 HalfWordAdd, u16 HalfWordData)
{
    //判断是否可以写入
    if(Flash_ReadHalfWord(HalfWordAdd) != 0xFFFF)return NO;
    //判断写入地址是否合理
    if(HalfWordAdd % 2 != 0)return NO;
    //判断Flash是否可以操作
    if(FLASH_GetStatus() != FLASH_COMPLETE)return E_ERROR;
    //写入及判断是否成功
    FLASH_Unlock();
    if(FLASH_ProgramHalfWord(HalfWordAdd, HalfWordData) != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return E_ERROR;
    }
    FLASH_Lock();
    //判断写入的是否一致
    if(Flash_ReadHalfWord(HalfWordAdd) != HalfWordData)return FLASH_TIMEOUT;

    return FLASH_COMPLETE;
}

/**************************************************
 * @brief  写小于一页的数据，最好是在一页之内，虽然跨页也能写，但是容易出问题
 * @param  StartAddress         写入的地址
 * @param  Data                 写入的数组指针
 * @param  DataNumberToWrite    写入的数据长度（u16）
 * @return u8 
 * @date 2022-09-07   10:02
***************************************************/
u8 Flash_WriteData(u32 StartAddress, u16* Data, u16 DataNumberToWrite)
{
    u16 i;
    if (Flash_ReadtoWrite(StartAddress, DataNumberToWrite) == NO)
    {
        return NO;  // 已经有数据了，就不写入，也不拆除
    }
    else
    {
        //开始写入
        for(i = 0; i < DataNumberToWrite; i++)
        {
            if(Flash_WriteHalfWord(StartAddress, Data[i]) != FLASH_COMPLETE)return NO;
            StartAddress += 2;   //偏移两个字节
        }
    }

    return OK;
}

/**************************************************
 * @brief 用来批量写入Flash，最好一页一页的写，尽量不要越界，同时最好是从每页首地址开始，不波及到其他数据
 * @param  PageAddress          写入的地址
 * @param  Data                 要写入的数据的指针
 * @param  DataNumberToWrite    要写入的数据的长度(u16)
 * @return u8 
 * @date 2022-07-04   13:35
***************************************************/
u8 Flash_WritePageData(u32 PageAddress, u16* Data, u16 DataNumberToWrite)
{
    u16 i;
    u32 sector;
    //先检测是否可以写入
    if(Flash_ReadtoWrite(PageAddress, DataNumberToWrite) == NO)
    {
        sector = (DataNumberToWrite * 2) / FLASH_PAGE_SIZE; //查看数据需要写几页
        if((DataNumberToWrite * 2) % FLASH_PAGE_SIZE > 0)
        {
            sector ++;  //当数据剩余数据不足一页时，当一页写
        }
        Flash_ErasePages(PageAddress, sector);
    }
    //开始写入
    for(i = 0; i < DataNumberToWrite; i++)
    {
        if(Flash_WriteHalfWord(PageAddress, Data[i]) != FLASH_COMPLETE)return NO;
        PageAddress += 2;   //偏移两个字节
    }

    return OK;
}


















