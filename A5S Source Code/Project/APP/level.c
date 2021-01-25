
#include "level.h"
#include "gcodeplayer.h"
#include "Dlion.h"
/**********************调试宏定义**************************/

#define LEVELUI_DEBUG_EN

#ifdef LEVELUI_DEBUG_EN

#define LEVELUI_DEBUG	DBG

#else
#define LEVELUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



#define LEVEL_NUM_ROW	1//总共1行控件
#define LEVEL_ROW_COUNT	4//每行用4个控件
#define LEVEL_SECOND_ROW_COUNT	2//第二行2个控件

#define LEVEL_BMP_TOTAL_NUMS	 6

#if	MERCHIN == A1

	//打印头距离热床边框的距离
	#define X_OFFSET	50
	#define Y_OFFSET	50
/////////////////////////////////////////////////////
#elif MERCHIN == A3
	//打印头距离热床边框的距离
	#define X_OFFSET	30
	#define Y_OFFSET	30
/////////////////////////////////////////////////////
#elif MERCHIN == A4
	//打印头距离热床边框的距离
	#define X_OFFSET	30
	#define Y_OFFSET	30
/////////////////////////////////////////////////////
#elif MERCHIN == A5
	//打印头距离热床边框的距离
	#define X_OFFSET	50
	#define Y_OFFSET	50
/////////////////////////////////////////////////////	
#elif MERCHIN == A6
	//打印头距离热床边框的距离
	#define X_OFFSET	50
	#define Y_OFFSET	50	
/////////////////////////////////////////////////////	
#endif	

#define Z_OFFSET	10//Z轴偏离热床高度

#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)

#define LEVEL_SENSE		KEY_6_LONG
#define LEVEL_REPT		KEY_6_DOWN
#define LEVEL_KEYID		KID_JOY_L

//sensor's position acording to the extruder
#define LEVEL_SENSE_X_POS	32
#define LEVEL_SENSE_Y_POS	17

//extruder offset position for the first levelling point
#define LEVEL_SENSE_OFFSET_X_POS 50
#define LEVEL_SENSE_OFFSET_Y_POS 80

#define LEVEL_SENSE_OFFSET_X_POS_COMP 20
#define LEVEL_SENSE_OFFSET_Y_POS_COMP 50
//调平坐标设置
#define FIRST_POINT_X	(LEVEL_SENSE_OFFSET_X_POS - LEVEL_SENSE_X_POS)
#define FIRST_POINT_Y	(LEVEL_SENSE_OFFSET_Y_POS - LEVEL_SENSE_Y_POS)

#define SECOND_POINT_X	(X_MAX_LENGTH - LEVEL_SENSE_X_POS - LEVEL_SENSE_OFFSET_X_POS_COMP)
#define SECOND_POINT_Y	(LEVEL_SENSE_OFFSET_Y_POS - LEVEL_SENSE_Y_POS)

#define THIRD_POINT_X	(X_MAX_LENGTH - LEVEL_SENSE_X_POS - LEVEL_SENSE_OFFSET_X_POS_COMP)
#define THIRD_POINT_Y	(Y_MAX_LENGTH - LEVEL_SENSE_Y_POS - LEVEL_SENSE_OFFSET_Y_POS_COMP)

#define FOURTH_POINT_X	(LEVEL_SENSE_OFFSET_X_POS - LEVEL_SENSE_X_POS)
#define FOURTH_POINT_Y	(Y_MAX_LENGTH - LEVEL_SENSE_Y_POS - LEVEL_SENSE_OFFSET_Y_POS_COMP)


#define FIFTH_POINT_X	(X_MAX_LENGTH/2 - LEVEL_SENSE_X_POS)
#define FIFTH_POINT_Y	(Y_MAX_LENGTH/2 - LEVEL_SENSE_Y_POS)

static void Check_Beep(void);
static void Level_BeepBlock_clear(void);

typedef	struct 
{
	u8 Beeped;
	u32 escpTime;
}_xLvBeep_s;

static _xLvBeep_s LvBeep = {FALSE,0};
#else
//调平坐标设置
#define FIRST_POINT_X	X_OFFSET
#define FIRST_POINT_Y	Y_OFFSET

#define SECOND_POINT_X	(X_MAX_LENGTH - X_OFFSET)
#define SECOND_POINT_Y	Y_OFFSET

#define THIRD_POINT_X	(X_MAX_LENGTH - X_OFFSET)
#define THIRD_POINT_Y	(Y_MAX_LENGTH - Y_OFFSET)

#define FOURTH_POINT_X	X_OFFSET
#define FOURTH_POINT_Y	(Y_MAX_LENGTH - Y_OFFSET)


#define FIFTH_POINT_X	(X_MAX_LENGTH/2)
#define FIFTH_POINT_Y	(Y_MAX_LENGTH/2)
#endif


_gui_dev *levelUIdev;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
static uint8_t levelHomeOK = FALSE;//调平回原点标志




typedef enum 
{
	NONE_LEVEL_ICON = 0XFF,
	IS_LEVEL_1POINT_ICON = 0,
	IS_LEVEL_2POINT_ICON = 1,
	IS_LEVEL_3POINT_ICON = 2,
	IS_LEVEL_4POINT_ICON = 3,
	IS_LEVEL_5POINT_ICON = 4,	
	
	IS_LEVEL_BACK_ICON = 5,
		
}_levelUI_selected_button_e;

_levelUI_selected_button_e LEVEL_SAVED_BUTTON = NONE_LEVEL_ICON;

#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)
_levelUI_selected_button_e LEVEL_SENSE_PT = NONE_LEVEL_ICON;
#endif

//icon的路径表,MOVE界面图标
char *const levelui_icon_path_tbl[GUI_LANGUAGE_NUM][LEVEL_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling2.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_leveling3.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_leveling4.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_leveling5.bin",	 
	
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	  
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling2.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_leveling3.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_leveling4.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_leveling5.bin",	 
	
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	  		
	}
};  

char *const LevelUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/调平",
	"Prepare/level"
};

void levelui_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;

	LCD_Clear_DMA(BLACK);
BACK_COLOR = BLACK;
POINT_COLOR = WHITE;
Show_Str(2,5,lcddev.width,font,(u8 *)LevelUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	levelUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( levelUIdev == 0)
	{
		LEVELUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	levelUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*LEVEL_BMP_TOTAL_NUMS);
	
	if( levelUIdev->icon == 0)
	{
		LEVELUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	levelUIdev->status = 0x3F ;
	
	

		for(j=0;j<LEVEL_ROW_COUNT;j++)
		{
			
			levelUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx  ;
			levelUIdev->icon[j].y = bmp_sart_offy ;
			levelUIdev->icon[j].width = PICTURE_X_SIZE;
			levelUIdev->icon[j].height = PICTURE_Y_SIZE;
			levelUIdev->icon[j].path = (u8*)levelui_icon_path_tbl[LANGUAGE][j];

		}
		for(j=LEVEL_ROW_COUNT; j < LEVEL_ROW_COUNT + LEVEL_SECOND_ROW_COUNT; j++)
		{
		//初始化返回控件
		levelUIdev->icon[j].x = bmp_sart_offx + (j-LEVEL_ROW_COUNT)*3*bmp_between_offx;
		levelUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
		levelUIdev->icon[j].width = PICTURE_X_SIZE;
		levelUIdev->icon[j].height = PICTURE_Y_SIZE;
		levelUIdev->icon[j].path = (u8*)levelui_icon_path_tbl[LANGUAGE][j];
			
		}
		

	
	
	levelUIdev->totalIcon = LEVEL_BMP_TOTAL_NUMS;
	
		for(j=0; j < levelUIdev->totalIcon; j++)
		{
			display_bmp_bin_to_LCD((u8*)levelUIdev->icon[j].path, levelUIdev->icon[j].x , levelUIdev->icon[j].y, levelUIdev->icon[j].width, levelUIdev->icon[j].height, levelUIdev->icon[j].color);
		}	

}
void level_screen(void)
{
	_levelUI_selected_button_e LEVEL_SELECTED_BUTTON = NONE_LEVEL_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		levelui_init();
		
		menu_action_gcode(USING_ABSOLUTE_POSITION);	
	}

	LEVEL_SELECTED_BUTTON = (_levelUI_selected_button_e)gui_touch_chk(levelUIdev);
	
	switch ( LEVEL_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_LEVEL_1POINT_ICON:
			 LEVEL_SAVED_BUTTON = IS_LEVEL_1POINT_ICON;
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)			 
			 LEVEL_SENSE_PT = LEVEL_SAVED_BUTTON;
			BEEP_OFF;
#endif			
				LEVELUI_DEBUG("1st point\r\n");
				level_first_point_callBack();
			break;
			
		case IS_LEVEL_2POINT_ICON:
	 LEVEL_SAVED_BUTTON = IS_LEVEL_2POINT_ICON;
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)	 
	 LEVEL_SENSE_PT = LEVEL_SAVED_BUTTON;
	BEEP_OFF;
#endif
				LEVELUI_DEBUG("2st point\r\n");
				level_second_point_callBack();
			break;			
					
		case IS_LEVEL_3POINT_ICON:
	LEVEL_SAVED_BUTTON = IS_LEVEL_3POINT_ICON;
	
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)	
	LEVEL_SENSE_PT = LEVEL_SAVED_BUTTON;
			BEEP_OFF;
#endif

				LEVELUI_DEBUG("3st point\r\n");
				level_third_point_callBack();
			break;			
			
		case IS_LEVEL_4POINT_ICON:
		LEVEL_SAVED_BUTTON = IS_LEVEL_4POINT_ICON;		
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)		
		LEVEL_SENSE_PT = LEVEL_SAVED_BUTTON;
			BEEP_OFF;
#endif

				LEVELUI_DEBUG("4st point\r\n");
				level_fourth_point_callBack();
			break;
			
/****************************主页的第二排图标****************************/			

		case IS_LEVEL_5POINT_ICON:		
		
	 LEVEL_SAVED_BUTTON = IS_LEVEL_5POINT_ICON;
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)	 
	 LEVEL_SENSE_PT = LEVEL_SAVED_BUTTON;
	 BEEP_OFF;
#endif

			LEVELUI_DEBUG("5st point \r\n");
			level_fifth_point_callBack();
			break;	
			
		case IS_LEVEL_BACK_ICON:		
		currentMenu = main_screen;
		free_flag = IS_TRUE;
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)	 		
		LEVEL_SENSE_PT =  NONE_LEVEL_ICON;
#endif
			LEVELUI_DEBUG("level back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
if(LEVEL_SAVED_BUTTON != NONE_LEVEL_ICON)
{
	
	gui_fill_rectangle_DMA(levelUIdev->icon[LEVEL_SAVED_BUTTON].x, levelUIdev->icon[LEVEL_SAVED_BUTTON].y, levelUIdev->icon[LEVEL_SAVED_BUTTON].width, levelUIdev->icon[LEVEL_SAVED_BUTTON].height,BLACK);
	display_bmp_bin_to_LCD((u8*)levelUIdev->icon[LEVEL_SAVED_BUTTON].path, levelUIdev->icon[LEVEL_SAVED_BUTTON].x , levelUIdev->icon[LEVEL_SAVED_BUTTON].y, levelUIdev->icon[LEVEL_SAVED_BUTTON].width, levelUIdev->icon[LEVEL_SAVED_BUTTON].height, levelUIdev->icon[LEVEL_SAVED_BUTTON].color);
	LEVEL_SAVED_BUTTON = NONE_LEVEL_ICON;

}	
#if (MERCHIN	== A5 && A5S_USE_LEVEL_BEEP == 1)
	if(LEVEL_SENSE_PT != NONE_LEVEL_ICON)//调平点感应检测
	{

		Check_Beep();
		
	}
#endif
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,levelUIdev->icon);
		myfree(SRAMIN,levelUIdev);
		LEVELUI_DEBUG("free levelUIdev \r\n");
		
		levelHomeOK = FALSE;
#if	(MERCHIN == A5  && A5S_USE_LEVEL_BEEP == 1)		
		Level_BeepBlock_clear();
#endif

	}

}

void level_first_point_callBack(void)
{
	
	char cmd[30];
	u32 feedrate = 1800;
	float xPos = FIRST_POINT_X;
	float yPos = FIRST_POINT_Y;
	
	float zPos = Z_OFFSET;
	if( current_position[Z_AXIS] < zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}
	if(levelHomeOK == FALSE)
	{
		user_cmd_move_allHome();//所有轴回原点
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}
	
	feedrate = 4800;
	sprintf(cmd,"G1 F%d X%.1f Y%.1f",feedrate,xPos,yPos);//移动x、y轴
	menu_action_gcode(cmd);
	
	level_z_home(zPos);
	levelHomeOK = TRUE;
}



void level_second_point_callBack(void)
{
	char cmd[30];
	u32 feedrate = 1800;
	float xPos = SECOND_POINT_X;
	float yPos = SECOND_POINT_Y;
	
	float zPos = Z_OFFSET;
	
	if( current_position[Z_AXIS] < zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}
	if(levelHomeOK == FALSE)
	{
		user_cmd_move_allHome();//所有轴回原点
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}

	feedrate = 4800;
	sprintf(cmd,"G1 F%d X%.1f Y%.1f",feedrate,xPos,yPos);//移动x、y轴
	menu_action_gcode(cmd);
	level_z_home(zPos);
	
	levelHomeOK = TRUE;
}
void level_third_point_callBack(void)
{
	char cmd[30];
	u32 feedrate = 1800;
	float xPos = THIRD_POINT_X;
	float yPos = THIRD_POINT_Y;
	float zPos = Z_OFFSET;
	if( current_position[Z_AXIS] < zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}
	
	if(levelHomeOK == FALSE)
	{
		user_cmd_move_allHome();//所有轴回原点
			
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}

	feedrate = 4800;
	sprintf(cmd,"G1 F%d X%.1f Y%.1f",feedrate,xPos,yPos);//移动x、y轴
	menu_action_gcode(cmd);
	
	level_z_home(zPos);
	
	levelHomeOK = TRUE;

}
void level_fourth_point_callBack(void)
{
	char cmd[30];
	u32 feedrate = 1800;
	float xPos = FOURTH_POINT_X;
	float yPos = FOURTH_POINT_Y;
	float zPos = Z_OFFSET;
	if( current_position[Z_AXIS] < zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}

	if(levelHomeOK == FALSE)
	{
		user_cmd_move_allHome();//所有轴回原点

		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);	
	}

	feedrate = 4800;
	sprintf(cmd,"G1 F%d X%.1f Y%.1f",feedrate,xPos,yPos);//移动x、y轴
	menu_action_gcode(cmd);
	
	level_z_home(zPos);
	levelHomeOK = TRUE;
}
void level_fifth_point_callBack(void)
{
	char cmd[30];
	u32 feedrate = 1800;
	float xPos = FIFTH_POINT_X;
	float yPos = FIFTH_POINT_Y;
	float zPos = Z_OFFSET;
	if( current_position[Z_AXIS] < zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}

	if(levelHomeOK == FALSE)
	{
		user_cmd_move_allHome();//所有轴回原点
		
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先升起
		menu_action_gcode(cmd);		
	}

	feedrate = 4800;
	sprintf(cmd,"G1 F%d X%.1f Y%.1f",feedrate,xPos,yPos);//移动x、y轴
	menu_action_gcode(cmd);
	level_z_home(zPos);
	levelHomeOK = TRUE;
}

void level_z_home(float hight)
{
	char cmd[20];
	float zPos = 0;
	u32 feedrate = 1800;
	u8 i;
	for(i = 0; i < 1; i++)//z轴先下降->再上升，重复两次
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先下降
		menu_action_gcode(cmd);	
		
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,hight);//z 轴先升起
		menu_action_gcode(cmd);	
						
	}
	sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);//z 轴先下降
	menu_action_gcode(cmd);	
}

#if (MERCHIN	== A5 && A5S_USE_LEVEL_BEEP == 1)
static void Check_Beep(void)
{
	u8 keyValue;
	u8 rvt;
	keyValue = bsp_GetKeyState(LEVEL_KEYID);

	if(keyValue == TRUE)// continuous keep for a long time
	{
	//	LEVELUI_DEBUG("not sense\r\n");
		BEEP_ON;
	}
	else //not available
	{

		BEEP_OFF;
	//	LEVELUI_DEBUG("not sense\r\n");
	}

}

static void Level_BeepBlock_clear(void)
{
	LvBeep.Beeped = FALSE;
	LvBeep.escpTime = 0;
	BEEP_OFF;
}
#endif
