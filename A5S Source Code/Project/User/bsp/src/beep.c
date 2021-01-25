#include "beep.h"
#include "bsp.h"

#define BEEP_ON_TIME	(100)
#define BEEP_TIME_CYCLE	(300)

#define BEEP_BIG_CYCLE_ON_TIME	(60 * 5*100)
#define BEEP_BIG_CYCLE_OFF_TIME	( 60 * 10 *100)

//��ƽ������ʱ��
#define LEVEL_CYCLE_ON_TIME		(100 )
#define LEVEL_CYCLE_OFF_TIME	( 100 )

typedef struct
{
	u8 BeepEn;//ʹ�ܷ�������־����
//	u16 beepCnt;//�δ�ѭ������
	u32 beepTimeCnt;//һ����С���ڵĵδ�ʱ�����,��1s�����ڼ���
	u32 EsacpTime;//ʱ�����
}_Beep_Ctrl_s;
_Beep_Ctrl_s beepCtrlBlock;


static u8 Is_Beep_Enable(void);
static void Beep_Cycle(u32 OnTm,u32 periodTm);
static void Beep_Ctrl(u32 cycleOnTm,u32 cycleOffTm);
//static void vCalm_Beep_For_Fila_Out(void);
static void Level_UI_Beep(void);

//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(BEEP_APBX, ENABLE);	 //ʹ��GPIOE�˿�ʱ��
 
 GPIO_InitStructure.GPIO_Pin = BEEP_PIN;				 //BEEP-->PE.4 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
 GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);	 //���ݲ�����ʼ��GPIOE.4
 
 GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_PIN);//���0���رշ��������


}


/*******
* �������δ���̣�һ���ӵδ�500ms�������δ�5���ӣ��ټ��10���Ӻ��ظ�֮ǰ�Ĳ���
******/
static void vCalm_Beep_For_Fila_Out(void)
{
	u8 isEnable;
	isEnable = Is_Beep_Enable();
	if(TRUE == isEnable)
	{
		//����ʵ��
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