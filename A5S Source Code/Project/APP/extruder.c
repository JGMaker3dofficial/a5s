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
#include "extruder.h"


#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"
/**********************调试宏定义**************************/

#define EXTRUDERUI_DEBUG_EN

#ifdef EXTRUDERUI_DEBUG_EN

#define EXTRUDERUI_DEBUG	DBG

#else
#define EXTRUDERUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define	FILA_IN_OUT_TEMP		185
#define	SET_FILA_IN_OUT_TEMP	(FILA_IN_OUT_TEMP + 2)

/***********************私有宏*************************/



/************************************************/
#define EXTRUDER_FILA_STEP_ICON 		2
#define EXTRUDER_FILA_SPEED_ICON			2
#define EXTRUDER_MENU_TOTAL_BMP_NUM			10
#define EXTRUDER_MENU_VALID_BMP_NUM  	(EXTRUDER_MENU_TOTAL_BMP_NUM - EXTRUDER_FILA_STEP_ICON - EXTRUDER_FILA_SPEED_ICON)

#define EXTRUDER_MENU_FIRST_ROW_BMP_NUM			2
#define EXTRUDER_MENU_SECOND_ROW_BMP_NUM		4


//主界面 控制器
_gui_dev *extruderUIdev;

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
static uint8_t  filament_step_count = 0;//进退料步进计数器
static uint8_t  filament_speed_count = 0;//进退料速度步进计数器

static int32_t	filament_length_count = 0;//进料长度计数器

typedef struct  
{
	uint8_t	isHeatOK;
	uint8_t	setHeatCMD;
	uint8_t	dispFlag;
}_heat_ext0_s;

_heat_ext0_s	heatSt;
/************************************************/
typedef enum 
{
	NONE_EXTRUDER_ICON = 0XFF,
	IS_EXTRUDER_IN_ICON = 0,
	IS_EXTRUDER_OUT_ICON = 1,
	
	IS_EXTRUDER_CLEAR_ICON = 2,
	IS_EXTRUDER_FILA_STEP1MM_ICON = 3,
	IS_EXTRUDER_FILA_SPEED_SLOW_ICON = 4,
	IS_EXTRUDER_BACK_ICON = 5,
	
	IS_EXTRUDER_FILA_STEP5MM_ICON = 6,
	IS_EXTRUDER_FILA_STEP10MM_ICON = 7,
	
	IS_EXTRUDER_FILA_SPEED_NORMAL_ICON = 8,
	IS_EXTRUDER_FILA_SPEED_FAST_ICON = 9,
	
}_extruderUI_selected_button_e;
_extruderUI_selected_button_e EXTRU_SAVED_BUTTON = NONE_EXTRUDER_ICON;


//icon的路径表,HOME界面图标
char *const extruUI_icon_path_tbl[GUI_LANGUAGE_NUM][EXTRUDER_MENU_TOTAL_BMP_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",//0
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",//1	  
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",//2	    
	"1:/SYSTEM/PICTURE/ch_bmp_step1_mm.bin",//3	    
	"1:/SYSTEM/PICTURE/ch_bmp_speed_slow.bin",//4	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//5    
   
   "1:/SYSTEM/PICTURE/ch_bmp_step5_mm.bin",//进退料步进 6
   "1:/SYSTEM/PICTURE/ch_bmp_step10_mm.bin",//进退料步进 7
   
   "1:/SYSTEM/PICTURE/ch_bmp_speed_normal.bin",//进退料速度步进 8
   "1:/SYSTEM/PICTURE/ch_bmp_speed_high.bin",//进退料速度步进 9
   },
   {
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",//0
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",//1	  
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",//2	    
	"1:/SYSTEM/PICTURE/en_bmp_step1_mm.bin",//3	    
	"1:/SYSTEM/PICTURE/en_bmp_speed_slow.bin",//4	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//5    
   
   "1:/SYSTEM/PICTURE/en_bmp_step5_mm.bin",//进退料步进 6
   "1:/SYSTEM/PICTURE/en_bmp_step10_mm.bin",//进退料步进 7
   
   "1:/SYSTEM/PICTURE/en_bmp_speed_normal.bin",//进退料速度步进 8
   "1:/SYSTEM/PICTURE/en_bmp_speed_high.bin",//进退料速度步进 9   
   }
};  

char *const EXTRUUI_temp_extu1[GUI_LANGUAGE_NUM] = 
{
	"挤出",
	"Extru:"
};

char *const	EXTRUUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/进退料",
	"Prepare/filament"
};
char *const tempInfor[GUI_LANGUAGE_NUM] = 
{
	"温度:",
	"Temp:"
};

char *const	filaHeat[GUI_LANGUAGE_NUM] = 
{
	"加热中...",
	"heating..."
};

typedef enum 
{
	NONE_FILAMENT_OPT = 0xFF,
	FILAMENT_IN_OPT = 0,
	FILAMENT_OUT_OPT = 1,
	FILAMENT_CLEAR_OPT = 2,
}_filament_option_e;

typedef enum 
{
   NONE_FILAMENT_DISTN = 0XFF,
   FILAMENT_DISTN_STEP1MM = 0,
   FILAMENT_DISTN_STEP5MM = 1,
   FILAMENT_DISTN_STEP10MM = 2,
}_filament_distn_step_e;






_filament_option_e FILAMENT_OPT_E = NONE_FILAMENT_OPT;
_filament_distn_step_e FILAMENT_DISTN_E = FILAMENT_DISTN_STEP1MM;
_filament_speed_step_e FILAMENT_SPEED_E = FILAMENT_SPEED_SLOW;
_lenght_unit_e LENGTH_UINT_E = UNIT_MM;//进退料长度单位

//显示		EXTU1             
//		   0/20				
//			Temp:175C
static void extruUI_display_text(int32_t filamentLength,float Extru1CurTemp,_lenght_unit_e unit)
{


	const char unit_mm[] = "mm";
	const char unit_cm[] = "cm";
	const char unit_temp[] = "'C";

	u16 xOffSet;
	u16 yOffSet;
	u16 width;
	u16 hight;
	u16 font = 16;
	int32_t setLength ;
	u32 savedLenght;
	u32 currentTemp = Extru1CurTemp;
	yOffSet = BMP_FONT_SIZE + PICTURE_Y_SIZE/3 - font;


	if( millis() > display_next_update_millis)//500ms 更新一次
	{
		display_next_update_millis = millis() + 250;

 
		POINT_COLOR = BLUE;
		BACK_COLOR = BLACK;
				
			xOffSet = 2*PICTURE_X_SIZE - 6*font/4;
			
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)EXTRUUI_temp_extu1[LANGUAGE],font,1);		

			width = 4*(font/2);
			hight = font;
		gui_fill_rectangle_DMA(xOffSet + 6*(font/2),yOffSet,width,hight,BLACK);//清区域色块
		
		POINT_COLOR = YELLOW;
//显示单位
		if(unit == UNIT_MM)
		{
			setLength = filamentLength;
			if(setLength < 0)//
			{
				savedLenght = -1*setLength;
				LCD_ShowString(xOffSet + 6*(font/2),yOffSet,lcddev.width,font,font,(u8 *)"-");	//display "-"
				LCD_ShowNum(xOffSet + 6*(font/2) + (font/2),yOffSet,savedLenght,3,font);//显示进料长度
				LCD_ShowString(xOffSet + 6*(font/2) + 4*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_mm);		
			}
			else
			{
				LCD_ShowNum(xOffSet + 6*(font/2),yOffSet,setLength,3,font);//显示进料长度	
				LCD_ShowString(xOffSet + 6*(font/2) + 3*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_mm);					
			}


		}
		else if(unit == UNIT_CM)
		{
			setLength = filamentLength;
			if(setLength < 0)//
			{
				savedLenght = -1*setLength;
				LCD_ShowString(xOffSet + 6*(font/2),yOffSet,lcddev.width,font,font,(u8 *)"-");	//display "-"
				LCD_ShowNum(xOffSet + 6*(font/2) + (font/2),yOffSet,savedLenght,3,font);//显示进料长度
			  LCD_ShowString(xOffSet + 6*(font/2) + 4*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_cm);	

			}
			else
			{
			LCD_ShowNum(xOffSet + 6*(font/2),yOffSet,setLength,3,font);//显示进料长度
			 LCD_ShowString(xOffSet + 6*(font/2) + 3*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_cm);	
			
			}
		}
		POINT_COLOR = BLUE;
		
		
		xOffSet = 2*PICTURE_X_SIZE - 6*font/4;
		yOffSet  += 2*font;
		hight = font;
		width = strlen(filaHeat[LANGUAGE]);
		if(current_temperature[0] < FILA_IN_OUT_TEMP)
		{
			if(heatSt.setHeatCMD == TRUE && heatSt.dispFlag == FALSE)//已发送加热命令
			{
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width*(font/2),hight,BLACK);//清区域色块
			POINT_COLOR = RED;
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)filaHeat[LANGUAGE],font,1);//提示加热
			EXTRUDERUI_DEBUG("heat\r\n");			
			heatSt.dispFlag = TRUE;
			}
		}
		else if(heatSt.dispFlag == TRUE && current_temperature[0] >= FILA_IN_OUT_TEMP)
		{
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width*(font/2),hight,BLACK);//清区域色块
			clear_heatSt();
			EXTRUDERUI_DEBUG("heat OK\r\n");		
		}
		
		yOffSet  += 2*font;
		Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)tempInfor[LANGUAGE],font,1);//显示Temp:
		
		POINT_COLOR = YELLOW;
		width = 3*(font/2);
		gui_fill_rectangle_DMA(xOffSet + 5*(font/2),yOffSet,width,hight,BLACK);//清区域色块
		LCD_ShowNum(xOffSet + 5*(font/2),yOffSet,currentTemp,3,font);//显示当前挤出电机温度值
		LCD_ShowString(xOffSet + 5*(font/2) + 3*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_temp);//显示'C

		POINT_COLOR = BLUE;
	}
	
}

//显示进退料总位移
static void extruUI_set_filament(_filament_option_e filamentOpt, _filament_distn_step_e distnStep,_filament_speed_step_e speedStep)
{
	int32_t saved_distance;
	float  extu1Temp = current_temperature[0];
	
	switch (filamentOpt)//操作对象
	{
		case FILAMENT_IN_OPT:
		switch (distnStep)//距离步进
			{
				case FILAMENT_DISTN_STEP1MM:
					filament_length_count++;

					break;
					
				case FILAMENT_DISTN_STEP5MM:

					filament_length_count += 5;					
					break;
					
				case FILAMENT_DISTN_STEP10MM:
				
					filament_length_count += 10;						
					break;				
					
				default:
					break;
			}
		if(filament_length_count >= 99900)//超过9m
		filament_length_count = 99900;

		

			break;
			
		case FILAMENT_OUT_OPT:
		switch (distnStep)//操作步进
			{
				case FILAMENT_DISTN_STEP1MM:
				filament_length_count--;
					break;
					
				case FILAMENT_DISTN_STEP5MM:
				filament_length_count -= 5;
					break;
					
				case FILAMENT_DISTN_STEP10MM:
				filament_length_count -= 10;
					break;				
					
				default:
					break;
			}
		if(filament_length_count <= -9900)//超过99cm
		filament_length_count = -9900;	
		

			break;
			
	case FILAMENT_CLEAR_OPT:
	filament_length_count = 0;

	break;
	
		default:
			break;
	}
	


	
	if(filament_length_count >= 1000 || filament_length_count <= -1000)//超过1000mm->单位转换mm->cm
		{
		saved_distance = filament_length_count/10;
		LENGTH_UINT_E = UNIT_CM;
		}
	else//MM display
		{
		saved_distance = filament_length_count;
		LENGTH_UINT_E = UNIT_MM;	
		}
	extruUI_display_text(saved_distance,extu1Temp,LENGTH_UINT_E);

}



static void extruUI_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint16_t xoff = 2;
	uint16_t yoff = 5;
	
LCD_Clear_DMA(BLACK);
Show_Str(xoff,yoff,lcddev.width,font,(u8 *)EXTRUUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	extruderUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( extruderUIdev == 0)
	{
		EXTRUDERUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	extruderUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*EXTRUDER_MENU_TOTAL_BMP_NUM);
	
	if( extruderUIdev->icon == 0)
	{
		EXTRUDERUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	extruderUIdev->status = 0x3F ;
	extruderUIdev->totalIcon = EXTRUDER_MENU_VALID_BMP_NUM;//控件检测数目
	// 0 1
	for(j=0;j < EXTRUDER_MENU_FIRST_ROW_BMP_NUM; j++)
	{
			extruderUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			extruderUIdev->icon[j].y = bmp_sart_offy ;
			extruderUIdev->icon[j].width = PICTURE_X_SIZE;
			extruderUIdev->icon[j].height = PICTURE_Y_SIZE;
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3 4 5  
		for(j=EXTRUDER_MENU_FIRST_ROW_BMP_NUM; j < extruderUIdev->totalIcon; j++)
		{
			
			extruderUIdev->icon[j].x = bmp_sart_offx + (j-2)*bmp_between_offx  ;
			extruderUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			extruderUIdev->icon[j].width = PICTURE_X_SIZE;
			extruderUIdev->icon[j].height = PICTURE_Y_SIZE;
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];

		}
	//初始化step5 、step10 icons	
	for(j=IS_EXTRUDER_FILA_STEP5MM_ICON; j < IS_EXTRUDER_FILA_STEP5MM_ICON + EXTRUDER_FILA_STEP_ICON; j++)
	{
			extruderUIdev->icon[j].x = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].x;
			extruderUIdev->icon[j].y = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].y;
			extruderUIdev->icon[j].width = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].width;
			extruderUIdev->icon[j].height = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].height;
			
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];
			
	}
	
	//初始化speed_nor 、speed_fast icons	
	for(j=IS_EXTRUDER_FILA_SPEED_NORMAL_ICON; j < IS_EXTRUDER_FILA_SPEED_NORMAL_ICON + EXTRUDER_FILA_SPEED_ICON; j++)
	{
			extruderUIdev->icon[j].x = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].x;
			extruderUIdev->icon[j].y = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].y;
			extruderUIdev->icon[j].width = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].width;
			extruderUIdev->icon[j].height = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].height;
			
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];
			
	}		
		
		
		for(j=0;j<extruderUIdev->totalIcon;j++)//不显示 切换显示的图标
		{
			if(j == IS_EXTRUDER_FILA_STEP1MM_ICON)//进退料步进距离
			{
				if(filament_step_count == 1)//display step1mm
				{
					display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].color);
				}
				
				else if(filament_step_count == 2)//display step5mm
				{
					display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].color);
				}
				else if(filament_step_count == 0)//display step10mm
				{
					display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[j].path, extruderUIdev->icon[j].x , extruderUIdev->icon[j].y, extruderUIdev->icon[j].width, extruderUIdev->icon[j].height, extruderUIdev->icon[j].color);					
				}

				
			}
			else if(j == IS_EXTRUDER_FILA_SPEED_SLOW_ICON)//进退料步进速度
			{
				if(filament_speed_count == 1)//display speed normal
				{
					display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].color);

				}
				
				else if(filament_speed_count == 2)//display speed fast
				{
					display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].color);

				}
				else if(filament_speed_count == 0)//display speed slow
				{
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[j].path, extruderUIdev->icon[j].x , extruderUIdev->icon[j].y, extruderUIdev->icon[j].width, extruderUIdev->icon[j].height, extruderUIdev->icon[j].color);
				}					
			}
			else
			display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[j].path, extruderUIdev->icon[j].x , extruderUIdev->icon[j].y, extruderUIdev->icon[j].width, extruderUIdev->icon[j].height, extruderUIdev->icon[j].color);
		}

}




/*
*********************************************************************************************************
*	函 数 名: extru_screen
*	功能说明: 预热设置界面
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void extru_screen(void)
{

_extruderUI_selected_button_e EXTRU_SELECTED_BUTTON = NONE_EXTRUDER_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  u8 isTrigedCmd = 0;
	  u8 saveButton = 0xff;
	  char TempBuffer[11];
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		extruUI_init();
	}

	EXTRU_SELECTED_BUTTON = (_extruderUI_selected_button_e)gui_touch_chk(extruderUIdev);
	
	switch ( EXTRU_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_EXTRUDER_IN_ICON://进料
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_IN_ICON;
		saveButton = FILAMENT_OPT_E = FILAMENT_IN_OPT;
		isTrigedCmd = 1;
				EXTRUDERUI_DEBUG("filament in\r\n");
				
			break;
			
		case IS_EXTRUDER_OUT_ICON://退料
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_OUT_ICON;
		saveButton = FILAMENT_OPT_E =FILAMENT_OUT_OPT;	
		isTrigedCmd = 1;
		EXTRUDERUI_DEBUG("filament out\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
		case IS_EXTRUDER_CLEAR_ICON://进退料清零
		
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_CLEAR_ICON;
		FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;	
				EXTRUDERUI_DEBUG("extruder screen\r\n");
				
			break;			
			
		case IS_EXTRUDER_FILA_STEP1MM_ICON://进退料距离步进
				filament_step_count++;
				switch (filament_step_count)
				{
					case 1://step5mm
					FILAMENT_DISTN_E = FILAMENT_DISTN_STEP5MM;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].color);
					
						break;
						
					case 2://step10mm
					FILAMENT_DISTN_E = FILAMENT_DISTN_STEP10MM;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP5MM_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].color);
					
						break;
						
					case 3://step1mm
					FILAMENT_DISTN_E = FILAMENT_DISTN_STEP1MM;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP10MM_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].color);
					filament_step_count = 0;
						break;			
						
					default:
						break;
				}
				
				
				EXTRUDERUI_DEBUG("filament step++\r\n");
				
			break;
			

		case IS_EXTRUDER_FILA_SPEED_SLOW_ICON://进退料速度步进
		filament_speed_count++;
			switch (filament_speed_count)
			{
				case 1:
				FILAMENT_SPEED_E = FILAMENT_SPEED_NORMAL;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].color);
				
					break;
					
				case 2:
				FILAMENT_SPEED_E = FILAMENT_SPEED_FAST;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_NORMAL_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].color);
				
					break;
					
				case 3:
				filament_speed_count = 0;
				FILAMENT_SPEED_E = FILAMENT_SPEED_SLOW;
				gui_fill_rectangle_DMA(extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].x, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_FAST_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].path, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].x , extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].y, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].width, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].height, extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].color);
				
					break;					
					
				default:
					break;
			}
							
				EXTRUDERUI_DEBUG("filament speed++\r\n");
				
			break;
			
		case IS_EXTRUDER_BACK_ICON:
		if(isOption2moreUiSelected == YES)currentMenu = gcode_print_option_to_more_screen;

		else if(isOption2moreUiSelected == NO)currentMenu = main_screen;
		
		
		FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;	//进退料界面退出后，清零之前进料长度
		free_flag = IS_TRUE;
				EXTRUDERUI_DEBUG("extru back\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
		if(EXTRU_SAVED_BUTTON == IS_EXTRUDER_IN_ICON || EXTRU_SAVED_BUTTON == IS_EXTRUDER_OUT_ICON)
		{
			if(current_temperature[0] < FILA_IN_OUT_TEMP )//温度低于设定温度
			{
				if( heatSt.setHeatCMD == FALSE)
				{
				heatSt.setHeatCMD = TRUE;
				sprintf(TempBuffer, "M104 S%d", SET_FILA_IN_OUT_TEMP);
				menu_action_gcode(TempBuffer);
				}
				FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;//当前设置加减操作无效
			}
		}
		
		if(EXTRU_SAVED_BUTTON != NONE_EXTRUDER_ICON)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
			gui_fill_rectangle_DMA(extruderUIdev->icon[EXTRU_SAVED_BUTTON].x, extruderUIdev->icon[EXTRU_SAVED_BUTTON].y, extruderUIdev->icon[EXTRU_SAVED_BUTTON].width, extruderUIdev->icon[EXTRU_SAVED_BUTTON].height,BLACK);				
			display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[EXTRU_SAVED_BUTTON].path, extruderUIdev->icon[EXTRU_SAVED_BUTTON].x , extruderUIdev->icon[EXTRU_SAVED_BUTTON].y, extruderUIdev->icon[EXTRU_SAVED_BUTTON].width, extruderUIdev->icon[EXTRU_SAVED_BUTTON].height, extruderUIdev->icon[EXTRU_SAVED_BUTTON].color);
			EXTRU_SAVED_BUTTON = NONE_EXTRUDER_ICON;
		}



	extruUI_set_filament(FILAMENT_OPT_E,FILAMENT_DISTN_E,FILAMENT_SPEED_E);//显示设置的进退料长度
	if(isTrigedCmd ==1)
	{
		isTrigedCmd = 0;
		exacute_manualFilament(saveButton,FILAMENT_SPEED_E,FILAMENT_DISTN_E);//发送gcode命令,发送命令在前，FILAMENT_OPT_E变量在extruUI_set_filament中被重置
	}

	FILAMENT_OPT_E = NONE_FILAMENT_OPT;//清零进退料设置,防止显示界面进退料位移一直加/减



	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,extruderUIdev->icon);
		myfree(SRAMIN,extruderUIdev);
		EXTRUDERUI_DEBUG("free extruderUIdev \r\n");
		clear_heatSt();
	}


}

//进退料
void exacute_manualFilament(u8 dirId, u8 speedID, u8 lengthID)
{



	u8 filaLength = 0;
	char data[14];
//判断挤出头是否加热到190度
	if(current_temperature[0] >= FILA_IN_OUT_TEMP )
	{		
		bsp_DelayMS(2);
		if(current_temperature[0] >= FILA_IN_OUT_TEMP )
		{

		if(lengthID == FILAMENT_DISTN_STEP5MM)  filaLength = 5;
		else if(lengthID == FILAMENT_DISTN_STEP1MM)  filaLength = 1;
		else if(lengthID == FILAMENT_DISTN_STEP10MM)  filaLength = 10;
		

		
		switch (speedID)
		{
			case FILAMENT_SPEED_SLOW:
				if(dirId == FILAMENT_IN_OPT)	sprintf(data, "G1 E%d %s",filaLength,(char *)FILAMENT_SLOW_SPEED_CMD);
				else if(dirId == FILAMENT_OUT_OPT)	sprintf(data, "G1 E-%d %s",filaLength,(char *)FILAMENT_SLOW_SPEED_CMD);
				break;
				
			case FILAMENT_SPEED_NORMAL:
				if(dirId == FILAMENT_IN_OPT)	sprintf(data, "G1 E%d %s",filaLength,(char *)FILAMENT_NORM_SPEED_CMD);
				else if(dirId == FILAMENT_OUT_OPT)	sprintf(data, "G1 E-%d %s",filaLength,(char *)FILAMENT_NORM_SPEED_CMD);
			
				break;
				
			case FILAMENT_SPEED_FAST:
				if(dirId == FILAMENT_IN_OPT)	sprintf(data, "G1 E%d %s",filaLength,(char *)FILAMENT_FAST_SPEED_CMD);
				else if(dirId == FILAMENT_OUT_OPT)	sprintf(data, "G1 E-%d %s",filaLength,(char *)FILAMENT_FAST_SPEED_CMD);
			
				break;
			
			default:
				break;
		}
		
		EXTRUDERUI_DEBUG("->%s\r\n",data);		
		enable_e0();
		menu_action_gcode(USING_RELATIVE_POSITION);
		menu_action_gcode(data);
		menu_action_gcode(USING_ABSOLUTE_POSITION);	
	
	
		}
		else
		{
			EXTRUDERUI_DEBUG("extru1 temperature is too low,heating... \r\n");			
		}
	
	}
	
	else
	{
		EXTRUDERUI_DEBUG("extru1 temperature is too low,heating... \r\n");			
	}



}


void clear_heatSt(void)
{
	heatSt.dispFlag = FALSE;
	heatSt.isHeatOK = FALSE;
	heatSt.setHeatCMD = FALSE;
}



