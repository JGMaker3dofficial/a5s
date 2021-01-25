/*
*********************************************************************************************************
*
*	模块名称 :进退料
*	文件名称 : fila.c
*	版    本 : V1.0
*	说    明 : 
*
*
*********************************************************************************************************
*/


/**********************包含头文件*************************/



#include "fila.h"


#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"



/**********************调试宏定义**************************/

#define FILA_DEBUG_EN

#ifdef FILA_DEBUG_EN

#define FILA_DEBUG	DBG

#else
#define FILA_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



FILA_IN_STATE filament_in = IN_FILA_NO;
FILA_OUT_STATE filament_out = OUT_FILA_NO;
/***********************私有宏*************************/






/************************************************/
#define FILA_ICON_NUM	4
#define FILA_FIRST_ROW_ICON_NUM 		2
#define FILA_SECOND_ROW_ICON_NUM 	2


/**************控件触摸检测ID定义*****************/
#define IS_FILA_IN_BUTTON		0
#define IS_FILA_OUT_BUTTON		1
#define IS_FILA_STOP_BUTTON		2
#define IS_FILA_BACK_BUTTON		3


#define FILA_DETECT_UI_NUM	3
#define FILA_RUN_OUT_STOP_BT	0
#define FILA_RUN_OUT_ChangeFila_BT	1
#define FILA_RUN_OUT_BACK_BT	2

/*******************断料后再次上料界面*********************/

#define FILA_FED_INUI_NUM	2










//主界面 控制器
_gui_dev *filaUIdev = NULL;
_gui_dev *changeFilaUIdev = NULL;
_gui_dev *filaRunOutUIdev = NULL;
_gui_dev *filaFedInUIdev = NULL;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
_fila_time_s	filaTime ;//断料的持续时间，如果断料时间过长，则停止加热挤出头+热床
static	u8 isFedInHeatOK = FALSE;//断料持续时间过长，再次上料时要重新加热
_parameter_state_s heatEnd;

char *const filaUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/进退料",
	"Prepare/set/filamet"	
};
char *const changeFilaUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/换料",
	"Prepare/set/change filamet"	
};


char *const filaIOUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/进料",
	"Prepare/set/filamet in/out"
};

char *const filaOutUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/退料",
	"Prepare/set/filamet out"
};

char *const filaUI_stopPirntInfor[GUI_LANGUAGE_NUM] = 
{
	"请暂停打印,再进退料",
	"please stop printing first,then filament"
};

char *const filaInUI_remidInfor [GUI_LANGUAGE_NUM]= 
{
	"进料",
	"feed fila"
};


char *const filaOutUI_remidfor [GUI_LANGUAGE_NUM]= 
{
	"退料",
	"output fila"
};

char *const filaFedInUI_remidfor1 [GUI_LANGUAGE_NUM]= 
{
	"耗材已接上",
	"filament fed in again"
};

char *const filaFedInUI_remidfor2 [GUI_LANGUAGE_NUM]= 
{
	"加热中",
	"heating"
};

char *const filaOutUI_path[GUI_LANGUAGE_NUM][FILA_DETECT_UI_NUM]=
{
	
	{"1:/SYSTEM/PICTURE/ch_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",
	"1:/SYSTEM/PICTURE/en_bmp_return.bin"
	}
};

char *const filaRunOut_rmid1 [GUI_LANGUAGE_NUM]=
{
	"耗材缺失,请",
	"filament runs out,please fed "
};

char *const filaRunOut_rmid2[GUI_LANGUAGE_NUM] =
{
	"重新上料",
	"filament again!"
};
	

char *const filadetctAgain_rmid1[GUI_LANGUAGE_NUM]= 
{
	"耗材缺失,请上料",
	"filament detected,please fed"
};

char *const filadetctAgain_rmid2[GUI_LANGUAGE_NUM] = 
{
	"足够的耗材,再返回",
	"enough filament,then back"
};
char *const waitInfor[GUI_LANGUAGE_NUM] =
{
	"请稍等...",
	"Wait..."

};
char *const filaTempInfor[GUI_LANGUAGE_NUM] =
{
	"挤出1温度:",
	"E1 Temp:"
};

u32 filaOutUI_Color = GRAYBLUE;
u32 timeForRemider;
u32 timeForTemp;
u32 timeForFilaDetect;
u8 pollFilaIOCnt;
float zPositnFila;//用于进退料时保存z轴移动前的位置
//耗材是否存在标志,上电默认耗材存在
//换料动作完成标志
//是否发生过耗材没有了，打印暂停情况

_fila_state_s filaState = 
{
FALSE,
TRUE,
};
/************************************************/


//icon的路径表,HOME界面图标
char *const filaUI_icon_path_tbl[GUI_LANGUAGE_NUM][FILA_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",	  
	

	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    
    },
	{
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",	  
	

	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	    		
	}
};  

char *const changeFilaUI_icon_path_tbl[GUI_LANGUAGE_NUM][FILA_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",	  
	

	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    
    },
	{
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",	  
	

	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	    		
	}
}; 

char *const FilaFedInUI_icon_path_tbl[GUI_LANGUAGE_NUM][FILA_FED_INUI_NUM]=
{
{
	"1:/SYSTEM/PICTURE/ch_bmp_bed_no_words.bin",//0	    
	"1:/SYSTEM/PICTURE/ch_bmp_extru1_no_words.bin",	//1
},
{
	"1:/SYSTEM/PICTURE/en_bmp_bed_no_words.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_extru1_no_words.bin",	   //1	
}
};
static void filaUI_init(void)
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
			Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreFilaIOUI_title[LANGUAGE],font,1);	
		else if(isOption2moreUiSelected == NO)
			Show_Str(x,y,lcddev.width,font,(u8 *)filaUI_titile[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	filaUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( filaUIdev == 0)
	{
		FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	filaUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*FILA_ICON_NUM);
	
	if( filaUIdev->icon == 0)
	{
		FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	filaUIdev->status = 0x3F ;
	filaUIdev->totalIcon = FILA_ICON_NUM;//控件检测数目
	// 0 1
	for(j=0;j<FILA_FIRST_ROW_ICON_NUM;j++)
	{
			filaUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			filaUIdev->icon[j].y = bmp_sart_offy ;
			filaUIdev->icon[j].width = PICTURE_X_SIZE;
			filaUIdev->icon[j].height = PICTURE_Y_SIZE;
			filaUIdev->icon[j].path = (u8*)filaUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3   
		for(j=FILA_FIRST_ROW_ICON_NUM;j<FILA_ICON_NUM;j++)
		{
			if(j == FILA_ICON_NUM - 1)
			filaUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
			
			else 
			filaUIdev->icon[j].x = bmp_sart_offx ;
			
			filaUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			filaUIdev->icon[j].width = PICTURE_X_SIZE;
			filaUIdev->icon[j].height = PICTURE_Y_SIZE;
			filaUIdev->icon[j].path = (u8*)filaUI_icon_path_tbl[LANGUAGE][j];

		}
		
		filaUIdev->totalIcon = FILA_ICON_NUM;
		
		
		for(j=0;j<filaUIdev->totalIcon;j++)//不显示 切换显示的图标
		{

			display_bmp_bin_to_LCD((u8*)filaUIdev->icon[j].path, filaUIdev->icon[j].x , filaUIdev->icon[j].y, filaUIdev->icon[j].width, filaUIdev->icon[j].height, filaUIdev->icon[j].color);
		}

}




void fila_screen(void)
{

	u8 SELECTED_BUTTON = 0xff;
	u8 savedButton = 0xff;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;


	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		filaUI_init();
	}
	
	SELECTED_BUTTON = gui_touch_chk(filaUIdev);
	
	switch ( SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_FILA_IN_BUTTON://加

		savedButton = IS_FILA_IN_BUTTON;
		filament_inUI();	

				FILA_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_FILA_OUT_BUTTON://减
	
		savedButton = IS_FILA_OUT_BUTTON;
		filament_outUI();

				FILA_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
			
		case IS_FILA_BACK_BUTTON:
		currentMenu = gcode_print_option_to_more_screen;
		

		free_flag = IS_TRUE;
				FILA_DEBUG("back_screen\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
	
	dynamic_reminder_infor(filament_in,filament_out);
	
		if(savedButton != 0XFF)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
		
				gui_fill_rectangle_DMA(filaUIdev->icon[savedButton].x, filaUIdev->icon[savedButton].y, filaUIdev->icon[savedButton].width, filaUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)filaUIdev->icon[savedButton].path, filaUIdev->icon[savedButton].x , filaUIdev->icon[savedButton].y, filaUIdev->icon[savedButton].width, filaUIdev->icon[savedButton].height, filaUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
		if(isOption2moreUiSelected == YES)
		{
			check_is_pause_exacute();//检查打印头是否执行靠边停止
		
		}



	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,filaUIdev->icon);
		myfree(SRAMIN,filaUIdev);

		FILA_DEBUG("free filaUIdev \r\n");
	}


}

//提示
//0--打印机处于打印状态，1--打印机处于非打印状态
u8 StopPrint_remiderUI(void)
{

	uint8_t rtv = 0;

	if(card.FILE_STATE != PS_PIRNT)//是否处于打印状态
	{	
		rtv = 1;
	}

	return rtv;
	

}

void diplay_fila_in_infor(void)
{
	uint8_t font = 24;
	uint8_t length;
	uint8_t hight;
	uint16_t x,y;
	uint8_t offset = 16;

	y = BMP_FONT_SIZE + 16;
	x = PICTURE_X_SIZE + 16;
	hight = font;
	length = 2*PICTURE_X_SIZE - 16;

	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//清进退料提示区域
	Show_Str(x ,y,lcddev.width,font,(u8 *)filaInUI_remidInfor[LANGUAGE],font,1);	
	y += font + offset;
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//清进退料提示区域
	Show_Str(x,y,lcddev.width,font,(u8 *)waitInfor[LANGUAGE],font,1);	
	
}

void diplay_fila_out_infor(void)
{

	uint8_t font = 24;
	uint8_t length;
	uint8_t hight;
	uint16_t x,y;
	uint8_t offset = 16;
	
	y = BMP_FONT_SIZE + 16;
	x = PICTURE_X_SIZE + 16;
	hight = font;
	length = 2*PICTURE_X_SIZE - 16;

	
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//清进退料提示区域
	Show_Str(x,y,lcddev.width,font,(u8 *)filaOutUI_remidfor[LANGUAGE],font,1);	
	y += font + offset;
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//清进退料提示区域
	Show_Str(x,y,lcddev.width,font,(u8 *)waitInfor[LANGUAGE],font,1);	

}
void dynamic_reminder_infor(FILA_IN_STATE in,FILA_OUT_STATE out)
{
	u8 length;
	uint8_t font = 24;
	uint16_t x,y;
	uint16_t width;
	uint16_t hight;
	uint8_t offset = 32;
	if(timeForTemp <= millis())
	{
		
		timeForTemp = millis() + 250;
		x = PICTURE_X_SIZE + 16;
		y = BMP_FONT_SIZE + 16 + 4*font;
		hight = font;

		POINT_COLOR = YELLOW;
		Show_Str(x,y,lcddev.width,font,(u8 *)filaTempInfor[LANGUAGE],font,1);//显示Temp:

		width = 3*(font/2);
		POINT_COLOR = WHITE;
		y += 2*font;
		gui_fill_rectangle_DMA(x,y,width,hight,BLACK);//清区域
		LCD_ShowNum(x ,y ,degHotend(0),3,font);//显示挤出头温度值

		POINT_COLOR = YELLOW;
		x+=4*(font/2);
		width = 2*(font/2);
		LCD_ShowString(x,y,width,font,font,(u8 *)"'C");//显示'C
	
		POINT_COLOR = WHITE;
	}
	if(filament_in ==IN_FILA_FINISH || filament_out == OUT_FILA_FINISH)//进退料完成
	{
		filament_in = IN_FILA_NO;
		filament_out = OUT_FILA_NO;
		POINT_COLOR = BROWN;
		
		x = PICTURE_X_SIZE + 16;
		y = BMP_FONT_SIZE +  font + offset;

		length =  11*(font/2);
		gui_fill_rectangle_DMA(x,y,length,font,BLACK);//清进退料提示区域
		POINT_COLOR = YELLOW;
		LCD_ShowString(x,y,lcddev.width,font,font,(u8 *)"END");//显示'C
		POINT_COLOR = WHITE;
	}

	
}


void filament_inUI(void)
{
	uint8_t font = 16;

	uint8_t length;
	uint8_t hight;

	uint16_t x,y;
	
	x = 2;
	y = 5;
	

	hight = font;

	gui_fill_rectangle_DMA(x,y,lcddev.width,hight,BLACK);//清标题栏区域
	if(isOption2moreUiSelected == YES)
		Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreFilaIOUI_title[LANGUAGE],font,1);	
	else if(isOption2moreUiSelected == NO)
		Show_Str(x,y,lcddev.width,font,(u8 *)filaIOUI_titile[LANGUAGE],font,1);	

	diplay_fila_in_infor();
	
	filament_in_callBack();
	
}


void filament_outUI(void)
{
	uint8_t font = 16;
	uint16_t x,y;
	uint8_t length;
	uint8_t hight;
	uint8_t offset = 16;
	x = 2;
	y = 5;
	
	hight = font;
	gui_fill_rectangle_DMA(x,y,lcddev.width,hight,BLACK);//清标题栏区域
	if(isOption2moreUiSelected == YES)
		Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreFilaIOUI_title[LANGUAGE],font,1);	
	else if(isOption2moreUiSelected == NO)
		Show_Str(x,y,lcddev.width,font,(u8 *)filaIOUI_titile[LANGUAGE],font,1);	

	diplay_fila_out_infor();
	filament_out_callBack();
	
}



void filament_in_callBack(void)
{

	float extur1CurTemp;
	float extru1TarTemp;
	u32 codenum;
	u8 target_direction;
	u8 tmp_extruder = 0;

	
	long residencyStart;
	float ePos;
	extur1CurTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//备份e轴位置
	
	//Z轴先向上移动20mm
	zMove_up_filament(PRINT_FILA_IN_Z_UP_LENGTH);
	
	
	filament_in = IN_FILA_START;//开始进料
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//挤出头温度过低
	{
				filament_in = IN_FILA_PREHEAT;//预热中
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//设置加热温度
				setWatch();
      			codenum = millis();
		      	/* See if we are heating up or cooling down */
      			target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
				#ifdef TEMP_RESIDENCY_TIME
		        	residencyStart = -1;
		        /* continue to loop until we have reached the target temp
		          _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
			        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
						  { 
						 // if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
						  
						  if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					       
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%d\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							//  lcd_update();////////////////////////////////////////////////
							dynamic_reminder_infor(filament_in,filament_out);
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		     	#else
		        	while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==FALSE)) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					         
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%ld\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							          //  lcd_update();////////////////////////////////////////////////
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		      	#endif //TEMP_RESIDENCY_TIME
	}
	

	
	
	//第一阶段		
	saved_feedrate = feedrate;
	feedrate = PRINT_FILA_IN_FAST_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] = 0.0f;
		destination[E_AXIS] = current_position[E_AXIS] + FAST_ENTER_FILAMENT_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += FAST_ENTER_FILAMENT_LENGTH;
	st_synchronize_update_temp();
	
	//第二阶段	
	feedrate = PRINT_FILA_IN_SLOW_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] + SLOW_ENTER_FILAMENT_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += SLOW_ENTER_FILAMENT_LENGTH;
	st_synchronize_update_temp();

	
	setTargetHotend(extru1TarTemp, active_extruder);


	filament_in = IN_FILA_FINISH;//进料完成

	zMove_down_filament(PRINT_FILA_IN_Z_UP_LENGTH);
	
	current_position[E_AXIS] = ePos ;//恢复e轴位置
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//恢复进退料前e轴位置
	 destination[E_AXIS]  = current_position[E_AXIS];

	feedrate = saved_feedrate;
}


void filament_out_callBack(void)
{
	float extur1CurTemp;
	float extru1TarTemp;
	u32 codenum;
	uint32_t preTime ;
	u8 target_direction;
	u8 tmp_extruder = 0;
	u8 cnt = 0;

	
	long residencyStart;
	float ePos;
	u32 feedRateMulti = 200;
	extur1CurTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	
	filament_out = OUT_FILA_START;//开始退料
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//备份e轴位置
	
	//Z轴先向上移动20mm
	zMove_up_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//挤出头温度过低
	{
				filament_out = OUT_FILA_PREHEAT;//预热中
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//设置加热温度
				setWatch();
      			codenum = millis();
		      	/* See if we are heating up or cooling down */
      			target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
				#ifdef TEMP_RESIDENCY_TIME
		        	residencyStart = -1;
		        /* continue to loop until we have reached the target temp
		          _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
			        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
					{ 
					//	  if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
						  
						  if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					       
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%d\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							 // lcd_update();////////////////////////////////////////////////
							 dynamic_reminder_infor(filament_in,filament_out);
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		     	#else
		        	while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==FALSE)) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					         
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%ld\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							          //  lcd_update();////////////////////////////////////////////////
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		      	#endif //TEMP_RESIDENCY_TIME
	}	
	//第一阶段	
	feedrate = PRINT_FILA_OUT_SLOW1_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] = 0.0f;
		destination[E_AXIS] = current_position[E_AXIS] + EXTRUSION_LENGTH_BEFORE_EXIT;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);	
		current_position[E_AXIS] += EXTRUSION_LENGTH_BEFORE_EXIT;
	st_synchronize_update_temp();
	
	
//第二阶段	
	feedrate = PRINT_FILA_OUT_SLOW2_SPEED;
		current_position[E_AXIS] = 1000.0f;//设定一个E轴位置		
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] - EXIT_FILAMENT_LENGTH_1;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] -= EXIT_FILAMENT_LENGTH_1;
	st_synchronize_update_temp();
	

//第三阶段	
	preTime = millis();
	while (millis() - preTime < PRINT_FILA_OUT_TIME)//等待15s，再快速回抽
	{
		/* Print Temp Reading and remaining time every 1 second while heating up/cooling down */
		if ((millis() - codenum) > 1000UL)
		{
			cnt++;

			FILA_DEBUG("T:%d s \r\n",cnt);
			FILA_DEBUG("E:%.1f c \r\n",degHotend(0));
		codenum = millis();
		}
		manage_heater();
		manage_inactivity();
		//lcd_update();////////////////////////////////////////////////
		dynamic_reminder_infor(filament_in,filament_out);
	}
	
	feedrate = PRINT_FILA_OUT_FAST_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] - EXIT_FILAMENT_LENGTH_2;	
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] -= EXIT_FILAMENT_LENGTH_2;
	st_synchronize_update_temp();

	
	
	setTargetHotend(extru1TarTemp, active_extruder);
	

	filament_out = OUT_FILA_FINISH;//标记进料完成

	
	zMove_down_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	

	
	current_position[E_AXIS] = ePos ;//恢复e轴位置
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//恢复进退料前e轴位置
	 destination[E_AXIS]  = current_position[E_AXIS];
	
	feedrate = saved_feedrate;
}

void st_synchronize_update_temp(void)
{
    while( blocks_queued()) {
        manage_heater();
        manage_inactivity();
	dynamic_reminder_infor(filament_in,filament_out);
    }
}

//进料和退料z轴移动要配对使用，移动参数要一致，即moveMM要一样
void zMove_up_filament(u16 moveMM)
{
	u8 i;
	
	

		feedrate = homing_feedrate[X_AXIS];
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[E_AXIS] = current_position[E_AXIS];

		if(current_position[Z_AXIS] >= moveMM  )//z轴位置是否够高
		{
			destination[Z_AXIS] = current_position[Z_AXIS];//备份上移前z轴位置
		}
		else if(current_position[Z_AXIS] < moveMM && current_position[Z_AXIS] < Z_MAX_POS)
		{
			zPositnFila = current_position[Z_AXIS];
			destination[Z_AXIS] = current_position[Z_AXIS] + moveMM;//z轴上移movemm距离
			if(destination[Z_AXIS] > Z_MAX_POS) //Z轴移动超范围
			{
				destination[Z_AXIS] = Z_MAX_POS;
				zPositnFila = Z_MAX_POS - current_position[Z_AXIS];//记录上移的距离
			}
		}
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		for (i = 0; i < NUM_AXIS; i++)	current_position[i] = destination[i];

	
	st_synchronize_update_temp();
}

void zMove_down_filament(u16 moveMM)
{
	u8 i;
	feedrate = homing_feedrate[X_AXIS];


		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[E_AXIS] = current_position[E_AXIS];
		if(current_position[Z_AXIS] - zPositnFila == moveMM)//之前移动过z轴
		{
			destination[Z_AXIS] = zPositnFila ;//z轴下移movemm距离
		}
		
		else destination[Z_AXIS] = current_position[Z_AXIS] ;
		
		if(current_position[Z_AXIS] == Z_MAX_POS) //Z轴移动超范围
		{
			destination[Z_AXIS] = current_position[Z_AXIS] - zPositnFila;
		}
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		for (i = 0; i < NUM_AXIS; i++)	current_position[i] = destination[i];
	st_synchronize_update_temp();
}



void fila_run_out_detet_screen(void)
{
    uint8_t num = FILA_DETECT_UI_NUM;//按键数

    uint8_t free_flag = 0;

	uint16_t xStart = 2;
	uint16_t yStart = 5;

	uint8_t *fileName = NULL;
	uint8_t *dispFileName = NULL;
	uint8_t length;
	uint8_t setLength;
	uint8_t font = 16;
	uint16_t width;
	uint8_t titleLength;
	uint8_t j;
	u8 SELECTED_BUTTON;
	char cmd[10];
    if( redraw_screen_flag == IS_TRUE )
    {
		uint16_t bmp_between_offx;
		uint16_t bmp_between_offy;
		redraw_screen_flag = IS_FALSE;
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		width = lcddev.width;
		Enable_Beep();
		filaTime.filaRunOutContnTime = millis();//记录此时的起始时间
		filaTime.isTimeOut = FALSE;

		LCD_Clear_DMA(BLACK);//清屏
		
		titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//获得标题栏提示信息长度
		Show_Str(xStart,yStart,width,font,(u8 *)gcode_print_title[LANGUAGE],font,1);	
		fileName = get_current_root_file_name(printFileName);//获得gcode文件名

		setLength = 30;//必须是2的整倍数，显示汉字要2个字节
		length = strlen((char *)fileName);
		if(length >= setLength)//是否分段显示gcode文件名
		{
			dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//分配内存
			if(dispFileName == NULL)
			{	
				FILA_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
				
			}
			else
			{
				
				strncpy((char *)dispFileName,(const char *)fileName,setLength);
				dispFileName[setLength] = '\0';
				
				Show_Str(xStart + titleLength*(font/2),yStart,width,font,(u8*)dispFileName, font, 1); 
				gui_memin_free(dispFileName);//释放内存
				
				xStart = 2;
				dispFileName = fileName + setLength;
				Show_Str(xStart,yStart + font, width,font,(u8*)dispFileName, font, 1); 
			}						
		}
		else 
		Show_Str(xStart + titleLength*(font/2),yStart,width,font,(u8*)fileName, font, 1); //显示gcode文件

		free_current_root_file_name_memory(fileName);//释放内存
	
	 	

		//为主界面控制器申请内存
		filaRunOutUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( filaRunOutUIdev == 0)
		{
			FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		filaRunOutUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*num);
		
		if( filaRunOutUIdev->icon == 0)
		{
			FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		 bmp_between_offx = PICTURE_X_SIZE + 2;
		 bmp_between_offy = PICTURE_Y_SIZE + 2;
		filaRunOutUIdev->status = 0x3F ;
		
		

		for(j = 0; j < num ; j++)
		{
				
			 if(j == num - 1 )	filaRunOutUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx;
			else if(j == 0)filaRunOutUIdev->icon[j].x = bmp_sart_offx ;
			else if(j == 1)filaRunOutUIdev->icon[j].x = bmp_sart_offx + bmp_between_offx/2 + bmp_between_offx;
			
			filaRunOutUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy ;
			filaRunOutUIdev->icon[j].width = PICTURE_X_SIZE;
			filaRunOutUIdev->icon[j].height = PICTURE_Y_SIZE;
			filaRunOutUIdev->icon[j].path = (u8*)filaOutUI_path[LANGUAGE][j];
			filaRunOutUIdev->icon[j].color = filaOutUI_Color;
			
		}

		filaRunOutUIdev->totalIcon = num;
		
			for(j = 0; j < filaRunOutUIdev->totalIcon; j++)
			display_bmp_bin_to_LCD((u8*)filaRunOutUIdev->icon[j].path, filaRunOutUIdev->icon[j].x , filaRunOutUIdev->icon[j].y, filaRunOutUIdev->icon[j].width, filaRunOutUIdev->icon[j].height, filaRunOutUIdev->icon[j].color);			
		

    }
	
	
	fila_run_out_disply(FILA_IO_STATE);
	filaState.filaIOState = check_fila_run_out();

	if(filaTime.isTimeOut == TRUE && filaTime.firstSet == FALSE)
	{
		
		filaTime.firstSet = TRUE;
		filaTime.bck_bedTep = current_temperature_bed;//备份温度
		filaTime.bck_ex0Tep = current_temperature[0];
	
		sprintf(cmd,"M104 S%.1f",0.0);//停止加热
		 enquecommand(cmd);
		 
		sprintf(cmd,"M140 S%.1f",0.0);
		enquecommand(cmd);			
	}

//断料检测到后，暂停打印，液晶界面才刷新
	fila_run_out_pause();	
	

	



//按键检测
	SELECTED_BUTTON = gui_touch_chk(filaRunOutUIdev);	
	if(SELECTED_BUTTON == FILA_RUN_OUT_BACK_BT)
	{

		free_flag = IS_TRUE;
		if(filaTime.firstSet == TRUE)	{currentMenu = heat_contenue_print_screen;	filaTime.firstSet = FALSE;}//此时挤出头 + 热床已经降温
		else currentMenu = gcode_print_option_screen;
				
		FILA_DEBUG("fila run out back \r\n");
	}
	else if(SELECTED_BUTTON ==FILA_RUN_OUT_STOP_BT)
	{
		free_flag = IS_TRUE;
		currentMenu = stop_screen;			
		FILA_DEBUG("STOP \r\n");
	}
	else if(SELECTED_BUTTON ==FILA_RUN_OUT_ChangeFila_BT)//换料
	{
		free_flag = IS_TRUE;
		currentMenu = change_fila_screen;		
		isOption2moreUiSelected = YES;//进入换料界面后，直接返回optionToMore界面
		FILA_DEBUG("change fila\r\n");
	}
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,filaRunOutUIdev->icon);
		myfree(SRAMIN,filaRunOutUIdev);
		bsp_ClearKey();//断料IO检测缓存清零
		filaRunOutUIdev = NULL;
		filaRunOutUIdev = NULL;
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;
		clear_filaTime_s();
		FILA_DEBUG("free filaRunOutUIdev \r\n");
		Disable_Beep();
	}

}


void fila_run_out_disply(u8 state)
{

	uint8_t font = 16;
	uint16_t xStart ;
	uint16_t yStart ;
	uint16_t hight = 2*font;
	uint16_t width = lcddev.width;

	xStart = 10;
	yStart = yStart + 3*font + 5;

	if( state == FILA_STATE_OUT )//断料
	{
			if( filaTime.dipFirstRun == 0)
				{
				POINT_COLOR = RED;
				filaTime.dipFirstRun = 1;
				gui_fill_rectangle_DMA(xStart,yStart,lcddev.width,hight,BLACK);//清标题栏区域
				Show_Str(xStart,yStart,width,font,(u8 *)filaRunOut_rmid1[LANGUAGE],font,1); 
				Show_Str(xStart,yStart + font,width,font,(u8 *)filaRunOut_rmid2[LANGUAGE],font,1);	

			}
			
			if(abs(millis() - filaTime.filaRunOutContnTime) >= FILA_RUN_OUT_TIME) //长时间断料
			{
				filaTime.isTimeOut = TRUE;
			}

		Enable_Beep();
	}
	else if(state == FILA_STATE_FED  )//上料
	{

		if(filaTime.dipFirstRun == 1)
			{
			
		POINT_COLOR = BLUE;
		filaTime.dipFirstRun = 0;
		gui_fill_rectangle_DMA(xStart,yStart,lcddev.width,hight,BLACK);//清标题栏区域
		Show_Str(xStart,yStart,width,font,(u8 *)filadetctAgain_rmid1[LANGUAGE],font,1);	
		Show_Str(xStart,yStart + font,width,font,(u8 *)filadetctAgain_rmid2[LANGUAGE],font,1);
		
			}
		filaTime.filaRunOutContnTime = millis();//调整计时基数

		Disable_Beep();
	}



	POINT_COLOR = WHITE;
	
}

//检测断料
//返回值：1--断料,0--上料
u8 check_fila_run_out(void)
{
	u8 rtv = 0xff;
	u8 key;
	key = bsp_GetKey(); //检测按键
	if( key == FILA_RUN_OUT_DET)//断料
	{
		rtv = 1;
	}
	else if(key == FILA_FED_DET)//上料
	{
		rtv = 0;
	}
	return rtv;
}


/******************************换料**********************************/
static void changeFilaUI_init(void)
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
			Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreChangeFilaUI_title[LANGUAGE],font,1);	
		else if(isOption2moreUiSelected == NO)
			Show_Str(x,y,lcddev.width,font,(u8 *)changeFilaUI_titile[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	changeFilaUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( changeFilaUIdev == 0)
	{
		FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	changeFilaUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*FILA_ICON_NUM);
	
	if( changeFilaUIdev->icon == 0)
	{
		FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	changeFilaUIdev->status = 0x3F ;
	changeFilaUIdev->totalIcon = FILA_ICON_NUM;//控件检测数目
	// 0 1
	for(j=0;j<FILA_FIRST_ROW_ICON_NUM;j++)
	{
			changeFilaUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			changeFilaUIdev->icon[j].y = bmp_sart_offy ;
			changeFilaUIdev->icon[j].width = PICTURE_X_SIZE;
			changeFilaUIdev->icon[j].height = PICTURE_Y_SIZE;
			changeFilaUIdev->icon[j].path = (u8*)changeFilaUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3   
		for(j=FILA_FIRST_ROW_ICON_NUM;j<FILA_ICON_NUM;j++)
		{
			if(j == FILA_ICON_NUM - 1)
			changeFilaUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
			
			else 
			changeFilaUIdev->icon[j].x = bmp_sart_offx ;
			
			changeFilaUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			changeFilaUIdev->icon[j].width = PICTURE_X_SIZE;
			changeFilaUIdev->icon[j].height = PICTURE_Y_SIZE;
			changeFilaUIdev->icon[j].path = (u8*)changeFilaUI_icon_path_tbl[LANGUAGE][j];

		}
		
		changeFilaUIdev->totalIcon = FILA_ICON_NUM;
		
		
		for(j=0;j<changeFilaUIdev->totalIcon;j++)//不显示 切换显示的图标
		{

			display_bmp_bin_to_LCD((u8*)changeFilaUIdev->icon[j].path, changeFilaUIdev->icon[j].x , changeFilaUIdev->icon[j].y, changeFilaUIdev->icon[j].width, changeFilaUIdev->icon[j].height, changeFilaUIdev->icon[j].color);
		}
	
}
void change_fila_screen(void)
{
	u8 SELECTED_BUTTON = 0xff;
	u8 savedButton = 0xff;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;


	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		changeFilaUI_init();
	}
	
	SELECTED_BUTTON = gui_touch_chk(changeFilaUIdev);
	
	switch ( SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_FILA_IN_BUTTON://加

		savedButton = IS_FILA_IN_BUTTON;
		change_filament_inUI();	

				FILA_DEBUG("in fila\r\n");
				
			break;
			
		case IS_FILA_OUT_BUTTON://减
	
		savedButton = IS_FILA_OUT_BUTTON;
		change_filament_outUI();

				FILA_DEBUG("out fila\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
			
		case IS_FILA_STOP_BUTTON:
		
		FILA_DEBUG("stop changeFila\r\n");
		break;
			
			
		case IS_FILA_BACK_BUTTON:
		
		
		if(isOption2moreUiSelected == YES)
		currentMenu = gcode_print_option_to_more_screen;
		
		else if(isOption2moreUiSelected == NO)
		currentMenu = set_screen;
		
		free_flag = IS_TRUE;
				FILA_DEBUG("back_screen\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
	
	dynamic_reminder_infor(filament_in,filament_out);
	
		if(savedButton != 0XFF)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
		
				gui_fill_rectangle_DMA(changeFilaUIdev->icon[savedButton].x, changeFilaUIdev->icon[savedButton].y, changeFilaUIdev->icon[savedButton].width, changeFilaUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)changeFilaUIdev->icon[savedButton].path, changeFilaUIdev->icon[savedButton].x , changeFilaUIdev->icon[savedButton].y, changeFilaUIdev->icon[savedButton].width, changeFilaUIdev->icon[savedButton].height, changeFilaUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
		if(isOption2moreUiSelected == YES)//暂停/开始按键触发
		{
			check_is_pause_exacute();//检查打印头是否执行靠边停止
		
		}



	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,changeFilaUIdev->icon);
		myfree(SRAMIN,changeFilaUIdev);

		FILA_DEBUG("free changeFilaUIdev \r\n");
	}
	
}

void change_filament_inUI(void)
{

	diplay_fila_in_infor();
	change_filament_in_callBack();
	
}

void change_filament_outUI(void)
{
	diplay_fila_out_infor();
	change_filament_out_callBack();
	
}


void change_filament_in_callBack(void)
{
	float extur1CurTemp;
	float extru1TarTemp;
	float savedExTemp;
	u32 codenum;
	u8 target_direction;
	u8 tmp_extruder = 0;


	long residencyStart;
	float ePos;
	extur1CurTemp = degHotend(0);
	savedExTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//备份e轴位置
	
	//Z轴先向上移动20mm
	FILA_DEBUG("Zup\r\n");
	zMove_up_filament(CHANGE_FILA_IN_Z_UP_LENGTH);
	
	FILA_DEBUG("extur1CurTemp = %f, extru1TarTemp = %f\r\n", extur1CurTemp,extru1TarTemp );
	filament_in = IN_FILA_START;//开始进料
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//挤出头温度过低
	{
		FILA_DEBUG("heating...\r\n");
				filament_in = IN_FILA_PREHEAT;//预热中
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//设置加热温度
				setWatch();
      			codenum = millis();
		      	/* See if we are heating up or cooling down */
      			target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
				#ifdef TEMP_RESIDENCY_TIME
		        	residencyStart = -1;
		        /* continue to loop until we have reached the target temp
		          _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
			        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
					{ 
					//	  if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
						  
						  if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					       
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%d\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							//  lcd_update();////////////////////////////////////////////////
							dynamic_reminder_infor(filament_in,filament_out);
						 #ifdef TEMP_RESIDENCY_TIME
							/* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
							  or when current temp falls outside the hysteresis after target temp was reached */
							 if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
								  (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
								  (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
							  {
								residencyStart = millis();
							  }
						#endif //TEMP_RESIDENCY_TIME
					}
		     	#else
		        	while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==FALSE)) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					         
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%ld\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							          //  lcd_update();////////////////////////////////////////////////
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		      	#endif //TEMP_RESIDENCY_TIME
			extur1CurTemp = degHotend(0);
			extru1TarTemp = degTargetHotend(0);
	}
	

	
	
	//第一阶段	
	FILA_DEBUG("step1 ...\r\n");
	saved_feedrate = feedrate;
	feedrate = CHANGE_FILA_IN_FAST_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] = 0.0f;
		destination[E_AXIS] = current_position[E_AXIS] + CHANGE_FILA_FAST_ENTER_LENGTH;	
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += CHANGE_FILA_FAST_ENTER_LENGTH;
	st_synchronize_update_temp();
	
	FILA_DEBUG("st1 cp:%.1f,dp:%.1f\r\n",current_position[E_AXIS],destination[E_AXIS]);
	
	//第二阶段	
	FILA_DEBUG("step2 ...\r\n");
	feedrate = CHANGE_FILA_IN_SLOW_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] + CHANGE_FILA_SLOW_ENTER_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += CHANGE_FILA_SLOW_ENTER_LENGTH;
	st_synchronize_update_temp();

	FILA_DEBUG("st2 cp:%.1f,dp:%.1f\r\n",current_position[E_AXIS],destination[E_AXIS]);
	
	setTargetHotend(savedExTemp, active_extruder);//恢复换料前的温度值

	filament_in = IN_FILA_FINISH;//进料完成
	zMove_down_filament(CHANGE_FILA_IN_Z_UP_LENGTH);
	
	current_position[E_AXIS] = ePos ;//恢复e轴位置
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//恢复进退料前e轴位置
	 destination[E_AXIS]  = current_position[E_AXIS];

	feedrate = saved_feedrate;
}

void change_filament_out_callBack(void)
{
	float extur1CurTemp;
	float extru1TarTemp;
	float savedExTemp;
	u32 codenum;
	uint32_t preTime ;
	u8 target_direction;
	u8 tmp_extruder = 0;
	u8 cnt = 0;

	
	long residencyStart;
	float ePos;
	u32 feedRateMulti = 200;
	extur1CurTemp = degHotend(0);
	savedExTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	
	filament_out = OUT_FILA_START;//开始退料
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//备份e轴位置
	
	//Z轴先向上移动20mm
	zMove_up_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//挤出头温度过低
	{
				filament_out = OUT_FILA_PREHEAT;//预热中
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//设置加热温度
				setWatch();
      			codenum = millis();
		      	/* See if we are heating up or cooling down */
      			target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
				#ifdef TEMP_RESIDENCY_TIME
		        	residencyStart = -1;
		        /* continue to loop until we have reached the target temp
		          _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
			        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
					{ 
					//	  if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
						  
						  if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					       
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%d\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							 // lcd_update();////////////////////////////////////////////////
							 dynamic_reminder_infor(filament_in,filament_out);
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
					}
		     	#else
		        	while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==FALSE)) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    FILA_DEBUG("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					         
					            #ifdef TEMP_RESIDENCY_TIME
					              FILA_DEBUG(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 FILA_DEBUG("%ld\n", codenum );
					              }
					              else
					              {
					                 FILA_DEBUG("?\n");
					              }
					            #else
					              FILA_DEBUG("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
							          //  lcd_update();////////////////////////////////////////////////
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		      	#endif //TEMP_RESIDENCY_TIME
				extur1CurTemp = degHotend(0);
			extru1TarTemp = degTargetHotend(0);			
	}
	
	
	//第一阶段
	feedrate = CHANGE_FILA_OUT_SLOW1_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] =CHANGE_FILA_OUT_TOTAL_LENGTH;
		destination[E_AXIS] = current_position[E_AXIS] + CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += CHANGE_FILA_EXTRUSION_LENGTH_BEFORE_EXIT;
	st_synchronize_update_temp();
	FILA_DEBUG("out1 cp:%.1f,dp:%.1f\r\n",current_position[E_AXIS],destination[E_AXIS]);
//第二阶段
	feedrate = CHANGE_FILA_OUT_SLOW2_SPEED;	
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] - CHANGE_FILA_EXIT_LENGTH_1;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] -= CHANGE_FILA_EXIT_LENGTH_1;
	st_synchronize_update_temp();
	FILA_DEBUG("out2 cp:%.1f,dp:%.1f\r\n",current_position[E_AXIS],destination[E_AXIS]);
//第三阶段
	feedrate = CHANGE_FILA_OUT_FAST_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] - CHANGE_FILA_EXIT_LENGTH_2;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] -= CHANGE_FILA_EXIT_LENGTH_2;
	st_synchronize_update_temp();

FILA_DEBUG("out3 cp:%.1f,dp:%.1f\r\n",current_position[E_AXIS],destination[E_AXIS]);
	
	setTargetHotend(savedExTemp, active_extruder);//恢复换料前挤出头温度
	


	filament_out = OUT_FILA_FINISH;//标记进料完成
	zMove_down_filament(PRINT_FILA_OUT_Z_UP_LENGTH);//Z轴下降到换料前高度
	
	
	
	current_position[E_AXIS] = ePos ;//恢复e轴位置
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//恢复进退料前e轴位置
	 destination[E_AXIS]  = current_position[E_AXIS];
	
	feedrate = saved_feedrate;
}

void heat_until_targetTem(void)
{
	char cmd[12];
	if(filaTime.firstRun == FALSE)
	{
	filaTime.firstRun = TRUE;
	sprintf(cmd,"M104 S%.1f",filaTime.bck_ex0Tep);//停止加热
	enquecommand(cmd);
	 
	sprintf(cmd,"M140 S%.1f",filaTime.bck_bedTep);
	enquecommand(cmd);	
	filaTime.heating = TRUE;
	}
	
	if(current_temperature_bed > filaTime.bck_bedTep - TEP_OFFSET && current_temperature[0] > filaTime.bck_ex0Tep - TEP_OFFSET)
	{
		filaTime.firstRun = FALSE;
		isFedInHeatOK = TRUE;
		filaTime.heating = FALSE;
	}
	else return;

	
		
}
void heat_contenue_print_screen(void)
{
	
	uint16_t xStart = 2;
	uint16_t yStart = 5;
	uint16_t width ;
	uint16_t Length;
	uint8_t free_flag = 0;
	uint8_t font = 24;
	uint8_t j;
	uint8_t num = FILA_FED_INUI_NUM;

	
	if( redraw_screen_flag == IS_TRUE)
	{
		
		redraw_screen_flag = IS_FALSE;
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		width = lcddev.width;
		filaTime.dispTime1 = millis();
		LCD_Clear_DMA(BLACK);//清屏
		
		
		Show_Str(xStart,yStart,width,font,(u8 *)filaFedInUI_remidfor1[LANGUAGE],font,1);	
		POINT_COLOR = RED;
		Show_Str(xStart,yStart + 2*font,width,font,(u8 *)filaFedInUI_remidfor2[LANGUAGE],font,1);	
		POINT_COLOR = WHITE;
				//为主界面控制器申请内存
		filaFedInUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( filaFedInUIdev == 0)
		{
			FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		filaFedInUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*num);
		
		if( filaFedInUIdev->icon == 0)
		{
			FILA_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		xStart = 41;
		yStart = PICTURE_Y_SIZE;

		for(j = 0; j < num; j++)
		{
		
		filaFedInUIdev->icon[j].x = xStart + xStart*2*j + j*PICTURE_X_SIZE  ;
		filaFedInUIdev->icon[j].y = yStart ;
		filaFedInUIdev->icon[j].width = PICTURE_X_SIZE;
		filaFedInUIdev->icon[j].height = PICTURE_Y_SIZE;
		filaFedInUIdev->icon[j].path = (u8*)FilaFedInUI_icon_path_tbl[LANGUAGE][j];		
		}
		filaFedInUIdev->totalIcon = num;
		
		for(j = 0; j < num ; j++)
		display_bmp_bin_to_LCD((u8*)filaFedInUIdev->icon[j].path, filaFedInUIdev->icon[j].x , filaFedInUIdev->icon[j].y, filaFedInUIdev->icon[j].width, filaFedInUIdev->icon[j].height, filaFedInUIdev->icon[j].color);	
		xStart = 2;
		yStart = 5;
		heatEnd.x = filaFedInUIdev->icon[0].x - 3;
		heatEnd.y = lcddev.height -  30;
	}

	
	if(  filaTime.dispTime1 <= millis())
	{
		filaTime.dispTime1 = millis() + 1000;
		Length = strlen((char *)filaFedInUI_remidfor2[LANGUAGE]);
		width = lcddev.width;
		yStart = 5 + 2*font;
		xStart = 2 + Length*(font/2);
		gui_fill_rectangle_DMA(xStart ,yStart ,6*(font/2),font,BLACK);//清进退料提示区域
		POINT_COLOR = BLUE;
		
		
		switch(filaTime.dispState)
		{
			case 0 :
			Show_Str(xStart,yStart ,width,font,(u8 *)"..",font,1);//显示一个小数点	
			filaTime.dispState += 1;
			break;
			
			case 1 :
			Show_Str(xStart,yStart ,width,font,(u8 *)"....",font,1);//显示一个小数点	
			filaTime.dispState += 1;
			break;
			
			case 2 :
			Show_Str(xStart,yStart,width,font,(u8 *)"......",font,1);//显示一个小数点	
			filaTime.dispState = 0;
			break;
			
			default :
			break;		
		}
		POINT_COLOR = WHITE;
	}
	get_printUI_parameter(&heatEnd);
	display_heat_paramter(&heatEnd);
filaState.filaIOState = check_fila_run_out();
	heat_until_targetTem();

	if(isFedInHeatOK == TRUE)
	{
		isFedInHeatOK = FALSE;
		heat_end_exacute();
		currentMenu = gcode_print_option_screen;	
		free_flag = IS_TRUE;
	}

	
	
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,filaFedInUIdev->icon);
		myfree(SRAMIN,filaFedInUIdev);
		filaFedInUIdev->icon = NULL;
		filaFedInUIdev = NULL;
		clear_filaTime_s();
		FILA_DEBUG("free filaFedINUIdev \r\n");
	}
}
//开始打印界面显示
void heat_end_exacute(void)
{
	float ePos;
	float extur1CurTemp;
	float extru1TarTemp;
	extur1CurTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	if(extur1CurTemp > filaTime.bck_ex0Tep - TEP_OFFSET && extru1TarTemp > filaTime.bck_ex0Tep - TEP_OFFSET)//
	{
	//第一阶段		
	FILA_DEBUG("$# fila in\r\n");
	ePos = current_position[E_AXIS];//备份e轴位置
	saved_feedrate = feedrate;
	feedrate = PRINT_FILA_IN_FAST_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] = 0.0f;
		destination[E_AXIS] = current_position[E_AXIS] + FAST_ENTER_FILAMENT_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += FAST_ENTER_FILAMENT_LENGTH;
	st_synchronize_no_lcd();
	
	//第二阶段	
	feedrate = PRINT_FILA_IN_SLOW_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] + SLOW_ENTER_FILAMENT_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += SLOW_ENTER_FILAMENT_LENGTH;
	st_synchronize_no_lcd();

	
	
	current_position[E_AXIS] = ePos ;//恢复e轴位置
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//恢复进退料前e轴位置
	 destination[E_AXIS]  = current_position[E_AXIS];

	feedrate = saved_feedrate;
	FILA_DEBUG("$# fila in end\r\n");
	}
}


void clear_filaTime_s(void)
{
			filaTime.firstSet = FALSE;
            filaTime.firstRun = FALSE;
			filaTime.isTimeOut = FALSE;
			filaTime.dipFirstRun = FALSE;
			filaTime.dispState = FALSE;
			filaTime.heating = FALSE;
}
	
