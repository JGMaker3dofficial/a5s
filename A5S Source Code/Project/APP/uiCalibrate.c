

#include "uiCalibrate.h"

/**********************调试宏定义**************************/

#define UICALBT_DEBUG_EN

#ifdef UICALBT_DEBUG_EN

#define UICALBT_DEBUG	DBG

#else
#define UICALBT_DEBUG(format,...)	((void) 0)

#endif
/************************************************/


char *const uiCalbt_title[GUI_LANGUAGE_NUM] = 
{
	"准备/设置/屏幕校准",
	"Prepare/Set/LCD calibrate"
};


char *const	uiCalbtRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"是否重新校准屏幕 ？",
	"Are you sure to calibrate LCD screen ?"
};




void ui_calibrate_screen(void)
{

	uint8_t num = 2;//按键数
    uint8_t i;
    uint8_t selx;

    uint8_t free_flag = 0;
	uint8_t font = 16;
	uint16_t xStart = 2;
	uint16_t yStart = 5;
	 if( redraw_screen_flag == IS_TRUE )
	 {
		redraw_screen_flag = IS_FALSE;
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;	 
		
		LCD_Clear_DMA(BLACK);//清屏
		
		Show_Str(xStart ,yStart,lcddev.width,font,(u8 *)uiCalbt_title[LANGUAGE],font,1);
		
		Show_Str(xStart ,yStart + 3*font,lcddev.width,font,(u8 *)uiCalbtRemidInfor[LANGUAGE],font,1);
		
        screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
        if(screen_key_group)//按键创建成功
        {

            uint16_t botton_width = 80;
            uint16_t botton_height = 40;

			xStart = 40;
			yStart = 100 + font;
			
            for( i = 0; i < num; i++)//创建2个按键
            {
                screen_key_group[i] = btn_creat(xStart, yStart, botton_width, botton_height, i, 4);
				 xStart += 160;		
            }
            for( i = 0; i < num; i++)
            {
                screen_key_group[i]->bcfucolor = BLACK;
                screen_key_group[i]->bcfdcolor = WHITE;

                screen_key_group[i]->bkctbl[0] = CYAN;
                screen_key_group[i]->bkctbl[1] = RED;
                screen_key_group[i]->bkctbl[2] = CYAN;
                screen_key_group[i]->bkctbl[3] = RED;

                screen_key_group[i]->caption = (u8 *)CONFIRM_CAPTION_TBL[LANGUAGE][i];
            }
            for(i = 0; i < num; i++)
            {
                btn_draw(screen_key_group[i]);
            }

        }
        else
        {
            UICALBT_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
            return;
        }
		
		
		
		
		
	 }
	 //按键检测
    selx = screen_key_chk(screen_key_group, &in_obj, num);
	if(selx & BUTTON_ACTION)
	switch (selx & BUTTON_SELECTION_MASK)
	{
		case YES:
			ui_calibrate_callBack();
			currentMenu = main_screen;
            free_flag = 1;
			break;
			
		case NO:
		currentMenu = set_screen;
            free_flag = 1;		
			break;
			
		default:
		
			break;
	}
	
	if(free_flag == 1) //释放内存
    {
        redraw_screen_flag = IS_TRUE;
        for(i = 0; i < num; i++)
        {
            btn_delete(screen_key_group[i]);
        }
        gui_memin_free(screen_key_group);
		UICALBT_DEBUG("free Lcd calibrate \r\n");
		
    } 
}


void ui_calibrate_callBack(void)
{
	TP_Adjust();
//	AT24CXX_WriteOneByte(LCD_CALBT_STATE_ADDR, RESET_VALUE);//复位字库更新标志
//	SoftReset();
}
