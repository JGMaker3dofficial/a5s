/*
*********************************************************************************************************
*
*	ģ������ : GUIӦ�ù���ģ��
*	�ļ����� : common.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __COMMON_H
#define __COMMON_H 	

/**********************����ͷ�ļ�*************************/

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

/**********************���ȫ�ֱ�������*************************/
//ÿ��ͼ��Ĳ�����Ϣ
__packed typedef struct _m_mui_icos
{										    
	uint16_t x;			//ͼ�����꼰�ߴ�
	uint16_t y;
	uint8_t width;
	uint8_t height; 
	uint8_t * path;		//ͼ��·��
	uint8_t * name;		//ͼ������

	uint16_t color;//ͼ��ı���ɫ	
}_gui_icon_info;

//���� ������
__packed typedef struct _m_mui_dev
{					
	uint16_t tpx;			//���������һ�ε�X����
	uint16_t tpy;			//���������һ�ε�Y����
	uint16_t status;			//��ǰѡ��״̬.
						//bit15~bit8:����
	uint8_t totalIcon;				
						
						//bit7:0,û����Ч����;1,����Ч����.
						//bit6:ͼ�갴��ģʽѡ��λ��0,������һ��״̬���ɿ�����һ��״̬��1,������һ��״̬���ٴΰ�������һ��״̬
						
						//bit5~0:0~62,��ѡ�е�ͼ����;0X3F,û��ѡ���κ�ͼ��  
	_gui_icon_info *icon;	//ͼ��ָ��
}_gui_dev;



/**********************GUIӦ�ý���ȫ�ֱ���*************************/


typedef enum 
{
	IS_FALSE = 0,
	IS_TRUE = 1,
}_GUI_TURE_OR_FALSE;


/******************************************/

											
typedef enum 
{
	CHINESE = 0,		//0,��������
	ENGLISH = 1,		//1,Ӣ��
}_languageSet_e;



typedef enum 
{
	NONE_OPT = 0xFF,
	ADD_OPT = 1,
	DEC_OPT ,
	CLEAR_OPT,
}_obj_option_e;

/**********************��غ궨��*************************/
//Ӳ��ƽ̨��Ӳ���汾����	   	
#define HARDWARE_VERSION	   		30		//Ӳ���汾,�Ŵ�10��,��1.0��ʾΪ10
#define SOFTWARE_VERSION	    	230		//����汾,�Ŵ�100��,��1.00,��ʾΪ100

//ϵͳ���ݱ����ַ			  
#define SYSTEM_PARA_SAVE_BASE 		100		//ϵͳ��Ϣ�����׵�ַ.��100��ʼ.
			    


//APP���ܹ�����Ŀ
#define APP_FUNS_NUM	9



//��������ѡ����Ŀ��������Ϣ
#define APP_ITEM_BTN1_WIDTH		60	  		//��2������ʱ�Ŀ��
#define APP_ITEM_BTN2_WIDTH		100			//ֻ��1������ʱ�Ŀ��
#define APP_ITEM_BTN_HEIGHT		30			//�����߶�
#define APP_ITEM_ICO_SIZE		32			//ICOͼ��ĳߴ�

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//ѡ��ʱ�ı���ɫ
#define APP_WIN_BACK_COLOR	 	0XC618		//���屳��ɫ


#define APP_FB_TOPBAR_HEIGHT	20			//�ļ��������,���������ĸ߶�
#define APP_FB_BTMBAR_HEIGHT	20			//�ļ��������/���Խ���,�ײ������ĸ߶�
#define APP_TEST_TOPBAR_HEIGHT	20			//���Խ���,���������߶�

//��ֵ����
#define	app_pi	3.1415926535897932384626433832795  





#define PICTURE_X_SIZE	78			//ͼƬX�᳤��
#define PICTURE_Y_SIZE	104			//ͼƬY�᳤��

#define COUNT_ARRY_LENGTH(arryName)	(sizeof(arryName)/sizeof(arryName[0]))
/*********************************************/


/**********************��������*************************/
//��ͼ��/ͼƬ·��
extern char *const  APP_OK_PIC;			//ȷ��ͼ��
extern char*const  APP_CANCEL_PIC;		//ȡ��ͼ��
extern char*const  APP_UNSELECT_PIC;		//δѡ��ͼ��
extern char*const  APP_SELECT_PIC;		//ѡ��ͼ��
extern char*const  APP_ASCII_60;			//����������·��
extern char*const  APP_ASCII_28;			//����������·��
extern char*const  APP_VOL_PIC;			//����ͼƬ·��

extern char*const APP_ASCII_S6030;	//����ܴ�����·��
extern char*const APP_ASCII_5427;		//��ͨ������·��
extern char*const APP_ASCII_3618;		//��ͨ������·��
extern char*const APP_ASCII_2814;		//��ͨ������·��

extern u8* asc2_s6030;				//���������60*30���������
extern u8* asc2_5427;				//��ͨ����54*27���������
extern u8* asc2_3618;				//��ͨ����36*18���������
extern u8* asc2_2814;				//��ͨ����28*14���������

extern const u8 APP_ALIENTEK_ICO1824[];	//��������ͼ��,�����flash
extern const u8 APP_ALIENTEK_ICO2432[];	//��������ͼ��,�����flash
extern const u8 APP_ALIENTEK_ICO3648[];	//��������ͼ��,�����flash
extern const u8 HEATER_ICO3535[];
extern const u8 BED_ICO4035[];
extern char *const APP_YESNO_CAPTION_TBL[GUI_LANGUAGE_NUM][2];

extern _languageSet_e	LANGUAGE;
//app��Ҫ���ܽ������

extern char*const  APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern char*const  APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
/***********************************************/
////////////////////////////////////////////////////////////////////////////////////////////









/**********************��������*************************/
																			    
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

u8 app_system_file_check(char* diskx);//ϵͳ�ļ����
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode);
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize);
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode));
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2);
void app_get_version(u8*buf,u32 ver,u8 len);//�õ��汾��

void app_usmart_getsn(void);//USMARTר��.
u8 app_system_parameter_init(void);//ϵͳ��Ϣ��ʼ��
void set_language(u8 id);
/**********************************************/





#endif






























































