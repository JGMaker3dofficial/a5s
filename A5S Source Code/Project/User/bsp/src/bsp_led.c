/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*

*********************************************************************************************************
*/

#include "bsp_led.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED1_APBX | LEDx1_APBX |RESET_APBX, ENABLE);	 

	//PCBA LED for indectating
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);				
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);				

#if 1	/* ����IO���Ƹ�λ��IOĬ�Ͼ��Ǹ������� */
	/* 
	�����IO��ʼ������ʡ�ԣ���ΪIOĬ�Ͼ��Ǹ������롣
	����ΪBootLoader������Ϊ���������1��BootLoader�Ѿ����������޸ģ����������ٴγ�ʼ��Ϊ�������롣 */
 //forced reset pin 
	GPIO_SetBits(RESET_GPIO_PORT,RESET_PIN);		
	GPIO_InitStructure.GPIO_Pin = RESET_PIN;				 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(RESET_GPIO_PORT, &GPIO_InitStructure);	
#endif
	
//external led for lighting
	GPIO_InitStructure.GPIO_Pin = LEDx1_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(LEDx1_GPIO_PORT, &GPIO_InitStructure);				
	GPIO_ResetBits(LEDx1_GPIO_PORT,LEDx1_PIN);	


}

/**************************************************************/
