
/*
*********************************************************************************************************
*
*	模块名称 : 打印界面
*	文件名称 : print.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/
#ifndef __PRINT_H
#define __PRINT_H 	 

/**********************包含头文件*************************/
#include "app_gui.h"
#include "common.h"
/*****************************************/


/**********************相关宏定义*************************/

#define PRINT_BMP_NUMS	5	//打印界面图标个数  

#define PRINT_SET_BMP_NUMS	6	//打印界面图标个数  


/*****************************************/

/**********************打印设置中所有图标ID***********************/
//根据创建按钮时分配的ID值来修改这里的值
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

//加减标志
#define ADD_DIR		0X11
#define SUB_DIR		 0X55

//热床极限值
#define BED_MIN_TEMP	10
#define BED_MAX_TEMP	50

//挤出头极限值
#define EXRUDER_MIN_TEMP	10
#define EXRUDER_MAX_TEMP	275


//风扇极限值
#define FAN_MIN_DUTY	0
#define FAN_MAX_DUTY	100



#define GCODE_OFFSET   		9 //"0:\GCODE\"

 
//区分是哪一个界面，预热界面中的进退料和打印设置界面中的进退料共用相同代码
#define IS_PRINT_SET_SCREEN 	1
#define IS_PRHEAT_SCREEN   		2
/**********************************************/


/**********************进退料提示信息************************/
#define FILAM_IN_ERROR_MESG	"请暂停打印,再进行进料操作!"
#define FILAM_OUT_ERROR_MESG	"请暂停打印,再进行退料操作!"

#define FILAM_IN_REMIN_MESG	"是否确认执行进料操作 ?"
#define FILAM_OUT_REMIN_MESG	"是否确认执行退料操作 ?"


#define FILAM_IN_MESG_BEGIN	"正在进料中..."
#define FILAM_IN_MESG_DONE	"进料完成!"

#define FILAM_OUT_MESG_BEGIN	"正在退料中..."
#define FILAM_OUT_MESG_DONE		"退料完成!"










/**********************导出变量************************/

extern float step_value[3];

extern float set_value[3];
extern uint8_t adj_value_flag;
extern _GUI_TURE_OR_FALSE is_set_value;


extern _GUI_TURE_OR_FALSE is_set_value ;
extern uint8_t *printFileNameNOPrefix;
extern uint8_t *printFileName;//记录当前打印的文件名，包含目录
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

	u8 activeButtonNum;//UI界面活动的文件图标个数,依每一页面设定
	u8 fixButtonNum;//UI界面固定的控制图标个数，比如上一页、下一页、返回等控制图标，依每一页面设定
	
	u32 pageCnt;//页面计数器
	u32 TotalPage;//当前目录下的文件/文件夹需要多少页来显示，遍历完当前目录就得到该值

}_file_gui_attribute_s;


__packed typedef struct
{
	u16 fliter;						//文件过滤器,大类过滤，初始化设置
									//参见"exfuns.h"定义
	u16 fliterex;					//文件扩展的过滤器//用来选择大类里面的小类,最多支持16种不同的文件.默认0XFFFF,即不对小类进行刷选.，初始化设置
	u16 atualReadItems;	//单次盘寻某目录下文件的实际个剩，最大为atualReadItems ==activeButtonNum
	u16 readItemsOffSet;//某个目录下，盘寻偏移。当按下换页控件时，假如换页成功，则新页的读取的文件偏移 readItemsOffSet = pageCnt*activeButtonNum，
	_dest_file_type_e destFileType;//选中的目标文件类别
	u8 fileDepth;
	u8 selectedButtonID;//扫描界面选中的控件，上一页、下一页或者返回
	u8 *destFileName;//选中的目标文件名字
	
	u8 *path;//当前文件目录
	u8 **FileNameListTab;//当前文件名列表
	u8 *FileNameListAttributeTab;//当前文件名列表的属性表,1--gcode文件，0--文件夹
	
	 _file_gui_attribute_s *fileGuiAttr;//当前页面的GUI属性
	 
	
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
	float bedCurTemp;//热床当前温度
	float bedDestTemp;//热床目标设定温度
	u8 fanSpeed;
	float etr0CurTemp;//挤出头0当前温度
	float etr0DestTemp;//挤出头0目标设定温度
	
}_parameter_state_s;

typedef enum 
{
	CNETRAL = 0XFF,
	YES = 0,
	NO = 1,
	
}_yes_no_e;
//只有进入文件管理器时，检测SD卡拔插才有意义
typedef enum
{
	
	UNPLUG_IN_SELECT_FILE = 0,//文件浏览界面
	UNPLUG_IN_CONFIRM_FILE = 1,//gcode文件确认界面
	UNPLUG_IN_PRINT_FILE = 2,//打印gcode界面
	UNPLUG_IN_PRINT_OPTION = 3,//打印gcode子界面，即option界面
	UNPLUG_IN_PRINT_MORE = 4,//打印gcode子界面，即option界面的下一个界面：more界面
	UNPLUG_IN_STOP_PRINT = 5,//停止打印界面
	UNPLUG_IN_PD_PRINT = 6,//断电续打
//	UNPLUG_IN_FT_PRINT = 7,//断料持续时间过长加热界面
//	UNPLUG_IN_FD_PRINT = 8,//断料检测界面
//	UNPLUG_IN_PAUSE_PRINT = 9,//暂停打印过程中
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
/**********************导出函数************************/



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
