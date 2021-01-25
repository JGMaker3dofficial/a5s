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

#include "preheat.h"

#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

/**********************调试宏定义**************************/

#define PREHEATUI_DEBUG_EN

#ifdef PREHEATUI_DEBUG_EN

#define PREHEATUI_DEBUG	DBG

#else
#define PREHEATUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/




/***********************私有宏*************************/
#define PREHEAT_SET_BG_COLOR	GRAY//设置界面背景颜色
#define PREHEAT_SET_FONT_COLOR	BLACK//设置按钮字体颜色


#define PREHEAT_MIN_BUTTON_ID		8
#define PREHEAT_MAX_BUTTON_ID		9

#define SET_MIN_ID		0XAA
#define SET_MAX_ID		0XC5

/************************************************/
#define EXTI_NUM			3
#define PREHEAT_MENU_TOTAL_BMP_NUM			9
#define PREHEAT_MENU_VALID_BMP_NUM  	(PREHEAT_MENU_TOTAL_BMP_NUM - EXTI_NUM)
#define PREHEAT_MENU_FIRST_ROW_BMP_NUM		2
#define PREHEAT_MENU_SECOND_ROW_BMP_NUM		4


/*****************************************/
#define TempSense_HIGHT_VALUE	300
#define TempSense_LOW_VALUE		5

#define SET_EXT0_TEMP_VALUE		190
#define SET_BED_TEMP_VALUE		45

#define CLC_EXT0_TEMP_VALUE		0
#define CLC_BED_TEMP_VALUE		0


#define DISP_EX0_BIT		(1<<0)
#define DISP_BED_BIT		(1<<1)
#define	DISP_FLAG_SET		(DISP_EX0_BIT | DISP_BED_BIT)
#define PRESET_TEMP		30

#define	USER_SET_EX0_TEMP_MAX	(HEATER_0_MAXTEMP - 5)
#define	USER_SET_BED_TEMP_MAX	(BED_MAXTEMP - 5)



//主界面 控制器
_gui_dev *preheatUIdev;

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
static uint8_t  heat_step_count = 0;//加热阶梯计数器
static uint8_t  extruder_bed_count = 0;//切换挤出头/热床计数器

static uint8_t firstDispFlag = FALSE;//进入预热界面，还未触发加减操作时，显示0温度
static u16 dispCurTempValue;//每隔2s显示一次当前温度时的缓存


/************************************************/
typedef enum 
{
	NONE_PREHEAT_ICON = 0XFF,
	IS_PREHEAT_ADD_ICON = 0,
	IS_PREHEAT_DEC_ICON,
	IS_PREHEAT_EXTRUDER1_ICON,
	IS_PREHEAT_HEAT_STEP_ICON,
	IS_PREHEAT_CLEAR_ICON,
	IS_PREHEAT_BACK_ICON,
	
	IS_PREHEAT_BED_ICON,
	IS_PREHEAT_HEAT_STEP5_ICON,
	IS_PREHEAT_HEAT_STEP10_ICON
	
}_preheatUI_selected_button_e;
_preheatUI_selected_button_e PREHEAT_SAVED_BUTTON = NONE_PREHEAT_ICON;


//icon的路径表,HOME界面图标
char *const preheatUI_icon_path_tbl[GUI_LANGUAGE_NUM][PREHEAT_MENU_TOTAL_BMP_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",	  
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_step1_degree.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    
   
   "1:/SYSTEM/PICTURE/ch_bmp_bed.bin",//挤出头与热床切换,6
   "1:/SYSTEM/PICTURE/ch_bmp_step5_degree.bin",//加热档切换,7
   "1:/SYSTEM/PICTURE/ch_bmp_step10_degree.bin",//加热档切换,8
   },
   {
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",	  
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_step1_degree.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	    
   
   "1:/SYSTEM/PICTURE/en_bmp_bed.bin",//挤出头与热床切换,6
   "1:/SYSTEM/PICTURE/en_bmp_step5_degree.bin",//加热档切换,7
   "1:/SYSTEM/PICTURE/en_bmp_step10_degree.bin",//加热档切换,8   
   }
};  

char *const infor_temp_bed[GUI_LANGUAGE_NUM] = 
{
	"热床:",
	"BED:"
};

char *const infor_temp_extu1[GUI_LANGUAGE_NUM] = 
{
	"挤出头1:",
	"EXTU1:"
};
char *const preheatUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/预热",
	"Prepare/preheat"
};

char *const infor_temp_error[GUI_LANGUAGE_NUM] = 
{
	"热敏电阻异常",
	"Sensor Error"
};


typedef enum 
{
   NONE_OBJ_ID = 0XFF,
   EXTRUDER1_OBJ_ID = 1,
   BED_OBJ_ID,
}_obj_updt_id_e;

typedef enum 
{
	NONE_TEMP_STEP = 0XFF,
	TEMP_STEP1C = 1,
	TEMP_STEP5C,
	TEMP_STEP10C,
}_temp_updt_step_e;


_obj_option_e OPT_INFOR_E = NONE_OPT;//加减操作
_obj_updt_id_e UPDT_INFOR_E = EXTRUDER1_OBJ_ID;//操作对象
_temp_updt_step_e TEMP_UPDT_STEP_E = TEMP_STEP1C;//操作步进

static void Send_Restart_Move_Cmd(void);


//显示		EXTU1             BED     
//		   0/20				0/50
//
static void preheatUI_display_Temp(_obj_updt_id_e obj_id,float bedCurTemp,float Extru1CurTemp)
{

	u8 curTempError = 0;
	u16 xOffSet;
	u16 yOffSet;
	u16 width;
	u16 hight;
	u16 font = 24;
	u16 setTempValue;
	u16 curTempValue;
	float tarEx0Temp;
	 float tarBedTemp;
	u16 Xstart;


	if( millis() > display_next_update_millis)//500ms 更新一次
	{
		display_next_update_millis = millis() + 250;
		POINT_COLOR = BLUE;
		BACK_COLOR = BLACK;
		
		xOffSet = 2*PICTURE_X_SIZE - font;
		yOffSet = BMP_FONT_SIZE + 16;
		
		hight = font;
		width = 8*(font/2);
		
		gui_fill_rectangle_DMA(xOffSet,yOffSet,width,hight,BLACK);//清区域色块
		if(obj_id == BED_OBJ_ID)
		{

				if(firstDispFlag & DISP_BED_BIT )
				{
							tarBedTemp = degTargetBed();
							if(tarBedTemp > PRESET_TEMP)	setTempValue = tarBedTemp;
							else setTempValue = set_value[BED_ID];
				}
				
				
				else 
				{
					if(isOption2moreUiSelected == YES	)	
					{
							tarBedTemp = degTargetBed();
							if(tarBedTemp > PRESET_TEMP)	setTempValue= tarBedTemp;
							else setTempValue = set_value[BED_ID];					
					}
						
					else setTempValue = 0;
				}
				

			
			curTempValue = bedCurTemp;
			
			if(curTempValue > TempSense_HIGHT_VALUE || curTempValue < TempSense_LOW_VALUE)//热敏电阻是否有故障判断
				{
				curTempError = 1;
				firstDispFlag = FALSE;
				}
			else curTempError = 0;
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)infor_temp_bed[LANGUAGE],font,1);	
		}
		
		else if(obj_id == EXTRUDER1_OBJ_ID)
		{
			if(firstDispFlag & DISP_EX0_BIT)
			{
					tarEx0Temp = degTargetHotend(0);
					if(tarEx0Temp > PRESET_TEMP)	setTempValue = tarEx0Temp;
					else setTempValue = set_value[E1_ID];					
				

			}
				
			else 
			{
				if(isOption2moreUiSelected == YES	)	
				{
					tarEx0Temp = degTargetHotend(0);
					if(tarEx0Temp > PRESET_TEMP)	setTempValue = tarEx0Temp;
					else setTempValue = set_value[E1_ID];
				}
		
				else setTempValue = 0;
			}
			
			
			curTempValue = Extru1CurTemp;

			if(curTempValue > TempSense_HIGHT_VALUE || curTempValue < TempSense_LOW_VALUE)//热敏电阻是否有故障判断
				{
				curTempError = 2;
				firstDispFlag = FALSE;
				}
			else curTempError = 0;
			
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)infor_temp_extu1[LANGUAGE],font,1);		
		}
		
		if(curTempError )//显示热敏电阻故障信息
			{
			POINT_COLOR = RED;
			xOffSet = PICTURE_X_SIZE + 2;
			yOffSet += 2*font;
			
			width = (font/2)*(strlen((char *)infor_temp_error[LANGUAGE]));
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width,hight,BLACK);//清区域色块

			Show_Str(xOffSet,yOffSet,width,font,(u8 *)infor_temp_error[LANGUAGE],font,1);		

			
			POINT_COLOR = BLUE;
		}
		else
		{
			
			POINT_COLOR = WHITE;
			yOffSet += 2*font;

			
			Xstart = PICTURE_X_SIZE + 2;
			width = (font/2)*(strlen((char *)infor_temp_error[LANGUAGE]));//清除热敏电阻异常显示信息
			gui_fill_rectangle_DMA(Xstart,yOffSet,width,hight,BLACK);//清区域色块
			
			
			if( millis() > display_CurTemp_millis)//2s 更新一次
			{
				display_CurTemp_millis = millis() + 2000;
				dispCurTempValue = curTempValue;
				
			}
			LCD_ShowNum(xOffSet,yOffSet,dispCurTempValue,3,font);//显示当前温度值

			POINT_COLOR = BLUE;
			xOffSet  += (font/2)*3;
			LCD_ShowString(xOffSet,yOffSet,font/2,font,font,(u8 *)"/");//显示'/'

			POINT_COLOR = WHITE;
			xOffSet  += (font/2);
			LCD_ShowNum(xOffSet,yOffSet,setTempValue,3,font);//显示设定温度值
			Send_Restart_Move_Cmd();
			
		}
	}

	
}


static void preheatUI_set_temp(_obj_updt_id_e obj_id, _obj_option_e opt,_temp_updt_step_e step)
{

	 char TempBuffer[32];
	 float tarEx0Temp;
	 float tarBedTemp;
	switch (obj_id)//操作对象
	{
		case EXTRUDER1_OBJ_ID:
			if(degHotend0() > TempSense_HIGHT_VALUE || degHotend0() < TempSense_LOW_VALUE)//判断热敏电阻温度是否异常
				{
				PREHEATUI_DEBUG("#1sensor error\r\n");
				firstDispFlag = FALSE;
				break;
			}
			tarEx0Temp = degTargetHotend(0);
			if(tarEx0Temp > PRESET_TEMP)	set_value[E1_ID] = tarEx0Temp;
			
		switch (step)//操作步进
			{
				case TEMP_STEP1C:
					if(opt == ADD_OPT)
					{
						set_value[E1_ID] += 1.0;
						if(set_value[E1_ID] >= USER_SET_EX0_TEMP_MAX)
						set_value[E1_ID] = USER_SET_EX0_TEMP_MAX;
					}
					else if(opt == DEC_OPT)
					{
						set_value[E1_ID] -= 1.0;	
						if(set_value[E1_ID] < 0)
						set_value[E1_ID] = 0;
					}
					else if(opt == CLEAR_OPT) set_value[E1_ID] = CLC_EXT0_TEMP_VALUE;
					
					break;
					
				case TEMP_STEP5C:
					if(opt == ADD_OPT)
					{
						set_value[E1_ID] += 5.0;
						if(set_value[E1_ID] >= USER_SET_EX0_TEMP_MAX)
						set_value[E1_ID] = USER_SET_EX0_TEMP_MAX;
					}
					else if(opt == DEC_OPT)
					{
						set_value[E1_ID] -= 5.0;	
						if(set_value[E1_ID] < 0)
						set_value[E1_ID] = 0.0;
					}	
					else if(opt == CLEAR_OPT) set_value[E1_ID] = CLC_EXT0_TEMP_VALUE;	
					break;
					
				case TEMP_STEP10C:
					if(opt == ADD_OPT)
					{
						set_value[E1_ID] += 10.0;
						if(set_value[E1_ID] >= USER_SET_EX0_TEMP_MAX)
						set_value[E1_ID] = USER_SET_EX0_TEMP_MAX;
					}
					else if(opt == DEC_OPT)
					{
						set_value[E1_ID] -= 10.0;	
						if(set_value[E1_ID] < 0)
						set_value[E1_ID] = 0.0;
					}		
					else if(opt == CLEAR_OPT) set_value[E1_ID] = CLC_EXT0_TEMP_VALUE;
					break;				
					
				default:
					break;
			}
		setTargetHotend(set_value[E1_ID],0);
		sprintf(TempBuffer, "M104 S%.1f", set_value[E1_ID]);
        menu_action_gcode(TempBuffer);
			break;
			
		case BED_OBJ_ID:
			if(degBed() > TempSense_HIGHT_VALUE || degBed() < TempSense_LOW_VALUE)//判断热敏电阻温度是否异常
				{
				PREHEATUI_DEBUG("#2sensor error\r\n");
				firstDispFlag = FALSE;
				break;	
				}
			tarBedTemp = degTargetBed();
			if(tarBedTemp > PRESET_TEMP)	set_value[BED_ID] = tarBedTemp;
		switch (step)//操作步进
			{
				case TEMP_STEP1C:
					if(opt == ADD_OPT)
					{
						set_value[BED_ID] += 1.0;
						if(set_value[BED_ID] >= USER_SET_BED_TEMP_MAX)
						set_value[BED_ID] = USER_SET_BED_TEMP_MAX;						
						
					}
					else if(opt == DEC_OPT)
					{
						set_value[BED_ID] -= 1.0;	
						if(set_value[BED_ID] < 0)
						set_value[BED_ID] = 0.0;
					}
					else if(opt == CLEAR_OPT) set_value[BED_ID] =CLC_BED_TEMP_VALUE;
					break;
					
				case TEMP_STEP5C:
					if(opt == ADD_OPT)
					{
						set_value[BED_ID] += 5.0;
						if(set_value[BED_ID] >= USER_SET_BED_TEMP_MAX)
						set_value[BED_ID] = USER_SET_BED_TEMP_MAX;							
					}
					else if(opt == DEC_OPT)
					{
						set_value[BED_ID] -= 5.0;	
						if(set_value[BED_ID] < 0)
						set_value[BED_ID] = 0.0;
					}
					else if(opt == CLEAR_OPT) set_value[BED_ID] = CLC_BED_TEMP_VALUE;
					break;
					
				case TEMP_STEP10C:
					if(opt == ADD_OPT)
					{
						set_value[BED_ID] += 10.0;
						if(set_value[BED_ID] >= USER_SET_BED_TEMP_MAX)
						set_value[BED_ID] = USER_SET_BED_TEMP_MAX;							
					}
					else if(opt == DEC_OPT)
					{
						set_value[BED_ID] -= 10.0;	
						if(set_value[BED_ID] < 0)
						set_value[BED_ID] = 0.0;
					}	
					else if(opt == CLEAR_OPT) set_value[BED_ID] =CLC_BED_TEMP_VALUE;
					break;				
					
				default:
					break;
			}
		setTargetBed(set_value[BED_ID]);
	    sprintf(TempBuffer, "M140 S%.1f", set_value[BED_ID]);
        menu_action_gcode(TempBuffer);	
			break;
			
		default:
			break;
	}

	
	OPT_INFOR_E = NONE_OPT;//清零设置标志

}




static void preheatUI_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint16_t xoff = 2;
	uint16_t yoff = 5;

LCD_Clear_DMA(BLACK);
BACK_COLOR = BLACK;
POINT_COLOR = WHITE;

Show_Str(xoff,yoff,lcddev.width,font,(u8 *)preheatUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	preheatUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( preheatUIdev == 0)
	{
		PREHEATUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	preheatUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*PREHEAT_MENU_TOTAL_BMP_NUM);
	
	if( preheatUIdev->icon == 0)
	{
		PREHEATUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	preheatUIdev->status = 0x3F ;
	preheatUIdev->totalIcon = PREHEAT_MENU_VALID_BMP_NUM;//控件检测数目
	// 0 1
	for(j=0;j<PREHEAT_MENU_FIRST_ROW_BMP_NUM;j++)
	{
			preheatUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			preheatUIdev->icon[j].y = bmp_sart_offy ;
			preheatUIdev->icon[j].width = PICTURE_X_SIZE;
			preheatUIdev->icon[j].height = PICTURE_Y_SIZE;
			preheatUIdev->icon[j].path = (u8*)preheatUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3 4 5  
		for(j=PREHEAT_MENU_FIRST_ROW_BMP_NUM;j<PREHEAT_MENU_VALID_BMP_NUM;j++)
		{
			
			preheatUIdev->icon[j].x = bmp_sart_offx + (j-2)*bmp_between_offx  ;
			preheatUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			preheatUIdev->icon[j].width = PICTURE_X_SIZE;
			preheatUIdev->icon[j].height = PICTURE_Y_SIZE;
			preheatUIdev->icon[j].path = (u8*)preheatUI_icon_path_tbl[LANGUAGE][j];

		}
		

		j = 6;//热床
			preheatUIdev->icon[j].x = preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].x;
			preheatUIdev->icon[j].y = preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].y;
			preheatUIdev->icon[j].width = preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].width;
			preheatUIdev->icon[j].height = preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].height;
			
			preheatUIdev->icon[j].path = (u8*)preheatUI_icon_path_tbl[LANGUAGE][IS_PREHEAT_BED_ICON];
			preheatUIdev->icon[j].color = preheatUIdev->icon[IS_PREHEAT_BED_ICON].color;				
	
			

		
		j++;//7
		preheatUIdev->icon[j].x = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].x;
		preheatUIdev->icon[j].y = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].y;
		preheatUIdev->icon[j].width = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].width;
		preheatUIdev->icon[j].height = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].height;
		
		preheatUIdev->icon[j].path = (u8*)preheatUI_icon_path_tbl[LANGUAGE][IS_PREHEAT_HEAT_STEP5_ICON];
		preheatUIdev->icon[j].color = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].color;
		
		j++	;//8
		preheatUIdev->icon[j].x = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].x;
		preheatUIdev->icon[j].y = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].y;
		preheatUIdev->icon[j].width = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].width;
		preheatUIdev->icon[j].height = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].height;
		
		preheatUIdev->icon[j].path = (u8*)preheatUI_icon_path_tbl[LANGUAGE][IS_PREHEAT_HEAT_STEP10_ICON];
		preheatUIdev->icon[j].color = preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].color;
		
		
		for(j=0;j<preheatUIdev->totalIcon;j++)//不显示 切换显示的图标
		{
			if(j == IS_PREHEAT_EXTRUDER1_ICON)//挤出头/热床
			{
				if(extruder_bed_count == 1)//显示热床
				{
					display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_BED_ICON].path, preheatUIdev->icon[IS_PREHEAT_BED_ICON].x , preheatUIdev->icon[IS_PREHEAT_BED_ICON].y, preheatUIdev->icon[IS_PREHEAT_BED_ICON].width, preheatUIdev->icon[IS_PREHEAT_BED_ICON].height, preheatUIdev->icon[IS_PREHEAT_BED_ICON].color);
				}
				
				else if(extruder_bed_count == 0)//显示挤出头
				{
				//display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].path, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].x , preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].y, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].width, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].height, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].color);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[j].path, preheatUIdev->icon[j].x , preheatUIdev->icon[j].y, preheatUIdev->icon[j].width, preheatUIdev->icon[j].height, preheatUIdev->icon[j].color);
				}
			}
			else if(j == IS_PREHEAT_HEAT_STEP_ICON)//加热阶梯
			{
				if(heat_step_count == 1)//display step5
				{
					display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].color);

				}
				
				else if(heat_step_count == 2)//display step10
				{
					display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].color);

				}
				else if(heat_step_count == 0)//display step1
				{
				//display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].color);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[j].path, preheatUIdev->icon[j].x , preheatUIdev->icon[j].y, preheatUIdev->icon[j].width, preheatUIdev->icon[j].height, preheatUIdev->icon[j].color);
				}					
			}
			else
			display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[j].path, preheatUIdev->icon[j].x , preheatUIdev->icon[j].y, preheatUIdev->icon[j].width, preheatUIdev->icon[j].height, preheatUIdev->icon[j].color);
		}

}




/*
*********************************************************************************************************
*	函 数 名: preheat_set_screen
*	功能说明: 预热设置界面
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void preheat_screen(void)
{

_preheatUI_selected_button_e PREHEAT_SELECTED_BUTTON = NONE_PREHEAT_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  _obj_updt_id_e saveCodeID = NONE_OBJ_ID;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		preheatUI_init();
	}

	PREHEAT_SELECTED_BUTTON = (_preheatUI_selected_button_e)gui_touch_chk(preheatUIdev);
	
	switch ( PREHEAT_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_PREHEAT_ADD_ICON://加
		OPT_INFOR_E = ADD_OPT;
		PREHEAT_SAVED_BUTTON = IS_PREHEAT_ADD_ICON;

		gui_fill_rectangle_DMA(preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height,BLACK);
		saveCodeID = UPDT_INFOR_E;
				
				PREHEATUI_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_PREHEAT_DEC_ICON://减
		OPT_INFOR_E = DEC_OPT;
		PREHEAT_SAVED_BUTTON = IS_PREHEAT_DEC_ICON;

		gui_fill_rectangle_DMA(preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height,BLACK);
		saveCodeID = UPDT_INFOR_E;
		
				PREHEATUI_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
		case IS_PREHEAT_EXTRUDER1_ICON://挤出头/热床
		
			extruder_bed_count++;
			if(extruder_bed_count == 1)
			{
				UPDT_INFOR_E = BED_OBJ_ID;
				gui_fill_rectangle_DMA(preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].x, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].y, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].width, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_BED_ICON].path, preheatUIdev->icon[IS_PREHEAT_BED_ICON].x , preheatUIdev->icon[IS_PREHEAT_BED_ICON].y, preheatUIdev->icon[IS_PREHEAT_BED_ICON].width, preheatUIdev->icon[IS_PREHEAT_BED_ICON].height, preheatUIdev->icon[IS_PREHEAT_BED_ICON].color);
			
			}
			else
			{
				extruder_bed_count = 0;
				UPDT_INFOR_E = EXTRUDER1_OBJ_ID;
				gui_fill_rectangle_DMA(preheatUIdev->icon[IS_PREHEAT_BED_ICON].x, preheatUIdev->icon[IS_PREHEAT_BED_ICON].y, preheatUIdev->icon[IS_PREHEAT_BED_ICON].width, preheatUIdev->icon[IS_PREHEAT_BED_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].path, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].x , preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].y, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].width, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].height, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].color);
				
			}
				PREHEATUI_DEBUG("extruder screen\r\n");
				
			break;			
			
		case IS_PREHEAT_HEAT_STEP_ICON://温度步进设置
				heat_step_count++;
				switch (heat_step_count)
				{
					case 1://display step5
					TEMP_UPDT_STEP_E = TEMP_STEP5C;
				gui_fill_rectangle_DMA(preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].x, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].color);
					
						break;
						
					case 2://display step10
					TEMP_UPDT_STEP_E = TEMP_STEP10C;
				gui_fill_rectangle_DMA(preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].x, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP5_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].color);
					
						break;
						
					case 3://display step1
					TEMP_UPDT_STEP_E = TEMP_STEP1C;
				gui_fill_rectangle_DMA(preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].x, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP10_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].path, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].x , preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].y, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].width, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].height, preheatUIdev->icon[IS_PREHEAT_HEAT_STEP_ICON].color);
					heat_step_count = 0;
						break;			
						
					default:
						break;
				}
				
				
				PREHEATUI_DEBUG("heat step_screen\r\n");
				
			break;
			

		case IS_PREHEAT_CLEAR_ICON://清零设置值
				OPT_INFOR_E = CLEAR_OPT;
				saveCodeID = UPDT_INFOR_E;
				PREHEAT_SAVED_BUTTON = IS_PREHEAT_CLEAR_ICON;
		gui_fill_rectangle_DMA(preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height,BLACK);
			
				
				PREHEATUI_DEBUG("clear \r\n");
				
			break;
			
		case IS_PREHEAT_BACK_ICON:
		if(isOption2moreUiSelected == YES	)
		currentMenu = gcode_print_option_to_more_screen;
		
		else if(isOption2moreUiSelected == NO)
		currentMenu = main_screen;
		
		PREHEAT_SAVED_BUTTON = NONE_PREHEAT_ICON;
		free_flag = IS_TRUE;
				PREHEATUI_DEBUG("back_screen\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
		if(firstDispFlag != DISP_FLAG_SET) 
		{
			if(1 == extruder_bed_count)//热床
			{
				if(PREHEAT_SAVED_BUTTON == IS_PREHEAT_ADD_ICON || PREHEAT_SAVED_BUTTON ==IS_PREHEAT_DEC_ICON)
				firstDispFlag |= DISP_BED_BIT;
			}
			else if( 0 == extruder_bed_count )//挤出头
			{
				if(PREHEAT_SAVED_BUTTON == IS_PREHEAT_ADD_ICON || PREHEAT_SAVED_BUTTON ==IS_PREHEAT_DEC_ICON)
				firstDispFlag |= DISP_EX0_BIT;	
				
			}
		}	
	
		if(PREHEAT_SAVED_BUTTON != NONE_PREHEAT_ICON)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
		
			display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[PREHEAT_SAVED_BUTTON].path, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x , preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].color);
			PREHEAT_SAVED_BUTTON = NONE_PREHEAT_ICON;
		}

//display temperature

preheatUI_set_temp(saveCodeID,OPT_INFOR_E,TEMP_UPDT_STEP_E);
preheatUI_display_Temp(UPDT_INFOR_E,current_temperature_bed,current_temperature[0]);

	if(isOption2moreUiSelected == YES )//确保打印界面先按下暂停后再进入改页面才检测打印头靠边停止
	{
		check_is_pause_exacute();//检查打印头是否执行靠边停止

	}





	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,preheatUIdev->icon);
		myfree(SRAMIN,preheatUIdev);
		PREHEATUI_DEBUG("free preheatdev \r\n");
		//firstDispFlag = FALSE;
	}


}


static void Send_Restart_Move_Cmd(void)
{
	u8 moveSt;
	moveSt = Get_Move_State();
	if(moveSt == TRUE)
		{
	menu_action_gcode(RESTART_MOVE_ACTION);//restart move action
	}
	 
}







