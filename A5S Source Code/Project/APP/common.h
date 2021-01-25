/*
*********************************************************************************************************
*
*	模块名称 : GUI应用公共模块
*	文件名称 : common.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __COMMON_H
#define __COMMON_H 	

/**********************包含头文件*************************/

#include "sys.h"
#include "gui.h"
#include "guix.h"
#include "touch.h"
#include "piclib.h"
#include "bmp.h"
#include "string.h"

#include "ff.h"
#include "exfuns.h"  
#include "fontupd.h"
#include "text.h"		


/******************************************/



#define RAM_ADDR 0X20000000

/**********************相关全局变量定义*************************/
//每个图标的参数信息
__packed typedef struct _m_mui_icos
{										    
	uint16_t x;			//图标坐标及尺寸
	uint16_t y;
	uint8_t width;
	uint8_t height; 
	uint8_t * path;		//图标路径
	uint8_t * name;		//图标名字

	uint16_t color;//图标的背景色	
}_gui_icon_info;

//界面 控制器
__packed typedef struct _m_mui_dev
{					
	uint16_t tpx;			//触摸屏最近一次的X坐标
	uint16_t tpy;			//触摸屏最近一次的Y坐标
	uint16_t status;			//当前选中状态.
						//bit15~bit8:保留
	uint8_t totalIcon;				
						
						//bit7:0,没有有效触摸;1,有有效触摸.
						//bit6:图标按键模式选择位：0,按下是一种状态、松开是另一种状态；1,按下是一种状态，再次按下是另一种状态
						
						//bit5~0:0~62,被选中的图标编号;0X3F,没有选中任何图标  
	_gui_icon_info *icon;	//图标指针
}_gui_dev;



/**********************GUI应用界面全局变量*************************/


typedef enum 
{
	IS_FALSE = 0,
	IS_TRUE = 1,
}_GUI_TURE_OR_FALSE;


/******************************************/

											
typedef enum 
{
	CHINESE = 0,		//0,简体中文
	ENGLISH = 1,		//1,英文
}_languageSet_e;



typedef enum 
{
	NONE_OPT = 0xFF,
	ADD_OPT = 1,
	DEC_OPT ,
	CLEAR_OPT,
}_obj_option_e;

/**********************相关宏定义*************************/
//硬件平台软硬件版本定义	   	
#define HARDWARE_VERSION	   		30		//硬件版本,放大10倍,如1.0表示为10
#define SOFTWARE_VERSION	    	230		//软件版本,放大100倍,如1.00,表示为100

//系统数据保存基址			  
#define SYSTEM_PARA_SAVE_BASE 		100		//系统信息保存首地址.从100开始.
			    


//APP的总功能数目
#define APP_FUNS_NUM	9



//弹出窗口选择条目的设置信息
#define APP_ITEM_BTN1_WIDTH		60	  		//有2个按键时的宽度
#define APP_ITEM_BTN2_WIDTH		100			//只有1个按键时的宽度
#define APP_ITEM_BTN_HEIGHT		30			//按键高度
#define APP_ITEM_ICO_SIZE		32			//ICO图标的尺寸

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//选择时的背景色
#define APP_WIN_BACK_COLOR	 	0XC618		//窗体背景色


#define APP_FB_TOPBAR_HEIGHT	20			//文件浏览界面,顶部横条的高度
#define APP_FB_BTMBAR_HEIGHT	20			//文件浏览界面/测试界面,底部横条的高度
#define APP_TEST_TOPBAR_HEIGHT	20			//测试界面,顶部横条高度

//π值定义
#define	app_pi	3.1415926535897932384626433832795  





#define PICTURE_X_SIZE	78			//图片X轴长度
#define PICTURE_Y_SIZE	104			//图片Y轴长度

#define COUNT_ARRY_LENGTH(arryName)	(sizeof(arryName)/sizeof(arryName[0]))
/*********************************************/


/**********************导出变量*************************/
//各图标/图片路径
extern char *const  APP_OK_PIC;			//确认图标
extern char*const  APP_CANCEL_PIC;		//取消图标
extern char*const  APP_UNSELECT_PIC;		//未选中图标
extern char*const  APP_SELECT_PIC;		//选中图标
extern char*const  APP_ASCII_60;			//大数字字体路径
extern char*const  APP_ASCII_28;			//大数字字体路径
extern char*const  APP_VOL_PIC;			//音量图片路径

extern char*const APP_ASCII_S6030;	//数码管大字体路径
extern char*const APP_ASCII_5427;		//普通大字体路径
extern char*const APP_ASCII_3618;		//普通大字体路径
extern char*const APP_ASCII_2814;		//普通大字体路径

extern u8* asc2_s6030;				//数码管字体60*30大字体点阵集
extern u8* asc2_5427;				//普通字体54*27大字体点阵集
extern u8* asc2_3618;				//普通字体36*18大字体点阵集
extern u8* asc2_2814;				//普通字体28*14大字体点阵集

extern const u8 APP_ALIENTEK_ICO1824[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO2432[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO3648[];	//启动界面图标,存放在flash
extern const u8 HEATER_ICO3535[];
extern const u8 BED_ICO4035[];
extern char *const APP_YESNO_CAPTION_TBL[GUI_LANGUAGE_NUM][2];

extern _languageSet_e	LANGUAGE;
//app主要功能界面标题

extern char*const  APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
/***********************************************/
////////////////////////////////////////////////////////////////////////////////////////////









/**********************导出函数*************************/
																			    
u32  app_get_rand(u32 max);
void app_srand(u32 seed);
void app_set_lcdsize(u8 mode);
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 app_get_numlen(long long num,u8 dir);
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor);
void app_filebrower(u8 *topname,u8 mode);		 
void app_showbigchar(u8 *fontbase,u16 x,u16 y,u8 chr,u8 size,u16 color,u16 bkcolor);
void app_showbigstring(u8 *fontbase,u16 x,u16 y,const u8 *p,u8 size,u16 color,u16 bkcolor);
void app_showbignum(u8 *fontbase,u16 x,u16 y,u32 num,u8 len,u8 size,u16 color,u16 bkcolor);
void app_showbig2num(u8 *fontbase,u16 x,u16 y,u8 num,u8 size,u16 color,u16 bkcolor);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height);
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor);
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_system_file_check(char* diskx);//系统文件检测
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode);
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize);
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode));
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2);
void app_get_version(u8*buf,u32 ver,u8 len);//得到版本号

void app_usmart_getsn(void);//USMART专用.
u8 app_system_parameter_init(void);//系统信息初始化
void set_language(u8 id);
/**********************************************/





#endif






























































