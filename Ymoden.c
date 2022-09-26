#include "Ymoden/Ymoden.h"

Ymoden_Stu Ymoden;
YmodenFlash_Stu YmodenFlash;

static void YModen_ACK(void) {
	printf("%c", YMODEM_ACK);
}

static void YModen_NACK(void) {
	printf("%c", YMODEM_NAK);
}

static void YModen_CAN(void) {
    printf("%c", YMODEM_CA);
}

static void YModen_C(void) {
	printf("%c", YMODEM_C);
}

void Set_YModen_Status(ProcessStatus process) {
	Ymoden.process = process;
}

ProcessStatus Get_YModen_Status(void) {
	ProcessStatus process = Ymoden.process;
	return process;
}

/**************************************************
 * @brief  Check the filename object
 * @param  data             读取文件名的开始地址
 * @return u8               
 * @date   2022-09-09   11:33
***************************************************/
static u8 Check_FILENAME(u8* data)
{
    u8 *p;
    u8 i = 0;
    p = memchr(data, 0x00, 32);     // 最长32个字符，超出就不要
    if (p == NULL)
    {
        return 0;
    }
    else
    {
        i = p - data;   // 获取长度
        memset(Ymoden.filename, 0, sizeof(Ymoden.filename));   // 先清除原本的内容
        memcpy(Ymoden.filename, data, i + 1);   // 获取文件名
    }

    // 检测文件名是否为设定的文件名
    i = strncmp(FILENAME_RECV, Ymoden.filename, sizeof(FILENAME_RECV) - 1);
    if (i != 0)
    {
        p = NULL;
        return 0;
    }

    p = NULL;
    return 1;
}

/**************************************************
 * @brief  Get the filesize object
 * @param  data             读取数据长度的开始地址
 * @return u8               
 * @date   2022-09-09   11:18
***************************************************/
static u8 Get_FILESIZE(u8* data)
{
    u8 *p;
    u8 i = 0;
    u8 j = 0;
    p = memchr(data, 0x00, 10);     // 150K最多才6位
    if (p == NULL)
    {
        return 0;
    }
    else
    {
        i = p - data;   // 获取长度
        while(i > 0)
        {
            // 验证当前值是否为协议数字
            if (data[j] <= 0x30 && data[j] >= 0x39)
            {
                Ymoden.filesize = 0;
                p = NULL;
                return 0;
            }

            // 获取值
            Ymoden.filesize += (data[j++]&0x0F) * pow(10, i - 1);    // 例如1234，就是1*1000 + ……
            i--;
        }
    }

    p = NULL;
    return 1;
}

/**************************************************
 * @brief  手动选择功能显示
 * @date   2022-09-09   16:29
***************************************************/
void PrintfTips(void)
{
    if (Ymoden.process == SET_PARAMETER)
    {
        printf("\r\n");
        printf("##################################################\r\n");
        printf("#  1.Start program\r\n");
        printf("#  2.Update program\r\n");
        printf("#  3.Exit parameter\r\n");
        printf("#  4.Set WhritePage 1\t0x%.8x\r\n", STARTADDR_PAGE1);
        printf("#  5.Set WhritePage 2\t0x%.8x\r\n", STARTADDR_PAGE2);
        printf("#  6.Set StartPage 1(Flash)\t0x%.8x\r\n", STARTADDR_PAGE1);
        printf("#  7.Set StartPage 2(Flash)\t0x%.8x\r\n", STARTADDR_PAGE2);
        printf("#\r\n");
        printf("#  Now start/write Address: \t0x%.8x\r\n", Ymoden.startaddr);
        printf("#  Last updata Address:\t\t0x%.8x\r\n", YmodenFlash.last_startaddr);
        printf("#  Updata number:%d\r\n", YmodenFlash.updata_num);
        printf("##################################################\r\n");
    }
    else
    {
        printf("\r\n");
        printf("##################################################\r\n");
        printf("#  1.Start program\r\n");
        printf("#  2.Update program\r\n");
        printf("#  3.Set parameter\r\n");
        printf("#\r\n");
        printf("#  Now start/write Address: \t0x%.8x\r\n", Ymoden.startaddr);
        printf("#  Last updata Address:\t\t0x%.8x\r\n", YmodenFlash.last_startaddr);
        printf("#  Updata number:%d\r\n", YmodenFlash.updata_num);
        printf("##################################################\r\n");
    }
}

/**************************************************
 * @brief  Ymoden参数初始化
 * @date   2022-09-09   17:44
***************************************************/
void Ymoden_Init(void)
{
    Ymoden.process      = WAIT_ENTER_SYSTEM;
    Ymoden.status       = WAIT_START_UPDATA;
    Ymoden.id           = 0x01;
    Ymoden.addr         = STARTADDR_PAGE1;
    Ymoden.startaddr    = STARTADDR_PAGE1;
    Ymoden.filesize     = 0;

    YmodenFlash.startaddr = STARTADDR_PAGE1;
    YmodenFlash.filesize = 0;
    YmodenFlash.updata_num = 0;
}

/**************************************************
 * @brief  Ymoden程序主要控制逻辑
 * @date   2022-09-09   09:33
***************************************************/
void Ymoden_Program(void)
{
    static u8 cnt = 0;
    switch (Get_YModen_Status())
    {
    case WAIT_ENTER_SYSTEM:
        DelayMs(100);
        cnt++;
        if (cnt >= 50)      // 5s 延时
        {
            __set_PRIMASK(1);   // 关总中断，就是硬fault会被强制执行，所以需要关掉
            if (JumpToAPP(Ymoden.startaddr) == 0)
            {
                __set_PRIMASK(0);  //开总中断,跳转失败就重启中断
                printf("PROGRAM ERROR!\r\n");
                PrintfTips();
                cnt = 0;
                Set_YModen_Status(WAIT_ENTER_SYSTEM);
            }
        }
        break;

    case START_PROGRAM:
        printf("Jump To APP!\r\n");
        __set_PRIMASK(1);   // 关总中断，就是硬fault会被强制执行，所以需要关掉
        if (JumpToAPP(Ymoden.startaddr) == 0)
        {
            __set_PRIMASK(0);  //开总中断,跳转失败就重启中断
            printf("Jump To APP Failed!\r\n");
            printf("PROGRAM ERROR!\r\n");
            PrintfTips();
            cnt = 0;
            Set_YModen_Status(WAIT_ENTER_SYSTEM);
        }
        break;

    case UPDATE_PROGRAM:
        printf("C");
        DelayMs(1000);
        break;

    case SET_PARAMETER:
        cnt = 0;
        break;

    case UPDATE_SUCCESS:
        printf("\r\n");
        printf("UPDATA SUCCESS!\r\n");
        printf("RECV_LEN:%d\tACTR_LEN:%d\r\n", Ymoden.filesize, (Ymoden.addr - Ymoden.startaddr));

        // 记录要保存的数据
        YmodenFlash.startaddr = Ymoden.startaddr;
        YmodenFlash.filesize = Ymoden.filesize;
        memcpy(YmodenFlash.filename, Ymoden.filename, 32);

        // 在这里进行数据存储
        StartAddr_Write(&YmodenFlash);
        UPDATA_SUCCESS();
        printf("Flash Save Success!\r\n");
        PrintfTips();
        cnt = 0;
        Set_YModen_Status(WAIT_ENTER_SYSTEM);
        break;
    
    case UPDATE_FAILED:
        printf("UPDATA FAILED!\r\n");
        PrintfTips();
        cnt = 0;
        Set_YModen_Status(WAIT_ENTER_SYSTEM);
        break;
    
    default:
        break;
    }
}

/**************************************************
 * @brief  Ymoden串口数据处理
 * @param  data             接收数据的数组地址
 * @param  len              接收到的数据长度
 * @date   2022-09-09   09:33
***************************************************/
void Ymoden_Recv(u8 *data, u16 *len)
{
    u8 type = data[0];  // 获取数据类型
    u8 numlen = 0;
    switch (Ymoden.status)
    {
    // 开始接收数据/控制其他功能
    case WAIT_START_UPDATA:
        if (type == YMODEM_SOH)
        {
            // 防止触摸屏数据导致无法进入APP
            // 因为Modbus协议开头为地址，一般为01，因此补码肯定对不上
            if ((data[1] + data[2]) != 0xFF)    
            {
                break;
            }

            // 检测文件名和数据码
            if (Check_FILENAME(&data[3]) != 1)
            {
                YModen_CAN();   // 取消本次传输
                YModen_CAN();   // 取消本次传输
                Ymoden.status = WAIT_START_UPDATA;
                Ymoden.process = UPDATE_FAILED;
                break;
            }

            // 接收长度
            numlen = ((u8*)memchr(&data[3], 0x00, 35) - (u8*)&data[3]) + 1;
            if (Get_FILESIZE(&data[numlen + 3]) != 1)
            {
                YModen_CAN();   // 取消本次传输
                YModen_CAN();   // 取消本次传输
                Ymoden.status = WAIT_START_UPDATA;
                Ymoden.process = UPDATE_FAILED;
                break;
            }

            // 文件名和长度正常后，更新标志位，准备开始接收数据
            Ymoden.process = BUSY;
            // 写入Flash 更新开始标志位
            UPDATA_Flag_Erase();    // 先擦除，防止上一次的遗留
            UPDATA_Flag_Write();    // 重新写入
            // 以及获取要写入的地址
            Ymoden.addr = Ymoden.startaddr;
            // 还有地址和片区管理，记录之前的启动地址
            if (YmodenFlash.updata_num != 0)
                YmodenFlash.last_startaddr = YmodenFlash.startaddr;
            YmodenFlash.updata_num ++;
            // 擦除原程序，准备写入新程序
            USART_Cmd(USART3, DISABLE);
            Flash_ErasePages(Ymoden.startaddr, (Ymoden.filesize / FLASH_PAGE_SIZE) + 1);
            USART_Cmd(USART3, ENABLE);

            YModen_ACK();
            YModen_C();
            Ymoden.status = RECEVE_DATA;    // 准备接收数据
        }
        else if (type == '1' && *len == 1)
        {
            Ymoden.process = START_PROGRAM;
        }
        else if (type == '2' && *len == 1)
        {
            Ymoden.process = UPDATE_PROGRAM;
        }
        else if (type == '3' && *len == 1)
        {
            if (Ymoden.process != SET_PARAMETER)
                Ymoden.process = SET_PARAMETER;
            else 
                Ymoden.process = WAIT_ENTER_SYSTEM;
            PrintfTips();
        }
        else if (type == '4' && *len == 1)
        {
            if (Ymoden.process == SET_PARAMETER)
                Ymoden.addr = STARTADDR_PAGE1;
            Ymoden.startaddr = Ymoden.addr;
            PrintfTips();
        }
        else if (type == '5' && *len == 1)
        {
            if (Ymoden.process == SET_PARAMETER)
                Ymoden.addr = STARTADDR_PAGE2;
            Ymoden.startaddr = Ymoden.addr;
            PrintfTips();
        }
        else if (type == '6' && *len == 1)
        {
            if (Ymoden.process == SET_PARAMETER)
            {
                YmodenFlash.startaddr = STARTADDR_PAGE1;
                Ymoden.startaddr = YmodenFlash.startaddr;
                StartAddr_Write(&YmodenFlash);
                printf("Set StartPage1 Success!\r\n");
                printf("Flash Save Success!\r\n");
            }
            PrintfTips();
        }
        else if (type == '7' && *len == 1)
        {
            if (Ymoden.process == SET_PARAMETER)
            {
                YmodenFlash.startaddr = STARTADDR_PAGE2;
                Ymoden.startaddr = YmodenFlash.startaddr;
                StartAddr_Write(&YmodenFlash);
                printf("Set StartPage1 Success!\r\n");
                printf("Flash Save Success!\r\n");
            }
            PrintfTips();
        }
        else
        {
            // printf("Error Key!\r\n");
        }
        break;

    // 接收数据并写入Flash
    case RECEVE_DATA:
        if (type == YMODEM_SOH || type == YMODEM_STX) 
        {
            // 检测编码和补码是否正确
            if ((data[1] + data[2]) != 0xFF)
            {
                YModen_NACK();      // 发送接收错误信号，请求重发
            }
            else
            {
                if (type == YMODEM_SOH) 
                {
                    Flash_WriteData(Ymoden.addr, (u16*)&data[3], 128 / 2);
                    Ymoden.addr += 128;
                }
                else 
                {
                    Flash_WriteData(Ymoden.addr, (u16*)&data[3], 1024 / 2);
                    Ymoden.addr += 1024;
                }
                YModen_ACK();
            }
        }
        else if (type == YMODEM_EOT) // 第一次结束信号，等待第二次
        {
            YModen_NACK();
            Ymoden.status = RECEVE_END;
        }
        else 
        {
            YModen_CAN();   // 出错了就取消发送
            YModen_CAN();   // 出错了就取消发送
            Ymoden.status = WAIT_START_UPDATA;
        }
        break;

    // 完成一个文件的接收接收（Ymoden可能有多个文件，不过这里只传一个文件）
    case RECEVE_END:
        if (type == YMODEM_EOT) // 第二次等待信号，完成接收
        {
            YModen_ACK();
            YModen_C();
            Ymoden.status = RECEVE_SUCCESS;
        }
        else
        {
            YModen_CAN();   // 出错了就取消发送
            YModen_CAN();   // 出错了就取消发送
            Ymoden.status = WAIT_START_UPDATA;
        }
        break;

    // 完成此次数据传输
    case RECEVE_SUCCESS:
        if (type == YMODEM_SOH && data[4] == 0) // 收到此次更新结束信号
        {
            YModen_ACK();
            Ymoden.status = WAIT_START_UPDATA;
            Ymoden.process = UPDATE_SUCCESS;
        }
        else
        {
            YModen_CAN();   // 出错了就取消发送
            YModen_CAN();   // 出错了就取消发送
            Ymoden.status = WAIT_START_UPDATA;
        }
        break;
    }
    *len = 0;   // 等待下一次数据接收
}


