// 2004.05.28, 2004.10.20, 2005.01.08, 2005.03.25, 2005.04.28, 2005.07.18, 2005.07.28, 2005.09.19, 2007.12.19, 2008.10.15
//****************************************
//**  Copyright  (C)  W.ch  1999-2008   **
//**  Web:  http://www.winchiphead.com  **
//****************************************
//**  DLL for USB interface chip CH341  **
//**  C, VC5.0                          **
//****************************************
//
// Partially translated from Chinese using google translate, some extra comments are added
//
// USB bus interface chip CH341 parallel port application layer interface library V2.1
// Nanjing Qinheng Electronics Co., Ltd. Author: W.ch 2008.10
// CH341-DLL V2.1
// Operating environment: Windows 98/ME, Windows 2000/XP
// support USB chip: CH341, CH341A
// USB => Parallel, I2C, SPI, JTAG ...
//

#ifndef		_CH341_DLL_H
#define		_CH341_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#define		mOFFSET( s, m )			( (ULONG) & ( ( ( s * ) 0 ) -> m ) )	// get relative offset address of a structure member

#ifndef		max
#define		max( a, b )				( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef		min
#define		min( a, b )				( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifdef		ExAllocatePool
#undef		ExAllocatePool						// Delete memory allocation with TAG
#endif

#ifndef		NTSTATUS
typedef		LONG	NTSTATUS;
#endif


typedef	struct	_USB_SETUP_PKT {				// USB控制传输的建立阶段的数据请求包结构
	UCHAR			mUspReqType;				// 00H 请求类型
	UCHAR			mUspRequest;				// 01H 请求代码
	union	{
		struct	{
			UCHAR	mUspValueLow;				// 02H 值参数低字节
			UCHAR	mUspValueHigh;				// 03H 值参数高字节
		};
		USHORT		mUspValue;					// 02H-03H 值参数
	};
	union	{
		struct	{
			UCHAR	mUspIndexLow;				// 04H 索引参数低字节
			UCHAR	mUspIndexHigh;				// 05H 索引参数高字节
		};
		USHORT		mUspIndex;					// 04H-05H 索引参数
	};
	USHORT			mLength;					// 06H-07H 数据阶段的数据长度
} mUSB_SETUP_PKT, *mPUSB_SETUP_PKT;


#define		mCH341_PACKET_LENGTH	32			// CH341支持的数据包的长度
#define		mCH341_PKT_LEN_SHORT	8			// CH341支持的短数据包的长度


typedef	struct	_WIN32_COMMAND {				// 定义WIN32命令接口结构
	union	{
		ULONG		mFunction;					// 输入时指定功能代码或者管道号
		NTSTATUS	mStatus;					// 输出时返回操作状态
	};
	ULONG			mLength;					// 存取长度,返回后续数据的长度
	union	{
		mUSB_SETUP_PKT	mSetupPkt;				// USB控制传输的建立阶段的数据请求
		UCHAR			mBuffer[ mCH341_PACKET_LENGTH ];	// 数据缓冲区,长度为0至255B
	};
} mWIN32_COMMAND, *mPWIN32_COMMAND;


// WIN32应用层接口命令
#define		IOCTL_CH341_COMMAND		( FILE_DEVICE_UNKNOWN << 16 | FILE_ANY_ACCESS << 14 | 0x0f34 << 2 | METHOD_BUFFERED )	// 专用接口

#define		mWIN32_COMMAND_HEAD		mOFFSET( mWIN32_COMMAND, mBuffer )	// WIN32命令接口的头长度

#define		mCH341_MAX_NUMBER		16			// 最多同时连接的CH341数

#define		mMAX_BUFFER_LENGTH		0x1000		// 数据缓冲区最大长度4096

#define		mMAX_COMMAND_LENGTH		( mWIN32_COMMAND_HEAD + mMAX_BUFFER_LENGTH )	// 最大数据长度加上命令结构头的长度

#define		mDEFAULT_BUFFER_LEN		0x0400		// 数据缓冲区默认长度1024

#define		mDEFAULT_COMMAND_LEN	( mWIN32_COMMAND_HEAD + mDEFAULT_BUFFER_LEN )	// 默认数据长度加上命令结构头的长度


// CH341端点地址
#define		mCH341_ENDP_INTER_UP	0x81		// CH341的中断数据上传端点的地址
#define		mCH341_ENDP_INTER_DOWN	0x01		// CH341的中断数据下传端点的地址
#define		mCH341_ENDP_DATA_UP		0x82		// CH341的数据块上传端点的地址
#define		mCH341_ENDP_DATA_DOWN	0x02		// CH341的数据块下传端点的地址


// 设备层接口提供的管道操作命令
#define		mPipeDeviceCtrl			0x00000004	// CH341的综合控制管道
#define		mPipeInterUp			0x00000005	// CH341的中断数据上传管道
#define		mPipeDataUp				0x00000006	// CH341的数据块上传管道
#define		mPipeDataDown			0x00000007	// CH341的数据块下传管道

// 应用层接口的功能代码
#define		mFuncNoOperation		0x00000000	// 无操作
#define		mFuncGetVersion			0x00000001	// 获取驱动程序版本号
#define		mFuncGetConfig			0x00000002	// 获取USB设备配置描述符
#define		mFuncSetTimeout			0x00000009	// 设置USB通讯超时
#define		mFuncSetExclusive		0x0000000b	// 设置独占使用
#define		mFuncResetDevice		0x0000000c	// 复位USB设备
#define		mFuncResetPipe			0x0000000d	// 复位USB管道
#define		mFuncAbortPipe			0x0000000e	// 取消USB管道的数据请求

// CH341并口专用的功能代码
#define		mFuncSetParaMode		0x0000000f	// 设置并口模式
#define		mFuncReadData0			0x00000010	// 从并口读取数据块0
#define		mFuncReadData1			0x00000011	// 从并口读取数据块1
#define		mFuncWriteData0			0x00000012	// 向并口写入数据块0
#define		mFuncWriteData1			0x00000013	// 向并口写入数据块1
#define		mFuncWriteRead			0x00000014	// 先输出再输入
#define		mFuncBufferMode			0x00000020	// 设定缓冲上传模式及查询缓冲区中的数据长度
#define		mFuncBufferModeDn		0x00000021	// 设定缓冲下传模式及查询缓冲区中的数据长度


// USB设备标准请求代码
#define		mUSB_CLR_FEATURE		0x01
#define		mUSB_SET_FEATURE		0x03
#define		mUSB_GET_STATUS			0x00
#define		mUSB_SET_ADDRESS		0x05
#define		mUSB_GET_DESCR			0x06
#define		mUSB_SET_DESCR			0x07
#define		mUSB_GET_CONFIG			0x08
#define		mUSB_SET_CONFIG			0x09
#define		mUSB_GET_INTERF			0x0a
#define		mUSB_SET_INTERF			0x0b
#define		mUSB_SYNC_FRAME			0x0c

// CH341控制传输的厂商专用请求类型
#define		mCH341_VENDOR_READ		0xC0		// 通过控制传输实现的CH341厂商专用读操作
#define		mCH341_VENDOR_WRITE		0x40		// 通过控制传输实现的CH341厂商专用写操作

// Manufacturer-specific request code for CH341 control transmission
#define		mCH341_PARA_INIT		0xB1		// 初始化并口
#define		mCH341_I2C_STATUS		0x52		// 获取I2C接口的状态
#define		mCH341_I2C_COMMAND		0x53		// 发出I2C接口的命令

// CH341并口操作命令代码
#define		mCH341_PARA_CMD_R0		0xAC		// 从并口读数据0,次字节为长度
#define		mCH341_PARA_CMD_R1		0xAD		// 从并口读数据1,次字节为长度
#define		mCH341_PARA_CMD_W0		0xA6		// 向并口写数据0,从次字节开始为数据流
#define		mCH341_PARA_CMD_W1		0xA7		// 向并口写数据1,从次字节开始为数据流
#define		mCH341_PARA_CMD_STS		0xA0		// 获取并口状态

// CH341A parallel port operation command code
#define		mCH341A_CMD_SET_OUTPUT	0xA1		// 设置并口输出
#define		mCH341A_CMD_IO_ADDR		0xA2		// MEM带地址读写/输入输出,从次字节开始为命令流
#define		mCH341A_CMD_PRINT_OUT	0xA3		// PRINT兼容打印方式输出,从次字节开始为数据流
#define		mCH341A_CMD_PWM_OUT		0xA4		// PWM数据输出的命令包,从次字节开始为数据流
#define		mCH341A_CMD_SHORT_PKT	0xA5		// 短包,次字节是该命令包的真正长度,再次字节及之后的字节是原命令包
#define		mCH341A_CMD_SPI_STREAM	0xA8		// SPI接口的命令包,从次字节开始为数据流
//#define		mCH341A_CMD_SIO_STREAM	0xA9		// SIO接口的命令包,从次字节开始为数据流
#define		mCH341A_CMD_I2C_STREAM	0xAA		// I2C接口的命令包,从次字节开始为I2C命令流
#define		mCH341A_CMD_UIO_STREAM	0xAB		// UIO接口的命令包,从次字节开始为命令流
#define		mCH341A_CMD_PIO_STREAM	0xAE		// PIO接口的命令包,从次字节开始为数据流

// CH341A控制传输的厂商专用请求代码
#define		mCH341A_BUF_CLEAR		0xB2		// 清除未完成的数据
#define		mCH341A_I2C_CMD_X		0x54		// 发出I2C接口的命令,立即执行
#define		mCH341A_DELAY_MS		0x5E		// 以亳秒为单位延时指定时间
#define		mCH341A_GET_VER			0x5F		// 获取芯片版本

#define		mCH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )	// CH341在EPP/MEM方式下单次读写数据块的最大长度
#define		mCH341A_EPP_IO_MAX		0xFF		// CH341A在EPP/MEM方式下单次读写数据块的最大长度

#define		mCH341A_CMD_IO_ADDR_W	0x00		// MEM带地址读写/输入输出的命令流:写数据,位6-位0为地址,下一个字节为待写数据
#define		mCH341A_CMD_IO_ADDR_R	0x80		// MEM带地址读写/输入输出的命令流:读数据,位6-位0为地址,读出数据一起返回

#define		mCH341A_CMD_I2C_STM_STA	0x74		// I2C接口的命令流:产生起始位
#define		mCH341A_CMD_I2C_STM_STO	0x75		// I2C接口的命令流:产生停止位
#define		mCH341A_CMD_I2C_STM_OUT	0x80		// I2C接口的命令流:输出数据,位5-位0为长度,后续字节为数据,0长度则只发送一个字节并返回应答
#define		mCH341A_CMD_I2C_STM_IN	0xC0		// I2C接口的命令流:输入数据,位5-位0为长度,0长度则只接收一个字节并发送无应答
#define		mCH341A_CMD_I2C_STM_MAX	( min( 0x3F, mCH341_PACKET_LENGTH ) )	// I2C接口的命令流单个命令输入输出数据的最大长度
#define		mCH341A_CMD_I2C_STM_SET	0x60		// I2C接口的命令流:设置参数,位2=SPI的I/O数(0=单入单出,1=双入双出),位1位0=I2C速度(00=低速,01=标准,10=快速,11=高速)
#define		mCH341A_CMD_I2C_STM_US	0x40		// I2C接口的命令流:以微秒为单位延时,位3-位0为延时值
#define		mCH341A_CMD_I2C_STM_MS	0x50		// I2C接口的命令流:以亳秒为单位延时,位3-位0为延时值
#define		mCH341A_CMD_I2C_STM_DLY	0x0F		// I2C接口的命令流单个命令延时的最大值
#define		mCH341A_CMD_I2C_STM_END	0x00		// I2C接口的命令流:命令包提前结束

#define		mCH341A_CMD_UIO_STM_IN	0x00		// UIO接口的命令流:输入数据D7-D0
#define		mCH341A_CMD_UIO_STM_DIR	0x40		// UIO接口的命令流:设定I/O方向D5-D0,位5-位0为方向数据
#define		mCH341A_CMD_UIO_STM_OUT	0x80		// UIO接口的命令流:输出数据D5-D0,位5-位0为数据
#define		mCH341A_CMD_UIO_STM_US	0xC0		// UIO接口的命令流:以微秒为单位延时,位5-位0为延时值
#define		mCH341A_CMD_UIO_STM_END	0x20		// UIO接口的命令流:命令包提前结束


// CH341并口工作模式
#define		mCH341_PARA_MODE_EPP	0x00		// CH341并口工作模式为EPP方式
#define		mCH341_PARA_MODE_EPP17	0x00		// CH341A并口工作模式为EPP方式V1.7
#define		mCH341_PARA_MODE_EPP19	0x01		// CH341A并口工作模式为EPP方式V1.9
#define		mCH341_PARA_MODE_MEM	0x02		// CH341并口工作模式为MEM方式
#define		mCH341_PARA_MODE_ECP	0x03		// CH341A并口工作模式为ECP方式


// I/O方向设置位定义,直接输入的状态信号的位定义,直接输出的位数据定义
#define		mStateBitERR			0x00000100	// 只读可写,ERR#引脚输入状态,1:高电平,0:低电平
#define		mStateBitPEMP			0x00000200	// 只读可写,PEMP引脚输入状态,1:高电平,0:低电平
#define		mStateBitINT			0x00000400	// 只读可写,INT#引脚输入状态,1:高电平,0:低电平
#define		mStateBitSLCT			0x00000800	// 只读可写,SLCT引脚输入状态,1:高电平,0:低电平
#define		mStateBitWAIT			0x00002000	// 只读可写,WAIT#引脚输入状态,1:高电平,0:低电平
#define		mStateBitDATAS			0x00004000	// 只写可读,DATAS#/READ#引脚输入状态,1:高电平,0:低电平
#define		mStateBitADDRS			0x00008000	// 只写可读,ADDRS#/ADDR/ALE引脚输入状态,1:高电平,0:低电平
#define		mStateBitRESET			0x00010000	// 只写,RESET#引脚输入状态,1:高电平,0:低电平
#define		mStateBitWRITE			0x00020000	// 只写,WRITE#引脚输入状态,1:高电平,0:低电平
#define		mStateBitSCL			0x00400000	// 只读,SCL引脚输入状态,1:高电平,0:低电平
#define		mStateBitSDA			0x00800000	// 只读,SDA引脚输入状态,1:高电平,0:低电平


#define		MAX_DEVICE_PATH_SIZE	128			// 设备名称的最大字符数
#define		MAX_DEVICE_ID_SIZE		64			// 设备ID的最大字符数


typedef VOID ( CALLBACK * mPCH341_INT_ROUTINE ) (	// Interrupt service routine
	ULONG iStatus );	// Interrupt status data, refer to the bit description below
						// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
						// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, and bit 11 corresponds to the SLCT pin of CH341


HANDLE	WINAPI	CH341OpenDevice(	// Open the CH341 device and return the handle. If an error occurs, it will be invalid.
	ULONG			iIndex );		// Specify the CH341 device serial number, 0 corresponds to the first device


VOID	WINAPI	CH341CloseDevice(	// Shut down the CH341 device
	ULONG			iIndex );		// Specify CH341 device serial number


ULONG	WINAPI	CH341GetVersion( );  // Get the DLL version number and return the version number


ULONG WINAPI CH341DriverCommand(	// Directly pass the command to the driver, return 0 if an error occurs, otherwise return the data length
	ULONG iIndex,					// Specify the CH341 device serial number. DLLs above V1.6 can also be the handle after the device is opened.
	mPWIN32_COMMAND ioCommand );	// Pointer to the command structure
// This program returns the data length after the call, and still returns the command structure. If it is a read operation, the data is returned in the command structure,
// The returned data length is 0 when the operation fails, and is the length of the entire command structure when the operation is successful. For example, if one byte is read, mWIN32_COMMAND_HEAD+1 is returned.
// Before calling the command structure, respectively provide: pipe number or command function code, length of access data (optional), data (optional)
// After the command structure is called, it returns: operation status code, length of subsequent data (optional),
// The operation status code is a code defined by WINDOWS, please refer to NTSTATUS.H,
// The length of subsequent data refers to the length of data returned by the read operation. The data is stored in the subsequent buffer. It is generally 0 for write operations.


ULONG	WINAPI	CH341GetDrvVersion( );  // Get the driver version number, return the version number, or return 0 if an error occurs


BOOL WINAPI CH341ResetDevice(	// Reset USB device
	ULONG iIndex );				// Specify CH341 device serial number	


BOOL WINAPI CH341GetDeviceDescr(	// Read device descriptor
	ULONG iIndex,					// Specify CH341 device serial number
	PVOID oBuffer,					// Points to a buffer large enough to save descriptors
	PULONG ioLength );				// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341GetConfigDescr(  // 读取配置描述符
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存描述符
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL WINAPI CH341SetIntRoutine(			// Set interrupt service routine
	ULONG iIndex,						// Specify CH341 device serial number
	mPCH341_INT_ROUTINE iIntRoutine );	// Specify the interrupt service program. If it is NULL, the interrupt service will be canceled. Otherwise, the program will be called during the interrupt.	


BOOL WINAPI CH341ReadInter(		// Read interrupt data
	ULONG iIndex,				// Specify CH341 device serial number
	PULONG iStatus );			// Points to a double word unit, used to save the read interrupt status data, see below
	// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
	// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, and bit 11 corresponds to the SLCT pin of CH341


BOOL	WINAPI	CH341AbortInter(  // 放弃中断数据读操作
	ULONG			iIndex );  // 指定CH341设备序号


BOOL	WINAPI	CH341SetParaMode(  // 设置并口模式
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iMode );  // 指定并口模式: 0为EPP模式/EPP模式V1.7, 1为EPP模式V1.9, 2为MEM模式


BOOL	WINAPI	CH341InitParallel(  // 复位并初始化并口,RST#输出低电平脉冲
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iMode );  // 指定并口模式: 0为EPP模式/EPP模式V1.7, 1为EPP模式V1.9, 2为MEM模式, >= 0x00000100 保持当前模式


BOOL	WINAPI	CH341ReadData0(  // 从0#端口读取数据块
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存读取的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL	WINAPI	CH341ReadData1(  // 从1#端口读取数据块
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存读取的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL	WINAPI	CH341AbortRead(  // 放弃数据块读操作
	ULONG			iIndex );  // 指定CH341设备序号


BOOL	WINAPI	CH341WriteData0(  // 向0#端口写出数据块
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341WriteData1(  // 向1#端口写出数据块
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341AbortWrite(  // 放弃数据块写操作
	ULONG			iIndex );  // 指定CH341设备序号


BOOL	WINAPI	CH341GetStatus(  // 通过CH341直接输入数据和状态
	ULONG			iIndex,  // 指定CH341设备序号
	PULONG			iStatus );  // 指向一个双字单元,用于保存状态数据,参考下面的位说明
// 位7-位0对应CH341的D7-D0引脚
// 位8对应CH341的ERR#引脚, 位9对应CH341的PEMP引脚, 位10对应CH341的INT#引脚, 位11对应CH341的SLCT引脚, 位23对应CH341的SDA引脚
// 位13对应CH341的BUSY/WAIT#引脚, 位14对应CH341的AUTOFD#/DATAS#引脚,位15对应CH341的SLCTIN#/ADDRS#引脚


BOOL WINAPI CH341ReadI2C( 	// Read one byte of data from the I2C interface
	ULONG iIndex, 			// Specify CH341 device serial number
	UCHAR iDevice, 			// The lower 7 bits specify the I2C device address
	UCHAR iAddr, 			// Specify the address of the data unit
	PUCHAR oByte );			// Points to a byte unit, used to save the read byte data	


BOOL WINAPI CH341WriteI2C(	// Write one byte of data to the I2C interface
	ULONG iIndex,			// Specify CH341 device serial number
	UCHAR iDevice,			// The lower 7 bits specify the I2C device address
	UCHAR iAddr,			// Specify the address of the data unit
	UCHAR iByte );			// Byte data to be written	


BOOL WINAPI CH341EppReadData(	// Read data in EPP mode: WR#=1, DS#=0, AS#=1, D0-D7=input
	ULONG iIndex,				// Specify CH341 device serial number
	PVOID oBuffer,				// Points to a buffer large enough to save the read data
	PULONG ioLength );			// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341EppReadAddr(  // EPP方式读地址: WR#=1, DS#=1, AS#=0, D0-D7=input
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存读取的地址数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL	WINAPI	CH341EppWriteData(  // EPP方式写数据: WR#=0, DS#=0, AS#=1, D0-D7=output
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341EppWriteAddr(  // EPP方式写地址: WR#=0, DS#=1, AS#=0, D0-D7=output
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备写出的地址数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341EppSetAddr(  // EPP方式设置地址: WR#=0, DS#=1, AS#=0, D0-D7=output
	ULONG			iIndex,  // 指定CH341设备序号
	UCHAR			iAddr );  // 指定EPP地址


BOOL	WINAPI	CH341MemReadAddr0(  // MEM方式读地址0: WR#=1, DS#/RD#=0, AS#/ADDR=0, D0-D7=input
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存从地址0读取的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL	WINAPI	CH341MemReadAddr1(  // MEM方式读地址1: WR#=1, DS#/RD#=0, AS#/ADDR=1, D0-D7=input
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			oBuffer,  // 指向一个足够大的缓冲区,用于保存从地址1读取的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度


BOOL	WINAPI	CH341MemWriteAddr0(  // MEM方式写地址0: WR#=0, DS#/RD#=1, AS#/ADDR=0, D0-D7=output
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备向地址0写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341MemWriteAddr1(  // MEM方式写地址1: WR#=0, DS#/RD#=1, AS#/ADDR=1, D0-D7=output
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备向地址1写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


BOOL	WINAPI	CH341SetExclusive(  // 设置独占使用当前CH341设备
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iExclusive );  // 为0则设备可以共享使用,非0则独占使用


BOOL	WINAPI	CH341SetTimeout(  // 设置USB数据读写的超时
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iWriteTimeout,  // 指定USB写出数据块的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)
	ULONG			iReadTimeout );  // 指定USB读取数据块的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)


BOOL WINAPI CH341ReadData(	// Read data block
	ULONG iIndex,			// Specify CH341 device serial number
	PVOID oBuffer,			// Points to a buffer large enough to save the read data
	PULONG ioLength );		// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341WriteData(  // 写出数据块
	ULONG			iIndex,  // 指定CH341设备序号
	PVOID			iBuffer,  // 指向一个缓冲区,放置准备写出的数据
	PULONG			ioLength );  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度


PVOID	WINAPI	CH341GetDeviceName(  // 返回指向CH341设备名称的缓冲区,出错则返回NULL
	ULONG			iIndex );  // 指定CH341设备序号,0对应第一个设备


ULONG	WINAPI	CH341GetVerIC(  // 获取CH341芯片的版本,返回:0=设备无效,0x10=CH341,0x20=CH341A
	ULONG			iIndex );  // 指定CH341设备序号
#define		IC_VER_CH341A		0x20
#define		IC_VER_CH341A3		0x30


BOOL	WINAPI	CH341FlushBuffer(  // 清空CH341的缓冲区
	ULONG			iIndex );  // 指定CH341设备序号


BOOL WINAPI CH341WriteRead(	// Execute data flow command, output first and then input
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iWriteLength,		// Write length, the length to be written
	PVOID iWriteBuffer,		// Points to a buffer to place the data to be written.
	ULONG iReadStep,		// The length of a single block to be read, the total length to be read is (iReadStep*iReadTimes)
	ULONG iReadTimes,		// Number of times to prepare for reading
	PULONG oReadLength,		// Points to the length unit, and returns the actual read length.
	PVOID oReadBuffer );	// Points to a buffer large enough to save the read data	
	

/*
Mode:
 Bit 1-Bit 0: I2C interface speed/SCL frequency
	00=low speed/20KHz
	01=standard/100KHz (default value)
	10=fast/400KHz
	11=high speed/750KHz
 Bit 2: SPI I/O number/IO pin
	0=single input and single output (D3 clock/D5 output/D7 input) (default value)
	1=double input and double output (D3 clock/D5 output D4 Out/D7 in D6 in)
 Bit 7: Bit order in SPI byte
	0=low end first
	1=high end first
 Other bits reserved, must be 0
*/
BOOL WINAPI CH341SetStream(	// Set the serial port stream mode
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iMode );			// Specify mode


BOOL WINAPI CH341SetDelaymS(	// Set the hardware asynchronous delay, return soon after the call, and delay the specified number of milliseconds before the next stream operation
	ULONG iIndex,				//Specify CH341 device serial number
	ULONG iDelay );				//Specify the number of milliseconds of delay	
	

// write, then read; set data pointer to NULL, data size to 0 to skip
BOOL WINAPI CH341StreamI2C(	// Processes I2C data stream, 2-wire interface, clock line is SCL pin, data line is SDA pin (quasi-bidirectional I/O), speed is about 56K bytes
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iWriteLength,		// Number of data bytes to be written
	PVOID iWriteBuffer,		// Points to a buffer to place the data to be written. The first byte is usually the I2C device address and read and write direction bits.
	ULONG iReadLength,		// Number of data bytes to be read
	PVOID oReadBuffer );	// Points to a buffer, and returns the read data	
	

typedef	enum	_EEPROM_TYPE {					// EEPROM型号
	ID_24C01,
	ID_24C02,
	ID_24C04,
	ID_24C08,
	ID_24C16,
	ID_24C32,
	ID_24C64,
	ID_24C128,
	ID_24C256,
	ID_24C512,
	ID_24C1024,
	ID_24C2048,
	ID_24C4096
} EEPROM_TYPE;


BOOL	WINAPI	CH341ReadEEPROM(  // 从EEPROM中读取数据块,速度约56K字节
	ULONG			iIndex,  // 指定CH341设备序号
	EEPROM_TYPE		iEepromID,  // 指定EEPROM型号
	ULONG			iAddr,  // 指定数据单元的地址
	ULONG			iLength,  // 准备读取的数据字节数
	PUCHAR			oBuffer );  // 指向一个缓冲区,返回后是读入的数据


BOOL	WINAPI	CH341WriteEEPROM(  // 向EEPROM中写入数据块
	ULONG			iIndex,  // 指定CH341设备序号
	EEPROM_TYPE		iEepromID,  // 指定EEPROM型号
	ULONG			iAddr,  // 指定数据单元的地址
	ULONG			iLength,  // 准备写出的数据字节数
	PUCHAR			iBuffer );  // 指向一个缓冲区,放置准备写出的数据


BOOL	WINAPI	CH341GetInput(  // 通过CH341直接输入数据和状态,效率比CH341GetStatus更高
	ULONG			iIndex,  // 指定CH341设备序号
	PULONG			iStatus );  // 指向一个双字单元,用于保存状态数据,参考下面的位说明
// 位7-位0对应CH341的D7-D0引脚
// 位8对应CH341的ERR#引脚, 位9对应CH341的PEMP引脚, 位10对应CH341的INT#引脚, 位11对应CH341的SLCT引脚, 位23对应CH341的SDA引脚
// 位13对应CH341的BUSY/WAIT#引脚, 位14对应CH341的AUTOFD#/DATAS#引脚,位15对应CH341的SLCTIN#/ADDRS#引脚


BOOL	WINAPI	CH341SetOutput(  // 设置CH341的I/O方向,并通过CH341直接输出数据
/* ***** 谨慎使用该API, 防止修改I/O方向使输入引脚变为输出引脚导致与其它输出引脚之间短路而损坏芯片 ***** */
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iEnable,  // 数据有效标志,参考下面的位说明
// 位0为1说明iSetDataOut的位15-位8有效,否则忽略
// 位1为1说明iSetDirOut的位15-位8有效,否则忽略
// 位2为1说明iSetDataOut的7-位0有效,否则忽略
// 位3为1说明iSetDirOut的位7-位0有效,否则忽略
// 位4为1说明iSetDataOut的位23-位16有效,否则忽略
	ULONG			iSetDirOut,  // 设置I/O方向,某位清0则对应引脚为输入,某位置1则对应引脚为输出,并口方式下默认值为0x000FC000,参考下面的位说明
	ULONG			iSetDataOut );  // 输出数据,如果I/O方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平,参考下面的位说明
// 位7-位0对应CH341的D7-D0引脚
// 位8对应CH341的ERR#引脚, 位9对应CH341的PEMP引脚, 位10对应CH341的INT#引脚, 位11对应CH341的SLCT引脚
// 位13对应CH341的WAIT#引脚, 位14对应CH341的DATAS#/READ#引脚,位15对应CH341的ADDRS#/ADDR/ALE引脚
// 以下引脚只能输出,不考虑I/O方向: 位16对应CH341的RESET#引脚, 位17对应CH341的WRITE#引脚, 位18对应CH341的SCL引脚, 位29对应CH341的SDA引脚


BOOL	WINAPI	CH341Set_D5_D0(  // 设置CH341的D5-D0引脚的I/O方向,并通过CH341的D5-D0引脚直接输出数据,效率比CH341SetOutput更高
/* ***** 谨慎使用该API, 防止修改I/O方向使输入引脚变为输出引脚导致与其它输出引脚之间短路而损坏芯片 ***** */
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iSetDirOut,  // 设置D5-D0各引脚的I/O方向,某位清0则对应引脚为输入,某位置1则对应引脚为输出,并口方式下默认值为0x00全部输入
	ULONG			iSetDataOut );  // 设置D5-D0各引脚的输出数据,如果I/O方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平
// 以上数据的位5-位0分别对应CH341的D5-D0引脚


BOOL	WINAPI	CH341StreamSPI3(  // 该API已失效,请勿使用
	ULONG			iIndex,
	ULONG			iChipSelect,
	ULONG			iLength,
	PVOID			ioBuffer );


BOOL	WINAPI	CH341StreamSPI4(  // 处理SPI数据流,4线接口,时钟线为DCK/D3引脚,输出数据线为DOUT/D5引脚,输入数据线为DIN/D7引脚,片选线为D0/D1/D2,速度约68K字节
/* SPI时序: DCK/D3引脚为时钟输出, 默认为低电平, DOUT/D5引脚在时钟上升沿之前的低电平期间输出, DIN/D7引脚在时钟下降沿之前的高电平期间输入 */
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iChipSelect,  // 片选控制, 位7为0则忽略片选控制, 位7为1则参数有效: 位1位0为00/01/10分别选择D0/D1/D2引脚作为低电平有效片选
	ULONG			iLength,  // 准备传输的数据字节数
	PVOID			ioBuffer );  // 指向一个缓冲区,放置准备从DOUT写出的数据,返回后是从DIN读入的数据


BOOL	WINAPI	CH341StreamSPI5(  // 处理SPI数据流,5线接口,时钟线为DCK/D3引脚,输出数据线为DOUT/D5和DOUT2/D4引脚,输入数据线为DIN/D7和DIN2/D6引脚,片选线为D0/D1/D2,速度约30K字节*2
/* SPI时序: DCK/D3引脚为时钟输出, 默认为低电平, DOUT/D5和DOUT2/D4引脚在时钟上升沿之前的低电平期间输出, DIN/D7和DIN2/D6引脚在时钟下降沿之前的高电平期间输入 */
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iChipSelect,  // 片选控制, 位7为0则忽略片选控制, 位7为1则参数有效: 位1位0为00/01/10分别选择D0/D1/D2引脚作为低电平有效片选
	ULONG			iLength,  // 准备传输的数据字节数
	PVOID			ioBuffer,  // 指向一个缓冲区,放置准备从DOUT写出的数据,返回后是从DIN读入的数据
	PVOID			ioBuffer2 );  // 指向第二个缓冲区,放置准备从DOUT2写出的数据,返回后是从DIN2读入的数据


BOOL	WINAPI	CH341BitStreamSPI(  // 处理SPI位数据流,4线/5线接口,时钟线为DCK/D3引脚,输出数据线为DOUT/DOUT2引脚,输入数据线为DIN/DIN2引脚,片选线为D0/D1/D2,速度约8K位*2
	ULONG			iIndex,  // 指定CH341设备序号
	ULONG			iLength,  // 准备传输的数据位数,一次最多896,建议不超过256
	PVOID			ioBuffer );  // 指向一个缓冲区,放置准备从DOUT/DOUT2/D2-D0写出的数据,返回后是从DIN/DIN2读入的数据
/* SPI时序: DCK/D3引脚为时钟输出, 默认为低电平, DOUT/D5和DOUT2/D4引脚在时钟上升沿之前的低电平期间输出, DIN/D7和DIN2/D6引脚在时钟下降沿之前的高电平期间输入 */
/* ioBuffer中的一个字节共8位分别对应D7-D0引脚, 位5输出到DOUT, 位4输出到DOUT2, 位2-位0输出到D2-D0, 位7从DIN输入, 位6从DIN2输入, 位3数据忽略 */
/* 在调用该API之前,应该先调用CH341Set_D5_D0设置CH341的D5-D0引脚的I/O方向,并设置引脚的默认电平 */


BOOL	WINAPI	CH341SetBufUpload(  // 设定内部缓冲上传模式
	ULONG			iIndex,  // 指定CH341设备序号,0对应第一个设备
	ULONG			iEnableOrClear );  // 为0则禁止内部缓冲上传模式,使用直接上传,非0则启用内部缓冲上传模式并清除缓冲区中的已有数据
// 如果启用内部缓冲上传模式,那么CH341驱动程序创建线程自动接收USB上传数据到内部缓冲区,同时清除缓冲区中的已有数据,当应用程序调用CH341ReadData后将立即返回缓冲区中的已有数据


LONG	WINAPI	CH341QueryBufUpload(  // 查询内部上传缓冲区中的已有数据包个数,成功返回数据包个数,出错返回-1
	ULONG			iIndex );  // 指定CH341设备序号,0对应第一个设备


BOOL	WINAPI	CH341SetBufDownload(  // 设定内部缓冲下传模式
	ULONG			iIndex,  // 指定CH341设备序号,0对应第一个设备
	ULONG			iEnableOrClear );  // 为0则禁止内部缓冲下传模式,使用直接下传,非0则启用内部缓冲下传模式并清除缓冲区中的已有数据
// 如果启用内部缓冲下传模式,那么当应用程序调用CH341WriteData后将仅仅是将USB下传数据放到内部缓冲区并立即返回,而由CH341驱动程序创建的线程自动发送直到完毕


LONG	WINAPI	CH341QueryBufDownload(  // 查询内部下传缓冲区中的剩余数据包个数(尚未发送),成功返回数据包个数,出错返回-1
	ULONG			iIndex );  // 指定CH341设备序号,0对应第一个设备


BOOL	WINAPI	CH341ResetInter(  // 复位中断数据读操作
	ULONG			iIndex );  // 指定CH341设备序号


BOOL	WINAPI	CH341ResetRead(  // 复位数据块读操作
	ULONG			iIndex );  // 指定CH341设备序号


BOOL	WINAPI	CH341ResetWrite(  // 复位数据块写操作
	ULONG			iIndex );  // 指定CH341设备序号


typedef		VOID	( CALLBACK	* mPCH341_NOTIFY_ROUTINE ) (  // 设备事件通知回调程序
	ULONG			iEventStatus );  // 设备事件和当前状态(在下行定义): 0=设备拔出事件, 3=设备插入事件

#define		CH341_DEVICE_ARRIVAL		3		// 设备插入事件,已经插入
#define		CH341_DEVICE_REMOVE_PEND	1		// 设备将要拔出
#define		CH341_DEVICE_REMOVE			0		// 设备拔出事件,已经拔出


BOOL	WINAPI	CH341SetDeviceNotify(  // 设定设备事件通知程序
	ULONG					iIndex,  // 指定CH341设备序号,0对应第一个设备
	PCHAR					iDeviceID,  // 可选参数,指向字符串,指定被监控的设备的ID,字符串以\0终止
	mPCH341_NOTIFY_ROUTINE	iNotifyRoutine );  // 指定设备事件回调程序,为NULL则取消事件通知,否则在检测到事件时调用该程序


BOOL	WINAPI	CH341SetupSerial(  // 设定CH341的串口特性,该API只能用于工作于串口方式的CH341芯片
	ULONG					iIndex,  // 指定CH341设备序号,0对应第一个设备
	ULONG					iParityMode,  // 指定CH341串口的数据校验模式: NOPARITY/ODDPARITY/EVENPARITY/MARKPARITY/SPACEPARITY
	ULONG					iBaudRate );  // 指定CH341串口的通讯波特率值,可以是50至3000000之间的任意值

/*  The following APIs can be used for CH341 chips that work in serial port mode. Other APIs can generally only be used for CH341 chips that work in parallel port mode.
	CH341OpenDevice
	CH341CloseDevice
	CH341SetupSerial
	CH341ReadData
	CH341WriteData
	CH341SetBufUpload
	CH341QueryBufUpload
	CH341SetBufDownload
	CH341QueryBufDownload
	CH341SetDeviceNotify
	CH341GetStatus
//  The above is the main API, the following is the secondary API
	CH341GetVersion
	CH341DriverCommand
	CH341GetDrvVersion
	CH341ResetDevice
	CH341GetDeviceDescr
	CH341GetConfigDescr
	CH341SetIntRoutine
	CH341ReadInter
	CH341AbortInter
	CH341AbortRead
	CH341AbortWrite
	CH341ReadI2C
	CH341WriteI2C
	CH341SetExclusive
	CH341SetTimeout
	CH341GetDeviceName
	CH341GetVerIC
	CH341FlushBuffer
	CH341WriteRead
	CH341ResetInter
	CH341ResetRead
	CH341ResetWrite
*/
HANDLE	WINAPI	CH341OpenDeviceEx(   // 打开CH341设备,返回句柄,出错则无效
    ULONG			iIndex );        // 指定CH341设备序号,0对应插入的第一个设备,1对应插入的第二个设备,为节约设备设备序号资源,用完后要关闭设备

VOID	WINAPI	CH341CloseDeviceEx(  // 关闭CH341设备
	ULONG			iIndex );        // 指定CH341设备序号

PCHAR	WINAPI	CH341GetDeviceNameEx(   // 返回指向CH341设备名称的缓冲区,出错则返回NULL
	ULONG			iIndex );           // 指定CH341设备序号,0对应第一个设备

BOOL	WINAPI	CH341SetDeviceNotifyEx(       // 设定设备事件通知程序
	ULONG					iIndex,           // 指定CH341设备序号,0对应第一个设备
	PCHAR					iDeviceID,        // 可选参数,指向字符串,指定被监控的设备的ID,字符串以\0终止
	mPCH341_NOTIFY_ROUTINE	iNotifyRoutine ); // 指定设备事件回调程序,为NULL则取消事件通知,否则在检测到事件时调用该程序


#ifdef __cplusplus
}
#endif

#endif		// _CH341_DLL_H
