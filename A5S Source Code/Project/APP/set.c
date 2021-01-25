
#include "set.h"

#include "gcodeplayer.h"
#include "Dlion.h"

/**********************调试宏定义**************************/

#define SETUI_DEBUG_EN

#ifdef SETUI_DEBUG_EN

#define SETUI_DEBUG	DBG

#else
#define SETUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/


#define SET_BMP_TOTAL_NUMS	 8


_gui_dev *setUIdev;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离

#define NONE_SET_ICON  		0XFF
#define IS_SET_FAN_ICON  	 0
#define IS_SET_FILA_ICON  	 1
#define IS_SET_ABOUT_ICON  	 2
#define IS_SET_FONT_ICON  	 3
#define IS_SET_ICON_ICON  	 4
#define IS_SET_LCDCALI_ICON  	 5
#define IS_SET_MOTOROFF_ICON  	 6

#define IS_SET_BACK_ICON  	 7



//icon的路径表,MOVE界面图标
char *const setui_icon_path_tbl[GUI_LANGUAGE_NUM][SET_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_about.bin",	    
 
 	"1:/SYSTEM/PICTURE/ch_bmp_fontUpdt.bin",//字体更新
	"1:/SYSTEM/PICTURE/ch_bmp_iconUpdt.bin",//图标更新	    
	"1:/SYSTEM/PICTURE/ch_bmp_lcdcalibt.bin",//屏幕校准
	"1:/SYSTEM/PICTURE/ch_bmp_motor_off.bin",//电机关闭
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_about.bin",	    
 
 	"1:/SYSTEM/PICTURE/en_bmp_fontUpdt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_iconUpdt.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_lcdcalibt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_motor_off.bin",
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	   	
	}
};  

char *const SetUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/设置",
	"Prepare/set"

};

void setui_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	LCD_Clear_DMA(BLACK);

	Show_Str(2,5,lcddev.width,font,(u8 *)SetUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	setUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( setUIdev == 0)
	{
		SETUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	setUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*SET_BMP_TOTAL_NUMS);
	
	if( setUIdev->icon == 0)
	{
		SETUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	setUIdev->status = 0x3F ;
	
	
		for(j=0;j<SET_BMP_TOTAL_NUMS;j++)
		{
			

			if(j < 4)	
			{
				setUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx  ;
				setUIdev->icon[j].y = bmp_sart_offy ;			
			}
			else if( j >= 4)
			{
				
				setUIdev->icon[j].x = bmp_sart_offx + (j - 4)*bmp_between_offx  ;
				setUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy ;					
			}

			setUIdev->icon[j].width = PICTURE_X_SIZE;
			setUIdev->icon[j].height = PICTURE_Y_SIZE;
			setUIdev->icon[j].path = (u8*)setui_icon_path_tbl[LANGUAGE][j];
	
		}
		

	setUIdev->totalIcon = SET_BMP_TOTAL_NUMS;
	
		for(j=0; j < setUIdev->totalIcon; j++)
		{
			display_bmp_bin_to_LCD((u8*)setUIdev->icon[j].path, setUIdev->icon[j].x , setUIdev->icon[j].y, setUIdev->icon[j].width, setUIdev->icon[j].height, setUIdev->icon[j].color);
		}	

}
void set_screen(void)
{
	u8 SET_SELECTED_BUTTON = NONE_SET_ICON;
	u8 j;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		setui_init();
	}

	SET_SELECTED_BUTTON = gui_touch_chk(setUIdev);
	
	switch ( SET_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_SET_FAN_ICON:
			free_flag = IS_TRUE;
			currentMenu = fan_screen;
				SETUI_DEBUG("fan\r\n");
				
			break;
			
		case IS_SET_FILA_ICON://换料
			free_flag = IS_TRUE;
			currentMenu = change_fila_screen;
				SETUI_DEBUG("change filament\r\n");
				
			break;		
			
		case IS_SET_ABOUT_ICON:
			free_flag = IS_TRUE;	
			currentMenu = about_screen;
				SETUI_DEBUG("about\r\n");
				
			break;						
			
			
		case IS_SET_FONT_ICON:
		free_flag = IS_TRUE;	
		currentMenu = font_update_screen;
		
			SETUI_DEBUG("update font\r\n");
			
		break;			
			
		case IS_SET_ICON_ICON:
		free_flag = IS_TRUE;	
		currentMenu = picture_update_screen;
			SETUI_DEBUG("update icons\r\n");
			
		break;				
			
			case IS_SET_LCDCALI_ICON:
		free_flag = IS_TRUE;	
		currentMenu = ui_calibrate_screen;
			SETUI_DEBUG("lcd calibration\r\n");
			
		break;			
			
			
			case IS_SET_MOTOROFF_ICON:
		
		j = IS_SET_MOTOROFF_ICON;
		gui_fill_rectangle_DMA(setUIdev->icon[j].x , setUIdev->icon[j].y, setUIdev->icon[j].width, setUIdev->icon[j].height, setUIdev->icon[j].color);
		display_bmp_bin_to_LCD((u8*)setUIdev->icon[j].path, setUIdev->icon[j].x , setUIdev->icon[j].y, setUIdev->icon[j].width, setUIdev->icon[j].height, setUIdev->icon[j].color);
		menu_action_gcode("M84\r\n");//关闭电机驱动
		SETUI_DEBUG("motor off\r\n");
			
		break;				
			
			
		case IS_SET_BACK_ICON:		
		currentMenu = main_screen;
		free_flag = IS_TRUE;
			SETUI_DEBUG("back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	

	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,setUIdev->icon);
		myfree(SRAMIN,setUIdev);
		SETUI_DEBUG("free setUIdev \r\n");
	}

}


