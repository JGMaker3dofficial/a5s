
#include "uiComplmt.h"
#include "gcodeplayer.h"
#include "Dlion.h"
/**********************调试宏定义**************************/

#define CMPLT_DEBUG_EN

#ifdef CMPLT_DEBUG_EN

#define CMPLT_DEBUG	DEBUG_PRINTF

#else
#define CMPLT_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define CMPLT_ROW_COUNT	2//每行用2个控件


#define CMPLT_BMP_TOTAL_NUMS	 3


_gui_dev *complmtUIdev;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离

static void vCallBack_Toggle_Led(void);

typedef enum 
{
	NONE_CMPLT_ICON = 0XFF,
	IS_OFF_CMPLT_ICON = 0,
	IS_ON_CMPLT_ICON = 1,


	IS_CMPLT_BACK_ICON = 2,
		
}_complmtUI_selected_button_e;

_complmtUI_selected_button_e CMPLT_SAVED_BUTTON = NONE_CMPLT_ICON;
//icon的路径表,MOVE界面图标
char *const complmtui_icon_path_tbl[GUI_LANGUAGE_NUM][CMPLT_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_zeroA.bin",
    "1:/SYSTEM/PICTURE/ch_bmp_zeroX.bin",


	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroX.bin",    

	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	 	
	
	}
};  

char *const complmtUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"更多/控制",
	"more/ctrl"

};

void complmtui_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	LCD_Clear_DMA(BLACK);

Show_Str(2,5,lcddev.width,font,(u8 *)complmtUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	complmtUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( complmtUIdev == 0)
	{
		CMPLT_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	complmtUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*CMPLT_BMP_TOTAL_NUMS);
	
	if( complmtUIdev->icon == 0)
	{
		CMPLT_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	complmtUIdev->status = 0x3F ;
	
	

		for(j=0;j<CMPLT_ROW_COUNT;j++)
		{
			
			complmtUIdev->icon[j].x = bmp_sart_offx  ;
			complmtUIdev->icon[j].y = bmp_sart_offy ;
			complmtUIdev->icon[j].width = PICTURE_X_SIZE;
			complmtUIdev->icon[j].height = PICTURE_Y_SIZE;
			complmtUIdev->icon[j].path = (u8*)complmtui_icon_path_tbl[LANGUAGE][j];

		}
		//初始化返回控件
		complmtUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx;
		complmtUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
		complmtUIdev->icon[j].width = PICTURE_X_SIZE;
		complmtUIdev->icon[j].height = PICTURE_Y_SIZE;
		complmtUIdev->icon[j].path = (u8*)complmtui_icon_path_tbl[LANGUAGE][j];

	
	
	complmtUIdev->totalIcon = CMPLT_BMP_TOTAL_NUMS;
	
		for(j=0; j < complmtUIdev->totalIcon; j++)
		{
			if(j == IS_ON_CMPLT_ICON) continue;
			display_bmp_bin_to_LCD((u8*)complmtUIdev->icon[j].path, complmtUIdev->icon[j].x , complmtUIdev->icon[j].y, complmtUIdev->icon[j].width, complmtUIdev->icon[j].height, complmtUIdev->icon[j].color);
		}	

}
void complmt_screen(void)
{
	_complmtUI_selected_button_e CMPLT_SELECTED_BUTTON = NONE_CMPLT_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  static u8 swIndex = 0;
	  u8 k;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		complmtui_init();
	}

	CMPLT_SELECTED_BUTTON = (_complmtUI_selected_button_e)gui_touch_chk(complmtUIdev);
	
	switch ( CMPLT_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_OFF_CMPLT_ICON:
			CMPLT_SAVED_BUTTON = IS_OFF_CMPLT_ICON;
			swIndex ++;
			if( swIndex > 2)	swIndex = 1;
			
			vCallBack_Toggle_Led();
				CMPLT_DEBUG("on/off exLed\r\n");
				
			break;
			

/****************************主页的第二排图标****************************/			

		case IS_CMPLT_BACK_ICON:		
		currentMenu = more_screen;
		free_flag = IS_TRUE;
			CMPLT_DEBUG("complmt back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
if(CMPLT_SAVED_BUTTON != NONE_CMPLT_ICON)
{
	if( swIndex == 1)	k = IS_ON_CMPLT_ICON;
	else if(swIndex == 2)	k = IS_OFF_CMPLT_ICON;
	gui_fill_rectangle_DMA(complmtUIdev->icon[k].x, complmtUIdev->icon[k].y, complmtUIdev->icon[k].width, complmtUIdev->icon[k].height,BLACK);
	display_bmp_bin_to_LCD((u8*)complmtUIdev->icon[k].path, complmtUIdev->icon[k].x , complmtUIdev->icon[k].y, complmtUIdev->icon[k].width, complmtUIdev->icon[k].height, complmtUIdev->icon[k].color);
	CMPLT_SAVED_BUTTON = NONE_CMPLT_ICON;
	
}	
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,complmtUIdev->icon);
		myfree(SRAMIN,complmtUIdev);
		CMPLT_DEBUG("free complmtUIdev \r\n");
		swIndex = 0;
	}

}

static void vCallBack_Toggle_Led(void)
{
	LEDx1 = !LEDx1;
}
