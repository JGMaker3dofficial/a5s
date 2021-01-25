
#include "more.h"
#include "gcodeplayer.h"
#include "Dlion.h"
/**********************���Ժ궨��**************************/

#define MOREUI_DEBUG_EN

#ifdef MOREUI_DEBUG_EN

#define MOREUI_DEBUG	DEBUG_PRINTF

#else
#define MOREUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/


#define MORE_ROW_COUNT	2//ÿ����1���ؼ�


#define MORE_BMP_TOTAL_NUMS	 3


_gui_dev *moreUIdev;
static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������



typedef enum 
{
	NONE_MORE_ICON = 0XFF,
	IS_LANGUAGE_MORE_ICON = 0,
	IS_CMPLT_MORE_ICON = 1,
	
	IS_MORE_BACK_ICON = 2,
		
}_moreUI_selected_button_e;

//_moreUI_selected_button_e MORE_SAVED_BUTTON = NONE_MORE_ICON;
//icon��·����,MOVE����ͼ��
char *const moreui_icon_path_tbl[GUI_LANGUAGE_NUM][MORE_BMP_TOTAL_NUMS]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_language.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroY.bin",
	

	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_language.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroY.bin",
    

	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	 	
	
	}
};  

char *const moreUITitleInfor[GUI_LANGUAGE_NUM] = 
{
	"׼��/����",
	"Prepare/more"

};

void moreui_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	LCD_Clear_DMA(BLACK);

Show_Str(2,5,lcddev.width,font,(u8 *)moreUITitleInfor[LANGUAGE],font,1);	
	//Ϊ����������������ڴ�
	moreUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( moreUIdev == 0)
	{
		MOREUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	moreUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*MORE_BMP_TOTAL_NUMS);
	
	if( moreUIdev->icon == 0)
	{
		MOREUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	moreUIdev->status = 0x3F ;
	
	

		for(j=0;j<MORE_ROW_COUNT;j++)
		{
			
			moreUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx  ;
			moreUIdev->icon[j].y = bmp_sart_offy ;
			moreUIdev->icon[j].width = PICTURE_X_SIZE;
			moreUIdev->icon[j].height = PICTURE_Y_SIZE;
			moreUIdev->icon[j].path = (u8*)moreui_icon_path_tbl[LANGUAGE][j];

		}
		//��ʼ�����ؿؼ�
		moreUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx;
		moreUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
		moreUIdev->icon[j].width = PICTURE_X_SIZE;
		moreUIdev->icon[j].height = PICTURE_Y_SIZE;
		moreUIdev->icon[j].path = (u8*)moreui_icon_path_tbl[LANGUAGE][j];

	
	
	moreUIdev->totalIcon = MORE_BMP_TOTAL_NUMS;
	
		for(j=0; j < moreUIdev->totalIcon; j++)
		{
			display_bmp_bin_to_LCD((u8*)moreUIdev->icon[j].path, moreUIdev->icon[j].x , moreUIdev->icon[j].y, moreUIdev->icon[j].width, moreUIdev->icon[j].height, moreUIdev->icon[j].color);
		}	

}
void more_screen(void)
{
	_moreUI_selected_button_e MORE_SELECTED_BUTTON = NONE_MORE_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		moreui_init();
	}

	MORE_SELECTED_BUTTON = (_moreUI_selected_button_e)gui_touch_chk(moreUIdev);
	
	switch ( MORE_SELECTED_BUTTON )
	{
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case IS_LANGUAGE_MORE_ICON:
			currentMenu = ui_language_screen;
			free_flag = IS_TRUE;
				MOREUI_DEBUG("language ui\r\n");
				
			break;
			
		case IS_CMPLT_MORE_ICON:
			currentMenu = complmt_screen;
			free_flag = IS_TRUE;
				MOREUI_DEBUG("complement ui\r\n");
				
			break;	

/****************************��ҳ�ĵڶ���ͼ��****************************/			

		case IS_MORE_BACK_ICON:		
		currentMenu = main_screen;
		free_flag = IS_TRUE;
			MOREUI_DEBUG("more back\r\n");
			break;	
			
/********************************************************/
			
		default:
			break;
	}
	

	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,moreUIdev->icon);
		myfree(SRAMIN,moreUIdev);
		MOREUI_DEBUG("free moreUIdev \r\n");
	}

}


