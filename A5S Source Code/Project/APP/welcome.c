#include "app_gui.h"
#include "temperature.h"
/**********************************/



/**********************LOGO�ߴ綨��**************************/
#define LOGO1_WIDTH 	320
#define LOGO1_HEIGHT 	181


#define LOGO2_WIDTH 	111
#define LOGO2_HEIGHT 	100

/************************************************/



/**********************���Ժ궨��**************************/

#define WELCOME_DEBUG_EN

#ifdef WELCOME_DEBUG_EN

	#define WELCOME_DEBUG	DBG

#else
	#define WELCOME_DEBUG(format,...)	((void) 0)	

#endif
/************************************************/


/**********************logoͼƬ�洢Ŀ¼����******************/
char *const logo_icos_path_tbl[GUI_LANGUAGE_NUM]=
{

	"1:/SYSTEM/PICTURE/ch_logo.bin",//320*240
	"1:/SYSTEM/PICTURE/en_logo.bin",

};
/************************************************/




/**********************���ļ�˽�б�������*********************/

/************************************************/

/*
*********************************************************************************************************
*	����˵��: ���ڸ��¼���ͷ�¶Ⱥ��ȴ��¶�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void display_temp(void)
{
    char TempBuffer[6];
    if(display_next_update_millis <= millis())
    {
	

        display_next_update_millis = millis() + 1000;

        sprintf(TempBuffer, "%3.0fC", degHotend(0));
        gui_show_strmid(EXTRUDER_TEM_XOFF, EXTRUDER_TEM_YOFF, 240 - EXTRUDER_TEM_XOFF , 16, BLACK, 16, (u8 *)TempBuffer, 0);


        sprintf(TempBuffer, "%3.0fC",degBed());
        gui_show_strmid(BED_TEM_XOFF,BED_TEM_YOFF,320 - BED_TEM_XOFF, 16, BLACK, 16, (u8 *)TempBuffer, 0);

    }
}


/*
*********************************************************************************************************
*	����˵��: �������棬��ʾ��˾logo
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void welcome_screen(void)
{
#if ZHONG_XING==0
LCD_LED=0;
//	LCD_Clear_DMA(BLACK);	
//	display_bin2LCD();
	display_big_pic_bin2LCD((u8*)logo_icos_path_tbl[LANGUAGE]);
LCD_LED=1;
	
	WELCOME_DEBUG("welcome going...\r\n");
	bsp_DelayMS(2000);
#endif

//	LCD_Clear_DMA(BLACK);	
}
