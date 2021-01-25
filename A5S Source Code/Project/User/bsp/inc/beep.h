#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP_APBX						RCC_APB2Periph_GPIOC
#define BEEP_PIN                         GPIO_Pin_3           
#define BEEP_GPIO_PORT                   GPIOC

//�������˿ڶ���
#define BEEP PCout(3)	// BEEP,�������ӿ�	
#define GET_BEEP_ST PCin(3)	// ��ȡbeep��IO�ܽ�	��ƽ

#define BEEP_ON			(BEEP = 1) 
#define BEEP_OFF		(BEEP = 0) 
void BEEP_Init(void);	//��ʼ��

void Enable_Beep(void);
void Disable_Beep(void);

void Beep_Task(void);

#endif

