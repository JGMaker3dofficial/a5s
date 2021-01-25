
/*
*********************************************************************************************************
*
*	ģ������ : ������
*	�ļ����� : mainui.c
*	��    �� : V1.0
*	˵    �� : ��ӡ�������棬��10��ͼ�� + һ���ȴ�bmp + һ���ȼ���ͷbmp ��ɡ�
*
*
*********************************************************************************************************
*/



#include "mainui.h"

/**********************���Ժ궨��**************************/

#define MAINUI_DEBUG_EN

#ifdef MAINUI_DEBUG_EN

	#define MAINUI_DEBUG	DBG

#else
	#define MAINUI_DEBUG(format,...)	((void) 0)	

#endif

#define NUM_ROW		2	//������ͼ��
#define ROW_SIZE	4	//ÿ��ͼ����ٸ�

/************************************************/

typedef enum 
{
	NO_SELECTED_ICON		= 0X3F,
	IS_PREHEAT_ICON			= 0,
	IS_MOVING_ICON 			= 1,
	IS_HOME_ICON			= 2,
	IS_PRINT_ICON			= 3,
	IS_EXTRUDE_ICON			= 4,
	IS_ZLEVEL_ICON			= 5,
	IS_SETTING_ICON			= 6,
	IS_MORE_ICON			= 7,
	
}_mainUI_button_e;



//������ ������
_gui_dev *muidev;

static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������


/************************************************/


char *const mianUITiltle[GUI_LANGUAGE_NUM] = 
{
	"׼��",
	"Prepare"
};

/**********************ͼƬ�洢Ŀ¼����******************/
//icon��·����,HOME����ͼ��
char *const mui_icon_path_tbl[GUI_LANGUAGE_NUM][HOME_BMP_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_mov.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zero.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_printing.bin",	    
	
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_leveling.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_set.bin",	    
	
	"1:/SYSTEM/PICTURE/ch_bmp_language.bin",
 
	  

	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/en_bmp_mov.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zero.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_printing.bin",	    
	
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_leveling.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_set.bin",	    

	"1:/SYSTEM/PICTURE/en_bmp_language.bin",

	}
};  


typedef enum 
{
	POS_BASE = 0XFF,
	POS_PREHEAT = 0,
	POS_MOVE ,
	POS_HOME,
	POS_PRINT,
	
	POS_EXTRUDER,
	POS_LEVEL,
	POS_SET,
	POS_MORE,
}_menu_position_e;

_menu_position_e CURRENT_MENU_POS = POS_BASE;






/*
*********************************************************************************************************
*	�� �� ��: mui_init
*	����˵��: ��ʼ������ͼ��������Ϣ������ģʽѡ���Ƿ���ʾͼ��
*	��    ��:  mode
mode @0-----ֻ��ʼ��ͼ�������Ϣ������ʾͼ��
mode @1-----��ʼ��ͼ�������Ϣ����ʾͼ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void mui_init(void)
{
	uint16_t i,j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	
	LCD_Clear_DMA(BLACK);

	Show_Str(2,5,lcddev.width,font,(u8 *)mianUITiltle[LANGUAGE],font,1);	
	//Ϊ����������������ڴ�
	muidev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( muidev == 0)
	{
		MAINUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	muidev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*HOME_BMP_NUMS);
	
	if( muidev->icon == 0)
	{
		MAINUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	muidev->status = 0x3F ;
	
	
	for(i=0;i<NUM_ROW;i++)
	{
		for(j=0;j<ROW_SIZE;j++)
		{
			
			muidev->icon[i*ROW_SIZE+j].x = bmp_sart_offx + j*bmp_between_offx  ;
			muidev->icon[i*ROW_SIZE+j].y = bmp_sart_offy + i*bmp_between_offy  ;
			muidev->icon[i*ROW_SIZE+j].width = PICTURE_X_SIZE;
			muidev->icon[i*ROW_SIZE+j].height = PICTURE_Y_SIZE;
			muidev->icon[i*ROW_SIZE+j].path = (u8*)mui_icon_path_tbl[LANGUAGE][i*ROW_SIZE+j];

		}
	} 
	muidev->totalIcon = HOME_BMP_NUMS;
		for(j=0;j<muidev->totalIcon;j++)
		{
			display_bmp_bin_to_LCD((u8*)muidev->icon[j].path, muidev->icon[j].x , muidev->icon[j].y, muidev->icon[j].width, muidev->icon[j].height, muidev->icon[j].color);
		}	
	
}


/*
*********************************************************************************************************
*	�� �� ��: mui_load_icon
*	����˵��: ��������ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void mui_load_icon(void)
{
	uint8_t i,j; 
	for(i=0;i<NUM_ROW;i++)
	{
		for(j=0;j<ROW_SIZE;j++)
		{
			display_bmp_bin_to_LCD((u8*)muidev->icon[i*ROW_SIZE+j].path, muidev->icon[i*ROW_SIZE+j].x , muidev->icon[i*ROW_SIZE+j].y, muidev->icon[i*ROW_SIZE+j].width, muidev->icon[i*ROW_SIZE+j].height, muidev->icon[i*ROW_SIZE+j].color);
		}
	}  	
} 

/*
*********************************************************************************************************
*	�� �� ��: mui_touch_chk
*	����˵��: �����津�����
*	��    ��:  ��
*	�� �� ֵ: �����µ�ͼ���
@0~0x3f ��Χ
*********************************************************************************************************
*/    
uint8_t mui_touch_chk(void)
{		 
	uint8_t i=0xff;																 
	if(tp_dev.sta&TP_PRES_DOWN)//�а���������
	{
		muidev->status|=0X80;	//�������Ч����
		muidev->tpx=tp_dev.x[0];
		muidev->tpy=tp_dev.y[0];
	}
	else if(muidev->status&0X80)//�����ɿ���,��������Ч����
	{
		
		for(i=0;i<HOME_BMP_NUMS;i++)
		{
			if((muidev->tpx > muidev->icon[i].x) && (muidev->tpx < muidev->icon[i].x + muidev->icon[i].width) && (muidev->tpy > muidev->icon[i].y)&&//
			   (muidev->tpy < muidev->icon[i].y + muidev->icon[i].height))
			{
				break;//�õ�ѡ�еı��	
			}
		}
		if(i >= HOME_BMP_NUMS)
		{
			i=0xff;//��Ч�ĵ㰴.	
		}
	muidev->status&=0X7F;//���������Ч��־
	} 	 
	return i; 
}


uint8_t gui_touch_chk(_gui_dev *buttonID)
{		 
	uint8_t i=0xff;																 
	if(tp_dev.sta&TP_PRES_DOWN)//�а���������
	{
		buttonID->status|=0X80;	//�������Ч����
		buttonID->tpx=tp_dev.x[0];
		buttonID->tpy=tp_dev.y[0];
	}
	else if(buttonID->status&0X80)//�����ɿ���,��������Ч����
	{
		
		for(i=0;i<buttonID->totalIcon;i++)
		{
			if((buttonID->tpx > buttonID->icon[i].x) && (buttonID->tpx < buttonID->icon[i].x + buttonID->icon[i].width) && (buttonID->tpy > buttonID->icon[i].y)&&//
			   (buttonID->tpy < buttonID->icon[i].y + buttonID->icon[i].height))
			{
				break;//�õ�ѡ�еı��	
			}
		}
		if(i >= buttonID->totalIcon)
		{
			i=0xff;//��Ч�ĵ㰴.	
		}
	buttonID->status&=0X7F;//���������Ч��־
	} 	 
	return i; 
}
/*
*********************************************************************************************************
*	�� �� ��: main_screen
*	����˵��: ���������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
//#define CURE_FOR_EXTRUDER
#ifndef CURE_FOR_EXTRUDER
void main_screen(void)
{
	_mainUI_button_e SELECTED_ICON = NO_SELECTED_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	
	if( redraw_screen_flag == IS_TRUE && eeprom_is_power_off_happen() == PW_OFF_NOT_HAPPEN)
	{
		redraw_screen_flag = IS_FALSE;
		
		mui_init();
	}

	SELECTED_ICON = (_mainUI_button_e)gui_touch_chk(muidev);
	
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN )//�������ϵ�
	{
		SELECTED_ICON = IS_PRINT_ICON;//ģ�ⴥ����ӡ����
	}
	switch ( SELECTED_ICON )
	{
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_PREHEAT_ICON:
				currentMenu = preheat_screen;
				free_flag = IS_TRUE;
				MAINUI_DEBUG("preheat_screen\r\n");
				
			break;
			
		case IS_MOVING_ICON:
				currentMenu = move_screen;
			free_flag = IS_TRUE;
				MAINUI_DEBUG("move_screen\r\n");
				
			break;			
					
		case IS_HOME_ICON:
		currentMenu = home_screen;
				free_flag = IS_TRUE;
				
				MAINUI_DEBUG("home screen\r\n");
				
			break;			
			
		case IS_PRINT_ICON:
		
	
		if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN )
		{
		MAINUI_DEBUG("powerD print screen\r\n");
		currentMenu = powerOffPrint_screen;
		
		}
		else 
		{
			currentMenu = print_screen;		
			MAINUI_DEBUG("print_screen\r\n");
		}
		redraw_fileSystemUI_flag = IS_TRUE;
		free_flag = IS_TRUE;		


				
			break;
			
/****************************��ҳ�ĵڶ���ͼ��****************************/			
		case IS_EXTRUDE_ICON:
		free_flag = IS_TRUE;
		currentMenu = extru_screen;
				MAINUI_DEBUG("extruder screen\r\n");
				
			break;
		case IS_ZLEVEL_ICON:
		free_flag = IS_TRUE;
		currentMenu = level_screen;
				MAINUI_DEBUG("zlevel_screen\r\n");
			break;
			
		case IS_SETTING_ICON:
		free_flag = IS_TRUE;
		currentMenu = set_screen;
				MAINUI_DEBUG("set_screen\r\n");
				
			break;			
			
		case IS_MORE_ICON:		
		
		free_flag = IS_TRUE;
		currentMenu = ui_language_screen;


			MAINUI_DEBUG("about screen\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,muidev->icon);
		myfree(SRAMIN,muidev);
		MAINUI_DEBUG("free muidev \r\n");
	}
}

#else

#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

typedef enum
{
	STEP_WAIT_CNT=0,	// �ȴ����Ӽ���ͷ
	STEP_HEATING,		// ���ȼ���ͷ
	STEP_EXTRUDING,		// ���ڼ���
	STEP_WAIT_DISCNT,	// �ȴ�����ͷ�Ͽ�����
}ExtruderTestStep;

void main_screen(void)	/* for magic����ͷ�����ξ� */
{
	static u8 initflg=0;
	static ExtruderTestStep step=STEP_WAIT_CNT;
	char tmpstr[8];
	
	if(initflg==0)
	{
		initflg = 1;
		
//		fanSpeed = 255;	// ���ַ���
		
		LCD_Clear(BLACK);
		POINT_COLOR = RED;
		Show_Str(82,20,lcddev.width,24,"Extruder Test",24,0);	
		POINT_COLOR = BLUE;
		Show_Str(10, 70,lcddev.width,24,"X-Endstop:",24,0);	
		Show_Str(10,110,lcddev.width,24,"Turbo Fan:",24,0);	
		Show_Str(10,150,lcddev.width,24,"Extruder1:",24,0);	
	}

	POINT_COLOR = YELLOW;
	if(X_MIN_PIN)	Show_Str(140, 70,lcddev.width,24,"OFF",24,0);
	else			Show_Str(140, 70,lcddev.width,24,"ON ",24,0);
	sprintf(tmpstr, "%3d", fanSpeed);
	Show_Str(140,110,lcddev.width,24,tmpstr,24,0);
	
	sprintf(tmpstr, "%3d/%3d", (int)current_temperature[0],target_temperature[0]);
	Show_Str(140,150,lcddev.width,24,tmpstr,24,0);
	
	POINT_COLOR = GREEN;
	switch(step)
	{
		case STEP_WAIT_CNT:
			if(current_temperature[0]<HEATER_0_MINTEMP || current_temperature[0]>HEATER_0_MAXTEMP)
			{
				Show_Str(10,190,lcddev.width,24,"Please connect extruder!",24,0);
			}
			else
			{
				target_temperature[0] = 200;
				Show_Str(10,190,lcddev.width,24,"extruder is heating ... ",24,0);
				step = STEP_HEATING;
				
				fanSpeed = 100;	// ���ַ���
			}
			break;
			
		case STEP_HEATING:
			if(current_temperature[0]>=(target_temperature[0]-5)) 
			{
				Show_Str(10,190,lcddev.width,24,"is extruding ...        ",24,0);
				menu_action_gcode(USING_RELATIVE_POSITION);
				menu_action_gcode("G1 E100 F500");
				menu_action_gcode(USING_ABSOLUTE_POSITION);	
				step = STEP_EXTRUDING;
				
				fanSpeed = 255;	// ���ַ���
			}		
			break;
			
		case STEP_EXTRUDING:
			if(buflen==0 && block_buffer_head==block_buffer_tail)
			{
				target_temperature[0] = 0;
				Show_Str(10,190,lcddev.width,24,"End of the test!        ",24,0);
				step = STEP_WAIT_DISCNT;
				
				fanSpeed = 0;	// ���ַ���
			}
			break;
			
		case STEP_WAIT_DISCNT:
			if(current_temperature[0]<HEATER_0_MINTEMP || current_temperature[0]>HEATER_0_MAXTEMP)
				step = STEP_WAIT_CNT;
			break;
			
		default: break;
	}		
	
}

#endif


