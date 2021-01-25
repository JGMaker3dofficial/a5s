#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP_APBX						RCC_APB2Periph_GPIOC
#define BEEP_PIN                         GPIO_Pin_3           
#define BEEP_GPIO_PORT                   GPIOC

//蜂鸣器端口定义
#define BEEP PCout(3)	// BEEP,蜂鸣器接口	
#define GET_BEEP_ST PCin(3)	// 获取beep的IO管脚	电平

#define BEEP_ON			(BEEP = 1) 
#define BEEP_OFF		(BEEP = 0) 
void BEEP_Init(void);	//初始化

void Enable_Beep(void);
void Disable_Beep(void);

void Beep_Task(void);

#endif

