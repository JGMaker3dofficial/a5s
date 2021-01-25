#include "beep.h"
#include "bsp.h"

#define BEEP_ON_TIME	(100)
#define BEEP_TIME_CYCLE	(300)

#define BEEP_BIG_CYCLE_ON_TIME	(60 * 5*100)
#define BEEP_BIG_CYCLE_OFF_TIME	( 60 * 10 *100)

//调平检测蜂鸣时间
#define LEVEL_CYCLE_ON_TIME		(100 )
#define LEVEL_CYCLE_OFF_TIME	( 100 )

typedef struct
{
	u8 BeepEn;//使能蜂鸣器标志变量
//	u16 beepCnt;//滴答循环次数
	u32 beepTimeCnt;//一个最小周期的滴答时间计数,以1s的周期计算
	u32 EsacpTime;//时间计数
}_Beep_Ctrl_s;
_Beep_Ctrl_s beepCtrlBlock;


static u8 Is_Beep_Enable(void);
static void Beep_Cycle(u32 OnTm,u32 periodTm);
static void Beep_Ctrl(u32 cycleOnTm,u32 cycleOffTm);
//static void vCalm_Beep_For_Fila_Out(void);
static void Level_UI_Beep(void);

//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(BEEP_APBX, ENABLE);	 //使能GPIOE端口时钟
 
 GPIO_InitStructure.GPIO_Pin = BEEP_PIN;				 //BEEP-->PE.4 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);	 //根据参数初始化GPIOE.4
 
 GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_PIN);//输出0，关闭蜂鸣器输出


}


/*******
* 蜂鸣器滴答过程：一秒钟滴答500ms，连续滴答5分钟，再间隔10分钟后，重复之前的步骤
******/
static void vCalm_Beep_For_Fila_Out(void)
{
	u8 isEnable;
	isEnable = Is_Beep_Enable();
	if(TRUE == isEnable)
	{
		//蜂鸣实现
		Beep_Ctrl(BEEP_BIG_CYCLE_ON_TIME,BEEP_BIG_CYCLE_OFF_TIME);
		
	}
}

static u8 Is_Beep_Enable(void)
{
	return beepCtrlBlock.BeepEn;
}

void Enable_Beep(void)
{
	if( FALSE == beepCtrlBlock.BeepEn)
	{
		DISABLE_INT();
		beepCtrlBlock.BeepEn = TRUE;
		ENABLE_INT();
	}
}

void Disable_Beep(void)
{
	if( TRUE == beepCtrlBlock.BeepEn)
	{
		DISABLE_INT();
		beepCtrlBlock.BeepEn = FALSE;	
		ENABLE_INT();
		BEEP_OFF;  
		beepCtrlBlock.beepTimeCnt = 0;
		beepCtrlBlock.EsacpTime = 0;
	}
}


static void Beep_Cycle(u32 OnTm,u32 periodTm)
{

	
	beepCtrlBlock.beepTimeCnt++;
	if( beepCtrlBlock.beepTimeCnt < OnTm)	BEEP_ON;  
	
	else if( beepCtrlBlock.beepTimeCnt >= OnTm && beepCtrlBlock.beepTimeCnt < periodTm)
	{
		BEEP_OFF;  
	}
	else if(beepCtrlBlock.beepTimeCnt >= periodTm) 
	{
		BEEP_OFF;  
		beepCtrlBlock.beepTimeCnt = 0;

	}	
}

static void Beep_Ctrl(u32 cycleOnTm,u32 cycleOffTm)
{
	
	beepCtrlBlock.EsacpTime++;
	
	if(beepCtrlBlock.EsacpTime <  cycleOnTm)	Beep_Cycle(BEEP_ON_TIME,BEEP_TIME_CYCLE);
	else if(beepCtrlBlock.EsacpTime >=  cycleOnTm && beepCtrlBlock.EsacpTime <  cycleOffTm)	BEEP_OFF;  
	else if(beepCtrlBlock.EsacpTime >=  cycleOffTm)	{beepCtrlBlock.EsacpTime = 0;BEEP_OFF;  }
	
}



void Beep_Task(void)
{
	vCalm_Beep_For_Fila_Out();

}