#ifndef _DELAY_H_
#define _DELAY_H_
#include "stm32f10x.h"

void Delay(u32 count);
// 微秒级延时
void DelayUs(u32 us);
// 毫秒级延时
void DelayMs(u16 ms);

#endif
