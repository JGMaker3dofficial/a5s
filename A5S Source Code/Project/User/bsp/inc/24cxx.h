#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"   
//////////////////////////////////////////////////////////////////////////////////	 


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define EE_TYPE AT24C64

#define	RESET_VALUE	0

//开机自检设置
#define OPEN_STATE_ADDR			0
#define OPEN_STATE_VALUE		0X55

//系统图标文件OK
#define SYSTEM_STATE_ADDR			1
#define SYSTEM_STATE_VALUE		0X35

//标准字库存在的记录
#define ST_FONT_STATE_ADDR			2
#define ST_FONT_STATE_VALUE		0XB5


//LCD屏幕校准
#define LCD_CALBT_STATE_ADDR		3
#define LCD_CALBT_STATE_VALUE		0X1A

//液晶界面语言设置
#define LANGUAGE_STATE_ADDR		4
#define LANGUAGE_STATE_EN		0X25
#define LANGUAGE_STATE_CH		0XC5

//打印界面图标状态的记录
#define PRINTUI_BMP_STATE_ADDR		5
#define PRINTUI_BMP_STATE_VALUE		0XA5

//开机界面LOGO图标状态的记录
#define LOGO_BMP_STATE_ADDR			6
#define LOGO_BMP_STATE_VALUE		0X75

//项目名称14bytes
#define PROJECT_NAME_BASE_ADDR			7
#define PROJECT_NAME	"A3"


//PCBA(硬件版本号)20bytes
//PCBA版本号一般情况下发行的版本不多，大类方面分一个测试版本和一个发行版本
#define PCBA_BASE_ADDR			21
#define	PCBA_VER_TEST					"V1.0 Test"
#define	PCBA_VER_RELEASE				"V1.0 Release"

//////////////////////////////////////////////////////////////////////////	 
//保存TFT_LCD触摸屏的初始化数据信息，占用14个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define SAVE_ADDR_BASE 40

//PN号，占20bytes
//比如：20171016-001A,具体定义未定义
#define PN_BASE_ADDR   60
#define PN_INFOR	"20171016-001A"

//SN号，占20bytes
//比如：20171016-001A,具体定义未定义
#define SN_BASE_ADDR   80
#define SN_INFOR	"20171016-001A"

//软件版本号--FW，25bytes
//比如2017-10-10 15：21：32 Test
#define FW_BASE_ADDR   100
#define FW_VER_TEST		"2017-10-16 15：21：32 Test"
#define FW_VER_RELEASE	"2017-10-16 15：21：32 Release"

//断电保存挤出头温度信息,4bytes
#define POWER_OFF_EXTUDER_BASE_ADDR   125

//断电保存热床温度信息,4bytes
#define POWER_OFF_BED_BASE_ADDR   129

//断电保存X轴位置信息,4bytes
#define POWER_OFF_X_BASE_ADDR   133

//断电保存Y轴位置信息,4bytes
#define POWER_OFF_Y_BASE_ADDR   137

//断电保存Z轴位置信息,4bytes
#define POWER_OFF_Z_BASE_ADDR   141					  
					  
//断电保存E轴位置信息,4bytes
#define POWER_OFF_E_BASE_ADDR   145					  
					  
//断电保存打印百分比,1byte
#define POWER_OFF_PERCENTAGE_BASE_ADDR   149		

//断电标志，1byte
#define POWER_OFF_FLAG_BASE_ADDR   150
#define POWER_OFF_FLAG_VAL_SET   0XAA
#define POWER_OFF_FLAG_VAL_RESET   0X05

//断电保存gcode文件行号,4bytes
#define POWER_OFF_FILE_LINE_NUM_BASE_ADDR   151

//断电保存gcode文件位置,4bytes
#define POWER_OFF_FILE_POS_BASE_ADDR   155

//断电保存gcode文件名,2bytes					  
#define POWER_OFF_FN_LENGTH_BASE_ADDR   159	
	
//断电保存打印时间，4bytes	
#define POWER_OFF_PRINT_TIME_BASE_ADDR  161	

//断电保存涡轮风扇值，1byte	
#define POWER_OFF_FAN__BASE_ADDR  	165	
	
//断电时保存命令缓冲区 + planner执行缓冲区中的gcode码条数	
#define POWER_OFF_NOT_EXACUTE_GCODE_BASE_ADDR	166


#define POWER_OFF_Z_UP_FLAG_ADDR	167//断电续打Z轴上移标志
#define POWER_OFF_Z_UP_FLAG_SET		0X3E
#define POWER_OFF_Z_UP_FLAG_RESET		0XA0




//断电保存gcode文件名,101bytes					  
#define POWER_OFF_FILE_NAME_BASE_ADDR   168					  
					  
					  
					  
					  
					  
u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据
u8 AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void); //初始化IIC
#endif
















