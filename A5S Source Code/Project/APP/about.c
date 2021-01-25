#include "about.h"
#include "gcodeplayer.h"
#include "Dlion.h"
#include "mainui.h"
/**********************调试宏定义**************************/

#define ABOUTUI_DEBUG_EN

#ifdef ABOUTUI_DEBUG_EN

#define ABOUTUI_DEBUG	DBG

#else
#define ABOUTUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/


#define ABOUT_BMP_TOTAL_NUMS	 1

#define NONE_ABOUT_ICON  		0XFF
#define IS_ABOUT_BACK_ICON  	 0

static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
//icon的路径表,MOVE界面图标
char *const aboutui_icon_path_tbl[GUI_LANGUAGE_NUM][ABOUT_BMP_TOTAL_NUMS]=
{ 
 
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	   
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	  
};  
char *const about_infor_pic[GUI_LANGUAGE_NUM] = 
{
"1:/SYSTEM/PICTURE/ch_about_infor.bin",
"1:/SYSTEM/PICTURE/en_about_infor.bin",
};
_gui_dev *aboutUIdev;
 char * const aboutTitleInfor[GUI_LANGUAGE_NUM] = 
{
	"准备/设置/关于",
	"Prepare/set/about"
};

char *const logoTitleInfor[GUI_LANGUAGE_NUM] = 
{
	"极光尔沃",
	"JGAURORA"
//	"JGMAKER"	// 深圳亿思博
};


void about_screen(void)
{
	u8 SELECTED_BUTTON = NONE_ABOUT_ICON;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  
	if(redraw_screen_flag == IS_TRUE)
	{
	
		uint16_t j;
		uint16_t bmp_between_offx;
		uint16_t bmp_between_offy;
		uint8_t font = 16;
		uint8_t font1;
		uint16_t xoff;
		uint8_t length;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	redraw_screen_flag = IS_FALSE;
		LCD_Clear_DMA(BLACK);
#if EN_QR_CODE==1	//显示二维码
		display_big_pic_bin2LCD((u8*)about_infor_pic[LANGUAGE]);
		xoff = 164;
		font1 = 16;
		font = 24;
#else
		Show_Str(2,5,lcddev.width,font,(u8 *)aboutTitleInfor[LANGUAGE],font,1);	
		font = 24;
	 #if MERCHIN != A1
		#if EN_DISP_COMPANY_NAME==1
		Show_Str(2,5 + font,lcddev.width,font,(u8 *)logoTitleInfor[LANGUAGE],font,1);//显示公司名称
		#endif	
	 #endif		
	 xoff = 2;
	 font1 = 24;
#endif


	
	
	 
	 Show_Str(xoff,5 + 3*font,lcddev.width,font,(u8 *)"FW_VER:",font,1);
	length = strlen((char *)"FW_VER:");
#if	MERCHIN == A1
LCD_ShowString(xoff + length*(font/2),5 + 3*font,lcddev.width,font,font,(u8 *)A1_FW_NAME);//显示软件版本号	
	
#elif MERCHIN == A3
LCD_ShowString(xoff + length*(font/2),5 + 3*font,lcddev.width,font,font,(u8 *)A3_FW_NAME);//显示软件版本号	

#elif MERCHIN == A4
LCD_ShowString(xoff + length*(font/2),5 + 3*font,lcddev.width,font,font,(u8 *)A4_FW_NAME);//显示软件版本号	

#elif MERCHIN == A5
LCD_ShowString(xoff + length*(font/2),5 + 3*font,lcddev.width,font,font,(u8 *)A5_FW_NAME);//显示软件版本号	

#elif MERCHIN == A6
LCD_ShowString(xoff + length*(font/2),5 + 3*font,lcddev.width,font,font,(u8 *)A6_FW_NAME);//显示软件版本号	

#endif
	 
LCD_ShowString(xoff,5 + 4*font,lcddev.width,font1,font1,(u8 *)FW_VERSION);//显示软件版本号	
font = 16;
		//为主界面控制器申请内存
		aboutUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( aboutUIdev == 0)
		{
			ABOUTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		aboutUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*ABOUT_BMP_TOTAL_NUMS);
		
		if( aboutUIdev->icon == 0)
		{
			ABOUTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		bmp_between_offx = PICTURE_X_SIZE + 2;
		bmp_between_offy = PICTURE_Y_SIZE + 2;
		aboutUIdev->status = 0x3F ;
		
		
		j = 0;

		aboutUIdev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
		aboutUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy ;


		aboutUIdev->icon[j].width = PICTURE_X_SIZE;
		aboutUIdev->icon[j].height = PICTURE_Y_SIZE;
		aboutUIdev->icon[j].path = (u8*)aboutui_icon_path_tbl[LANGUAGE][j];

		aboutUIdev->totalIcon = ABOUT_BMP_TOTAL_NUMS;

		display_bmp_bin_to_LCD((u8*)aboutUIdev->icon[j].path, aboutUIdev->icon[j].x , aboutUIdev->icon[j].y, aboutUIdev->icon[j].width, aboutUIdev->icon[j].height, aboutUIdev->icon[j].color);
	}
		
		
	SELECTED_BUTTON = gui_touch_chk(aboutUIdev);	
		
	if(SELECTED_BUTTON == IS_ABOUT_BACK_ICON)	
	{

		currentMenu = set_screen;
		free_flag = IS_TRUE;
		ABOUTUI_DEBUG("back\r\n");		
	}
		
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,aboutUIdev->icon);
		myfree(SRAMIN,aboutUIdev);
		ABOUTUI_DEBUG("free aboutUIdev \r\n");
	}

}
