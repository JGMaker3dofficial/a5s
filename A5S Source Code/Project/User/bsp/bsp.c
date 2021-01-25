/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F103)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件可以在开
*			  头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者     说明

*
*********************************************************************************************************
*/

#include "bsp.h"

char FW_VERSION[25];


/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改 system_stm32f103.c 文件
	*/

	/* 优先级分组设置为4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_InitLed();		/* 配置LED的GPIO端口 */
	bsp_InitKey();		/* 初始化按键 */
	
	bsp_InitTimer();	/* 初始化系统滴答定时器 (此函数会开中断) */
	
	bsp_InitUart1(115200);		/* 初始化串口驱动 */
	printf("App start ...\r\n");
	
	AT24CXX_Init();

	LCD_Init();			   		//初始化LCD   

	W25QXX_Init();				//初始化W25Q128,挂载fats时会初始化
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
	
	tp_dev.init(); 				//触摸屏初始化
	gui_init();	  				//gui初始化
	piclib_init();				//piclib初始化	

	BEEP_Init();
	init_backup_domain();
	
	power_vol_down_detect_init();
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer10ms
*	功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
*			任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
	bsp_KeyScan();		/* 每10ms扫描按键一次 */
	Beep_Task();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer1ms
*	功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
*			 可以放在此函数。比如：触摸坐标扫描。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void GT811_Timer1ms(void);
void bsp_RunPer1ms(void)
{
	LED0 =! LED0;
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Idle
*	功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
*			 本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void SaveScreenToBmp(uint16_t _index);
void bsp_Idle(void)
{
	/* --- 喂狗 */

	/* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */

	/* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
	//GUI_Exec();

	/* 例如 uIP 协议，可以插入uip轮询函数 */


}

void print_fw_infor(void)
{
	char fwNa[50];//注意宏A1_FW_NAME 、A3_FW_NAME不宜定义得太大
#if	MERCHIN == A1
	strncpy(fwNa,A1_FW_NAME,strlen(A1_FW_NAME));
	fwNa[strlen(A1_FW_NAME)] = '\0';
#elif	MERCHIN == A3
	strncpy(fwNa,A3_FW_NAME,strlen(A3_FW_NAME));
	fwNa[strlen(A3_FW_NAME)] = '\0';
#elif	MERCHIN == A4
	strncpy(fwNa,A4_FW_NAME,strlen(A4_FW_NAME));
	fwNa[strlen(A4_FW_NAME)] = '\0';	
	
#elif	MERCHIN == A5
	strncpy(fwNa,A5_FW_NAME,strlen(A5_FW_NAME));
	fwNa[strlen(A5_FW_NAME)] = '\0';	
	
#elif	MERCHIN == A6
	strncpy(fwNa,A6_FW_NAME,strlen(A6_FW_NAME));
	fwNa[strlen(A6_FW_NAME)] = '\0';	
#endif


	sprintf(FW_VERSION, "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c", 
BUILD_YEAR_CH0,
BUILD_YEAR_CH1,
BUILD_YEAR_CH2,
BUILD_YEAR_CH3,

BUILD_MONTH_CH0,
BUILD_MONTH_CH1,

BUILD_DAY_CH0,
BUILD_DAY_CH1,

BUILD_HOUR_CH0,
BUILD_HOUR_CH1,

BUILD_MIN_CH0,
BUILD_MIN_CH1,

BUILD_SEC_CH0,
BUILD_SEC_CH1
);

DBG("%s-%s\r\n",fwNa,FW_VERSION);
}
/******************************************************/
