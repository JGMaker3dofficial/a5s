/*
*********************************************************************************************************
*
*	ģ������ :Ԥ�Ƚ���ģ��
*	�ļ����� : preheat.c
*	��    �� : V1.0
*	˵    �� : Ԥ�Ƚ���
*
*
*********************************************************************************************************
*/


/**********************����ͷ�ļ�*************************/
#include "extruder.h"


#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"
/**********************���Ժ궨��**************************/

#define EXTRUDERUI_DEBUG_EN

#ifdef EXTRUDERUI_DEBUG_EN

#define EXTRUDERUI_DEBUG	DBG

#else
#define EXTRUDERUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define	FILA_IN_OUT_TEMP		185
#define	SET_FILA_IN_OUT_TEMP	(FILA_IN_OUT_TEMP + 2)

/***********************˽�к�*************************/



/************************************************/
#define EXTRUDER_FILA_STEP_ICON 		2
#define EXTRUDER_FILA_SPEED_ICON			2
#define EXTRUDER_MENU_TOTAL_BMP_NUM			10
#define EXTRUDER_MENU_VALID_BMP_NUM  	(EXTRUDER_MENU_TOTAL_BMP_NUM - EXTRUDER_FILA_STEP_ICON - EXTRUDER_FILA_SPEED_ICON)

#define EXTRUDER_MENU_FIRST_ROW_BMP_NUM			2
#define EXTRUDER_MENU_SECOND_ROW_BMP_NUM		4


//������ ������
_gui_dev *extruderUIdev;

static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������
static uint8_t  filament_step_count = 0;//�����ϲ���������
static uint8_t  filament_speed_count = 0;//�������ٶȲ���������

static int32_t	filament_length_count = 0;//���ϳ��ȼ�����

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


//icon��·����,HOME����ͼ��
char *const extruUI_icon_path_tbl[GUI_LANGUAGE_NUM][EXTRUDER_MENU_TOTAL_BMP_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",//0
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",//1	  
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",//2	    
	"1:/SYSTEM/PICTURE/ch_bmp_step1_mm.bin",//3	    
	"1:/SYSTEM/PICTURE/ch_bmp_speed_slow.bin",//4	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//5    
   
   "1:/SYSTEM/PICTURE/ch_bmp_step5_mm.bin",//�����ϲ��� 6
   "1:/SYSTEM/PICTURE/ch_bmp_step10_mm.bin",//�����ϲ��� 7
   
   "1:/SYSTEM/PICTURE/ch_bmp_speed_normal.bin",//�������ٶȲ��� 8
   "1:/SYSTEM/PICTURE/ch_bmp_speed_high.bin",//�������ٶȲ��� 9
   },
   {
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",//0
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",//1	  
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",//2	    
	"1:/SYSTEM/PICTURE/en_bmp_step1_mm.bin",//3	    
	"1:/SYSTEM/PICTURE/en_bmp_speed_slow.bin",//4	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//5    
   
   "1:/SYSTEM/PICTURE/en_bmp_step5_mm.bin",//�����ϲ��� 6
   "1:/SYSTEM/PICTURE/en_bmp_step10_mm.bin",//�����ϲ��� 7
   
   "1:/SYSTEM/PICTURE/en_bmp_speed_normal.bin",//�������ٶȲ��� 8
   "1:/SYSTEM/PICTURE/en_bmp_speed_high.bin",//�������ٶȲ��� 9   
   }
};  

char *const EXTRUUI_temp_extu1[GUI_LANGUAGE_NUM] = 
{
	"����",
	"Extru:"
};

char *const	EXTRUUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"׼��/������",
	"Prepare/filament"
};
char *const tempInfor[GUI_LANGUAGE_NUM] = 
{
	"�¶�:",
	"Temp:"
};

char *const	filaHeat[GUI_LANGUAGE_NUM] = 
{
	"������...",
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
_lenght_unit_e LENGTH_UINT_E = UNIT_MM;//�����ϳ��ȵ�λ

//��ʾ		EXTU1             
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


	if( millis() > display_next_update_millis)//500ms ����һ��
	{
		display_next_update_millis = millis() + 250;

 
		POINT_COLOR = BLUE;
		BACK_COLOR = BLACK;
				
			xOffSet = 2*PICTURE_X_SIZE - 6*font/4;
			
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)EXTRUUI_temp_extu1[LANGUAGE],font,1);		

			width = 4*(font/2);
			hight = font;
		gui_fill_rectangle_DMA(xOffSet + 6*(font/2),yOffSet,width,hight,BLACK);//������ɫ��
		
		POINT_COLOR = YELLOW;
//��ʾ��λ
		if(unit == UNIT_MM)
		{
			setLength = filamentLength;
			if(setLength < 0)//
			{
				savedLenght = -1*setLength;
				LCD_ShowString(xOffSet + 6*(font/2),yOffSet,lcddev.width,font,font,(u8 *)"-");	//display "-"
				LCD_ShowNum(xOffSet + 6*(font/2) + (font/2),yOffSet,savedLenght,3,font);//��ʾ���ϳ���
				LCD_ShowString(xOffSet + 6*(font/2) + 4*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_mm);		
			}
			else
			{
				LCD_ShowNum(xOffSet + 6*(font/2),yOffSet,setLength,3,font);//��ʾ���ϳ���	
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
				LCD_ShowNum(xOffSet + 6*(font/2) + (font/2),yOffSet,savedLenght,3,font);//��ʾ���ϳ���
			  LCD_ShowString(xOffSet + 6*(font/2) + 4*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_cm);	

			}
			else
			{
			LCD_ShowNum(xOffSet + 6*(font/2),yOffSet,setLength,3,font);//��ʾ���ϳ���
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
			if(heatSt.setHeatCMD == TRUE && heatSt.dispFlag == FALSE)//�ѷ��ͼ�������
			{
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width*(font/2),hight,BLACK);//������ɫ��
			POINT_COLOR = RED;
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)filaHeat[LANGUAGE],font,1);//��ʾ����
			EXTRUDERUI_DEBUG("heat\r\n");			
			heatSt.dispFlag = TRUE;
			}
		}
		else if(heatSt.dispFlag == TRUE && current_temperature[0] >= FILA_IN_OUT_TEMP)
		{
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width*(font/2),hight,BLACK);//������ɫ��
			clear_heatSt();
			EXTRUDERUI_DEBUG("heat OK\r\n");		
		}
		
		yOffSet  += 2*font;
		Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)tempInfor[LANGUAGE],font,1);//��ʾTemp:
		
		POINT_COLOR = YELLOW;
		width = 3*(font/2);
		gui_fill_rectangle_DMA(xOffSet + 5*(font/2),yOffSet,width,hight,BLACK);//������ɫ��
		LCD_ShowNum(xOffSet + 5*(font/2),yOffSet,currentTemp,3,font);//��ʾ��ǰ��������¶�ֵ
		LCD_ShowString(xOffSet + 5*(font/2) + 3*(font/2),yOffSet,lcddev.width,font,font,(u8 *)unit_temp);//��ʾ'C

		POINT_COLOR = BLUE;
	}
	
}

//��ʾ��������λ��
static void extruUI_set_filament(_filament_option_e filamentOpt, _filament_distn_step_e distnStep,_filament_speed_step_e speedStep)
{
	int32_t saved_distance;
	float  extu1Temp = current_temperature[0];
	
	switch (filamentOpt)//��������
	{
		case FILAMENT_IN_OPT:
		switch (distnStep)//���벽��
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
		if(filament_length_count >= 99900)//����9m
		filament_length_count = 99900;

		

			break;
			
		case FILAMENT_OUT_OPT:
		switch (distnStep)//��������
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
		if(filament_length_count <= -9900)//����99cm
		filament_length_count = -9900;	
		

			break;
			
	case FILAMENT_CLEAR_OPT:
	filament_length_count = 0;

	break;
	
		default:
			break;
	}
	


	
	if(filament_length_count >= 1000 || filament_length_count <= -1000)//����1000mm->��λת��mm->cm
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
	//Ϊ����������������ڴ�
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
	extruderUIdev->totalIcon = EXTRUDER_MENU_VALID_BMP_NUM;//�ؼ������Ŀ
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
	//��ʼ��step5 ��step10 icons	
	for(j=IS_EXTRUDER_FILA_STEP5MM_ICON; j < IS_EXTRUDER_FILA_STEP5MM_ICON + EXTRUDER_FILA_STEP_ICON; j++)
	{
			extruderUIdev->icon[j].x = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].x;
			extruderUIdev->icon[j].y = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].y;
			extruderUIdev->icon[j].width = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].width;
			extruderUIdev->icon[j].height = extruderUIdev->icon[IS_EXTRUDER_FILA_STEP1MM_ICON].height;
			
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];
			
	}
	
	//��ʼ��speed_nor ��speed_fast icons	
	for(j=IS_EXTRUDER_FILA_SPEED_NORMAL_ICON; j < IS_EXTRUDER_FILA_SPEED_NORMAL_ICON + EXTRUDER_FILA_SPEED_ICON; j++)
	{
			extruderUIdev->icon[j].x = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].x;
			extruderUIdev->icon[j].y = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].y;
			extruderUIdev->icon[j].width = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].width;
			extruderUIdev->icon[j].height = extruderUIdev->icon[IS_EXTRUDER_FILA_SPEED_SLOW_ICON].height;
			
			extruderUIdev->icon[j].path = (u8*)extruUI_icon_path_tbl[LANGUAGE][j];
			
	}		
		
		
		for(j=0;j<extruderUIdev->totalIcon;j++)//����ʾ �л���ʾ��ͼ��
		{
			if(j == IS_EXTRUDER_FILA_STEP1MM_ICON)//�����ϲ�������
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
			else if(j == IS_EXTRUDER_FILA_SPEED_SLOW_ICON)//�����ϲ����ٶ�
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
*	�� �� ��: extru_screen
*	����˵��: Ԥ�����ý���
*	��    ��: ��
*	�� �� ֵ: ��
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
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_EXTRUDER_IN_ICON://����
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_IN_ICON;
		saveButton = FILAMENT_OPT_E = FILAMENT_IN_OPT;
		isTrigedCmd = 1;
				EXTRUDERUI_DEBUG("filament in\r\n");
				
			break;
			
		case IS_EXTRUDER_OUT_ICON://����
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_OUT_ICON;
		saveButton = FILAMENT_OPT_E =FILAMENT_OUT_OPT;	
		isTrigedCmd = 1;
		EXTRUDERUI_DEBUG("filament out\r\n");
				
			break;			
			
/****************************��ҳ�ĵڶ���ͼ��****************************/							
		case IS_EXTRUDER_CLEAR_ICON://����������
		
		EXTRU_SAVED_BUTTON = IS_EXTRUDER_CLEAR_ICON;
		FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;	
				EXTRUDERUI_DEBUG("extruder screen\r\n");
				
			break;			
			
		case IS_EXTRUDER_FILA_STEP1MM_ICON://�����Ͼ��벽��
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
			

		case IS_EXTRUDER_FILA_SPEED_SLOW_ICON://�������ٶȲ���
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
		
		
		FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;	//�����Ͻ����˳�������֮ǰ���ϳ���
		free_flag = IS_TRUE;
				EXTRUDERUI_DEBUG("extru back\r\n");
			break;
			
		
/********************************************************/
			
		default:
			break;
	}
		if(EXTRU_SAVED_BUTTON == IS_EXTRUDER_IN_ICON || EXTRU_SAVED_BUTTON == IS_EXTRUDER_OUT_ICON)
		{
			if(current_temperature[0] < FILA_IN_OUT_TEMP )//�¶ȵ����趨�¶�
			{
				if( heatSt.setHeatCMD == FALSE)
				{
				heatSt.setHeatCMD = TRUE;
				sprintf(TempBuffer, "M104 S%d", SET_FILA_IN_OUT_TEMP);
				menu_action_gcode(TempBuffer);
				}
				FILAMENT_OPT_E =FILAMENT_CLEAR_OPT;//��ǰ���üӼ�������Ч
			}
		}
		
		if(EXTRU_SAVED_BUTTON != NONE_EXTRUDER_ICON)//����������Ҫ�л�����ͼ�� ��ͼ��ʱ����˸һ�����û�֪����������
		{
			gui_fill_rectangle_DMA(extruderUIdev->icon[EXTRU_SAVED_BUTTON].x, extruderUIdev->icon[EXTRU_SAVED_BUTTON].y, extruderUIdev->icon[EXTRU_SAVED_BUTTON].width, extruderUIdev->icon[EXTRU_SAVED_BUTTON].height,BLACK);				
			display_bmp_bin_to_LCD((u8*)extruderUIdev->icon[EXTRU_SAVED_BUTTON].path, extruderUIdev->icon[EXTRU_SAVED_BUTTON].x , extruderUIdev->icon[EXTRU_SAVED_BUTTON].y, extruderUIdev->icon[EXTRU_SAVED_BUTTON].width, extruderUIdev->icon[EXTRU_SAVED_BUTTON].height, extruderUIdev->icon[EXTRU_SAVED_BUTTON].color);
			EXTRU_SAVED_BUTTON = NONE_EXTRUDER_ICON;
		}



	extruUI_set_filament(FILAMENT_OPT_E,FILAMENT_DISTN_E,FILAMENT_SPEED_E);//��ʾ���õĽ����ϳ���
	if(isTrigedCmd ==1)
	{
		isTrigedCmd = 0;
		exacute_manualFilament(saveButton,FILAMENT_SPEED_E,FILAMENT_DISTN_E);//����gcode����,����������ǰ��FILAMENT_OPT_E������extruUI_set_filament�б�����
	}

	FILAMENT_OPT_E = NONE_FILAMENT_OPT;//�������������,��ֹ��ʾ���������λ��һֱ��/��



	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,extruderUIdev->icon);
		myfree(SRAMIN,extruderUIdev);
		EXTRUDERUI_DEBUG("free extruderUIdev \r\n");
		clear_heatSt();
	}


}

//������
void exacute_manualFilament(u8 dirId, u8 speedID, u8 lengthID)
{



	u8 filaLength = 0;
	char data[14];
//�жϼ���ͷ�Ƿ���ȵ�190��
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



