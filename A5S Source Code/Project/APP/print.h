
/*
*********************************************************************************************************
*
*	ģ������ : ��ӡ����
*	�ļ����� : print.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/
#ifndef __PRINT_H
#define __PRINT_H 	 

/**********************����ͷ�ļ�*************************/
#include "app_gui.h"
#include "common.h"
/*****************************************/


/**********************��غ궨��*************************/

#define PRINT_BMP_NUMS	5	//��ӡ����ͼ�����  

#define PRINT_SET_BMP_NUMS	6	//��ӡ����ͼ�����  


/*****************************************/

/**********************��ӡ����������ͼ��ID***********************/
//���ݴ�����ťʱ�����IDֵ���޸������ֵ
#define PRINT_SET_FILA_IN_BMP_ID	0
#define PRINT_SET_FILA_OUT_BMP_ID	1
#define PRINT_SET_BED_BMP_ID		2
#define PRINT_SET_EXTRUDER_BMP_ID	3
#define PRINT_SET_FAN_BMP_ID		4
#define PRINT_SET_BACK_BMP_ID		5
#define PRINT_SET_SUB_BUTTON_ID		6
#define PRINT_SET_ADD_BUTTON_ID		7


#define ADJ_BED_VAL_ID			0X00
#define ADJ_EXTRUDER_VAL_ID		0X01
#define ADJ_FAN_VAL_ID			0X02

//�Ӽ���־
#define ADD_DIR		0X11
#define SUB_DIR		 0X55

//�ȴ�����ֵ
#define BED_MIN_TEMP	10
#define BED_MAX_TEMP	50

//����ͷ����ֵ
#define EXRUDER_MIN_TEMP	10
#define EXRUDER_MAX_TEMP	275


//���ȼ���ֵ
#define FAN_MIN_DUTY	0
#define FAN_MAX_DUTY	100



#define GCODE_OFFSET   		9 //"0:\GCODE\"

 
//��������һ�����棬Ԥ�Ƚ����еĽ����Ϻʹ�ӡ���ý����еĽ����Ϲ�����ͬ����
#define IS_PRINT_SET_SCREEN 	1
#define IS_PRHEAT_SCREEN   		2
/**********************************************/


/**********************��������ʾ��Ϣ************************/
#define FILAM_IN_ERROR_MESG	"����ͣ��ӡ,�ٽ��н��ϲ���!"
#define FILAM_OUT_ERROR_MESG	"����ͣ��ӡ,�ٽ������ϲ���!"

#define FILAM_IN_REMIN_MESG	"�Ƿ�ȷ��ִ�н��ϲ��� ?"
#define FILAM_OUT_REMIN_MESG	"�Ƿ�ȷ��ִ�����ϲ��� ?"


#define FILAM_IN_MESG_BEGIN	"���ڽ�����..."
#define FILAM_IN_MESG_DONE	"�������!"

#define FILAM_OUT_MESG_BEGIN	"����������..."
#define FILAM_OUT_MESG_DONE		"�������!"










/**********************��������************************/

extern float step_value[3];

extern float set_value[3];
extern uint8_t adj_value_flag;
extern _GUI_TURE_OR_FALSE is_set_value;


extern _GUI_TURE_OR_FALSE is_set_value ;
extern uint8_t *printFileNameNOPrefix;
extern uint8_t *printFileName;//��¼��ǰ��ӡ���ļ���������Ŀ¼
/*****************************************/


typedef enum
{
    CHANGE_TO_PAUSE = 1,
    CHANGE_TO_START = 2,
} _button_change;

typedef enum 
{
	RESET_FLAG_ICON,
	IS_PRINT_SET_ICON,
	
}_which_set_icon;

typedef enum 
{
	NORMAL_PLAY,
	HAS_EXACUTE_PAUSE,
	HAS_EXACUTE_PLAY,
	
}_pause_play_e;


typedef enum 
{
	NORMAL_RESET,
	POWER_OFF_PRINT,
}_is_pwoff_print_e;

typedef enum 
{
	NO_FILE_TYPE = 0XFF,
	IS_FOLDER = 0,
	IS_GCODE_FILE = 1,
	
}_dest_file_type_e;


__packed typedef struct
{

	u8 activeButtonNum;//UI�������ļ�ͼ�����,��ÿһҳ���趨
	u8 fixButtonNum;//UI����̶��Ŀ���ͼ�������������һҳ����һҳ�����صȿ���ͼ�꣬��ÿһҳ���趨
	
	u32 pageCnt;//ҳ�������
	u32 TotalPage;//��ǰĿ¼�µ��ļ�/�ļ�����Ҫ����ҳ����ʾ�������굱ǰĿ¼�͵õ���ֵ

}_file_gui_attribute_s;


__packed typedef struct
{
	u16 fliter;						//�ļ�������,������ˣ���ʼ������
									//�μ�"exfuns.h"����
	u16 fliterex;					//�ļ���չ�Ĺ�����//����ѡ����������С��,���֧��16�ֲ�ͬ���ļ�.Ĭ��0XFFFF,������С�����ˢѡ.����ʼ������
	u16 atualReadItems;	//������ѰĳĿ¼���ļ���ʵ�ʸ�ʣ�����ΪatualReadItems ==activeButtonNum
	u16 readItemsOffSet;//ĳ��Ŀ¼�£���Ѱƫ�ơ������»�ҳ�ؼ�ʱ�����绻ҳ�ɹ�������ҳ�Ķ�ȡ���ļ�ƫ�� readItemsOffSet = pageCnt*activeButtonNum��
	_dest_file_type_e destFileType;//ѡ�е�Ŀ���ļ����
	u8 fileDepth;
	u8 selectedButtonID;//ɨ�����ѡ�еĿؼ�����һҳ����һҳ���߷���
	u8 *destFileName;//ѡ�е�Ŀ���ļ�����
	
	u8 *path;//��ǰ�ļ�Ŀ¼
	u8 **FileNameListTab;//��ǰ�ļ����б�
	u8 *FileNameListAttributeTab;//��ǰ�ļ����б�����Ա�,1--gcode�ļ���0--�ļ���
	
	 _file_gui_attribute_s *fileGuiAttr;//��ǰҳ���GUI����
	 
	
}_file_page_ctrl_block_s;

__packed typedef struct  
{
	u16 x;
	u16 y;
	u8 hour;
	u8 minute;
	u8 second;
}_print_time_s;


__packed typedef struct  
{
	u16 x;
	u16 y;
	float bedCurTemp;//�ȴ���ǰ�¶�
	float bedDestTemp;//�ȴ�Ŀ���趨�¶�
	u8 fanSpeed;
	float etr0CurTemp;//����ͷ0��ǰ�¶�
	float etr0DestTemp;//����ͷ0Ŀ���趨�¶�
	
}_parameter_state_s;

typedef enum 
{
	CNETRAL = 0XFF,
	YES = 0,
	NO = 1,
	
}_yes_no_e;
//ֻ�н����ļ�������ʱ�����SD���β��������
typedef enum
{
	
	UNPLUG_IN_SELECT_FILE = 0,//�ļ��������
	UNPLUG_IN_CONFIRM_FILE = 1,//gcode�ļ�ȷ�Ͻ���
	UNPLUG_IN_PRINT_FILE = 2,//��ӡgcode����
	UNPLUG_IN_PRINT_OPTION = 3,//��ӡgcode�ӽ��棬��option����
	UNPLUG_IN_PRINT_MORE = 4,//��ӡgcode�ӽ��棬��option�������һ�����棺more����
	UNPLUG_IN_STOP_PRINT = 5,//ֹͣ��ӡ����
	UNPLUG_IN_PD_PRINT = 6,//�ϵ�����
//	UNPLUG_IN_FT_PRINT = 7,//���ϳ���ʱ��������Ƚ���
//	UNPLUG_IN_FD_PRINT = 8,//���ϼ�����
//	UNPLUG_IN_PAUSE_PRINT = 9,//��ͣ��ӡ������
}_sdCardUnplugUI;


extern _print_time_s printTime;
extern _which_set_icon icon_flag;
extern _btn_obj **screen_key_group;
extern _pause_play_e PRINT_STATE;
extern _is_pwoff_print_e IS_PWOFF_PRINT;

//extern uint8_t isOption2moreUiPreheatSelected;
//extern uint8_t isOption2moreUiFanSelected;
extern uint8_t isOption2moreUiSelected;
extern char *const optToMoreFanUI_title[GUI_LANGUAGE_NUM];
extern char *const optToMoreFilaUI_title[GUI_LANGUAGE_NUM];
extern char *const optToMoreChangeFilaUI_title[GUI_LANGUAGE_NUM];
extern char *const optToMoreFilaInUI_title[GUI_LANGUAGE_NUM];
extern char *const filaIOUI_titile[GUI_LANGUAGE_NUM];

extern char *const optToMoreFilaIOUI_title [GUI_LANGUAGE_NUM];

extern char *const CONFIRM_CAPTION_TBL[GUI_LANGUAGE_NUM][2];
extern  uint32_t escape_time;
extern char *const sdCardUnplug_remind_infor1 [GUI_LANGUAGE_NUM];
extern char *const sdCardUnplug_remind_infor2 [GUI_LANGUAGE_NUM];
extern char *const sdCardUnplug_remind_infor3 [GUI_LANGUAGE_NUM];
extern char *const sdCardUnplug_remind_infor4 [GUI_LANGUAGE_NUM];
extern char *const gcode_print_title[GUI_LANGUAGE_NUM];
extern u8 filaExistFlag;
extern u8 pause_start_cnt ;
/**********************��������************************/



void print_screen(void);
void stop_screen(void);



void set_valu_for_set(const uint8_t id, const uint8_t dir);

void display_select_button(uint8_t button_num);

void sub_screen_fila_in(void);

void sub_screen_fila_out(void);



void check_is_pause_exacute(void);
/*****************************************/





u32 get_current_root_page_items(_file_page_ctrl_block_s *pFile);
void scan_root_file(_file_page_ctrl_block_s *pFile);
void display_file_page_icon(_file_page_ctrl_block_s *pFile);
void draw_file_page_infor(_file_page_ctrl_block_s *pFile);
u8 goto_next_fileRoot(u8 * newFn, u8 * srcFn);
u8 goto_prev_fileRoot(u8 * srcFn);
 void print_confirm_screen(void);
 
 
 void start_print_screen_init(void);
 void start_print_screen(void);
 void print_time_init(_print_time_s *time);
 void update_print_time(_print_time_s *time);
 void test_prag_bar(void);
void  get_printUI_parameter(_parameter_state_s *parState);
 void  set_printUI_paramter(_parameter_state_s *parState);
void update_printUI_parameter(_parameter_state_s *parState);
u8 is_print_over(void);
void display_current_print_parameter(_parameter_state_s *parState);
void  display_heat_paramter(_parameter_state_s *parState);
void  set_current_print_paramter(_parameter_state_s *parState);
void gcode_print_option_screen_init(void);
void gcode_print_option_screen(void);
void gcode_print_option_screen_init(void);
void gcode_print_option_to_more_screen(void);
void gcode_print_option_to_more_screen_int(void);
u8 * get_current_root_file_name(u8 *path);
void free_current_root_file_name_memory(u8 *fn);


void sd_card_absence_in_select_file_remind_screen(void);
void fila_run_out_pause(void);
void fila_run_out_print(void);

#endif
