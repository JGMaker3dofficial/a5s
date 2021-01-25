
#include "move.h"
#include "gcodeplayer.h"
#include "Dlion.h"
/**********************调试宏定义**************************/

#define MOVEUI_DEBUG_EN

#ifdef MOVEUI_DEBUG_EN

#define MOVEUI_DEBUG	DBG

#else
#define MOVEUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



#define MOVE_NUM_ROW	2//总共2行控件
#define MOVE_ROW_COUNT	4//每行用4个控件

#define MOVE_EXTI_BMP_NUMS	2 //要切换的控件数目
#define MOVE_BMP_TOTAL_NUMS	 10
#define MOVE_BMP_VALID_NUMS  (MOVE_BMP_TOTAL_NUMS - MOVE_EXTI_BMP_NUMS) //用于控件按下检测扫描

_gui_dev *moveUIdev;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
static uint8_t moveStepCount = 0;//移动阶梯计数器

MOVE_SCALE SELECTED_MOVE_STEP = MM01;//记录当前选择的移动阶梯
typedef enum 
{
	NONE_MOVE_ICON = 0XFF,
	IS_X_ADD_ICON = 0,
	IS_Y_ADD_ICON = 1,
	IS_Z_ADD_ICON = 2,
	IS_MM_STEP_0_1_ICON = 3,
	
	IS_X_DEC_ICON = 4,
	IS_Y_DEC_ICON = 5,
	IS_Z_DEC_ICON = 6,
	IS_MOVE_BACK_ICON = 7,
	
	IS_MM_STEP_1_ICON = 8,
	IS_MM_STEP_10_ICON = 9,

	
}_moveUI_selected_button_e;

_moveUI_selected_button_e MOVE_SAVED_BUTTON = NONE_MOVE_ICON;

//icon的路径表,MOVE界面图标
char *const moveui_icon_path_tbl[GUI_LANGUAGE_NUM][MOVE_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_yAdd.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zAdd.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_step_move0_1.bin",	    
	
	"1:/SYSTEM/PICTURE/ch_bmp_xDec.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_yDec.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_zDec.bin",	    
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	
	"1:/SYSTEM/PICTURE/ch_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move10.bin",
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_yAdd.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zAdd.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_step_move0_1.bin",	    
	
	"1:/SYSTEM/PICTURE/en_bmp_xDec.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_yDec.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_zDec.bin",	    
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	   
	
	"1:/SYSTEM/PICTURE/en_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move10.bin",	
	
	}
};  

char *const moveUITitleInfor[GUI_LANGUAGE_NUM] = 
{

	"准备/移动",
	"Prepare/move"
};


void moveui_init(void)
{
	uint16_t i,j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;

	LCD_Clear_DMA(BLACK);

Show_Str(2,5,lcddev.width,font,(u8 *)moveUITitleInfor[LANGUAGE],font,1);	
	//为主界面控制器申请内存
	moveUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( moveUIdev == 0)
	{
		MOVEUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	moveUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*MOVE_BMP_TOTAL_NUMS);
	
	if( moveUIdev->icon == 0)
	{
		MOVEUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	moveUIdev->status = 0x3F ;
	
	
	for(i=0;i<MOVE_NUM_ROW;i++)
	{
		for(j=0;j<MOVE_ROW_COUNT;j++)
		{
			
			moveUIdev->icon[i*MOVE_ROW_COUNT+j].x = bmp_sart_offx + j*bmp_between_offx  ;
			moveUIdev->icon[i*MOVE_ROW_COUNT+j].y = bmp_sart_offy + i*bmp_between_offy  ;
			moveUIdev->icon[i*MOVE_ROW_COUNT+j].width = PICTURE_X_SIZE;
			moveUIdev->icon[i*MOVE_ROW_COUNT+j].height = PICTURE_Y_SIZE;
			moveUIdev->icon[i*MOVE_ROW_COUNT+j].path = (u8*)moveui_icon_path_tbl[LANGUAGE][i*MOVE_ROW_COUNT+j];

		}
	}
	
	for(i = IS_MM_STEP_1_ICON; i < MOVE_BMP_TOTAL_NUMS; i++)//为move_step1mm、move_step10mm赋值
	{
			moveUIdev->icon[i].x = moveUIdev->icon[IS_MM_STEP_0_1_ICON].x  ;
			moveUIdev->icon[i].y = moveUIdev->icon[IS_MM_STEP_0_1_ICON].y  ;
			moveUIdev->icon[i].width = moveUIdev->icon[IS_MM_STEP_0_1_ICON].width;
			moveUIdev->icon[i].height = moveUIdev->icon[IS_MM_STEP_0_1_ICON].height;
			moveUIdev->icon[i].path = (u8*)moveui_icon_path_tbl[LANGUAGE][i];


	}	
	
	moveUIdev->totalIcon = MOVE_BMP_VALID_NUMS;
	
		for(j=0; j < moveUIdev->totalIcon; j++)
		{
			if(j == IS_MM_STEP_0_1_ICON)//第一排第四个控件
			{
				if(moveStepCount == 0) 			
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[j].path, moveUIdev->icon[j].x , moveUIdev->icon[j].y, moveUIdev->icon[j].width, moveUIdev->icon[j].height, moveUIdev->icon[j].color);
				
				else if(moveStepCount == 1) 
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[IS_MM_STEP_1_ICON].path, moveUIdev->icon[IS_MM_STEP_1_ICON].x , moveUIdev->icon[IS_MM_STEP_1_ICON].y, moveUIdev->icon[IS_MM_STEP_1_ICON].width, moveUIdev->icon[IS_MM_STEP_1_ICON].height, moveUIdev->icon[IS_MM_STEP_1_ICON].color);
				
				else if(moveStepCount == 2) 
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[IS_MM_STEP_10_ICON].path, moveUIdev->icon[IS_MM_STEP_10_ICON].x , moveUIdev->icon[IS_MM_STEP_10_ICON].y, moveUIdev->icon[IS_MM_STEP_10_ICON].width, moveUIdev->icon[IS_MM_STEP_10_ICON].height, moveUIdev->icon[IS_MM_STEP_10_ICON].color);
	
			}
			else
			display_bmp_bin_to_LCD((u8*)moveUIdev->icon[j].path, moveUIdev->icon[j].x , moveUIdev->icon[j].y, moveUIdev->icon[j].width, moveUIdev->icon[j].height, moveUIdev->icon[j].color);
		}	

}
void move_screen(void)
{
	_moveUI_selected_button_e MOVE_SELECTED_BUTTON = NONE_MOVE_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		moveui_init();
	}

	MOVE_SELECTED_BUTTON = (_moveUI_selected_button_e)gui_touch_chk(moveUIdev);
	
	switch ( MOVE_SELECTED_BUTTON )
	{
/**************************主页的第一排图标******************************/	
		case IS_X_ADD_ICON:
			
				MOVE_SAVED_BUTTON = IS_X_ADD_ICON;
				user_cmd_move_x(POSITIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("x+\r\n");
				
			break;
			
		case IS_Y_ADD_ICON:
		MOVE_SAVED_BUTTON = IS_Y_ADD_ICON;
			user_cmd_move_y(POSITIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("y+\r\n");
				
			break;			
					
		case IS_Z_ADD_ICON:
			MOVE_SAVED_BUTTON = IS_Z_ADD_ICON;
				user_cmd_move_z(POSITIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("z+\r\n");
				
			break;			
			
		case IS_MM_STEP_0_1_ICON:
	
			moveStepCount++;
			switch (moveStepCount)
				{
					case 1:
				SELECTED_MOVE_STEP = MM1;
				gui_fill_rectangle_DMA(moveUIdev->icon[IS_MM_STEP_0_1_ICON].x, moveUIdev->icon[IS_MM_STEP_0_1_ICON].y, moveUIdev->icon[IS_MM_STEP_0_1_ICON].width, moveUIdev->icon[IS_MM_STEP_0_1_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[IS_MM_STEP_1_ICON].path, moveUIdev->icon[IS_MM_STEP_1_ICON].x , moveUIdev->icon[IS_MM_STEP_1_ICON].y, moveUIdev->icon[IS_MM_STEP_1_ICON].width, moveUIdev->icon[IS_MM_STEP_1_ICON].height, moveUIdev->icon[IS_MM_STEP_1_ICON].color);
					
						break;
						
					case 2:
				SELECTED_MOVE_STEP = MM10;
				gui_fill_rectangle_DMA(moveUIdev->icon[IS_MM_STEP_1_ICON].x, moveUIdev->icon[IS_MM_STEP_1_ICON].y, moveUIdev->icon[IS_MM_STEP_1_ICON].width, moveUIdev->icon[IS_MM_STEP_1_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[IS_MM_STEP_10_ICON].path, moveUIdev->icon[IS_MM_STEP_10_ICON].x , moveUIdev->icon[IS_MM_STEP_10_ICON].y, moveUIdev->icon[IS_MM_STEP_10_ICON].width, moveUIdev->icon[IS_MM_STEP_10_ICON].height, moveUIdev->icon[IS_MM_STEP_10_ICON].color);
					
						break;
						
					case 3:
					moveStepCount = 0;
					SELECTED_MOVE_STEP = MM01;
				gui_fill_rectangle_DMA(moveUIdev->icon[IS_MM_STEP_10_ICON].x, moveUIdev->icon[IS_MM_STEP_10_ICON].y, moveUIdev->icon[IS_MM_STEP_10_ICON].width, moveUIdev->icon[IS_MM_STEP_10_ICON].height,BLACK);
				display_bmp_bin_to_LCD((u8*)moveUIdev->icon[IS_MM_STEP_0_1_ICON].path, moveUIdev->icon[IS_MM_STEP_0_1_ICON].x , moveUIdev->icon[IS_MM_STEP_0_1_ICON].y, moveUIdev->icon[IS_MM_STEP_0_1_ICON].width, moveUIdev->icon[IS_MM_STEP_0_1_ICON].height, moveUIdev->icon[IS_MM_STEP_0_1_ICON].color);
					
						break;						
					default:
						break;
				}
				
				MOVEUI_DEBUG("move step\r\n");
			break;
			
/****************************主页的第二排图标****************************/			
		case IS_X_DEC_ICON:
	MOVE_SAVED_BUTTON = IS_X_DEC_ICON;
		user_cmd_move_x(NEGTIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("x-\r\n");
				
			break;
		case IS_Y_DEC_ICON:
MOVE_SAVED_BUTTON = IS_Y_DEC_ICON;
		user_cmd_move_y(NEGTIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("y-\r\n");
			break;
			
		case IS_Z_DEC_ICON:
MOVE_SAVED_BUTTON = IS_Z_DEC_ICON;
		user_cmd_move_z(NEGTIVE_DIR,SELECTED_MOVE_STEP);
				MOVEUI_DEBUG("z-\r\n");
				
			break;			
			
		case IS_MOVE_BACK_ICON:		
		currentMenu = main_screen;
		free_flag = IS_TRUE;
			MOVEUI_DEBUG("move back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	
if(MOVE_SAVED_BUTTON != NONE_MOVE_ICON)
{
	
	gui_fill_rectangle_DMA(moveUIdev->icon[MOVE_SAVED_BUTTON].x, moveUIdev->icon[MOVE_SAVED_BUTTON].y, moveUIdev->icon[MOVE_SAVED_BUTTON].width, moveUIdev->icon[MOVE_SAVED_BUTTON].height,BLACK);
	display_bmp_bin_to_LCD((u8*)moveUIdev->icon[MOVE_SAVED_BUTTON].path, moveUIdev->icon[MOVE_SAVED_BUTTON].x , moveUIdev->icon[MOVE_SAVED_BUTTON].y, moveUIdev->icon[MOVE_SAVED_BUTTON].width, moveUIdev->icon[MOVE_SAVED_BUTTON].height, moveUIdev->icon[MOVE_SAVED_BUTTON].color);
	MOVE_SAVED_BUTTON = NONE_MOVE_ICON;
	
}	
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,moveUIdev->icon);
		myfree(SRAMIN,moveUIdev);
		MOVEUI_DEBUG("free moveUIdev \r\n");
	}

}
