#ifndef _YMODEN_H_
#define _YMODEN_H_

/* 头文件区 */
#include "head.h"
#include "type.h"

/* 预编译区 */
// Ymoden指令
#define YMODEM_SOH		0x01    // 128  Byte
#define YMODEM_STX		0x02    // 1024 Byte
#define YMODEM_EOT		0x04    // 接收完成
#define YMODEM_ACK		0x06    // 正确接收反馈
#define YMODEM_NAK		0x15    // 错误接收反馈，要求重新发送
#define YMODEM_CA		0x18    // 取消发送
#define YMODEM_C		0x43    // 请求发送文件

#define FILENAME_RECV   "MasterControl"   // 接收的文件名的前缀必须是这个，否则不接收
#define STARTADDR_PAGE1 0x8010000
#define STARTADDR_PAGE2 0x8040000

/* 参数定义区 */
typedef enum {
	UPDATE_SUCCESS = 0,     // 更新程序成功
    UPDATE_FAILED,          // 更新程序失败
	WAIT_ENTER_SYSTEM,      // 等待进入程序，等待时间结束后，立刻进入APP
	BUSY,                   // 忙碌状态
	START_PROGRAM,          // 立刻进入APP
	UPDATE_PROGRAM,         // 更新程序
	SET_PARAMETER,          // 参数设置
} ProcessStatus;

typedef enum {
    WAIT_START_UPDATA = 0,  // 等待更新
    RECEVE_DATA,            // 接收数据中
    RECEVE_END,             // 单个文件接收完成
    RECEVE_SUCCESS,         // 本次更新完成
} RecvStatus;

typedef struct Ymoden_Struct
{
    ProcessStatus process;  // BootLoad模式
	uint8_t status;         // 更新程序状态
	uint8_t id;             // 帧功能
	uint32_t addr;          // 更新程序完成后为程序结束地址
    uint32_t startaddr;     // 程序开始地址
	uint32_t filesize;      // 文件大小（byte）
	char filename[32];      // 文件名
} Ymoden_Stu;

typedef struct YmodenFlash_Struct
{
	uint8_t addr0;

	char filename[32];      // 文件名
	uint32_t filesize;      // 文件大小（byte）
	uint32_t startaddr;     // 程序开始地址
	uint32_t last_startaddr;	// 上一个启动地址
	uint32_t updata_num;		// 统计成功更新次数

	uint8_t addr1;

	uint16_t ymoden_size;			// 写入Flash的大小（单位：8位）
	uint16_t ymoden_u16_size;		// 写入Flash的大小（单位：16位，半字）
} YmodenFlash_Stu;


/* 参数声明区 */
extern Ymoden_Stu Ymoden;
extern YmodenFlash_Stu YmodenFlash;


/* 函数声明区 */
static void YModen_ACK(void);
static void YModen_NACK(void);
static void YModen_CAN(void);
static void YModen_C(void);
void Set_YModen_Status(ProcessStatus process);
ProcessStatus Get_YModen_Status(void);
static u8 Get_FILESIZE(u8* data);
static u8 Check_FILENAME(u8* data);
void PrintfTips(void);
void Ymoden_Init(void);
void Ymoden_Program(void);
void Ymoden_Recv(u8 *data, u16 *len);

#endif
