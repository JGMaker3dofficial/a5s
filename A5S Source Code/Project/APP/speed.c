

#include "speed.h"
/**********************包含头文件*************************/
#include "fan.h"





#include "temperature.h"
#include "planner.h"
#include "stepper.h"
#include "Dlion.h"
#include "gcodeplayer.h"

/**********************调试宏定义**************************/

#define SPEEDUI_DEBUG_EN

#ifdef SPEEDUI_DEBUG_EN

#define SPEEDUI_DEBUG	DBG

#else
#define SPEEDUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/




/***********************私有宏*************************/






/************************************************/
#define SPEED_ICON_NUM	5
#define SPEED_FIRST_ROW_ICON_NUM 		2
#define SPEED_SECOND_ROW_ICON_NUM 	3
#define SPEED_EXTN_ICON_NUM		2//额外的两个步进设置图标

/**************控件触摸检测ID定义*****************/
#define IS_SPEED_ADD_BUTTON		0
#define IS_SPEED_DEC_BUTTON		1
#define IS_SPEED_CLEAR_SET_BUTTON	2
#define IS_SPEED_STEP1_BUTTON		3
#define IS_SPEED_BACK_BUTTON		4

#define IS_SPEED_STEP5_BUTTON		5
#define IS_SPEED_STEP10_BUTTON		6
typedef enum 
{
	PRT_SPEED_1 = 0,
	PRT_SPEED_5 = 1,
	PRT_SPEED_10 = 2,
}_print_speed_e;


_print_speed_e	PRT_SPEED_STEP = PRT_SPEED_1;
u8 prtSpeedCnt = 0;
//主界面 控制器
_gui_dev *speedUIdev;

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
char *const speedUI_titile [GUI_LANGUAGE_NUM]= 
{
	"准备/设置/打印速率",
	"Prepare/set/print speed"
};

char *const speedUI_dispInfor [GUI_LANGUAGE_NUM]= 
{
	"打印速率",
	"print speed"
};



/************************************************/


//icon的路径表,HOME界面图标
char *const speedUI_icon_path_tbl[GUI_LANGUAGE_NUM][SPEED_ICON_NUM + SPEED_EXTN_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",//0
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",	  //1
	
	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",	 //2
	"1:/SYSTEM/PICTURE/ch_bmp_speed1.bin",	//3
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	    //4
	
	"1:/SYSTEM/PICTURE/ch_bmp_speed5.bin",	//5
	"1:/SYSTEM/PICTURE/ch_bmp_speed10.bin",	//6
    },
	{
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",	  
	
	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_speed1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	 	

	"1:/SYSTEM/PICTURE/en_bmp_speed5.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed10.bin",	
	}
};  


static void speedUI_init(void)
{
	uint16_t i,j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint16_t x,y;
	
	x = 2;
	y = 5;
	

LCD_Clear_DMA(BLACK);


	Show_Str(x,y,lcddev.width,font,(u8 *)speedUI_titile[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	speedUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( speedUIdev == 0)
	{
		SPEEDUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	speedUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*SPEED_ICON_NUM);
	
	if( speedUIdev->icon == 0)
	{
		SPEEDUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	speedUIdev->status = 0x3F ;
	speedUIdev->totalIcon = SPEED_ICON_NUM;//控件检测数目
	// 0 1
	for(j=0;j<SPEED_FIRST_ROW_ICON_NUM;j++)
	{
			speedUIdev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx ;
			speedUIdev->icon[j].y = bmp_sart_offy ;
			speedUIdev->icon[j].width = PICTURE_X_SIZE;
			speedUIdev->icon[j].height = PICTURE_Y_SIZE;
			speedUIdev->icon[j].path = (u8*)speedUI_icon_path_tbl[LANGUAGE][j];

	
	}
// 2 3   4
		for(j=SPEED_FIRST_ROW_ICON_NUM;j<SPEED_ICON_NUM;j++)
		{
			if(j == SPEED_ICON_NUM - 1)
			speedUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
			
			else 
			speedUIdev->icon[j].x = bmp_sart_offx + (j - SPEED_FIRST_ROW_ICON_NUM)*bmp_between_offx;
			
			speedUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy  ;
			speedUIdev->icon[j].width = PICTURE_X_SIZE;
			speedUIdev->icon[j].height = PICTURE_Y_SIZE;
			speedUIdev->icon[j].path = (u8*)speedUI_icon_path_tbl[LANGUAGE][j];

		}
		
		speedUIdev->totalIcon = SPEED_ICON_NUM;
		
		
		for(j=0;j<speedUIdev->totalIcon;j++)//不显示 切换显示的图标
		{
			if(j == IS_SPEED_STEP1_BUTTON)
			{
				if(prtSpeedCnt == 1)//step2 
					i = IS_SPEED_STEP5_BUTTON;
				else if(prtSpeedCnt == 2)//step3
				i = IS_SPEED_STEP10_BUTTON;
				else i = IS_SPEED_STEP1_BUTTON;
				display_bmp_bin_to_LCD((u8*)speedUI_icon_path_tbl[LANGUAGE][i], speedUIdev->icon[j].x , speedUIdev->icon[j].y, speedUIdev->icon[j].width, speedUIdev->icon[j].height, speedUIdev->icon[j].color);
				
			}
			
			else
			display_bmp_bin_to_LCD((u8*)speedUIdev->icon[j].path, speedUIdev->icon[j].x , speedUIdev->icon[j].y, speedUIdev->icon[j].width, speedUIdev->icon[j].height, speedUIdev->icon[j].color);
		}

}




void speed_screen(void)
{

	u8 SELECTED_BUTTON = 0xff;
	u8 savedButton = 0xff;
	u8 j,i;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  _obj_option_e  OPT_TYPE = NONE_OPT;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		speedUI_init();
	}

	SELECTED_BUTTON = gui_touch_chk(speedUIdev);
	
	switch ( SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_SPEED_ADD_BUTTON://加
		OPT_TYPE  = ADD_OPT;
		savedButton = IS_SPEED_ADD_BUTTON;

				SPEEDUI_DEBUG("add_screen\r\n");
				
			break;
			
		case IS_SPEED_DEC_BUTTON://减
		OPT_TYPE  = DEC_OPT;
		savedButton = IS_SPEED_DEC_BUTTON;
		

				SPEEDUI_DEBUG("dec_screen\r\n");
				
			break;			
			
/****************************主页的第二排图标****************************/							
			
		case IS_SPEED_CLEAR_SET_BUTTON://清零设置值
		OPT_TYPE  = CLEAR_OPT;
		savedButton = IS_SPEED_CLEAR_SET_BUTTON;
				SPEEDUI_DEBUG("clear screen\r\n");
				
			break;
			
			
		case IS_SPEED_STEP1_BUTTON:		//step设置
		prtSpeedCnt++;
		if(prtSpeedCnt == 1)//step2
		{
			j = IS_SPEED_STEP5_BUTTON;
		}
		else if(prtSpeedCnt == 2)//step3
		{
			j = IS_SPEED_STEP10_BUTTON;
		}
		else 
		{
			prtSpeedCnt = 0;//step1
			j = IS_SPEED_STEP1_BUTTON;
		}
		i = IS_SPEED_STEP1_BUTTON;
		gui_fill_rectangle_DMA(speedUIdev->icon[i].x, speedUIdev->icon[i].y, speedUIdev->icon[i].width, speedUIdev->icon[i].height,BLACK);			
		display_bmp_bin_to_LCD((u8*)speedUI_icon_path_tbl[LANGUAGE][j], speedUIdev->icon[i].x , speedUIdev->icon[i].y, speedUIdev->icon[i].width, speedUIdev->icon[i].height, speedUIdev->icon[i].color);

		SPEEDUI_DEBUG("step:%d\r\n",prtSpeedCnt);
		break;
			
			
		case IS_SPEED_BACK_BUTTON:

		currentMenu = gcode_print_option_to_more_screen;
		free_flag = IS_TRUE;
				SPEEDUI_DEBUG("back_screen\r\n");
			break;
			
	

/********************************************************/
			
		default:
			break;
	}
	
		if(savedButton != 0XFF)//触发到不需要切换界面图标 的图标时，闪烁一下让用户知道触发到了
		{
		
				gui_fill_rectangle_DMA(speedUIdev->icon[savedButton].x, speedUIdev->icon[savedButton].y, speedUIdev->icon[savedButton].width, speedUIdev->icon[savedButton].height,BLACK);			
				display_bmp_bin_to_LCD((u8*)speedUIdev->icon[savedButton].path, speedUIdev->icon[savedButton].x , speedUIdev->icon[savedButton].y, speedUIdev->icon[savedButton].width, speedUIdev->icon[savedButton].height, speedUIdev->icon[savedButton].color);
				savedButton = 0XFF;
	
		}
display_print_speed(OPT_TYPE,prtSpeedCnt);
OPT_TYPE = NONE_OPT;//清零操作ID
	if(isOption2moreUiSelected == YES && card.FILE_STATE == PS_PAUSE)//确保打印界面先按下暂停后再进入改页面才检测打印头靠边停止
	{
		check_is_pause_exacute();//检查打印头是否执行靠边停止

	}


	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,speedUIdev->icon);
		myfree(SRAMIN,speedUIdev);
		SPEEDUI_DEBUG("free speedUIdev \r\n");
	}


}

void set_print_speed(_obj_option_e opt,u8 stepID)
{
//打印速度设置

		float speed	= feedmultiply;
		switch (opt)
		{
			case ADD_OPT:
				switch (stepID)
				{
					case 0:
						speed += 1;
						if(speed > 500)	speed = 500;
						
						break;
						
					case 1:
						speed += 5;
						if(speed > 500)	speed = 500;
					
						break;
						
					case 2:
						speed += 10;
						if(speed > 500)	speed = 500;

						break;	
						
					default:
						break;
				}

				feedmultiply = speed;
				break;

		
			case DEC_OPT:
				switch (stepID)
				{
					case 0:
						speed -= 1;
						if(speed < 25)	speed = 25;					
						break;
						
					case 1:
						speed -= 5;
						if(speed < 25)	speed = 25;						
						break;
						
					case 2:
						speed -= 10;
						if(speed < 25)	speed = 25;						
						break;	
						
					default:
						break;
				}
			feedmultiply = speed;
		
				break;
				
	
			case CLEAR_OPT:
			speed = 100;	
			feedmultiply = speed;
	
				break;				
				
			default:
				break;
		}


}

void display_print_speed(_obj_option_e opt,u8 stepID)
{


	u16 xOffSet;
	u16 yOffSet;
	u16 width;
	u16 hight;
	u16 font = 24;

	u16 speed;
	
	hight = font;
	set_print_speed(opt,stepID);
	if( millis() > display_next_update_millis)//250ms 更新一次
	{
		display_next_update_millis = millis() + 250;
		width = 2*PICTURE_X_SIZE;
		 BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		
		speed = feedmultiply;//获取打印速度
		
		yOffSet = BMP_FONT_SIZE + font;
		xOffSet = PICTURE_X_SIZE + 4;
		
		
		 Show_Str(xOffSet,yOffSet,width,hight,(u8 *)speedUI_dispInfor[LANGUAGE],font,1);//显示"print speed"
		 

		POINT_COLOR = YELLOW;
		 width = 4*(font / 2);
		 yOffSet += font;
		 gui_fill_rectangle_DMA(xOffSet,yOffSet + font ,width,hight,BACK_COLOR);//清区域色块
		 
		gui_show_num(xOffSet , yOffSet + font, 3, POINT_COLOR, font, speed, 1); //显示speed
		
		POINT_COLOR = WHITE;
		LCD_ShowString(xOffSet  + 3 * (font / 2), yOffSet + font, width, hight, font, (u8*)"%"); //
		
		
		
		
		
		
	}
	
}
