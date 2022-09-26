/**************************************************
 * @file CRC16.c
 * @brief CRC16校验功能驱动
 * @author LTC (TheHellc@163.com)
 * @version 1.0
 * @date 2022-09-07   09:05
***************************************************/
#include "head.h"
#include "CRC16/CRC16_List.h"

/**************************************************
 * @brief  CRC16校验码获取
 * @param  puchMsg          数据数组的首地址
 * @param  usDataLen        除了CRC校验数据的数据长度
 * @return u16              2位8字节数据组成
 * @date 2022-09-07   09:04
***************************************************/
u16 CRC16(u8 *puchMsg, u16 usDataLen) 
{ 
    u8 uchCRCHi = 0xFF ;
    u8 uchCRCLo = 0xFF ; 
    u16 uIndex ; 
    while(usDataLen--) 
    { 
    uIndex = uchCRCHi ^ *puchMsg++ ;  
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
    uchCRCLo = auchCRCLo[uIndex] ; 
    } 
    return (uchCRCHi << 8 | uchCRCLo) ; 
} 

/**************************************************
 * @brief  检测接受到的数据CRC校验是否正确
 * @param  puchMsg          
 * @param  usDataLen        
 * @return u8 
 * @date 2022-09-07   09:26
***************************************************/
u8 CRC16_Check(u8 *puchMsg, u16 usDataLen)
{
    u16 CRCTemp;
    CRCTemp = CRC16(puchMsg, usDataLen - 2);

    if (puchMsg[usDataLen - 2] != (CRCTemp >> 8) || puchMsg[usDataLen - 1] != (CRCTemp&0x00FF))
        return 0;

    return 1;
}


