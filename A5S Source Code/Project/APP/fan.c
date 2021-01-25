/*
*********************************************************************************************************
*
*	模块名称 :预热界面模块
*	文件名称 : preheat.c
*	版    本 : V1.0
*	说    明 : 预热界面
*
*
*********************************************************************************************************
*/


/**********************包含头文件*************************/
#include "fan.h"





#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

/**********************调试宏定义**************************/

#define FANUI_DEBUG_EN

#ifdef FANUI_DEBUG_EN

#define FANUI_DEBUG	DBG

#else
#define FANUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/




/***********************私有宏*************************/






/************************************************/
#define FAN_ICON_NUM	4
#define FAN_FIRST_ROW_ICON_NUM 		2
#define FAN_SECOND_ROW_ICON_NUM 	2


/**************控件触摸检测ID定义*****************/
#define IS_FAN_ADD_BUTTON		0
#define IS_FAN_DEC_BUTTON		1
#define IS_FAN_CLEAR_SET_BUTTON	2
#define IS_FAN_BACK_BUTTON		3




//主界面 控制器
_gui_dev *fanUIdev;

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
char *const fanUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/风扇",
	"Prepare/set/fan"
};

char *const fanUI_dispInfor [GUI_LANGUAGE_NUM]= 
{
	"风速率",
	"fan speed"
};



/************************************************/


//icon的路径表,HOME界面图标
char *const fanUI_icon_path_tbl[GUI_LANGUAGE_NUM][FAN_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",	  
	

	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    
    },
	{
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",	  
	

	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	 		
	}
};  


static void fanUI_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint16_t x,y;
	
	x = 2;
	y = 5;
	

LCD_Clear_DMA(BLACK);

		if(isOption2moreUiSelected == YES)
			Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreFanUI_title[LANGUAGE],font,1);	
		else if(isOption2moreUiSelected == NO)
			Show_Str(x,y,lcddev.width,font,(u8 *)fanUI_titile[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	fanUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( fanUIdev == 0)
	{
		FANUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	fanUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*FAN_ICON_NUM);
	
	if( fanUIdev->icon == 0)
	{
		FANUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	fanUIdev->status = 0x3F ;
	fanUIdev->totalIcon = FAN_ICON_NUM;//控件检测数目
	// 0 1
	for(j=0;j<FAN_FIRST_ROW_ICON_NUM;j++)
	{
			fanUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			fanUIdev->icon[j].y = bmp_sart_offy ;
			fanUIdev->icon[j].width = PICTURE_X_SIZE;
			fanUIdev->icon[j].height = PICTURE_Y_SIZE;
			fanUIdev->icon[j].path = (u8*)fanUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3   
		for(j=FAN_FIRST_ROW_ICON_NUM;j<FAN_ICON_NUM;j++)
		{
			if(j == FAN_ICON_NUM - 1)
			fanUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
			
			else 
			fanUIdev->icon[j].x = bmp_sart_offx ;
			
			fanUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			fanUIdev->icon[j].width = PICTURE_X_SIZE;
			fanUIdev->icon[j].height = PICTURE_Y_SIZE;
			fanUIdev->icon[j].path = (u8*)fanUI_icon_path_tbl[LANGUAGE][j];

		}
		
		fanUIdev->totalIcon = FAN_ICON_NUM;
		
		
		for(j=0;j<fanUIdev->totalIcon;j++)//不显示 切换显示的图标
		{

			display_bmp_bin_to_LCD((u8*)fanUIdev->icon[j].path, fanUIdev->icon[j].x , fanUIdev->icon[j].y, fanUIdev->icon[j].width, fanUIdev->icon[j].height, fanUIdev->icon[j].color);
		}

}




void fan_screen(void)
{

	u8 SELECTED_BUTTON = 0xff;
	u8 savedButton = 0xff;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  _obj_option_e  OPT_TYPE = NONE_OPT;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		fanUI_init();
	}

	SELECTED_BUTTON = gui_touch_chk(fanUIdev);
	
	switch ( SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_FAN_ADD_BUTTON://加
		OPT_TYPE  = ADD_OPT;
		savedButton = IS_FAN_ADD_BUTTON;

				FANUI_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_FAN_DEC_BUTTON://减
		OPT_TYPE  = DEC_OPT;
		savedButton = IS_FAN_DEC_BUTTON;
		

				FANUI_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
			
		case IS_FAN_CLEAR_SET_BUTTON://清零设置值
		OPT_TYPE  = CLEAR_OPT;
		savedButton = IS_FAN_CLEAR_SET_BUTTON;
				FANUI_DEBUG("clear screen\r\n");
				
			break;
			
		case IS_FAN_BACK_BUTTON:
		if(isOption2moreUiSelected == YES)
		currentMenu = gcode_print_option_to_more_screen;
		
		else if(isOption2moreUiSelected == NO)
		currentMenu = set_screen;
		
		free_flag = IS_TRUE;
				FANUI_DEBUG("back_screen\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
	
		if(savedButton != 0XFF)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
		
				gui_fill_rectangle_DMA(fanUIdev->icon[savedButton].x, fanUIdev->icon[savedButton].y, fanUIdev->icon[savedButton].width, fanUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)fanUIdev->icon[savedButton].path, fanUIdev->icon[savedButton].x , fanUIdev->icon[savedButton].y, fanUIdev->icon[savedButton].width, fanUIdev->icon[savedButton].height, fanUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
display_fan_speed(OPT_TYPE);
	if(isOption2moreUiSelected == YES)//确保打印界面先按下暂停后再进入改页面才检测打印头靠边停止
	{
		check_is_pause_exacute();//检查打印头是否执行靠边停止

	}


	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,fanUIdev->icon);
		myfree(SRAMIN,fanUIdev);
		FANUI_DEBUG("free fanUIdev \r\n");
	}


}

void set_fan_speed(_obj_option_e opt)
{
//风扇速度设置
		switch (opt)
		{
			case ADD_OPT:
				fanSpeed += 10;//10% setp
				if(fanSpeed >= 256) fanSpeed = 255;//100%
				break;
				
			case DEC_OPT:
			fanSpeed -= 10;
			if(fanSpeed <= 0 ) fanSpeed = 0;//0%
				break;
				
			case CLEAR_OPT:
			fanSpeed = 0;//0%
				break;				
				
			default:
				break;
		}


}

void display_fan_speed(_obj_option_e opt)
{


	u16 xOffSet;
	u16 yOffSet;
	u16 width;
	u16 hight;
	u16 font = 24;

	u16 speed;
	
	hight = font;
	set_fan_speed(opt);
	if( millis() > display_next_update_millis)//250ms 更新一次
	{
		display_next_update_millis = millis() + 250;
		width = 2*PICTURE_X_SIZE;
		 BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		
		speed = fanSpeed;//获取风扇速度
		
		yOffSet = BMP_FONT_SIZE + font;
		xOffSet = PICTURE_X_SIZE + 2*font;
		
		
		 Show_Str(xOffSet,yOffSet,width,hight,(u8 *)fanUI_dispInfor[LANGUAGE],font,1);//显示"fan speed"
		 

		POINT_COLOR = YELLOW;
		 width = 4*(font / 2);
		 yOffSet += 2*font;
		 gui_fill_rectangle_DMA(xOffSet,yOffSet + font ,width,hight,BACK_COLOR);//清区域色块
		 
		gui_show_num(xOffSet , yOffSet + font, 3, POINT_COLOR, font, speed, 1); //显示speed
		
		POINT_COLOR = WHITE;
	//	LCD_ShowString(xOffSet  + 3 * (font / 2), yOffSet + font, width, hight, font, (u8*)"%"); //
		
		
		
		
		
		
	}
	
}




