
#include "home.h"
#include "gcodeplayer.h"
#include "Dlion.h"
/**********************调试宏定义**************************/

#define HOMEUI_DEBUG_EN

#ifdef HOMEUI_DEBUG_EN

#define HOMEUI_DEBUG	DBG

#else
#define HOMEUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



#define HOME_NUM_ROW	1//总共1行控件
#define HOME_ROW_COUNT	4//每行用4个控件


#define HOME_BMP_TOTAL_NUMS	 5

#define HOME_Z_UP	10//Z轴上台高度

_gui_dev *homeUIdev;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离



typedef enum 
{
	NONE_HOME_ICON = 0XFF,
	IS_ALL_HOME_ICON = 0,
	IS_X_HOME_ICON = 1,
	IS_Y_HOME_ICON = 2,
	IS_Z_HOME_ICON = 3,
	
	IS_HOME_BACK_ICON = 4,
		
}_homeUI_selected_button_e;

_homeUI_selected_button_e HOME_SAVED_BUTTON = NONE_HOME_ICON;
//icon的路径表,MOVE界面图标
char *const homeui_icon_path_tbl[GUI_LANGUAGE_NUM][HOME_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroX.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zeroY.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zeroZ.bin",	    

	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroX.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zeroY.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zeroZ.bin",	    

	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	 	
	
	}
};  

char *const homeUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/归零",
	"Prepare/home"

};

void homeui_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	LCD_Clear_DMA(BLACK);

Show_Str(2,5,lcddev.width,font,(u8 *)homeUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	homeUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( homeUIdev == 0)
	{
		HOMEUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	homeUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*HOME_BMP_TOTAL_NUMS);
	
	if( homeUIdev->icon == 0)
	{
		HOMEUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	homeUIdev->status = 0x3F ;
	
	

		for(j=0;j<HOME_ROW_COUNT;j++)
		{
			
			homeUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx  ;
			homeUIdev->icon[j].y = bmp_sart_offy ;
			homeUIdev->icon[j].width = PICTURE_X_SIZE;
			homeUIdev->icon[j].height = PICTURE_Y_SIZE;
			homeUIdev->icon[j].path = (u8*)homeui_icon_path_tbl[LANGUAGE][j];

		}
		//初始化返回控件
		homeUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx;
		homeUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
		homeUIdev->icon[j].width = PICTURE_X_SIZE;
		homeUIdev->icon[j].height = PICTURE_Y_SIZE;
		homeUIdev->icon[j].path = (u8*)homeui_icon_path_tbl[LANGUAGE][j];

	
	
	homeUIdev->totalIcon = HOME_BMP_TOTAL_NUMS;
	
		for(j=0; j < homeUIdev->totalIcon; j++)
		{
			display_bmp_bin_to_LCD((u8*)homeUIdev->icon[j].path, homeUIdev->icon[j].x , homeUIdev->icon[j].y, homeUIdev->icon[j].width, homeUIdev->icon[j].height, homeUIdev->icon[j].color);
		}	

}
void home_screen(void)
{
	_homeUI_selected_button_e HOME_SELECTED_BUTTON = NONE_HOME_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		homeui_init();
	}

	HOME_SELECTED_BUTTON = (_homeUI_selected_button_e)gui_touch_chk(homeUIdev);
	
	switch ( HOME_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_ALL_HOME_ICON:
			HOME_SAVED_BUTTON = IS_ALL_HOME_ICON;
			exacute_z_up();
			user_cmd_move_allHome();
				HOMEUI_DEBUG("all home\r\n");
				
			break;
			
		case IS_X_HOME_ICON:
	HOME_SAVED_BUTTON = IS_X_HOME_ICON;
	exacute_z_up();
	user_cmd_move_xHome();
				HOMEUI_DEBUG("x home\r\n");
				
			break;			
					
		case IS_Y_HOME_ICON:
	HOME_SAVED_BUTTON = IS_Y_HOME_ICON;
	exacute_z_up();
	user_cmd_move_yHome();
				HOMEUI_DEBUG("y home\r\n");
				
			break;			
			
		case IS_Z_HOME_ICON:
		HOME_SAVED_BUTTON = IS_Z_HOME_ICON;		
user_cmd_move_zHome();		
				HOMEUI_DEBUG("z home\r\n");
			break;
			
/****************************主页的第二排图标****************************/			

		case IS_HOME_BACK_ICON:		
		currentMenu = main_screen;
		free_flag = IS_TRUE;
			HOMEUI_DEBUG("home back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
if(HOME_SAVED_BUTTON != NONE_HOME_ICON)
{
	
	gui_fill_rectangle_DMA(homeUIdev->icon[HOME_SAVED_BUTTON].x, homeUIdev->icon[HOME_SAVED_BUTTON].y, homeUIdev->icon[HOME_SAVED_BUTTON].width, homeUIdev->icon[HOME_SAVED_BUTTON].height,BLACK);
	display_bmp_bin_to_LCD((u8*)homeUIdev->icon[HOME_SAVED_BUTTON].path, homeUIdev->icon[HOME_SAVED_BUTTON].x , homeUIdev->icon[HOME_SAVED_BUTTON].y, homeUIdev->icon[HOME_SAVED_BUTTON].width, homeUIdev->icon[HOME_SAVED_BUTTON].height, homeUIdev->icon[HOME_SAVED_BUTTON].color);
	HOME_SAVED_BUTTON = NONE_HOME_ICON;
	
}	
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,homeUIdev->icon);
		myfree(SRAMIN,homeUIdev);
		HOMEUI_DEBUG("free homeUIdev \r\n");
	}

}

void exacute_z_up(void)
{
	char cmd[30];
	u32 feedrate = 1800;
	float zPos = HOME_Z_UP;
	if( current_position[Z_AXIS] != zPos)
	{
		sprintf(cmd,"G1 F%d Z%.1f",feedrate,zPos);
		menu_action_gcode(cmd);		
	}
}
