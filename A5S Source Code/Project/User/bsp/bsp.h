#ifndef _BSP_H
#define _BSP_H

#include "Configuration.h"

#define STM32_V4
//#define STM32_X2

/* 检查是否定义了开发板型号 */
#if !defined (STM32_V4) && !defined (STM32_X2)
	#error "Please define the board model : STM32_X2 or STM32_V4"
#endif

/* 定义 BSP 版本号 */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU空闲时执行的函数 */
#ifdef CPU_IDLE
	#define CPU_IDLE()	   bsp_IdleTask()   
#else 
	#define CPU_IDLE()		(void(0))
#endif

/********************版本号宏********************************/
// Example of __DATE__ string: "Jul 27 2012"
#define	A1_FW_NAME	"A1"
#define	A3_FW_NAME	"A3S"
#define	A4_FW_NAME	"A4"
#define	A5_FW_NAME	"A5S"
#define	A6_FW_NAME	"A6"

#define BUILD_YEAR_CH0 (__DATE__[ 7])
#define BUILD_YEAR_CH1 (__DATE__[ 8])
#define BUILD_YEAR_CH2 (__DATE__[ 9])
#define BUILD_YEAR_CH3 (__DATE__[10])


#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')


#define BUILD_MONTH_CH0 \
((BUILD_MONTH_IS_OCT || BUILD_MONTH_IS_NOV || BUILD_MONTH_IS_DEC) ? '1' : '0')

#define BUILD_MONTH_CH1 \
( \
(BUILD_MONTH_IS_JAN) ? '1' : \
(BUILD_MONTH_IS_FEB) ? '2' : \
(BUILD_MONTH_IS_MAR) ? '3' : \
(BUILD_MONTH_IS_APR) ? '4' : \
(BUILD_MONTH_IS_MAY) ? '5' : \
(BUILD_MONTH_IS_JUN) ? '6' : \
(BUILD_MONTH_IS_JUL) ? '7' : \
(BUILD_MONTH_IS_AUG) ? '8' : \
(BUILD_MONTH_IS_SEP) ? '9' : \
(BUILD_MONTH_IS_OCT) ? '0' : \
(BUILD_MONTH_IS_NOV) ? '1' : \
(BUILD_MONTH_IS_DEC) ? '2' : \
/* error default */ '?' \
)

#define BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (__DATE__[ 5])

 

// Example of __TIME__ string: "21:06:19"


#define BUILD_HOUR_CH0 (__TIME__[0])
#define BUILD_HOUR_CH1 (__TIME__[1])

#define BUILD_MIN_CH0 (__TIME__[3])
#define BUILD_MIN_CH1 (__TIME__[4])

#define BUILD_SEC_CH0 (__TIME__[6])
#define BUILD_SEC_CH1 (__TIME__[7])
/**************************************************/

#define FLASH_APP1_ADDR		0x0800A000  	//第一个应用程序起始地址(存放在FLASH)
/* 这个宏仅用于调试阶段排错 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif
#include "gui.h"
#include "guix.h"
#include "touch.h"	
#include "bsp_led.h"
#include "bsp_key.h"
#include "debug.h"
#include "24cxx.h"
#include "myiic.h"
#include "spi.h"
#include "w25qxx.h"
#include "lcd.h"
#include "bmp.h"
#include "app_gui.h"
#include "backup_sram.h"
#include "pvd.h"
#include "dma.h"
#include "sdio_sdcard.h"


#include "Dlion.h"


#include "string.h"

#include "ff.h"
#include "exfuns.h"  
#include "fontupd.h"
#include "text.h"		
#include "piclib.h"
#include "common.h"
#include "beep.h"
extern char FW_VERSION[25];

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);
void print_fw_infor(void);


#endif

/**********************************************************/
