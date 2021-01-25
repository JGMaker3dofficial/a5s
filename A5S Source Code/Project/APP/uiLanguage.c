

#include "uiLanguage.h"

/**********************调试宏定义**************************/

#define UILAN_DEBUG_EN

#ifdef UILAN_DEBUG_EN

#define UILAN_DEBUG	DBG

#else
#define UILAN_DEBUG(format,...)	((void) 0)

#endif
/************************************************/


char *const uiLang_title[GUI_LANGUAGE_NUM] = 
{
	"准备/设置/语言",
	"Prepare/Set/Language"
};


char *const	uiLangRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"是否切换语言 ？",
	"Are you sure to switch language ?"
};




void ui_language_screen(void)
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
		
		Show_Str(xStart ,yStart,lcddev.width,font,(u8 *)uiLang_title[LANGUAGE],font,1);
		
		Show_Str(xStart ,yStart + 3*font,lcddev.width,font,(u8 *)uiLangRemidInfor[LANGUAGE],font,1);
		
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
            UILAN_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
            return;
        }
		
		
		
		
		
	 }
	 //按键检测
    selx = screen_key_chk(screen_key_group, &in_obj, num);
	if(selx & BUTTON_ACTION)
	switch (selx & BUTTON_SELECTION_MASK)
	{
		case YES:
			ui_language_callBack();
			currentMenu = main_screen;
            free_flag = 1;
			break;
			
		case NO:
		currentMenu = main_screen;
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
		UILAN_DEBUG("free language \r\n");
		
    } 
}


void ui_language_callBack(void)
{
	u8 temp;
	u8 date;
	if(LANGUAGE == ENGLISH)	date = LANGUAGE_STATE_CH;
	else date = LANGUAGE_STATE_EN;
	
	AT24CXX_WriteOneByte(LANGUAGE_STATE_ADDR, date);//标志
	
	temp = AT24CXX_ReadOneByte(LANGUAGE_STATE_ADDR);
	if(temp == date) UILAN_DEBUG("<#>language flag OK!\r\n");
	else UILAN_DEBUG("<@>language flag fail!\r\n");
	
	SoftReset();
}
