/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

	  
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
	 
#define DEBUG_EN

#ifdef DEBUG_EN
	 


#define	LEVEL_VERBOSE     1
#define	LEVEL_INFO        2
#define	LEVEL_DEBUG       3
#define	LEVEL_WARNING     4
#define	LEVEL_ERROR       5
#define	LEVEL_NONE        6


#define __LEVEL__   LEVEL_DEBUG


    #if defined(DEBUG_EN) || defined(DEBUG_ACC) || defined(DEBUG_PHILL)
        #define LOG(level, format, ...) __log(level, __func__, __LINE__, format, ##__VA_ARGS__);
    #else
        #define LOG(level, format, ...) 	((void)0)
    #endif
		

//打印调用LOG_DEBUG宏 的函数名 和 所在的行号等必要信息
#define LOG_DEBUG(level, format, ...)                               \
        if( level >= __LEVEL__ ) {                                  \
        char    str[64];                                            \
        snprintf(str, sizeof(str), "[%d|%s|%d]"format,              \
        level,__func__,__LINE__,##__VA_ARGS__);                     \
        simple_uart_putstring_debug((const uint8_t *)str);          \
        }
				
//#define DBG(format,...) 	log_printf(format,##__VA_ARGS__)
#define DBG(format,...) 	printf(format,##__VA_ARGS__)
				
				
				
				
  extern void uartTxNBytes(const uint8_t* buffer, uint16_t length);
  extern void simple_uart_put_debug(const uint8_t c);
	extern void simple_uart_putstring_debug(const uint8_t *str);	 
  void __log(uint32_t level, const char * func, uint32_t line, const char * format, ...);

extern void log_printf( const char * format, ...);
extern void log_printf_UART1( const char * format, ...);

extern void PrintHex(uint8_t* buff,uint8_t cnt);     
	 
#else				
				
#define LOG(level, format, ...) 								((void)0)
#define LOG_DEBUG(level, format, ...)  					((void)0)
#define DBG(format,...)								((void)0)	



#endif


				
#ifdef __cplusplus
}
#endif 


#endif
