
/*
*********************************************************************************************************
*
*	模块名称 : 主界面
*	文件名称 : mainui.c
*	版    本 : V1.0
*	说    明 : 打印机主界面，由10个图标 + 一个热床bmp + 一个热挤出头bmp 组成。
*
*
*********************************************************************************************************
*/



#include "mainui.h"

/**********************调试宏定义**************************/

#define MAINUI_DEBUG_EN

#ifdef MAINUI_DEBUG_EN

	#define MAINUI_DEBUG	DBG

#else
	#define MAINUI_DEBUG(format,...)	((void) 0)	

#endif

#define NUM_ROW		2	//多少行图标
#define ROW_SIZE	4	//每行图标多少个

/************************************************/

typedef enum 
{
	NO_SELECTED_ICON		= 0X3F,
	IS_PREHEAT_ICON			= 0,
	IS_MOVING_ICON 			= 1,
	IS_HOME_ICON			= 2,
	IS_PRINT_ICON			= 3,
	IS_EXTRUDE_ICON			= 4,
	IS_ZLEVEL_ICON			= 5,
	IS_SETTING_ICON			= 6,
	IS_MORE_ICON			= 7,
	
}_mainUI_button_e;



//主界面 控制器
_gui_dev *muidev;

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离


/************************************************/


char *const mianUITiltle[GUI_LANGUAGE_NUM] = 
{
	"准备",
	"Prepare"
};

/**********************图片存储目录定义******************/
//icon的路径表,HOME界面图标
char *const mui_icon_path_tbl[GUI_LANGUAGE_NUM][HOME_BMP_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_mov.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zero.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_printing.bin",	    
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_leveling.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_set.bin",	    
	
	"1:/SYSTEM/PICTURE/ch_bmp_language.bin",
 
	  

	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/en_bmp_mov.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zero.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_printing.bin",	    
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_leveling.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_set.bin",	    

	"1:/SYSTEM/PICTURE/en_bmp_language.bin",

	}
};  


typedef enum 
{
	POS_BASE = 0XFF,
	POS_PREHEAT = 0,
	POS_MOVE ,
	POS_HOME,
	POS_PRINT,
	
	POS_EXTRUDER,
	POS_LEVEL,
	POS_SET,
	POS_MORE,
}_menu_position_e;

_menu_position_e CURRENT_MENU_POS = POS_BASE;






/*
*********************************************************************************************************
*	函 数 名: mui_init
*	功能说明: 初始化界面图标的相关信息，根据模式选择是否显示图标
*	形    参:  mode
mode @0-----只初始化图标相关信息，不显示图标
mode @1-----初始化图标相关信息，显示图标
*	返 回 值: 无
*********************************************************************************************************
*/

static void mui_init(void)
{
	uint16_t i,j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	
	LCD_Clear_DMA(BLACK);

	Show_Str(2,5,lcddev.width,font,(u8 *)mianUITiltle[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	muidev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( muidev == 0)
	{
		MAINUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	muidev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*HOME_BMP_NUMS);
	
	if( muidev->icon == 0)
	{
		MAINUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	muidev->status = 0x3F ;
	
	
	for(i=0;i<NUM_ROW;i++)
	{
		for(j=0;j<ROW_SIZE;j++)
		{
			
			muidev->icon[i*ROW_SIZE+j].x = bmp_sart_offx + j*bmp_between_offx  ;
			muidev->icon[i*ROW_SIZE+j].y = bmp_sart_offy + i*bmp_between_offy  ;
			muidev->icon[i*ROW_SIZE+j].width = PICTURE_X_SIZE;
			muidev->icon[i*ROW_SIZE+j].height = PICTURE_Y_SIZE;
			muidev->icon[i*ROW_SIZE+j].path = (u8*)mui_icon_path_tbl[LANGUAGE][i*ROW_SIZE+j];

		}
	} 
	muidev->totalIcon = HOME_BMP_NUMS;
		for(j=0;j<muidev->totalIcon;j++)
		{
			display_bmp_bin_to_LCD((u8*)muidev->icon[j].path, muidev->icon[j].x , muidev->icon[j].y, muidev->icon[j].width, muidev->icon[j].height, muidev->icon[j].color);
		}	
	
}


/*
*********************************************************************************************************
*	函 数 名: mui_load_icon
*	功能说明: 主界面显示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void mui_load_icon(void)
{
	uint8_t i,j; 
	for(i=0;i<NUM_ROW;i++)
	{
		for(j=0;j<ROW_SIZE;j++)
		{
			display_bmp_bin_to_LCD((u8*)muidev->icon[i*ROW_SIZE+j].path, muidev->icon[i*ROW_SIZE+j].x , muidev->icon[i*ROW_SIZE+j].y, muidev->icon[i*ROW_SIZE+j].width, muidev->icon[i*ROW_SIZE+j].height, muidev->icon[i*ROW_SIZE+j].color);
		}
	}  	
} 

/*
*********************************************************************************************************
*	函 数 名: mui_touch_chk
*	功能说明: 主界面触摸检测
*	形    参:  无
*	返 回 值: 被按下的图标号
@0~0x3f 范围
*********************************************************************************************************
*/    
uint8_t mui_touch_chk(void)
{		 
	uint8_t i=0xff;																 
	if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
	{
		muidev->status|=0X80;	//标记有有效触摸
		muidev->tpx=tp_dev.x[0];
		muidev->tpy=tp_dev.y[0];
	}
	else if(muidev->status&0X80)//按键松开了,并且有有效触摸
	{
		
		for(i=0;i<HOME_BMP_NUMS;i++)
		{
			if((muidev->tpx > muidev->icon[i].x) && (muidev->tpx < muidev->icon[i].x + muidev->icon[i].width) && (muidev->tpy > muidev->icon[i].y)&&//
			   (muidev->tpy < muidev->icon[i].y + muidev->icon[i].height))
			{
				break;//得到选中的编号	
			}
		}
		if(i >= HOME_BMP_NUMS)
		{
			i=0xff;//无效的点按.	
		}
	muidev->status&=0X7F;//清除按键有效标志
	} 	 
	return i; 
}


uint8_t gui_touch_chk(_gui_dev *buttonID)
{		 
	uint8_t i=0xff;																 
	if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
	{
		buttonID->status|=0X80;	//标记有有效触摸
		buttonID->tpx=tp_dev.x[0];
		buttonID->tpy=tp_dev.y[0];
	}
	else if(buttonID->status&0X80)//按键松开了,并且有有效触摸
	{
		
		for(i=0;i<buttonID->totalIcon;i++)
		{
			if((buttonID->tpx > buttonID->icon[i].x) && (buttonID->tpx < buttonID->icon[i].x + buttonID->icon[i].width) && (buttonID->tpy > buttonID->icon[i].y)&&//
			   (buttonID->tpy < buttonID->icon[i].y + buttonID->icon[i].height))
			{
				break;//得到选中的编号	
			}
		}
		if(i >= buttonID->totalIcon)
		{
			i=0xff;//无效的点按.	
		}
	buttonID->status&=0X7F;//清除按键有效标志
	} 	 
	return i; 
}
/*
*********************************************************************************************************
*	函 数 名: main_screen
*	功能说明: 主界面操作
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/ 
//#define CURE_FOR_EXTRUDER
#ifndef CURE_FOR_EXTRUDER
void main_screen(void)
{
	_mainUI_button_e SELECTED_ICON = NO_SELECTED_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	
	if( redraw_screen_flag == IS_TRUE && eeprom_is_power_off_happen() == PW_OFF_NOT_HAPPEN)
	{
		redraw_screen_flag = IS_FALSE;
		
		mui_init();
	}

	SELECTED_ICON = (_mainUI_button_e)gui_touch_chk(muidev);
	
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN )//发生过断电
	{
		SELECTED_ICON = IS_PRINT_ICON;//模拟触发打印按键
	}
	switch ( SELECTED_ICON )
	{
/**************************主页的第一排图标******************************/	
		case IS_PREHEAT_ICON:
				currentMenu = preheat_screen;
				free_flag = IS_TRUE;
				MAINUI_DEBUG("preheat_screen\r\n");
				
			break;
			
		case IS_MOVING_ICON:
				currentMenu = move_screen;
			free_flag = IS_TRUE;
				MAINUI_DEBUG("move_screen\r\n");
				
			break;			
					
		case IS_HOME_ICON:
		currentMenu = home_screen;
				free_flag = IS_TRUE;
				
				MAINUI_DEBUG("home screen\r\n");
				
			break;			
			
		case IS_PRINT_ICON:
		
	
		if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN )
		{
		MAINUI_DEBUG("powerD print screen\r\n");
		currentMenu = powerOffPrint_screen;
		
		}
		else 
		{
			currentMenu = print_screen;		
			MAINUI_DEBUG("print_screen\r\n");
		}
		redraw_fileSystemUI_flag = IS_TRUE;
		free_flag = IS_TRUE;		


				
			break;
			
/****************************主页的第二排图标****************************/			
		case IS_EXTRUDE_ICON:
		free_flag = IS_TRUE;
		currentMenu = extru_screen;
				MAINUI_DEBUG("extruder screen\r\n");
				
			break;
		case IS_ZLEVEL_ICON:
		free_flag = IS_TRUE;
		currentMenu = level_screen;
				MAINUI_DEBUG("zlevel_screen\r\n");
			break;
			
		case IS_SETTING_ICON:
		free_flag = IS_TRUE;
		currentMenu = set_screen;
				MAINUI_DEBUG("set_screen\r\n");
				
			break;			
			
		case IS_MORE_ICON:		
		
		free_flag = IS_TRUE;
		currentMenu = ui_language_screen;


			MAINUI_DEBUG("about screen\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,muidev->icon);
		myfree(SRAMIN,muidev);
		MAINUI_DEBUG("free muidev \r\n");
	}
}

#else

#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

typedef enum
{
	STEP_WAIT_CNT=0,	// 等待连接挤出头
	STEP_HEATING,		// 加热挤出头
	STEP_EXTRUDING,		// 正在挤出
	STEP_WAIT_DISCNT,	// 等待挤出头断开连接
}ExtruderTestStep;

void main_screen(void)	/* for magic挤出头测试治具 */
{
	static u8 initflg=0;
	static ExtruderTestStep step=STEP_WAIT_CNT;
	char tmpstr[8];
	
	if(initflg==0)
	{
		initflg = 1;
		
//		fanSpeed = 255;	// 涡轮风扇
		
		LCD_Clear(BLACK);
		POINT_COLOR = RED;
		Show_Str(82,20,lcddev.width,24,"Extruder Test",24,0);	
		POINT_COLOR = BLUE;
		Show_Str(10, 70,lcddev.width,24,"X-Endstop:",24,0);	
		Show_Str(10,110,lcddev.width,24,"Turbo Fan:",24,0);	
		Show_Str(10,150,lcddev.width,24,"Extruder1:",24,0);	
	}

	POINT_COLOR = YELLOW;
	if(X_MIN_PIN)	Show_Str(140, 70,lcddev.width,24,"OFF",24,0);
	else			Show_Str(140, 70,lcddev.width,24,"ON ",24,0);
	sprintf(tmpstr, "%3d", fanSpeed);
	Show_Str(140,110,lcddev.width,24,tmpstr,24,0);
	
	sprintf(tmpstr, "%3d/%3d", (int)current_temperature[0],target_temperature[0]);
	Show_Str(140,150,lcddev.width,24,tmpstr,24,0);
	
	POINT_COLOR = GREEN;
	switch(step)
	{
		case STEP_WAIT_CNT:
			if(current_temperature[0]<HEATER_0_MINTEMP || current_temperature[0]>HEATER_0_MAXTEMP)
			{
				Show_Str(10,190,lcddev.width,24,"Please connect extruder!",24,0);
			}
			else
			{
				target_temperature[0] = 200;
				Show_Str(10,190,lcddev.width,24,"extruder is heating ... ",24,0);
				step = STEP_HEATING;
				
				fanSpeed = 100;	// 涡轮风扇
			}
			break;
			
		case STEP_HEATING:
			if(current_temperature[0]>=(target_temperature[0]-5)) 
			{
				Show_Str(10,190,lcddev.width,24,"is extruding ...        ",24,0);
				menu_action_gcode(USING_RELATIVE_POSITION);
				menu_action_gcode("G1 E100 F500");
				menu_action_gcode(USING_ABSOLUTE_POSITION);	
				step = STEP_EXTRUDING;
				
				fanSpeed = 255;	// 涡轮风扇
			}		
			break;
			
		case STEP_EXTRUDING:
			if(buflen==0 && block_buffer_head==block_buffer_tail)
			{
				target_temperature[0] = 0;
				Show_Str(10,190,lcddev.width,24,"End of the test!        ",24,0);
				step = STEP_WAIT_DISCNT;
				
				fanSpeed = 0;	// 涡轮风扇
			}
			break;
			
		case STEP_WAIT_DISCNT:
			if(current_temperature[0]<HEATER_0_MINTEMP || current_temperature[0]>HEATER_0_MAXTEMP)
				step = STEP_WAIT_CNT;
			break;
			
		default: break;
	}		
	
}

#endif


