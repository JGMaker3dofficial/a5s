/*
*********************************************************************************************************
*
*	ģ������ :������
*	�ļ����� : fila.c
*	��    �� : V1.0
*	˵    �� : 
*
*
*********************************************************************************************************
*/


/**********************����ͷ�ļ�*************************/



#include "fila.h"


#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"



/**********************���Ժ궨��**************************/

#define FILA_DEBUG_EN

#ifdef FILA_DEBUG_EN

#define FILA_DEBUG	DBG

#else
#define FILA_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



FILA_IN_STATE filament_in = IN_FILA_NO;
FILA_OUT_STATE filament_out = OUT_FILA_NO;
/***********************˽�к�*************************/






/************************************************/
#define FILA_ICON_NUM	4
#define FILA_FIRST_ROW_ICON_NUM 		2
#define FILA_SECOND_ROW_ICON_NUM 	2


/**************�ؼ��������ID����*****************/
#define IS_FILA_IN_BUTTON		0
#define IS_FILA_OUT_BUTTON		1
#define IS_FILA_STOP_BUTTON		2
#define IS_FILA_BACK_BUTTON		3


#define FILA_DETECT_UI_NUM	3
#define FILA_RUN_OUT_STOP_BT	0
#define FILA_RUN_OUT_ChangeFila_BT	1
#define FILA_RUN_OUT_BACK_BT	2

/*******************���Ϻ��ٴ����Ͻ���*********************/

#define FILA_FED_INUI_NUM	2










//������ ������
_gui_dev *filaUIdev = NULL;
_gui_dev *changeFilaUIdev = NULL;
_gui_dev *filaRunOutUIdev = NULL;
_gui_dev *filaFedInUIdev = NULL;
static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������
_fila_time_s	filaTime ;//���ϵĳ���ʱ�䣬�������ʱ���������ֹͣ���ȼ���ͷ+�ȴ�
static	u8 isFedInHeatOK = FALSE;//���ϳ���ʱ��������ٴ�����ʱҪ���¼���
_parameter_state_s heatEnd;

char *const filaUI_titile [GUI_LANGUAGE_NUM]= 
{
	"׼��/����/������",
	"Prepare/set/filamet"	
};
char *const changeFilaUI_titile [GUI_LANGUAGE_NUM]= 
{
	"׼��/����/����",
	"Prepare/set/change filamet"	
};


char *const filaIOUI_titile [GUI_LANGUAGE_NUM]= 
{
	"׼��/����/����",
	"Prepare/set/filamet in/out"
};

char *const filaOutUI_titile [GUI_LANGUAGE_NUM]= 
{
	"׼��/����/����",
	"Prepare/set/filamet out"
};

char *const filaUI_stopPirntInfor[GUI_LANGUAGE_NUM] = 
{
	"����ͣ��ӡ,�ٽ�����",
	"please stop printing first,then filament"
};

char *const filaInUI_remidInfor [GUI_LANGUAGE_NUM]= 
{
	"����",
	"feed fila"
};


char *const filaOutUI_remidfor [GUI_LANGUAGE_NUM]= 
{
	"����",
	"output fila"
};

char *const filaFedInUI_remidfor1 [GUI_LANGUAGE_NUM]= 
{
	"�Ĳ��ѽ���",
	"filament fed in again"
};

char *const filaFedInUI_remidfor2 [GUI_LANGUAGE_NUM]= 
{
	"������",
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
	"�Ĳ�ȱʧ,��",
	"filament runs out,please fed "
};

char *const filaRunOut_rmid2[GUI_LANGUAGE_NUM] =
{
	"��������",
	"filament again!"
};
	

char *const filadetctAgain_rmid1[GUI_LANGUAGE_NUM]= 
{
	"�Ĳ�ȱʧ,������",
	"filament detected,please fed"
};

char *const filadetctAgain_rmid2[GUI_LANGUAGE_NUM] = 
{
	"�㹻�ĺĲ�,�ٷ���",
	"enough filament,then back"
};
char *const waitInfor[GUI_LANGUAGE_NUM] =
{
	"���Ե�...",
	"Wait..."

};
char *const filaTempInfor[GUI_LANGUAGE_NUM] =
{
	"����1�¶�:",
	"E1 Temp:"
};

u32 filaOutUI_Color = GRAYBLUE;
u32 timeForRemider;
u32 timeForTemp;
u32 timeForFilaDetect;
u8 pollFilaIOCnt;
float zPositnFila;//���ڽ�����ʱ����z���ƶ�ǰ��λ��
//�Ĳ��Ƿ���ڱ�־,�ϵ�Ĭ�ϺĲĴ���
//���϶�����ɱ�־
//�Ƿ������Ĳ�û���ˣ���ӡ��ͣ���

_fila_state_s filaState = 
{
FALSE,
TRUE,
};
/************************************************/


//icon��·����,HOME����ͼ��
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
	//Ϊ����������������ڴ�
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
	filaUIdev->totalIcon = FILA_ICON_NUM;//�ؼ������Ŀ
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
		
		
		for(j=0;j<filaUIdev->totalIcon;j++)//����ʾ �л���ʾ��ͼ��
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
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_FILA_IN_BUTTON://��

		savedButton = IS_FILA_IN_BUTTON;
		filament_inUI();	

				FILA_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_FILA_OUT_BUTTON://��
	
		savedButton = IS_FILA_OUT_BUTTON;
		filament_outUI();

				FILA_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************��ҳ�ĵڶ���ͼ��****************************/							
			
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
	
		if(savedButton != 0XFF)//����������Ҫ�л�����ͼ�� ��ͼ��ʱ����˸һ�����û�֪����������
		{
		
				gui_fill_rectangle_DMA(filaUIdev->icon[savedButton].x, filaUIdev->icon[savedButton].y, filaUIdev->icon[savedButton].width, filaUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)filaUIdev->icon[savedButton].path, filaUIdev->icon[savedButton].x , filaUIdev->icon[savedButton].y, filaUIdev->icon[savedButton].width, filaUIdev->icon[savedButton].height, filaUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
		if(isOption2moreUiSelected == YES)
		{
			check_is_pause_exacute();//����ӡͷ�Ƿ�ִ�п���ֹͣ
		
		}



	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,filaUIdev->icon);
		myfree(SRAMIN,filaUIdev);

		FILA_DEBUG("free filaUIdev \r\n");
	}


}

//��ʾ
//0--��ӡ�����ڴ�ӡ״̬��1--��ӡ�����ڷǴ�ӡ״̬
u8 StopPrint_remiderUI(void)
{

	uint8_t rtv = 0;

	if(card.FILE_STATE != PS_PIRNT)//�Ƿ��ڴ�ӡ״̬
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

	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//���������ʾ����
	Show_Str(x ,y,lcddev.width,font,(u8 *)filaInUI_remidInfor[LANGUAGE],font,1);	
	y += font + offset;
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//���������ʾ����
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

	
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//���������ʾ����
	Show_Str(x,y,lcddev.width,font,(u8 *)filaOutUI_remidfor[LANGUAGE],font,1);	
	y += font + offset;
	gui_fill_rectangle_DMA(x,y,length,hight,BLACK);//���������ʾ����
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
		Show_Str(x,y,lcddev.width,font,(u8 *)filaTempInfor[LANGUAGE],font,1);//��ʾTemp:

		width = 3*(font/2);
		POINT_COLOR = WHITE;
		y += 2*font;
		gui_fill_rectangle_DMA(x,y,width,hight,BLACK);//������
		LCD_ShowNum(x ,y ,degHotend(0),3,font);//��ʾ����ͷ�¶�ֵ

		POINT_COLOR = YELLOW;
		x+=4*(font/2);
		width = 2*(font/2);
		LCD_ShowString(x,y,width,font,font,(u8 *)"'C");//��ʾ'C
	
		POINT_COLOR = WHITE;
	}
	if(filament_in ==IN_FILA_FINISH || filament_out == OUT_FILA_FINISH)//���������
	{
		filament_in = IN_FILA_NO;
		filament_out = OUT_FILA_NO;
		POINT_COLOR = BROWN;
		
		x = PICTURE_X_SIZE + 16;
		y = BMP_FONT_SIZE +  font + offset;

		length =  11*(font/2);
		gui_fill_rectangle_DMA(x,y,length,font,BLACK);//���������ʾ����
		POINT_COLOR = YELLOW;
		LCD_ShowString(x,y,lcddev.width,font,font,(u8 *)"END");//��ʾ'C
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

	gui_fill_rectangle_DMA(x,y,lcddev.width,hight,BLACK);//�����������
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
	gui_fill_rectangle_DMA(x,y,lcddev.width,hight,BLACK);//�����������
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
	ePos = current_position[E_AXIS];//����e��λ��
	
	//Z���������ƶ�20mm
	zMove_up_filament(PRINT_FILA_IN_Z_UP_LENGTH);
	
	
	filament_in = IN_FILA_START;//��ʼ����
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//����ͷ�¶ȹ���
	{
				filament_in = IN_FILA_PREHEAT;//Ԥ����
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//���ü����¶�
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
						 // if( card.FILE_STATE == PS_STOP) break;//��û��ʼ��ӡʱ���û�ǿ��ȡ����ӡ
						  
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
	

	
	
	//��һ�׶�		
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
	
	//�ڶ��׶�	
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


	filament_in = IN_FILA_FINISH;//�������

	zMove_down_filament(PRINT_FILA_IN_Z_UP_LENGTH);
	
	current_position[E_AXIS] = ePos ;//�ָ�e��λ��
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
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
	
	filament_out = OUT_FILA_START;//��ʼ����
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//����e��λ��
	
	//Z���������ƶ�20mm
	zMove_up_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//����ͷ�¶ȹ���
	{
				filament_out = OUT_FILA_PREHEAT;//Ԥ����
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//���ü����¶�
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
					//	  if( card.FILE_STATE == PS_STOP) break;//��û��ʼ��ӡʱ���û�ǿ��ȡ����ӡ
						  
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
	//��һ�׶�	
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
	
	
//�ڶ��׶�	
	feedrate = PRINT_FILA_OUT_SLOW2_SPEED;
		current_position[E_AXIS] = 1000.0f;//�趨һ��E��λ��		
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] - EXIT_FILAMENT_LENGTH_1;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] -= EXIT_FILAMENT_LENGTH_1;
	st_synchronize_update_temp();
	

//�����׶�	
	preTime = millis();
	while (millis() - preTime < PRINT_FILA_OUT_TIME)//�ȴ�15s���ٿ��ٻس�
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
	

	filament_out = OUT_FILA_FINISH;//��ǽ������

	
	zMove_down_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	

	
	current_position[E_AXIS] = ePos ;//�ָ�e��λ��
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
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

//���Ϻ�����z���ƶ�Ҫ���ʹ�ã��ƶ�����Ҫһ�£���moveMMҪһ��
void zMove_up_filament(u16 moveMM)
{
	u8 i;
	
	

		feedrate = homing_feedrate[X_AXIS];
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[E_AXIS] = current_position[E_AXIS];

		if(current_position[Z_AXIS] >= moveMM  )//z��λ���Ƿ񹻸�
		{
			destination[Z_AXIS] = current_position[Z_AXIS];//��������ǰz��λ��
		}
		else if(current_position[Z_AXIS] < moveMM && current_position[Z_AXIS] < Z_MAX_POS)
		{
			zPositnFila = current_position[Z_AXIS];
			destination[Z_AXIS] = current_position[Z_AXIS] + moveMM;//z������movemm����
			if(destination[Z_AXIS] > Z_MAX_POS) //Z���ƶ�����Χ
			{
				destination[Z_AXIS] = Z_MAX_POS;
				zPositnFila = Z_MAX_POS - current_position[Z_AXIS];//��¼���Ƶľ���
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
		if(current_position[Z_AXIS] - zPositnFila == moveMM)//֮ǰ�ƶ���z��
		{
			destination[Z_AXIS] = zPositnFila ;//z������movemm����
		}
		
		else destination[Z_AXIS] = current_position[Z_AXIS] ;
		
		if(current_position[Z_AXIS] == Z_MAX_POS) //Z���ƶ�����Χ
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
    uint8_t num = FILA_DETECT_UI_NUM;//������

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
		filaTime.filaRunOutContnTime = millis();//��¼��ʱ����ʼʱ��
		filaTime.isTimeOut = FALSE;

		LCD_Clear_DMA(BLACK);//����
		
		titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//��ñ�������ʾ��Ϣ����
		Show_Str(xStart,yStart,width,font,(u8 *)gcode_print_title[LANGUAGE],font,1);	
		fileName = get_current_root_file_name(printFileName);//���gcode�ļ���

		setLength = 30;//������2������������ʾ����Ҫ2���ֽ�
		length = strlen((char *)fileName);
		if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
		{
			dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
			if(dispFileName == NULL)
			{	
				FILA_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
				
			}
			else
			{
				
				strncpy((char *)dispFileName,(const char *)fileName,setLength);
				dispFileName[setLength] = '\0';
				
				Show_Str(xStart + titleLength*(font/2),yStart,width,font,(u8*)dispFileName, font, 1); 
				gui_memin_free(dispFileName);//�ͷ��ڴ�
				
				xStart = 2;
				dispFileName = fileName + setLength;
				Show_Str(xStart,yStart + font, width,font,(u8*)dispFileName, font, 1); 
			}						
		}
		else 
		Show_Str(xStart + titleLength*(font/2),yStart,width,font,(u8*)fileName, font, 1); //��ʾgcode�ļ�

		free_current_root_file_name_memory(fileName);//�ͷ��ڴ�
	
	 	

		//Ϊ����������������ڴ�
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
		filaTime.bck_bedTep = current_temperature_bed;//�����¶�
		filaTime.bck_ex0Tep = current_temperature[0];
	
		sprintf(cmd,"M104 S%.1f",0.0);//ֹͣ����
		 enquecommand(cmd);
		 
		sprintf(cmd,"M140 S%.1f",0.0);
		enquecommand(cmd);			
	}

//���ϼ�⵽����ͣ��ӡ��Һ�������ˢ��
	fila_run_out_pause();	
	

	



//�������
	SELECTED_BUTTON = gui_touch_chk(filaRunOutUIdev);	
	if(SELECTED_BUTTON == FILA_RUN_OUT_BACK_BT)
	{

		free_flag = IS_TRUE;
		if(filaTime.firstSet == TRUE)	{currentMenu = heat_contenue_print_screen;	filaTime.firstSet = FALSE;}//��ʱ����ͷ + �ȴ��Ѿ�����
		else currentMenu = gcode_print_option_screen;
				
		FILA_DEBUG("fila run out back \r\n");
	}
	else if(SELECTED_BUTTON ==FILA_RUN_OUT_STOP_BT)
	{
		free_flag = IS_TRUE;
		currentMenu = stop_screen;			
		FILA_DEBUG("STOP \r\n");
	}
	else if(SELECTED_BUTTON ==FILA_RUN_OUT_ChangeFila_BT)//����
	{
		free_flag = IS_TRUE;
		currentMenu = change_fila_screen;		
		isOption2moreUiSelected = YES;//���뻻�Ͻ����ֱ�ӷ���optionToMore����
		FILA_DEBUG("change fila\r\n");
	}
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,filaRunOutUIdev->icon);
		myfree(SRAMIN,filaRunOutUIdev);
		bsp_ClearKey();//����IO��⻺������
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

	if( state == FILA_STATE_OUT )//����
	{
			if( filaTime.dipFirstRun == 0)
				{
				POINT_COLOR = RED;
				filaTime.dipFirstRun = 1;
				gui_fill_rectangle_DMA(xStart,yStart,lcddev.width,hight,BLACK);//�����������
				Show_Str(xStart,yStart,width,font,(u8 *)filaRunOut_rmid1[LANGUAGE],font,1); 
				Show_Str(xStart,yStart + font,width,font,(u8 *)filaRunOut_rmid2[LANGUAGE],font,1);	

			}
			
			if(abs(millis() - filaTime.filaRunOutContnTime) >= FILA_RUN_OUT_TIME) //��ʱ�����
			{
				filaTime.isTimeOut = TRUE;
			}

		Enable_Beep();
	}
	else if(state == FILA_STATE_FED  )//����
	{

		if(filaTime.dipFirstRun == 1)
			{
			
		POINT_COLOR = BLUE;
		filaTime.dipFirstRun = 0;
		gui_fill_rectangle_DMA(xStart,yStart,lcddev.width,hight,BLACK);//�����������
		Show_Str(xStart,yStart,width,font,(u8 *)filadetctAgain_rmid1[LANGUAGE],font,1);	
		Show_Str(xStart,yStart + font,width,font,(u8 *)filadetctAgain_rmid2[LANGUAGE],font,1);
		
			}
		filaTime.filaRunOutContnTime = millis();//������ʱ����

		Disable_Beep();
	}



	POINT_COLOR = WHITE;
	
}

//������
//����ֵ��1--����,0--����
u8 check_fila_run_out(void)
{
	u8 rtv = 0xff;
	u8 key;
	key = bsp_GetKey(); //��ⰴ��
	if( key == FILA_RUN_OUT_DET)//����
	{
		rtv = 1;
	}
	else if(key == FILA_FED_DET)//����
	{
		rtv = 0;
	}
	return rtv;
}


/******************************����**********************************/
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
	//Ϊ����������������ڴ�
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
	changeFilaUIdev->totalIcon = FILA_ICON_NUM;//�ؼ������Ŀ
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
		
		
		for(j=0;j<changeFilaUIdev->totalIcon;j++)//����ʾ �л���ʾ��ͼ��
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
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_FILA_IN_BUTTON://��

		savedButton = IS_FILA_IN_BUTTON;
		change_filament_inUI();	

				FILA_DEBUG("in fila\r\n");
				
			break;
			
		case IS_FILA_OUT_BUTTON://��
	
		savedButton = IS_FILA_OUT_BUTTON;
		change_filament_outUI();

				FILA_DEBUG("out fila\r\n");
				
			break;			
			
/****************************��ҳ�ĵڶ���ͼ��****************************/							
			
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
	
		if(savedButton != 0XFF)//����������Ҫ�л�����ͼ�� ��ͼ��ʱ����˸һ�����û�֪����������
		{
		
				gui_fill_rectangle_DMA(changeFilaUIdev->icon[savedButton].x, changeFilaUIdev->icon[savedButton].y, changeFilaUIdev->icon[savedButton].width, changeFilaUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)changeFilaUIdev->icon[savedButton].path, changeFilaUIdev->icon[savedButton].x , changeFilaUIdev->icon[savedButton].y, changeFilaUIdev->icon[savedButton].width, changeFilaUIdev->icon[savedButton].height, changeFilaUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
		if(isOption2moreUiSelected == YES)//��ͣ/��ʼ��������
		{
			check_is_pause_exacute();//����ӡͷ�Ƿ�ִ�п���ֹͣ
		
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
	ePos = current_position[E_AXIS];//����e��λ��
	
	//Z���������ƶ�20mm
	FILA_DEBUG("Zup\r\n");
	zMove_up_filament(CHANGE_FILA_IN_Z_UP_LENGTH);
	
	FILA_DEBUG("extur1CurTemp = %f, extru1TarTemp = %f\r\n", extur1CurTemp,extru1TarTemp );
	filament_in = IN_FILA_START;//��ʼ����
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//����ͷ�¶ȹ���
	{
		FILA_DEBUG("heating...\r\n");
				filament_in = IN_FILA_PREHEAT;//Ԥ����
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//���ü����¶�
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
					//	  if( card.FILE_STATE == PS_STOP) break;//��û��ʼ��ӡʱ���û�ǿ��ȡ����ӡ
						  
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
	

	
	
	//��һ�׶�	
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
	
	//�ڶ��׶�	
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
	
	setTargetHotend(savedExTemp, active_extruder);//�ָ�����ǰ���¶�ֵ

	filament_in = IN_FILA_FINISH;//�������
	zMove_down_filament(CHANGE_FILA_IN_Z_UP_LENGTH);
	
	current_position[E_AXIS] = ePos ;//�ָ�e��λ��
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
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
	
	filament_out = OUT_FILA_START;//��ʼ����
	st_synchronize_update_temp();
	ePos = current_position[E_AXIS];//����e��λ��
	
	//Z���������ƶ�20mm
	zMove_up_filament(PRINT_FILA_OUT_Z_UP_LENGTH);
	
	if(extur1CurTemp < USER_FILAMENT_MINI_TEMP || extru1TarTemp < USER_FILAMENT_MINI_TEMP)//����ͷ�¶ȹ���
	{
				filament_out = OUT_FILA_PREHEAT;//Ԥ����
				setTargetHotend(ENTER_EXIT_FILAMENT_TEMP, tmp_extruder);//���ü����¶�
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
					//	  if( card.FILE_STATE == PS_STOP) break;//��û��ʼ��ӡʱ���û�ǿ��ȡ����ӡ
						  
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
	
	
	//��һ�׶�
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
//�ڶ��׶�
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
//�����׶�
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
	
	setTargetHotend(savedExTemp, active_extruder);//�ָ�����ǰ����ͷ�¶�
	


	filament_out = OUT_FILA_FINISH;//��ǽ������
	zMove_down_filament(PRINT_FILA_OUT_Z_UP_LENGTH);//Z���½�������ǰ�߶�
	
	
	
	current_position[E_AXIS] = ePos ;//�ָ�e��λ��
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
	 destination[E_AXIS]  = current_position[E_AXIS];
	
	feedrate = saved_feedrate;
}

void heat_until_targetTem(void)
{
	char cmd[12];
	if(filaTime.firstRun == FALSE)
	{
	filaTime.firstRun = TRUE;
	sprintf(cmd,"M104 S%.1f",filaTime.bck_ex0Tep);//ֹͣ����
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
		LCD_Clear_DMA(BLACK);//����
		
		
		Show_Str(xStart,yStart,width,font,(u8 *)filaFedInUI_remidfor1[LANGUAGE],font,1);	
		POINT_COLOR = RED;
		Show_Str(xStart,yStart + 2*font,width,font,(u8 *)filaFedInUI_remidfor2[LANGUAGE],font,1);	
		POINT_COLOR = WHITE;
				//Ϊ����������������ڴ�
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
		gui_fill_rectangle_DMA(xStart ,yStart ,6*(font/2),font,BLACK);//���������ʾ����
		POINT_COLOR = BLUE;
		
		
		switch(filaTime.dispState)
		{
			case 0 :
			Show_Str(xStart,yStart ,width,font,(u8 *)"..",font,1);//��ʾһ��С����	
			filaTime.dispState += 1;
			break;
			
			case 1 :
			Show_Str(xStart,yStart ,width,font,(u8 *)"....",font,1);//��ʾһ��С����	
			filaTime.dispState += 1;
			break;
			
			case 2 :
			Show_Str(xStart,yStart,width,font,(u8 *)"......",font,1);//��ʾһ��С����	
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
//��ʼ��ӡ������ʾ
void heat_end_exacute(void)
{
	float ePos;
	float extur1CurTemp;
	float extru1TarTemp;
	extur1CurTemp = degHotend(0);
	extru1TarTemp = degTargetHotend(0);
	if(extur1CurTemp > filaTime.bck_ex0Tep - TEP_OFFSET && extru1TarTemp > filaTime.bck_ex0Tep - TEP_OFFSET)//
	{
	//��һ�׶�		
	FILA_DEBUG("$# fila in\r\n");
	ePos = current_position[E_AXIS];//����e��λ��
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
	
	//�ڶ��׶�	
	feedrate = PRINT_FILA_IN_SLOW_SPEED;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		destination[E_AXIS] = current_position[E_AXIS] + SLOW_ENTER_FILAMENT_LENGTH;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] += SLOW_ENTER_FILAMENT_LENGTH;
	st_synchronize_no_lcd();

	
	
	current_position[E_AXIS] = ePos ;//�ָ�e��λ��
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
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
	
