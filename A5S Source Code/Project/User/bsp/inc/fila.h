/*
*********************************************************************************************************
*
*	ģ������ :
*	�ļ����� : fila.h
*	��    �� : V1.0
*	˵    �� : 
*
*
*********************************************************************************************************
*/


#ifndef __FILA_H
#define __FILA_H 	 

/****
�����ϲ��裺
���ϣ�
1)ȷ������ͷ�¶ȴﵽ���ϵ��趨�¶Ⱥ��ȿ��ٽ���һ��
2)���ٽ���һ��

���ϣ�
1)ȷ������ͷ�¶ȴﵽ�����趨���¶Ⱥ������ٽ���һ��
2)��������һС��
3)�ȴ�һ���Ѿ����õ�ʱ��
4)��������һ���


���ϲ��裺
����:
1)ȷ������ͷ�¶ȴﵽ���ϵ��趨�¶Ⱥ��ȿ��ٽ���һС��
2)���ٽ���һ���

����:
1)ȷ������ͷ�¶ȴﵽ���ϵ��趨�¶Ⱥ������ٽ���һС��
2)��������һ���
3)��������һС��

*/
#include "Configuration.h"

//�������¶�����
#define USER_FILAMENT_MINI_TEMP	180
#define ENTER_EXIT_FILAMENT_TEMP		200.0

// A3S ���������A3�����ϳ��Ȳ��
//A3S ��ʱ������ϳ��ȱ� A3s ������Ҫ��





//��ӡ�����н������ٶ�����,//unit:mm/minute
#define	PRINT_FILA_IN_FAST_SPEED	 (3 * 60)//unit:mm/minute
#define	PRINT_FILA_IN_SLOW_SPEED	 (5 * 60)
#define	PRINT_FILA_OUT_SLOW1_SPEED	 (7 * 60)//����1��ǰ�Ƚ���
#define	PRINT_FILA_OUT_SLOW2_SPEED	 (50 * 60)//����2����������
#define	PRINT_FILA_OUT_FAST_SPEED	 (100 * 60)//����3����������

//��ӡ�����н����ϳ�������//unit:mm
#define FAST_ENTER_FILAMENT_LENGTH		5//���ٽ���һ��,uint:mm
#define SLOW_ENTER_FILAMENT_LENGTH		10//���ٽ���һ��,uint:mm

#define EXTRUSION_LENGTH_BEFORE_EXIT	4//����ǰ�Ƚ���һ��,uint:mm
#define EXIT_FILAMENT_LENGTH_1			10//��������һ��,uint:mm
#define EXIT_FILAMENT_LENGTH_2			30//��������һ���,uint:mm

//��ӡ���̽����Ͽ�ʼʱZ������̧��һ�θ߶�//unit:mm
#define	PRINT_FILA_IN_Z_UP_LENGTH		20 //unit:mm
#define	PRINT_FILA_OUT_Z_UP_LENGTH		20 //unit:mm

#define	PRINT_FILA_OUT_TIME		(15*1000)	//unit:ms





//�����ٶ�����//unit:mm/minute
//#define	CHANGE_FILA_IN_FAST_SPEED	 (20 * 60)
//#define	CHANGE_FILA_IN_SLOW_SPEED	 (8 * 60)
//#define	CHANGE_FILA_OUT_SLOW1_SPEED	 (5 * 60)
//#define	CHANGE_FILA_OUT_SLOW2_SPEED	 (10 * 60)
//#define	CHANGE_FILA_OUT_FAST_SPEED	 (20 * 60)
#define	CHANGE_FILA_IN_FAST_SPEED	 (20 * 60)
#define	CHANGE_FILA_IN_SLOW_SPEED	 (4 * 60)
#define	CHANGE_FILA_OUT_SLOW1_SPEED	 (5 * 60)
#define	CHANGE_FILA_OUT_SLOW2_SPEED	 (10 * 60)
#define	CHANGE_FILA_OUT_FAST_SPEED	 (20 * 60)
/********
A1: ���Ϲܳ���660mm,���ϳ���750mm
A3S: ���Ϲܳ���520mm,���ϳ���610mm
************/
#if ( A1== MERCHIN)
//A1����
//���ϳ�������//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//�ܽ��ϳ���
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//�����ϳ���
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif	( A3== MERCHIN)

	#if A3_LIN_SHI==1
	//A3��ʱ��
	//���ϳ�������//unit:mm
	#define	CHANGE_FILA_IN_TOTAL_LENGTH	720			//�ܽ��ϳ���
	#define CHANGE_FILA_FAST_ENTER_LENGTH		40//unit:mm
	#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm

	#define	CHANGE_FILA_OUT_TOTAL_LENGTH	720		//�����ϳ���
	#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm

	#else

	//A3����������
	//���ϳ�������//unit:mm
	#define	CHANGE_FILA_IN_TOTAL_LENGTH	620			//�ܽ��ϳ���
	#define CHANGE_FILA_FAST_ENTER_LENGTH		40//unit:mm
	#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm

	#define	CHANGE_FILA_OUT_TOTAL_LENGTH	620		//�����ϳ���
	#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm

	#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif	( A5== MERCHIN)
//A5����
//���ϳ�������//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	820//790//750			//�ܽ��ϳ���
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	820//790//750		//�����ϳ���
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif	( A6== MERCHIN)
//A6����
//���ϳ�������//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//�ܽ��ϳ���
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//�����ϳ���
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif	( A4== MERCHIN)
//A4����
//���ϳ�������//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//�ܽ��ϳ���
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//�����ϳ���
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

//���Ϲ�����Z�������ĸ߶�����
#define	CHANGE_FILA_IN_Z_UP_LENGTH		20//unit:mm
#define	CHANGE_FILA_OUT_Z_UP_LENGTH		20//unit:mm








//�ϵ�����ǰ�Ľ����ϳ�������
#define POWER_DOWN_PRINT_FAST_FILA_IN_LEN	5
#define POWER_DOWN_PRINT_SLOW_FILA_IN_LEN	10
#define POWER_DOWN_PRINT_Z_UP_LEN	5//Z��ϵ�����׼���������ƶ���߶�


#define	FILA_RUN_OUT_DET	KEY_7_DOWN//����
#define	FILA_FED_DET		KEY_7_UP//����

#define FILA_IO_ID			KID_JOY_R
#define FILA_STATE_OUT	1//����
#define FILA_STATE_FED	0//��������

#define	FILA_IO		PCin(7)//��ѯ���IO


#define FILA_RUN_OUT_TIME	3600000	//1 hour

#define FILA_IO_STATE	(bsp_GetKeyState(FILA_IO_ID))


typedef enum fila_in_state{
	IN_FILA_NO = 0XFF,
	IN_FILA_START = 0x00,
	IN_FILA_PREHEAT = 0x10,
	IN_FILA_FINISH = 0x01
}FILA_IN_STATE;

typedef enum fila_out_state{
	OUT_FILA_NO = 0XFF,
	OUT_FILA_START = 0x00,
	OUT_FILA_PREHEAT = 0x10,
	OUT_FILA_FINISH = 0x01
}FILA_OUT_STATE;





/**********************����ͷ�ļ�*************************/
#include "app_gui.h"

typedef struct
{
	volatile int32_t filaRunOutContnTime;
	float bck_ex0Tep;
	float bck_bedTep;
	int32_t dispTime1;
	u8	dispState;
	u8 isTimeOut;
	u8 firstSet;
	u8 firstRun;
	u8 dipFirstRun;
	u8 heating;
}_fila_time_s;

extern _fila_time_s	filaTime;
/*****************************************/


typedef struct
{
//	u8 filaExist;//�Ĳ��Ƿ���ڣ�Ĭ�ϴ���
//	u8 filaChangeOK;//�ĲĻ����Ƿ���ɣ�Ĭ�����
	u8 filaPause;//������ͣ��־��Ĭ�ϲ���ͣ
	u8 filaIOState;//���ϼ��IO״̬
//	u8 filaBackRun;//������ʾ���ϼ�����Back�ؼ���ʾ��־
}_fila_state_s;

extern _fila_state_s filaState;



void filament_inUI(void);
void filament_outUI(void);
void filament_in_callBack(void);
void filament_out_callBack(void);
void fila_screen(void);
u8 StopPrint_remiderUI(void);
void dynamic_reminder_infor(FILA_IN_STATE in,FILA_OUT_STATE out);
void st_synchronize_update_temp(void);
void zMove_up_filament(u16 moveMM);
void zMove_down_filament(u16 moveMM);
void fila_run_out_detet_screen(void);
void fila_run_out_disply(u8 state);
u8 check_fila_run_out(void);


void change_filament_in_callBack(void);
void change_filament_out_callBack(void);
void change_filament_inUI(void);
void change_filament_outUI(void);
void change_fila_screen(void);

void heat_until_targetTem(void);
void heat_contenue_print_screen(void);

void heat_end_exacute(void);
void clear_filaTime_s(void);


/*****************************************/
#endif
