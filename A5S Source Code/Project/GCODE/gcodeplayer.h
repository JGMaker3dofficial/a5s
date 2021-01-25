/*********************���ظ�������***********************/
#ifndef __GCODEPLAYER_H
#define __GCODEPLAYER_H 	
/********************************************/


/*********************ͷ�ļ�����***********************/		   
#include <stm32f10x.h>
#include "ff.h"	   
#include "sys.h"
#include "extruder.h"
/********************************************/
#define MOVE_E1_SPEED_SLOW		1000
#define MOVE_E1_SPEED_NORMAL	2000
#define MOVE_E1_SPEED_FAST		3000

/*********************�ƶ�����궨��***************************/
#define MOVE_X_INC_10MM_CMD			"G1 X10 F4800\r\n"
#define MOVE_X_SUB_10MM_CMD			"G1 X-10 F4800\r\n"

#define MOVE_Y_INC_10MM_CMD			"G1 Y10 F4800\r\n"
#define MOVE_Y_SUB_10MM_CMD			"G1 Y-10 F4800\r\n"

#define MOVE_Z_INC_10MM_CMD			"G1 Z10 F400\r\n"
#define MOVE_Z_SUB_10MM_CMD			"G1 Z-10 F400\r\n"

#define MOVE_Z_INC_20MM_CMD			"G1 Z20 F400\r\n"//���ڴ�ӡ�����еĽ�����z���ƶ�
#define MOVE_Z_SUB_20MM_CMD			"G1 Z-20 F400\r\n"


#define MOVE_E_INC_10MM_CMD			"G1 E10 F1800\r\n"
#define MOVE_E_SUB_10MM_CMD			"G1 E-10 F1800\r\n"

#define MOVE_X_INC_1MM_CMD			"G1 X2 F3600\r\n"//"G1 X1 F4800\r\n"
#define MOVE_X_SUB_1MM_CMD			"G1 X-2 F3600\r\n"//"G1 X-1 F4800\r\n"

#define MOVE_Y_INC_1MM_CMD			"G1 Y2 F3600\r\n"//"G1 Y1 F4800\r\n"
#define MOVE_Y_SUB_1MM_CMD			"G1 Y-2 F3600\r\n"//"G1 Y-1 F4800\r\n"

#define MOVE_Z_INC_1MM_CMD			"G1 Z1 F400\r\n"
#define MOVE_Z_SUB_1MM_CMD			"G1 Z-1 F400\r\n"

#define MOVE_E_INC_1MM_CMD			"G1 E1 F1800\r\n"
#define MOVE_E_SUB_1MM_CMD			"G1 E-1 F1800\r\n"


#define MOVE_E_CMD			"G1 E\r\n"


#define MOVE_X_INC_0_1MM_CMD		"G1 X0.1 F4800\r\n"
#define MOVE_X_SUB_0_1MM_CMD		"G1 X-0.1 F4800\r\n"

#define MOVE_Y_INC_0_1MM_CMD		"G1 Y0.1 F4800\r\n"
#define MOVE_Y_SUB_0_1MM_CMD		"G1 Y-0.1 F4800\r\n"

#define MOVE_Z_INC_0_1MM_CMD		"G1 Z0.2 F400\r\n"//"G1 Z0.1 F400\r\n"
#define MOVE_Z_SUB_0_1MM_CMD		"G1 Z-0.2 F400\r\n"//"G1 Z-0.1 F400\r\n"

#define MOVE_E_INC_0_1MM_CMD		"G1 E0.1 F400\r\n"//"G1 E0.1 F1800\r\n"
#define MOVE_E_SUB_0_1MM_CMD		"G1 E-0.1 F400\r\n"//"G1 E-0.1 F1800\r\n"



#define FILAMENT_SLOW_SPEED_CMD			"F100\r\n"
#define FILAMENT_NORM_SPEED_CMD			"F500\r\n"
#define FILAMENT_FAST_SPEED_CMD			"F1000\r\n"



#define MOVE_GO_HOME				"G28\r\n"
#define MOVE_GO_HOME_X				"G28 X\r\n"
#define MOVE_GO_HOME_Y				"G28 Y\r\n"
#define MOVE_GO_HOME_Z				"G28 Z\r\n"

#define USING_RELATIVE_POSITION		"G91\r\n"
#define USING_ABSOLUTE_POSITION		"G90\r\n"
#define RESTART_MOVE_ACTION			"M999\r\n"


/********************************************/


/**********************�ṹ����**********************/
typedef enum {
   LS_SerialPrint = 0,
   LS_Count,
   LS_GetFilename
 }LsAction;

typedef enum 
{
	NONE_MM = 0XFF,
	MM01 = 0,
	MM1 = 1,
	MM10 = 2
}MOVE_SCALE;

typedef enum 
{
	NONE_DIR = 0XFF,
	POSITIVE_DIR = 0,
	NEGTIVE_DIR = 1,
}DIR_E;

typedef enum 
{
	PS_NO = 0,
	PS_PIRNT = 1,//���ڴ�ӡ
	PS_STOP,//��ӡֹͣ
	PS_PAUSE,//��ͣ��ӡ
	PS_POWER_OFF_PRIT,//�ϵ������ӡ
	PS_PRINT_OVER_NOR,//������ӡ���
}_PRINT_STATE;



typedef struct {
 u8 saving;
 volatile u8 sdprinting;	
 u8 cardOK ;
  //int16_t n;
 unsigned long autostart_atmillis;
 unsigned long  sdpos; 
 u8 autostart_stilltocheck;
 LsAction lsAction; //stored for recursion.
 u8 filename[32];
 FIL fgcode;
 _PRINT_STATE FILE_STATE;
}CardReader;


typedef struct
{

	u8 unplugFlag;//�ε�����־
	u32 saveLineNo;//SD�����ε�ʱ����¼gcode���ȡ�����к�
	u32 saveFpos;//�ļ���ȡλ��
	u32 firstFilePos;//��¼SD������ʱ��ǰ����gcode���е�����λ��
	_sdCardUnplugUI	SD_CARD_UNPLUG_UI;
}_sdCardUnplugCheck_s;

/********************************************/
extern _sdCardUnplugCheck_s sdCardUnplug;

/********************��������************************/
extern CardReader card;

extern volatile uint32_t  line_no;
extern uint32_t file_pos;
extern uint32_t fil_size;
extern _PRINT_STATE print_state;
extern uint32_t user_line_no;
extern uint8_t isPrintFileExist;
/********************************************/


/********************��������************************/
void card_ls(void);
void card_initsd(void);
void card_release(void);						//�ͷ�SD��
void card_openFile(char* fname,u8 read);
void card_startFileprint(void);
void card_pauseSDPrint(void);
void card_setIndex(long index);	  //�����ļ�λ��
void card_getStatus(void);  //��ȡSD��ӡ״̬
void card_closefile(void);
void card_removeFile(char* fname);

void card_printingHasFinished(void);
void card_checkautostart(void);
void card_write_command(char *buf);

void force_sdCard_pause(void);
void force_sdCard_start(void);

u8 card_eof(void);
int16_t card_get(void);

int16_t file_read(void);
uint8_t get_precent_file_printed(void);
uint32_t get_size_file_printed(void);
uint32_t get_line_no_file_printed(void);
uint32_t get_size_of_file(void);
void menu_action_gcode(char* pgcode);
void user_cmd_move_allHome(void);
void user_cmd_move_xHome(void);
void user_cmd_move_yHome(void);
void user_cmd_move_zHome(void);
void user_cmd_move_x(DIR_E dir, MOVE_SCALE scale);
void user_cmd_move_y(DIR_E dir, MOVE_SCALE scale);
void user_cmd_move_z(DIR_E dir, MOVE_SCALE scale);
void user_cmd_move_e(DIR_E dir, MOVE_SCALE scale);
void user_cmd_move_extru(int32_t length, _filament_speed_step_e speed);
u8 read_card_print_flag(void);

void deal_with_unplug_sdCard(void);
void deal_with_sdCard_plugAgain(void);
void reach_power_down_file_pos(u8 cmdLineNo,u32 pdFilePosHead,u32 *desFilePos);
/********************************************/


#endif



























