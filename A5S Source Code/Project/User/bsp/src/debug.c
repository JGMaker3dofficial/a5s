
#include "debug.h"  
#include "bsp_usart.h"

#ifdef DEBUG_EN
 
/*
*********************************************************************************************************
*	�� �� ��: __log
*	����˵��: ��ӡ�йص��ռ���Ϣ
*	��    ��: level�����Լ���
* func:�ַ���
* format����ӡ��ʽ������ "%s"��"%d"�ȣ����Ʊ�׼C���Դ�ӡ��ʽ������
*	length�����ݳ���
*���磺__log(LEVEL_DEBUG,"test",20,"%s");��ӡ��������ϢΪ [3]test(20):
*
*	�� �� ֵ: ��
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
