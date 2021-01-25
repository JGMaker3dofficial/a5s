/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "sys.h"


#define LED1_APBX						RCC_APB2Periph_GPIOC
#define LED1_PIN                         GPIO_Pin_13           
#define LED1_GPIO_PORT                   GPIOC

#define RESET_APBX							RCC_APB2Periph_GPIOC
#define RESET_PIN                         GPIO_Pin_5           
#define RESET_GPIO_PORT                   GPIOC


#define LEDx1_APBX						RCC_APB2Periph_GPIOB
#define LEDx1_PIN                         GPIO_Pin_0           
#define LEDx1_GPIO_PORT                   GPIOB

#define LED0 PCout(13)// 

#define RESET_USER PCout(5)//
#define LEDx1 PBout(0)// 
/* 供外部调用的函数声明 */
void bsp_InitLed(void);



#endif

/**************************************************************/
