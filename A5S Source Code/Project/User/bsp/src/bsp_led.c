/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*

*********************************************************************************************************
*/

#include "bsp_led.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
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

#if 1	/* 弃用IO控制复位，IO默认就是浮空输入 */
	/* 
	这里的IO初始化本可省略，因为IO默认就是浮空输入。
	但因为BootLoader中已设为推挽且输出1，BootLoader已经批量不好修改，所以这里再次初始化为浮空输入。 */
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
