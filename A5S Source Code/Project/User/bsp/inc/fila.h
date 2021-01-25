/*
*********************************************************************************************************
*
*	模块名称 :
*	文件名称 : fila.h
*	版    本 : V1.0
*	说    明 : 
*
*
*********************************************************************************************************
*/


#ifndef __FILA_H
#define __FILA_H 	 

/****
进退料步骤：
进料：
1)确保挤出头温度达到进料的设定温度后，先快速进料一段
2)慢速进料一段

退料：
1)确保挤出头温度达到退料设定的温度后，先慢速进料一段
2)中速退料一小段
3)等待一段已经设置的时间
4)快速退料一大段


换料步骤：
进料:
1)确保挤出头温度达到进料的设定温度后，先快速进料一小段
2)慢速进料一大端

退料:
1)确保挤出头温度达到退料的设定温度后，先慢速进料一小段
2)中速退料一大段
3)快速退料一小段

*/
#include "Configuration.h"

//进退料温度设置
#define USER_FILAMENT_MINI_TEMP	180
#define ENTER_EXIT_FILAMENT_TEMP		200.0

// A3S 改良版和老A3进退料长度差不多
//A3S 临时版进退料长度比 A3s 改良版要长





//打印过程中进退料速度设置,//unit:mm/minute
#define	PRINT_FILA_IN_FAST_SPEED	 (3 * 60)//unit:mm/minute
#define	PRINT_FILA_IN_SLOW_SPEED	 (5 * 60)
#define	PRINT_FILA_OUT_SLOW1_SPEED	 (7 * 60)//退料1：前先进料
#define	PRINT_FILA_OUT_SLOW2_SPEED	 (50 * 60)//退料2：慢速退料
#define	PRINT_FILA_OUT_FAST_SPEED	 (100 * 60)//退料3：快速退料

//打印过程中进退料长度设置//unit:mm
#define FAST_ENTER_FILAMENT_LENGTH		5//快速进料一段,uint:mm
#define SLOW_ENTER_FILAMENT_LENGTH		10//慢速进料一段,uint:mm

#define EXTRUSION_LENGTH_BEFORE_EXIT	4//退料前先进料一段,uint:mm
#define EXIT_FILAMENT_LENGTH_1			10//慢速退料一段,uint:mm
#define EXIT_FILAMENT_LENGTH_2			30//快速退料一大段,uint:mm

//打印过程进退料开始时Z轴先上抬起一段高度//unit:mm
#define	PRINT_FILA_IN_Z_UP_LENGTH		20 //unit:mm
#define	PRINT_FILA_OUT_Z_UP_LENGTH		20 //unit:mm

#define	PRINT_FILA_OUT_TIME		(15*1000)	//unit:ms





//换料速度设置//unit:mm/minute
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
A1: 导料管长度660mm,进料长度750mm
A3S: 导料管长度520mm,进料长度610mm
************/
#if ( A1== MERCHIN)
//A1机器
//换料长度设置//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//总进料长度
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//总退料长度
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif	( A3== MERCHIN)

	#if A3_LIN_SHI==1
	//A3临时版
	//换料长度设置//unit:mm
	#define	CHANGE_FILA_IN_TOTAL_LENGTH	720			//总进料长度
	#define CHANGE_FILA_FAST_ENTER_LENGTH		40//unit:mm
	#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm

	#define	CHANGE_FILA_OUT_TOTAL_LENGTH	720		//总退料长度
	#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm

	#else

	//A3机器改良版
	//换料长度设置//unit:mm
	#define	CHANGE_FILA_IN_TOTAL_LENGTH	620			//总进料长度
	#define CHANGE_FILA_FAST_ENTER_LENGTH		40//unit:mm
	#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm

	#define	CHANGE_FILA_OUT_TOTAL_LENGTH	620		//总退料长度
	#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
	#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm

	#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif	( A5== MERCHIN)
//A5机器
//换料长度设置//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	820//790//750			//总进料长度
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	820//790//750		//总退料长度
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif	( A6== MERCHIN)
//A6机器
//换料长度设置//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//总进料长度
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//总退料长度
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif	( A4== MERCHIN)
//A4机器
//换料长度设置//unit:mm
#define	CHANGE_FILA_IN_TOTAL_LENGTH	750			//总进料长度
#define CHANGE_FILA_FAST_ENTER_LENGTH		600//unit:mm
#define CHANGE_FILA_SLOW_ENTER_LENGTH		(CHANGE_FILA_IN_TOTAL_LENGTH - CHANGE_FILA_FAST_ENTER_LENGTH)//unit:mm


#define	CHANGE_FILA_OUT_TOTAL_LENGTH	750		//总退料长度
#define CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT	20//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_1			(CHANGE_FILA_OUT_TOTAL_LENGTH - CHANGE_FILA_EXIT_LENGTH_2 + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT)//unit:mm
#define CHANGE_FILA_EXIT_LENGTH_2			50//unit:mm
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

//换料过程中Z轴上升的高度设置
#define	CHANGE_FILA_IN_Z_UP_LENGTH		20//unit:mm
#define	CHANGE_FILA_OUT_Z_UP_LENGTH		20//unit:mm








//断电续打前的进退料长度设置
#define POWER_DOWN_PRINT_FAST_FILA_IN_LEN	5
#define POWER_DOWN_PRINT_SLOW_FILA_IN_LEN	10
#define POWER_DOWN_PRINT_Z_UP_LEN	5//Z轴断电续打准备工作上移额外高度


#define	FILA_RUN_OUT_DET	KEY_7_DOWN//断料
#define	FILA_FED_DET		KEY_7_UP//上料

#define FILA_IO_ID			KID_JOY_R
#define FILA_STATE_OUT	1//断料
#define FILA_STATE_FED	0//正常吐料

#define	FILA_IO		PCin(7)//查询检测IO


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





/**********************包含头文件*************************/
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
//	u8 filaExist;//耗材是否存在，默认存在
//	u8 filaChangeOK;//耗材换料是否完成，默认完成
	u8 filaPause;//换料暂停标志，默认不暂停
	u8 filaIOState;//断料检测IO状态
//	u8 filaBackRun;//控制显示断料检测界面Back控件显示标志
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
