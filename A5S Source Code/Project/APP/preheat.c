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

#include "preheat.h"

#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

/**********************���Ժ궨��**************************/

#define PREHEATUI_DEBUG_EN

#ifdef PREHEATUI_DEBUG_EN

#define PREHEATUI_DEBUG	DBG

#else
#define PREHEATUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/




/***********************˽�к�*************************/
#define PREHEAT_SET_BG_COLOR	GRAY//���ý��汳����ɫ
#define PREHEAT_SET_FONT_COLOR	BLACK//���ð�ť������ɫ


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



//������ ������
_gui_dev *preheatUIdev;

static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������
static uint8_t  heat_step_count = 0;//���Ƚ��ݼ�����
static uint8_t  extruder_bed_count = 0;//�л�����ͷ/�ȴ�������

static uint8_t firstDispFlag = FALSE;//����Ԥ�Ƚ��棬��δ�����Ӽ�����ʱ����ʾ0�¶�
static u16 dispCurTempValue;//ÿ��2s��ʾһ�ε�ǰ�¶�ʱ�Ļ���


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


//icon��·����,HOME����ͼ��
char *const preheatUI_icon_path_tbl[GUI_LANGUAGE_NUM][PREHEAT_MENU_TOTAL_BMP_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",	  
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_step1_degree.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    
   
   "1:/SYSTEM/PICTURE/ch_bmp_bed.bin",//����ͷ���ȴ��л�,6
   "1:/SYSTEM/PICTURE/ch_bmp_step5_degree.bin",//���ȵ��л�,7
   "1:/SYSTEM/PICTURE/ch_bmp_step10_degree.bin",//���ȵ��л�,8
   },
   {
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",	  
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_step1_degree.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	    
   
   "1:/SYSTEM/PICTURE/en_bmp_bed.bin",//����ͷ���ȴ��л�,6
   "1:/SYSTEM/PICTURE/en_bmp_step5_degree.bin",//���ȵ��л�,7
   "1:/SYSTEM/PICTURE/en_bmp_step10_degree.bin",//���ȵ��л�,8   
   }
};  

char *const infor_temp_bed[GUI_LANGUAGE_NUM] = 
{
	"�ȴ�:",
	"BED:"
};

char *const infor_temp_extu1[GUI_LANGUAGE_NUM] = 
{
	"����ͷ1:",
	"EXTU1:"
};
char *const preheatUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"׼��/Ԥ��",
	"Prepare/preheat"
};

char *const infor_temp_error[GUI_LANGUAGE_NUM] = 
{
	"���������쳣",
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


_obj_option_e OPT_INFOR_E = NONE_OPT;//�Ӽ�����
_obj_updt_id_e UPDT_INFOR_E = EXTRUDER1_OBJ_ID;//��������
_temp_updt_step_e TEMP_UPDT_STEP_E = TEMP_STEP1C;//��������

static void Send_Restart_Move_Cmd(void);


//��ʾ		EXTU1             BED     
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


	if( millis() > display_next_update_millis)//500ms ����һ��
	{
		display_next_update_millis = millis() + 250;
		POINT_COLOR = BLUE;
		BACK_COLOR = BLACK;
		
		xOffSet = 2*PICTURE_X_SIZE - font;
		yOffSet = BMP_FONT_SIZE + 16;
		
		hight = font;
		width = 8*(font/2);
		
		gui_fill_rectangle_DMA(xOffSet,yOffSet,width,hight,BLACK);//������ɫ��
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
			
			if(curTempValue > TempSense_HIGHT_VALUE || curTempValue < TempSense_LOW_VALUE)//���������Ƿ��й����ж�
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

			if(curTempValue > TempSense_HIGHT_VALUE || curTempValue < TempSense_LOW_VALUE)//���������Ƿ��й����ж�
				{
				curTempError = 2;
				firstDispFlag = FALSE;
				}
			else curTempError = 0;
			
			Show_Str(xOffSet,yOffSet,lcddev.width,font,(u8 *)infor_temp_extu1[LANGUAGE],font,1);		
		}
		
		if(curTempError )//��ʾ�������������Ϣ
			{
			POINT_COLOR = RED;
			xOffSet = PICTURE_X_SIZE + 2;
			yOffSet += 2*font;
			
			width = (font/2)*(strlen((char *)infor_temp_error[LANGUAGE]));
			gui_fill_rectangle_DMA(xOffSet,yOffSet,width,hight,BLACK);//������ɫ��

			Show_Str(xOffSet,yOffSet,width,font,(u8 *)infor_temp_error[LANGUAGE],font,1);		

			
			POINT_COLOR = BLUE;
		}
		else
		{
			
			POINT_COLOR = WHITE;
			yOffSet += 2*font;

			
			Xstart = PICTURE_X_SIZE + 2;
			width = (font/2)*(strlen((char *)infor_temp_error[LANGUAGE]));//������������쳣��ʾ��Ϣ
			gui_fill_rectangle_DMA(Xstart,yOffSet,width,hight,BLACK);//������ɫ��
			
			
			if( millis() > display_CurTemp_millis)//2s ����һ��
			{
				display_CurTemp_millis = millis() + 2000;
				dispCurTempValue = curTempValue;
				
			}
			LCD_ShowNum(xOffSet,yOffSet,dispCurTempValue,3,font);//��ʾ��ǰ�¶�ֵ

			POINT_COLOR = BLUE;
			xOffSet  += (font/2)*3;
			LCD_ShowString(xOffSet,yOffSet,font/2,font,font,(u8 *)"/");//��ʾ'/'

			POINT_COLOR = WHITE;
			xOffSet  += (font/2);
			LCD_ShowNum(xOffSet,yOffSet,setTempValue,3,font);//��ʾ�趨�¶�ֵ
			Send_Restart_Move_Cmd();
			
		}
	}

	
}


static void preheatUI_set_temp(_obj_updt_id_e obj_id, _obj_option_e opt,_temp_updt_step_e step)
{

	 char TempBuffer[32];
	 float tarEx0Temp;
	 float tarBedTemp;
	switch (obj_id)//��������
	{
		case EXTRUDER1_OBJ_ID:
			if(degHotend0() > TempSense_HIGHT_VALUE || degHotend0() < TempSense_LOW_VALUE)//�ж����������¶��Ƿ��쳣
				{
				PREHEATUI_DEBUG("#1sensor error\r\n");
				firstDispFlag = FALSE;
				break;
			}
			tarEx0Temp = degTargetHotend(0);
			if(tarEx0Temp > PRESET_TEMP)	set_value[E1_ID] = tarEx0Temp;
			
		switch (step)//��������
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
			if(degBed() > TempSense_HIGHT_VALUE || degBed() < TempSense_LOW_VALUE)//�ж����������¶��Ƿ��쳣
				{
				PREHEATUI_DEBUG("#2sensor error\r\n");
				firstDispFlag = FALSE;
				break;	
				}
			tarBedTemp = degTargetBed();
			if(tarBedTemp > PRESET_TEMP)	set_value[BED_ID] = tarBedTemp;
		switch (step)//��������
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

	
	OPT_INFOR_E = NONE_OPT;//�������ñ�־

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
	//Ϊ����������������ڴ�
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
	preheatUIdev->totalIcon = PREHEAT_MENU_VALID_BMP_NUM;//�ؼ������Ŀ
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
		

		j = 6;//�ȴ�
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
		
		
		for(j=0;j<preheatUIdev->totalIcon;j++)//����ʾ �л���ʾ��ͼ��
		{
			if(j == IS_PREHEAT_EXTRUDER1_ICON)//����ͷ/�ȴ�
			{
				if(extruder_bed_count == 1)//��ʾ�ȴ�
				{
					display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_BED_ICON].path, preheatUIdev->icon[IS_PREHEAT_BED_ICON].x , preheatUIdev->icon[IS_PREHEAT_BED_ICON].y, preheatUIdev->icon[IS_PREHEAT_BED_ICON].width, preheatUIdev->icon[IS_PREHEAT_BED_ICON].height, preheatUIdev->icon[IS_PREHEAT_BED_ICON].color);
				}
				
				else if(extruder_bed_count == 0)//��ʾ����ͷ
				{
				//display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].path, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].x , preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].y, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].width, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].height, preheatUIdev->icon[IS_PREHEAT_EXTRUDER1_ICON].color);
				display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[j].path, preheatUIdev->icon[j].x , preheatUIdev->icon[j].y, preheatUIdev->icon[j].width, preheatUIdev->icon[j].height, preheatUIdev->icon[j].color);
				}
			}
			else if(j == IS_PREHEAT_HEAT_STEP_ICON)//���Ƚ���
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
*	�� �� ��: preheat_set_screen
*	����˵��: Ԥ�����ý���
*	��    ��: ��
*	�� �� ֵ: ��
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
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_PREHEAT_ADD_ICON://��
		OPT_INFOR_E = ADD_OPT;
		PREHEAT_SAVED_BUTTON = IS_PREHEAT_ADD_ICON;

		gui_fill_rectangle_DMA(preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height,BLACK);
		saveCodeID = UPDT_INFOR_E;
				
				PREHEATUI_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_PREHEAT_DEC_ICON://��
		OPT_INFOR_E = DEC_OPT;
		PREHEAT_SAVED_BUTTON = IS_PREHEAT_DEC_ICON;

		gui_fill_rectangle_DMA(preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height,BLACK);
		saveCodeID = UPDT_INFOR_E;
		
				PREHEATUI_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************��ҳ�ĵڶ���ͼ��****************************/							
		case IS_PREHEAT_EXTRUDER1_ICON://����ͷ/�ȴ�
		
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
			
		case IS_PREHEAT_HEAT_STEP_ICON://�¶Ȳ�������
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
			

		case IS_PREHEAT_CLEAR_ICON://��������ֵ
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
			if(1 == extruder_bed_count)//�ȴ�
			{
				if(PREHEAT_SAVED_BUTTON == IS_PREHEAT_ADD_ICON || PREHEAT_SAVED_BUTTON ==IS_PREHEAT_DEC_ICON)
				firstDispFlag |= DISP_BED_BIT;
			}
			else if( 0 == extruder_bed_count )//����ͷ
			{
				if(PREHEAT_SAVED_BUTTON == IS_PREHEAT_ADD_ICON || PREHEAT_SAVED_BUTTON ==IS_PREHEAT_DEC_ICON)
				firstDispFlag |= DISP_EX0_BIT;	
				
			}
		}	
	
		if(PREHEAT_SAVED_BUTTON != NONE_PREHEAT_ICON)//����������Ҫ�л�����ͼ�� ��ͼ��ʱ����˸һ�����û�֪����������
		{
		
			display_bmp_bin_to_LCD((u8*)preheatUIdev->icon[PREHEAT_SAVED_BUTTON].path, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].x , preheatUIdev->icon[PREHEAT_SAVED_BUTTON].y, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].width, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].height, preheatUIdev->icon[PREHEAT_SAVED_BUTTON].color);
			PREHEAT_SAVED_BUTTON = NONE_PREHEAT_ICON;
		}

//display temperature

preheatUI_set_temp(saveCodeID,OPT_INFOR_E,TEMP_UPDT_STEP_E);
preheatUI_display_Temp(UPDT_INFOR_E,current_temperature_bed,current_temperature[0]);

	if(isOption2moreUiSelected == YES )//ȷ����ӡ�����Ȱ�����ͣ���ٽ����ҳ��ż���ӡͷ����ֹͣ
	{
		check_is_pause_exacute();//����ӡͷ�Ƿ�ִ�п���ֹͣ

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







