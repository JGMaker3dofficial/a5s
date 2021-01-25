
#include "debug.h"  
#include "bsp_usart.h"

#ifdef DEBUG_EN
 
/*
*********************************************************************************************************
*	函 数 名: __log
*	功能说明: 打印有关的日记信息
*	形    参: level：调试级别
* func:字符串
* format：打印格式，比如 "%s"、"%d"等，类似标准C语言打印格式的类型
*	length：数据长度
*例如：__log(LEVEL_DEBUG,"test",20,"%s");打印出来的信息为 [3]test(20):
*
*	返 回 值: 无
*********************************************************************************************************
*/ 
void __log(uint32_t level, const char * func, uint32_t line, const char * format, ...)
{
    if( level >= __LEVEL__ ) 
    {
        char    str[64];
        va_list ap;
        
        snprintf(str, sizeof(str), "[%d]%s(%d):",level,func,line);
        simple_uart_putstring(USART1, (const uint8_t *)str);
        
        va_start(ap, format);
        (void)vsnprintf(str, sizeof(str), format, ap);
        va_end(ap);
        
        simple_uart_putstring(USART1, (const uint8_t *)str);
        simple_uart_putstring(USART1, (const uint8_t *)"\r\n");
    }
}

//void log_printf( const char * format, ...)
//{
//    char str[512];
//    va_list ap;
//           
//    va_start(ap, format);
//    (void)vsnprintf(str, sizeof(str), format, ap);
//    va_end(ap);
//    
//    simple_uart_putstring_debug((const uint8_t *)str);
//}
void log_printf_UART1( const char * format, ...)
{
    char str[512];
    va_list ap;
           
    va_start(ap, format);
    (void)vsnprintf(str, sizeof(str), format, ap);
    va_end(ap);
    
//simple_uart_putstring(USART2, "TX: [");//DBG
//simple_uart_putstring(USART2, (const uint8_t *)str);//DBG
//simple_uart_putstring(USART2, "]\r\n");//DBG
    simple_uart_putstring(USART1, (const uint8_t *)str);
}

void PrintHex(uint8_t* buff,uint8_t cnt)
{
    uint8_t i;

    printf("PrintInfoDump:\r\n");   
    for(i = 0; i < cnt; i++)
    {
        printf("0x%x ",buff[i]);
    }
    printf("\r\n");
}

#endif
