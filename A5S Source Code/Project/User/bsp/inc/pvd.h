/*
*********************************************************************************************************
*
*	ģ������ : power voltage down detect modul
*	�ļ����� : pvd.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __PVD_H
#define __PVD_H

#include "sys.h"

#define POWER_DOWN_DETECT_IO				GPIO_Pin_0
#define POWER_DOWN_DETECT_PORT				GPIOA
#define POWER_DOWN_DETECT_PORT_CLK			RCC_APB2Periph_GPIOA
#define POWER_DOWN_DETECT_EXTI_SOURCE 		GPIO_PortSourceGPIOA
#define POWER_DOWN_DETECT_EXTI_IO 			GPIO_PinSource0
#define POWER_DOWN_DETECT_EXTI_LINE 	 		EXTI_Line0
#define POWER_DOWN_DETECT_INT				EXTI0_IRQHandler


extern volatile u8 paueStartFlag;

void power_vol_down_detect_init(void);
void PVD_callBack(void);

void power_down_callBack(void);
void power_down_callBack1(void);

/* ���ⲿ���õĺ������� */




#endif

/**************************************************************/
