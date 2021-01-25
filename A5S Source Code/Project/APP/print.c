/*
*********************************************************************************************************
*
*	ģ������ :��ӡ����ģ��
*	�ļ����� : print.c
*	��    �� : V1.0
*	˵    �� : ��ӡ����
*
*
*********************************************************************************************************
*/


/**********************����ͷ�ļ�*************************/
#include "print.h"



#include "temperature.h"
#include "gcodeplayer.h"
#include "stepper.h"
#include "Dlion.h"
#include "planner.h"

#include "progressbar.h"

/*********************************************/

/**********************���Ժ궨��**************************/

#define PRINTUI_DEBUG_EN

#ifdef PRINTUI_DEBUG_EN

#define PRINTUI_DEBUG	DBG

#else
#define PRINTUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define PRINT_NUM_ROW	1//�ܹ�1�пؼ�
#define PRINT_ROW_COUNT	4//ÿ����4���ؼ�


#define PRINT_FILE_ICON_OFFSET  3


#define PRINT_FILE_ICON_NUM			5//ÿҳ�����ʾ5��
#define PRINT_FILE_FIX_ICON_NUM		3


#define PRINT_FILE_ROOT		"0:"//SD��
#define FILE_NAME_MAX_LENGTH	512    //·�����ȣ����򿪵��ļ�Ŀ¼Ƕ�׵��ļ����ﵽ�����ʱ���Ͳ�������Ƕ����ȥ�ˣ�����Ӧ�������ø�ֵ��С
#define DEST_FILE_NAME_MAX_LENGTH	100//ѡ�е�Ŀ���ļ�/�ļ��г��ȣ�����ʵ�����������ֵ
#define PRINT_FOLDER_FILE_COLOR	 GRAY

#define GCODE_PRINT_ICON_NUM	5//gcode��ӡ�����ͼ����Ŀ
#define GCODE_PRINT_BACK_ICON_ID	4//gcode��ӡ�����˻ؿؼ����

#define GCODE_PRINT_OPTION_BUTTON	3//ѡ����߷��ؿؼ��±�


/******************gcode ��ӡoption �ؼ�ѡ�����******************/
#define GCODE_OPTION_ICON_NUM			5 //pause �� start �����ؼ�����ͬһλ��
#define GCODE_OPTION_STOP_BUTTON_ID		0
#define GCODE_OPTION_PAUSE_BUTTON_ID	1
#define GCODE_OPTION_MORE_BUTTON_ID		2
#define GCODE_OPTION_BACK_BUTTON_ID	 	3
#define GCODE_OPTION_START_BUTTON_ID	4



/**************************gcode��ӡ option->more ����***************************/
#define GCODE_OPTION_TO_MORE_ICON_NUM			6
#define GCODE_OPTION_TO_MORE_PREHEAT_BT_ID			0
#define GCODE_OPTION_TO_MORE_FAN_BT_ID				1
#define GCODE_OPTION_TO_MORE_SPEED_BT_ID				2 //��ӡ�ٶ�����
#define GCODE_OPTION_TO_MORE_CHANGE_FILA_BT_ID				3
#define GCODE_OPTION_TO_MORE_FILA_BT_ID				4

#define GCODE_OPTION_TO_MORE_BACK_BT_ID				5


#define SD_UNPLUG_UI_ICON_NUM				2
#define SD_UNPLUG_UI_STOP_BT_ID				0
#define SD_UNPLUG_UI_BACK_BT_ID				1











/******************************************************************/
_gui_dev *printUIdev = NULL;
static uint16_t	bmp_sart_offx = 1;//��һ��ͼ�����ߵ�һ��ͼƬ����߿����
static uint16_t bmp_sart_offy = 24 ;//��һ��ͼ����ߵ�һ��ͼƬ�������������
static uint8_t hasGotoConfirmPrintFileScreen = 0;//����gcode�ļ�ȷ�Ͻ����־
static uint8_t sdCardIconDispFlag = 0;//SD���β���ʾ�����л�ͼ����Ʊ�־
static	uint8_t	 isPauseStart ;	//������ͣ/��ӡ���ִ�в�����־
typedef enum 
{
	NONE_PRINT_ICON = 0XFF,
	
	IS_PRINT_UP_ICON = 0,
	IS_PRINT_DOWN_ICON = 1,
	IS_PRINT_BACK_ICON = 2,
	
	IS_PRINT_FIRST_ICON = 3,
	IS_PRINT_SECOND_ICON = 4,
	IS_PRINT_THIRD_ICON = 5,
	IS_PRINT_FORTH_ICON = 6,
	
	IS_PRINT_FIFTH_ICON = 7,

		
}_printUI_selected_button_e;

_printUI_selected_button_e PRINT_SAVED_BUTTON = NONE_PRINT_ICON;
//icon��·����,MOVE����ͼ��
char *const printui_icon_path_tbl[GUI_LANGUAGE_NUM][PRINT_FILE_FIX_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_pageUp.bin",//0	    
	"1:/SYSTEM/PICTURE/ch_bmp_pageDown.bin",	   //1	 
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//2
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_pageUp.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_pageDown.bin",	   //1	 
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//2		
	}
	
};  
char *const printui_folderIcon_path [GUI_LANGUAGE_NUM]= 
{
	"1:/SYSTEM/PICTURE/ch_bmp_fileSys.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fileSys.bin"
};

char *const printui_fileIcon_path [GUI_LANGUAGE_NUM]=
{
	 "1:/SYSTEM/PICTURE/ch_bmp_file.bin",
	 "1:/SYSTEM/PICTURE/en_bmp_file.bin"
};



char *const print_confirm_infor[GUI_LANGUAGE_NUM] =
{
	"�Ƿ��ӡ���ģ��",
	 "Are you sure to print the model?"
};

char *const print_stop_infor[GUI_LANGUAGE_NUM] = 
{
	"�Ƿ�ֹͣ��ӡ���ģ��",
	"Sure to stop printing the model?"
};

char *const print_confirm_title[GUI_LANGUAGE_NUM] =
{
	"׼��/ѡ��",
	"Prepare/pickUp"
};

char *const gcode_print_title [GUI_LANGUAGE_NUM]= 
{
	"��ӡ->",
	"Print->"
};

char *const gcode_print_time [GUI_LANGUAGE_NUM] = 
{
	"ʱ��: ",
	"Time: "
};

char *const optToMoreUI_title[GUI_LANGUAGE_NUM] =
{
	"��ӡ.../����/����",
	 "Print.../option/more"
};

char *const optToMoreFanUI_title[GUI_LANGUAGE_NUM]= 
{
	"��ӡ.../����/����/����",
	"Print.../option/more/fan"
};

char *const optToMoreChangeFilaUI_title[GUI_LANGUAGE_NUM]= 
{
	"��ӡ.../����/����/����",
	"Print.../option/more/change filament"
};

char *const optToMoreFilaIOUI_title[GUI_LANGUAGE_NUM] = 
{
	"��ӡ.../����/����/������",
	"Print.../option/more/filament in/out"
};





//SD���γ���ʾ��Ϣ
char *const sdCardUnplug_title [GUI_LANGUAGE_NUM]=
{
	"׼��/��ӡ",
	 "Prepare/print"
};

char *const sdCardUnplug_remind_infor1 [GUI_LANGUAGE_NUM]= 
{
	"����洢��",
	"Pls Check if the SD card"
};

char *const sdCardUnplug_remind_infor2 [GUI_LANGUAGE_NUM]= 
{
	"���ߴ�ӡ�ļ�ȱʧ",
	"or print file is lost"
};

char *const sdCardUnplug_remind_infor3[GUI_LANGUAGE_NUM] = 
{
	"->���ٴβ���洢��",
	"->Then try again!"
};

char *const sdCardUnplug_remind_infor4 [GUI_LANGUAGE_NUM]=
{
	"->�Ƿ���ͣ��?",
	 "-> be sure stop print?"
};



char *const sdCardplug_remind_infor1[GUI_LANGUAGE_NUM] = 
{
	"->�洢���Ѳ���,�뷵��",
	"->SD is pluged in,pls back"
};

char *const sdCardUnplugIcon_path[GUI_LANGUAGE_NUM][SD_UNPLUG_UI_ICON_NUM] = 
{
	{
	"1:/SYSTEM/PICTURE/ch_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",
	}
	
	
};



char *const gcodePrintui_icon_path_tbl[GUI_LANGUAGE_NUM][GCODE_PRINT_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_bed_no_words.bin",//0	    
	"1:/SYSTEM/PICTURE/ch_bmp_fan_no_words.bin",	   //1	 
	"1:/SYSTEM/PICTURE/ch_bmp_extru1_no_words.bin",	//2
	
	"1:/SYSTEM/PICTURE/ch_bmp_menu.bin",	//3
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//4
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_bed_no_words.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_fan_no_words.bin",	   //1	 
	"1:/SYSTEM/PICTURE/en_bmp_extru1_no_words.bin",	//2
	
	"1:/SYSTEM/PICTURE/en_bmp_menu.bin",	//3
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//4
		
	}

};  




char *const optionui_icon_path_tbl[GUI_LANGUAGE_NUM][GCODE_OPTION_ICON_NUM]=
{ 

	{
	"1:/SYSTEM/PICTURE/ch_bmp_stop.bin",//0	    
	"1:/SYSTEM/PICTURE/ch_bmp_pause.bin",	   //1	 
	"1:/SYSTEM/PICTURE/ch_bmp_more.bin",	//2
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//3
	
	"1:/SYSTEM/PICTURE/ch_bmp_resume.bin",	//4
	},
	{
	
	"1:/SYSTEM/PICTURE/en_bmp_stop.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_pause.bin",	   //1	 
	"1:/SYSTEM/PICTURE/en_bmp_more.bin",	//2
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//3
	
	"1:/SYSTEM/PICTURE/en_bmp_resume.bin",	//4
		
	}
	
};  



char *const opMoreui_icon_path_tbl[GUI_LANGUAGE_NUM][GCODE_OPTION_TO_MORE_ICON_NUM]=
{ 
	{
	"1:/SYSTEM/PICTURE/ch_bmp_preHeat.bin",//0	    
	"1:/SYSTEM/PICTURE/ch_bmp_fan.bin",	   //1	 
	"1:/SYSTEM/PICTURE/ch_bmp_speed.bin",//2--��ӡ�ٶ�����
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",	//3
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	//4
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//5
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_fan.bin",	   //1	 
	"1:/SYSTEM/PICTURE/en_bmp_speed.bin",//2--��ӡ�ٶ�����
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",	//3
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",//4
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//5
	
	}
	
};  


char *const fanInfor[GUI_LANGUAGE_NUM] = 
{
	"����:",
	"FAN:"
};

char *const bedInfor[GUI_LANGUAGE_NUM] = 
{
	"�ȴ�:",
	"Bed:"
};
char *const extrudInfor[GUI_LANGUAGE_NUM] = 
{
	"����:",
	"Etru:"
};
_yes_no_e IS_PAUSE_BT = YES;
_print_time_s printTime;
_parameter_state_s printParam;
u8 pause_start_cnt = 0;//����--�ָ�ͼ�꣬ż��--��ͣͼ�꣬��ӡ��ɸ�λ��־
//uint8_t isOption2moreUiPreheatSelected = NO;//gcode�ļ���ӡ ����->option����->more�����е�Ԥ�ȴ�����־,��ӡ��ɾ͸�λ�ñ�־
//uint8_t isOption2moreUiFanSelected = NO;//gcode�ļ���ӡ ����->option����->more�����еķ��ȴ�����־,��ӡ��ɾ͸�λ�ñ�־
uint8_t optionBackBtSet = 0;//��ʼ��ӡ�����е�ѡ��ͼ��/����ͼ���л���־λ��1--�л�������ͼ�꣬0--�л���ѡ��ͼ��

uint8_t isOption2moreUiSelected = NO;//���뵽��ӡ����->option����->more�����־������ĳЩ�����ظ�ʹ�ú󣬷��ؿ���


/************************��ӡ����ͼ��·��***********************/

/******************************************************************/

//static uint32_t filam_adj_update_time = 0;
 uint32_t escape_time;

uint8_t pause_op_counter ;
/**********************************************/

/********************************ȫ�ֱ���**********************/
_gui_dev *pintdev = NULL;
_gui_dev *startPintdev = NULL;
_gui_dev *optiondev = NULL;//gcode�ļ���ӡѡ�����
_gui_dev *optionToMoredev = NULL;//gcode�ļ���ӡѡ�����->more ����
_gui_dev *sdCardDev = NULL;//SD card �γ�������

_progressbar_obj* prgb = NULL;
_btn_obj **screen_key_group = NULL;




_file_page_ctrl_block_s *curFilePageCtrlBlock = NULL;


uint8_t  curPageCnt = 0;


float step_value[3] = {45.0, 190.0, 50.0}; //bed--45C��Extrude--190C��Fan--50%

float set_value[3] = {45.0, 190.0, 50.0}; //bed--45C��Extrude--190C��Fan--50%//preheatUI����




uint8_t adj_value_flag = 0;//�趨�¶ȡ�����ֵ��id��־
_GUI_TURE_OR_FALSE is_set_value = IS_FALSE;//����趨�ȴ�������ͷ�ͷ���

//ȷ������ȡ��  
char *const CONFIRM_CAPTION_TBL[GUI_LANGUAGE_NUM][2]=
{
	{"ȷ��","ȡ��"},
	{"YES","NO"}
}; 


uint8_t *printFileName = NULL;//��¼��ǰ��ӡ���ļ���������Ŀ¼
uint8_t *printFileNameNOPrefix = NULL;//��¼��ǰ��ӡ���ļ���,ȥ��ǰ׺"0:/GCODE/"


_pause_play_e PRINT_STATE = NORMAL_PLAY;
_is_pwoff_print_e IS_PWOFF_PRINT = NORMAL_RESET;
volatile unsigned char temp_cmd_index ;
/**********************************************/


void update_print_time(_print_time_s *time)
{
	uint16_t x_start, y_start;
	 uint16_t width,hight;
	 static uint32_t print_update_time = 0;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	
	 x_start = time->x;
	 y_start = time->y;
	 width = lcddev.width;
	 hight = 16;

	 //����
	 if( print_update_time <= millis())
	 {
		print_update_time = millis() + 300;
		
		if(card.FILE_STATE == PS_STOP || card.FILE_STATE == PS_PRINT_OVER_NOR ) 
		escape_time = stoptime / 1000;//������ӡ���� + �û�������ӡ
		
        else if (card.FILE_STATE == PS_NO) 
		{
            escape_time = 0;
        }
        else escape_time = (millis() - starttime) / 1000;//ms->s exchage
		
		escape_time += powerDownPrintTime;//�����ϴζϵ��ӡ��ʱ��
		
        time->hour = escape_time / 60 / 60;
        time->minute = escape_time / 60 % 60;
		time->second = escape_time % 60;
		print_time_init(time);
		
	 }
}

void print_time_init(_print_time_s *time)
{
	 
	 uint16_t x_start, y_start;
	 uint16_t width,hight;

	 uint16_t f_size = 16;
	 uint16_t length;
	 uint16_t offset;
	 uint8_t hour;
	 uint8_t minute;
	 uint8_t second;
	 
	 width = 12*(f_size / 2);
	 hight = f_size;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	 x_start = time->x;
	 y_start = time->y;
	hour = time->hour;
	minute = time->minute;
	second = time->second;
	
	 length = strlen((char *)gcode_print_time[LANGUAGE]);
	 offset = length*(f_size / 2);
	 gui_fill_rectangle_DMA(x_start + offset, y_start,width,hight,BACK_COLOR);
	 Show_Str(x_start,y_start,width,hight,(u8 *)gcode_print_time[LANGUAGE],f_size,1);//��ʾ"Time: "
	 
    gui_show_num(x_start + length*(f_size / 2), y_start, 2, POINT_COLOR, f_size, hour, 1); //��ʾСʱxx
    LCD_ShowString(x_start + length*(f_size / 2) + 3 * (f_size / 2), y_start , width, hight, f_size, (u8*)":"); //��ʾ��
    gui_show_num(x_start +  length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2),  y_start, 2, POINT_COLOR, f_size, minute, 1); //��ʾ����xx
    LCD_ShowString(x_start + length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2) + 3 * (f_size / 2), y_start,width, hight, f_size, (u8*)":"); //��ʾ��
    gui_show_num(x_start +  length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2)+ 3 * (f_size / 2) +  2 * (f_size / 2),  y_start , 2, POINT_COLOR, f_size, second, 1); //��ʾ��xx	
	 
		
}

//�ļ�ҳ���ƿ鴴������ʼ��ʱ�����ظ�����
_file_page_ctrl_block_s * create_file_page_ctrl_block(u8 * path)
{
	_file_page_ctrl_block_s * filePageCtrlBlock;
	u16 fileNameLength;
	u16 destFileNameLength;
	//_file_page_ctrl_block_s�ṹ��
	filePageCtrlBlock=(_file_page_ctrl_block_s*)gui_memin_malloc(sizeof(_file_page_ctrl_block_s));	//�����ڴ�
	if(filePageCtrlBlock==NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//�ڴ���䲻��.
	}
	gui_memset((u8*)filePageCtrlBlock,0,sizeof(_file_page_ctrl_block_s));//�������г�Ա����
	
#if _USE_LFN						
 	fileNameLength = _MAX_LFN * 4 + 1;
	destFileNameLength = _MAX_LFN * 2 + 1;
#else
	fileNameLength = FILE_NAME_MAX_LENGTH;
	destFileNameLength = DEST_FILE_NAME_MAX_LENGTH;
#endif	 	

	//����Ŀ¼����
	filePageCtrlBlock->path=(u8*)gui_memin_malloc(fileNameLength);	//�����ڴ�
	if(filePageCtrlBlock->path==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//�ڴ���䲻��.

	}

	strcpy((char *)filePageCtrlBlock->path, (char *)path);
	
	filePageCtrlBlock->destFileName = (u8*)gui_memin_malloc(destFileNameLength);	//�����ڴ�
	if(filePageCtrlBlock->destFileName==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock->path);
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//�ڴ���䲻��.
	}
	
	
	//�ļ�\�ļ���GUI����
	filePageCtrlBlock->fileGuiAttr = (_file_gui_attribute_s*)gui_memin_malloc(sizeof(_file_gui_attribute_s));	//�����ڴ�
	if(filePageCtrlBlock->fileGuiAttr==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock->path);
		gui_memin_free(filePageCtrlBlock->destFileName);
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//�ڴ���䲻��.

	}
	gui_memset((u8*)filePageCtrlBlock->fileGuiAttr,0,sizeof(_file_gui_attribute_s));//�������г�Ա����
	
	filePageCtrlBlock->fileGuiAttr->activeButtonNum = PRINT_FILE_ICON_NUM;
	filePageCtrlBlock->fileGuiAttr->fixButtonNum = PRINT_FILE_FIX_ICON_NUM;
	filePageCtrlBlock->fileGuiAttr->pageCnt = 1;//�ӵ�һҳ��ʾ
	
	filePageCtrlBlock->fliter = 0;//�����ļ�����(�����ļ���)
	filePageCtrlBlock->fliterex = 0xffff;//����С����й���(����С�������Ҫ��)
	filePageCtrlBlock->fileDepth = 0;
	filePageCtrlBlock->destFileType = NO_FILE_TYPE;
	
	filePageCtrlBlock->FileNameListAttributeTab = NULL;
	filePageCtrlBlock->FileNameListTab = NULL;	
	filePageCtrlBlock->atualReadItems = 0;
	filePageCtrlBlock->readItemsOffSet = 0;
	filePageCtrlBlock->selectedButtonID = NONE_PRINT_ICON;
	return filePageCtrlBlock;
}
//�ͷ��ļ�ҳ����ƿ��ڴ�
static void free_file_page_ctrl_block(_file_page_ctrl_block_s *pFile)
{
	u8 i;

	u8 fileNameListItems;
	if(pFile == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error\r\n",__FILE__,__LINE__);
		return;
	}
	if(pFile->fileGuiAttr != NULL)//�ļ�GUI����
	{
		fileNameListItems = pFile->fileGuiAttr->activeButtonNum;//����ļ����б������
	}
	
	if(pFile->FileNameListAttributeTab != NULL)//�ļ�/�ļ������Ա�
	{
		gui_memin_free(pFile->FileNameListAttributeTab);
	}
	
	if(pFile->path != NULL)//�ļ�/�ļ���Ŀ¼
	{
		gui_memin_free(pFile->path);
	}
	
	if(pFile->FileNameListTab != NULL)//�ļ�/�ļ��������б�
	{
		for(i = 0 ; i < fileNameListItems ; i++)
		{
			if(pFile->FileNameListTab[i] != NULL) 
			gui_memin_free(pFile->FileNameListTab[i]);//��������
		}
		gui_memin_free(pFile->FileNameListTab);//��������
	}
	
	if(pFile->fileGuiAttr != NULL)//�ļ�GUI����
	{
		gui_memin_free(pFile->fileGuiAttr);
	}

	if(pFile->destFileName != NULL)//ѡ�е�Ŀ���ļ���
	{
		gui_memin_free(pFile->destFileName);
	}
	gui_memin_free(pFile);

	
	gui_memin_free(printUIdev->icon);
	gui_memin_free(printUIdev);
	printUIdev =NULL;
	pFile =  NULL;
	curFilePageCtrlBlock =  NULL;//ȷ��ָ��Ϊ0
}
//��ʼ��1���ļ���ʾ������ƿ�
u8 init_file_page_ctrl_block(u8 * path)
{
	u8 rtv = 0;
	curFilePageCtrlBlock = create_file_page_ctrl_block(path);
	curFilePageCtrlBlock->fliter = FLBOX_FLT_GCODE;//�ļ��������
	return rtv;
}

//��Ѱ��ǰĿ¼������ļ�/�ļ�����Ŀ
//����ֵ���Ϊ0XFFFFFFFE,����ǰĿ¼���ļ�/�ļ�����Ŀ���ܳ�������������ֵ
u32 get_current_root_page_items(_file_page_ctrl_block_s *pFile)
{
	u8 res=0; 
	
 	DIR filedir;		
	FILINFO fileinfo;	 	   
	
	u16 type=0XFFFF;
	u16 typeex=0;
	u32 totalItems = 0;	  
	u16 totalPage;
	u16 folderCnt = 0;
	u16 fileCnt = 0;
	
	u8 *fn;   //This function is assuming non-Unicode cfg. 
	if(pFile == NULL)  //��Ч·��
	{
		totalItems = 0xFFFFFFFF;
		PRINTUI_DEBUG("file:%s,line:%d -> path error\r\n",__FILE__,__LINE__);
		return totalItems;
	}

	if(pFile->path == NULL)  //��Ч·��
	{
		totalItems = 0xFFFFFFFF;
		PRINTUI_DEBUG("file:%s,line:%d -> path error\r\n",__FILE__,__LINE__);
		return totalItems;
	}
	
#if _USE_LFN						
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = gui_memin_malloc(fileinfo.lfsize);
	if(fileinfo.lfname==NULL)
	{
		totalItems = 0xFFFFFFFF;
		PRINTUI_DEBUG("file:%s,line:%d malloc error \r\n",__FILE__,__LINE__);
		return totalItems;
	}

   	gui_memset((char *)fileinfo.lfname,0,fileinfo.lfsize);
#endif	 
	
	res=f_opendir(&filedir,(const TCHAR*)pFile->path); //��һ��Ŀ¼
	if(res==FR_OK)		
	{
		
	}
	while(res==FR_OK)//������ǰĿ¼�µ������ļ�
	{
		
			res=f_readdir(&filedir,&fileinfo);//��ȡ�ļ���Ϣ
			if(res!=FR_OK||fileinfo.fname[0]==0)
			{
				break;		//������/��ĩβ��,�˳�   
			}
			if (fileinfo.fname[0]=='.'&&fileinfo.fname[1]==0)continue;//���Ա���Ŀ¼
			fn=(u8*)(*fileinfo.lfname?fileinfo.lfname:fileinfo.fname);
			
			
		if(fileinfo.fattrib & AM_DIR)//�ļ���
		{
			totalItems++;
			folderCnt++;
			//DBG("->file:%s\r\n",fn);				
		}
		else if(fileinfo.fattrib & AM_ARC )//��һ���鵵�ļ���mark=1
		{
			type=f_typetell(fn);	//�������
			typeex=type&0XF;		//���С��ı��
			type>>=4;				//��ô���ı��
			if(((1<<type) & pFile->fliter) && ((1<<typeex) & pFile->fliterex ) )//��С���Ͷ���ȷ��Ŀ���ļ�
			{
				totalItems++;
				fileCnt++;
				//DBG("->file:%s\r\n",fn);				
			}
			else continue;	//����Ҫ������	
		}
		else continue;		//��������һ�� 	
	}
	 res=f_closedir(&filedir); //�ص�Ŀ¼
	gui_memin_free(fileinfo.lfname);
	
	totalPage = totalItems/pFile->fileGuiAttr->activeButtonNum ;//��ҳ
	if(totalItems % pFile->fileGuiAttr->activeButtonNum )totalPage += 1;//����һҳ��һҳ��
	
	pFile->fileGuiAttr->TotalPage = totalPage;
	
	DBG("->totalItems:%d = folderNum:%d,fileNum:%d\r\n",totalItems,folderCnt,fileCnt);	
	return totalItems;
	
}

//����ָ��Ŀ¼��ָ���������ļ�/�ļ���
//�����ļ����б������б�ȣ����ļ�ƫ�Ʊ�����
void scan_root_file(_file_page_ctrl_block_s *pFile)
{
	
	u8 res=0; 
 	DIR filedir;		
	FILINFO fileinfo;	 	   
	u16 type=0XFFFF;

	u16 typeex=0;
	u16 readItemOffSet;//��ȡ�ļ�ƫ�ƣ���Ϊ�л�����ʱ��Ҫ�л���һҳ���ļ�/�ļ��У�����ƫ��
	u16 readCnt = 0;
	u16 aquireItems;//������ȡ�ļ�/�ļ�����Ŀ
	u16 actualReadItems = 0;//ʵ�ʶ�ȡ���ļ�/�ļ�����Ŀ
	
	u8 *path= NULL;//��ǰ·��
	u8 *fn = NULL;   //This function is assuming non-Unicode cfg. 
	u32 findex;	 
	u16 fnLength;
	u16 folderCnt = 0;
	u16 fileCnt = 0;
	if(pFile == NULL)//�ļ�ָ����Ч�Լ��
	{
		PRINTUI_DEBUG("file:%s,line:%d -> pFile error\r\n",__FILE__,__LINE__);
		return ;		
	}
#if _USE_LFN						
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = gui_memin_malloc(fileinfo.lfsize);
	if(fileinfo.lfname==NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d malloc error \r\n",__FILE__,__LINE__);
		return ;
	}
   	gui_memset((char *)fileinfo.lfname,0,fileinfo.lfsize);
#endif	 
	if(pFile->fileGuiAttr==NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
		gui_memin_free(fileinfo.lfname);
		return ;			//�ڴ���䲻��.
	}

	readItemOffSet = pFile->readItemsOffSet;//��ȡ��ȡ�ļ�ƫ��
	path = pFile->path;//��ȡ��ǰĿ¼
	

	aquireItems = pFile->fileGuiAttr->activeButtonNum;//���һ�α����ļ�/�ļ�����Ŀ
	if(pFile->FileNameListTab == NULL)//ΪaquireItems���ļ�/�ļ���������ָ�������ڴ�,��һ����Ѱ
	{
		pFile->FileNameListTab = (u8**)gui_memin_malloc(sizeof(u8 *) * aquireItems);	//ΪaquireItems �� u8 ��ָ������ڴ�
		if(pFile->FileNameListTab==NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
			gui_memin_free(fileinfo.lfname);
			return ;			//�ڴ���䲻��.
		}		
	}
	//������һ����Ѱ���ļ����б�
	gui_memset((u8**)pFile->FileNameListTab,0,sizeof(u8 *)*aquireItems);//�������г�Ա����,��p[0]=p[1]=p[2]=...=p[aquireItems - 1] =NULL;
	
	//��δΪÿ���ļ�/�ļ��е����ֵľ��崢�泤�������ڴ棬���ݱ����õ����ļ�/�ļ��������������ڴ�

	
	if(pFile->FileNameListAttributeTab == NULL)//ΪaquireItems���ļ�/�ļ��ж�Ӧ�����������ڴ棬ʵ��ʹ���Զ������ļ�/�ļ�������Ϊ׼,��һ����Ѱ
	{
		pFile->FileNameListAttributeTab = (u8*)gui_memin_malloc(sizeof(u8 ) * aquireItems);	//�����ڴ�,FileNameListAttributeTab�� u8 * ���ͣ�1��bytes--��Ӧһ������
		
		if(pFile->FileNameListAttributeTab==NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
			gui_memin_free(fileinfo.lfname);
			gui_memin_free(pFile->FileNameListTab);
			return ;			//�ڴ���䲻��.
		}
	}
	//������һ����Ѱ���ļ����Ա�
	gui_memset((u8*)pFile->FileNameListAttributeTab,0,sizeof(u8) * aquireItems);//�������г�Ա����
	
	
	res=f_opendir(&filedir,(const TCHAR*)path); //��һ��Ŀ¼
	if(res==FR_OK)
	{
		
	}
	while(res==FR_OK)//������ǰĿ¼��aquireItems���ļ�/�ļ���
	{
						
			if(actualReadItems == aquireItems )//����ָ���������ļ�/�ļ���
			{
								
				break;
			}
			findex=filedir.index;//��¼�´��ļ���indexֵ

			res=f_readdir(&filedir,&fileinfo);//��ȡ�ļ���Ϣ
			if(res!=FR_OK||fileinfo.fname[0]==0)
			{
			
			
				break;		//������/��ĩβ��,�˳�   
			}
			if (fileinfo.fname[0]=='.'&&fileinfo.fname[1]==0)continue;//���Ա���Ŀ¼
			fn=(u8*)(*fileinfo.lfname?fileinfo.lfname:fileinfo.fname);
			
			fnLength = strlen((char *)fn) + 1;
						
			
			if(fileinfo.fattrib & AM_DIR)//�ļ���
			{
				readCnt++;
				
				if(readCnt <= readItemOffSet) continue;//��ȡ����һҳ���ļ�/�ļ���
				folderCnt++;
				DBG("#index:%d\r\n",findex);	
					if(pFile->FileNameListTab[actualReadItems]  == NULL)
					{
						pFile->FileNameListTab[actualReadItems] = (u8*)gui_memin_malloc(sizeof(u8 ) * fnLength);
						if(pFile->FileNameListTab[actualReadItems]==NULL)
						{
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							gui_memin_free(fileinfo.lfname);
							gui_memin_free(pFile->FileNameListTab);
							gui_memin_free(pFile->FileNameListAttributeTab);
							for(;actualReadItems > 0;actualReadItems--)gui_memin_free(pFile->FileNameListTab[actualReadItems - 1]);//actualReadItems�������ڴ���ټ�1
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							return ;			//�ڴ���䲻��.
						}
					
					}				
				strcpy((char *)pFile->FileNameListTab[actualReadItems],(char *)fn);
				DBG("->folder:%s\r\n",pFile->FileNameListTab[actualReadItems]);	
				//Ϊ�ļ������Ը�ֵ
				pFile->FileNameListAttributeTab[actualReadItems] = IS_FOLDER;
				
				actualReadItems++;
			
			}
			else if(fileinfo.fattrib & AM_ARC )//��һ���鵵�ļ���mark=1
			{
				type=f_typetell(fn);	//�������
				typeex=type&0XF;		//���С��ı��
				type>>=4;				//��ô���ı��
				if( ((1<<type) & pFile->fliter) && ((1<<typeex) & pFile->fliterex ))//��С���Ͷ���ȷ��Ŀ���ļ�
				{
					readCnt++;
					if(readCnt <= readItemOffSet) continue;//��ȡ����һҳ���ļ�/�ļ���
					fileCnt++;
					DBG("#index:%d\r\n",findex);
					
					if(pFile->FileNameListTab[actualReadItems]  == NULL)
					{
						pFile->FileNameListTab[actualReadItems] = (u8*)gui_memin_malloc(sizeof(u8 ) * fnLength);
						if(pFile->FileNameListTab[actualReadItems]==NULL)
						{
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							gui_memin_free(fileinfo.lfname);
							gui_memin_free(pFile->FileNameListTab);
							gui_memin_free(pFile->FileNameListAttributeTab);		
							for(;actualReadItems > 0;actualReadItems--)gui_memin_free(pFile->FileNameListTab[actualReadItems - 1]);//actualReadItems�������ڴ���ټ�1
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							return ;			//�ڴ���䲻��.
						}
					
					}

					
					strcpy((char *)pFile->FileNameListTab[actualReadItems],(char *)fn);
					
					DBG("->file:%s\r\n",pFile->FileNameListTab[actualReadItems]);	
					//Ϊ�ļ����Ը�ֵ
					pFile->FileNameListAttributeTab[actualReadItems] = IS_GCODE_FILE;

						actualReadItems++;
						
				}
				else continue;	//����Ҫ������	
			}
			else continue;		//��������һ�� 	
			
	}
	pFile->atualReadItems = actualReadItems;
	//��ҳ���ⲿʵ��
	pFile->readItemsOffSet +=  actualReadItems;//���¶�ȡ�ļ�ƫ�ƣ���Ҫ������һ��Ŀ¼ʱ���ļ�ƫ��Ҫ����
		 res=f_closedir(&filedir); //�ص�Ŀ¼
		gui_memin_free(fileinfo.lfname);
		DBG("->folderNum:%d,fileNum:%d\r\n",folderCnt,fileCnt);	
}
//��ñ���Ŀ¼����,���ߴӴ�Ŀ¼�Ĵ�ӡ�ļ����а��������Ŀ¼��gcode�ļ�
u8 * get_current_root_file_name(u8 *path)
{
	u8 pathLength;
	u8 *tempPath = path;
	u8 *fn = NULL;
	u8 fileNameLength = 0;
	pathLength = strlen((char *)tempPath) + 1;
	while(*tempPath != '\0')//�ҵ��ַ���ĩβ
	{
		tempPath++;
	}
	
	while(*tempPath != '/' && pathLength)//���ַ���ĩβ��ǰ��
	{
		tempPath--;
		fileNameLength++;
		pathLength--;
	}
	//�ҵ����һ��Ŀ¼
	tempPath++;//ָ��ָ�����һ��Ŀ¼
	
	fn = (u8*)gui_memin_malloc(fileNameLength );	//�����ڴ�
	if(fn==NULL)
	{	
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//�ڴ���䲻��.
	}
	else
	{
		strcpy((char *)fn,(const char *)tempPath);
	}
	return fn;
}

void free_current_root_file_name_memory(u8 *fn)
{
	if(fn == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d ->parameter error\r\n",__FILE__,__LINE__);
		return;
	}
	else
	{
		gui_memin_free(fn);
	}
}
//��ʾ�ļ�/�ļ���ͼ�����
void display_file_page_icon(_file_page_ctrl_block_s *pFile)
{


	if(pFile == NULL)//�ļ�ָ����Ч�Լ��
	{
		PRINTUI_DEBUG("file:%s,line:%d -> pFile error\r\n",__FILE__,__LINE__);
		return ;		
	}

	if(pFile->FileNameListTab == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d ->error\r\n",__FILE__,__LINE__);
		return ;		
	}

	if(pFile->FileNameListAttributeTab == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d ->error\r\n",__FILE__,__LINE__);
		return ;		
	}

	if(pFile->fileGuiAttr == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d ->error\r\n",__FILE__,__LINE__);
		return ;		
	}	
//Ҫ��ʾ���ļ�\�ļ����������ļ�\�ļ�������������Ӧ���ļ�\�ļ������Ա�,GUI���Զ����ڵ������
	
	draw_file_page_infor(pFile);
	pFile->selectedButtonID = NONE_PRINT_ICON;//��������ʾ����λ���ư���
}

//void draw_file_page_infor(const u8 actIconNum,const u8 fixIconNum,const u8 *fileAttr,const u8 **fileNameList,const u16 curPage,const u16 totalPage,const u8 buttonID,const u8 *path)
void draw_file_page_infor(_file_page_ctrl_block_s *pFile)
{
	uint16_t i,j;
	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font;
	uint8_t iconNum;
	
	uint8_t length;
	
	uint8_t actIconNum;
	uint8_t fixIconNum;
	uint8_t *fileAttr = NULL;
	uint8_t **fnList = NULL;
	uint8_t curPage;
	uint8_t totalPage;
	uint8_t buttonID;
	uint8_t *desFileName = NULL;

	uint8_t fileDepth;
	uint8_t dispOffSet = 0;
	u16 xOff = 2;
	u16 yOff = 5;
	u16 xEnd;
	u16 yEnd;
	u16 strOff;
	
	uint8_t needToDrawGui = 0xff;
	
	static uint8_t timeCnt = 0;	//������������������
	static uint8_t haveDrawed = 0;	//��־�Ƿ��Ѿ��ػ����
	
	font = 16;
	actIconNum = pFile->atualReadItems;//ʵ�ʶ�ȡ�����ļ�\�ļ��и���
	fixIconNum = pFile->fileGuiAttr->fixButtonNum;
	fileAttr = pFile->FileNameListAttributeTab;
	fnList = pFile->FileNameListTab;
	curPage = pFile->fileGuiAttr->pageCnt;
	totalPage = pFile->fileGuiAttr->TotalPage;
	buttonID = pFile->selectedButtonID;
	

	fileDepth = pFile->fileDepth;
	if(redraw_fileSystemUI_flag == IS_TRUE)//�ӱ�Ĺ��ܽ����л����ļ�ϵͳ����
	{
		redraw_fileSystemUI_flag = IS_FALSE;
		timeCnt = 0;
	}
	if(redraw_fileSystemUI_flag == IS_FALSE)
	{
		timeCnt++;
	}
	
	if(timeCnt >= 2)//�ǵ�һ�ν��뱾����
	{
		timeCnt = 2;
		
		if(curPage <= totalPage)//
		{			

			if(buttonID == IS_PRINT_DOWN_ICON || buttonID == IS_PRINT_UP_ICON)//�ؼ�--��һҳ/��һҳ������
			{
				needToDrawGui = 1;//��Ҫ�ػ�UI
				 timeCnt = 0;
				  haveDrawed = 1;
			}	
			
		}
		if(needToDrawGui != 1) //�����ػ�
		{			
			return;
		}
	}
	if(timeCnt == 1 && haveDrawed == 1) //�����ػ�
	{
		haveDrawed = 0;
		return;
	}
	
	BACK_COLOR = BLACK;
    POINT_COLOR = BLUE;
	iconNum =actIconNum + fixIconNum;
	desFileName = get_current_root_file_name(pFile->path);//��ñ�Ŀ¼���֣���ʾ�������ͷ��ڴ�
	LCD_Clear_DMA(BLACK);//����
	length = strlen((char*)print_confirm_title[LANGUAGE]);
	Show_Str(xOff,yOff,lcddev.width,font,(u8 *)print_confirm_title[LANGUAGE],font,1);	
	LCD_ShowNum(xOff + length*(font/2),yOff,curPage,2,font);
	LCD_ShowString(xOff + length*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/");	
	LCD_ShowNum(xOff + length*(font/2)+ 3*(font/2),yOff,totalPage,2,font);
	LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2),yOff,lcddev.width,font,font,(u8 *)PRINT_FILE_ROOT);
	if(fileDepth == 1)//��һ��Ƕ��
	{
		xEnd = lcddev.width;
		yEnd = yOff + font;
		LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/");	
		if(desFileName != NULL)
		gui_show_ptstr(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2)+ 3*(font/2),yOff,xEnd,yEnd,dispOffSet,POINT_COLOR,font,(u8 *)desFileName,1);
	}
	if(fileDepth > 1 )//ȷ�����ļ�����·����ȴ���1
	{
		xEnd = lcddev.width;
		yEnd = yOff + font;
		LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/.../");	
		if(desFileName != NULL)
		gui_show_ptstr(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2)+ 2*(font/2) + 5*(font/2),yOff,xEnd,yEnd,dispOffSet,POINT_COLOR,font,(u8 *)desFileName,1);
	}
	
	free_current_root_file_name_memory(desFileName);
	
	if(printUIdev == NULL )	//Ϊ����������������ڴ�,��һ�γ�ʼ�� ����֮ǰ�Ѿ���ʼ���������ٴγ�ʼ��
	{
		printUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( printUIdev == 0)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
	}
	
	if(((u32)printUIdev->icon & RAM_ADDR) == RAM_ADDR)//֮ǰ�Ѿ�������ڴ���
	{
		gui_memin_free(printUIdev->icon);
	}
	
	printUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*iconNum);//ÿ�ζ����������ڴ�
		
	if( printUIdev->icon == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}

	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	printUIdev->status = 0x3F ;
	
	//Ϊ�̶���ͼ�긳ֵ,�ڶ��ź���������ͼ��
			for(j=0;j < fixIconNum; j++)
			{			
				printUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx + bmp_between_offx;
				printUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
				printUIdev->icon[j].width = PICTURE_X_SIZE;
				printUIdev->icon[j].height = PICTURE_Y_SIZE;
				printUIdev->icon[j].path = (u8*)printui_icon_path_tbl[LANGUAGE][j];

			}
	
//Ϊ��̬��ͼ�긳ֵ����̬ͼ���±��ƫ��ΪfixIconNum������һ����̬ͼ���Ӧ���±��Ϊ0+3=3����������		
		for(i = 0,j=fixIconNum;j < actIconNum + fixIconNum ; j++,i++)
		{
			
			if(i == PRINT_FILE_ICON_NUM - 1)//���ͼ�����5��ʱ����Ҫ�ڵڶ�����ʾ
			{
				if( j == actIconNum + fixIconNum - 1)//�ڶ��ŵ�һ��ͼ��
				{
					printUIdev->icon[j].x = bmp_sart_offx ;
					printUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
				}
			}
			else
			{
				printUIdev->icon[j].x = bmp_sart_offx + i*bmp_between_offx  ;
				printUIdev->icon[j].y = bmp_sart_offy;
			}
			
			if(fileAttr[i] == 1)//gcode�ļ�
			{
				printUIdev->icon[j].path = (u8*)printui_fileIcon_path[LANGUAGE];	
			}
			else printUIdev->icon[j].path = (u8*)printui_folderIcon_path[LANGUAGE];//�ļ���
			
			printUIdev->icon[j].width = PICTURE_X_SIZE;
			printUIdev->icon[j].height = PICTURE_Y_SIZE;
			

		}

	printUIdev->totalIcon = iconNum;
	strOff = PICTURE_Y_SIZE - 2*font;
	POINT_COLOR = GREEN;
		for(i = 0,j=fixIconNum;j < actIconNum + fixIconNum ; j++,i++)//�Ȼ��ͼ��
		{
			display_bmp_bin_to_LCD((u8*)printUIdev->icon[j].path, printUIdev->icon[j].x , printUIdev->icon[j].y, printUIdev->icon[j].width, printUIdev->icon[j].height, printUIdev->icon[j].color);
			
			gui_show_ptstr(printUIdev->icon[j].x, printUIdev->icon[j].y + strOff, printUIdev->icon[j].x + printUIdev->icon[j].width ,printUIdev->icon[j].y + strOff + font,0,POINT_COLOR,font,fnList[i],1);
		}	
	POINT_COLOR = BLUE;

		for(j=0;j < fixIconNum; j++)
		{
			display_bmp_bin_to_LCD((u8*)printUIdev->icon[j].path, printUIdev->icon[j].x , printUIdev->icon[j].y, printUIdev->icon[j].width, printUIdev->icon[j].height, printUIdev->icon[j].color);
		
		}
		


}

//����һ��Ŀ¼
//����ֵ��0--�ɹ����أ���·�������Ѿ������ı䣬����ֵ--����
u8 goto_next_fileRoot(u8 * newFn, u8 * srcFn)
{
	u8 *tempFn = NULL;
	u8 rtv = 0xff;
	u8 tempLength;
	if(newFn == NULL || srcFn == NULL)
	{
	
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error!\r\n",__FILE__,__LINE__);
		return rtv;
	}
	tempLength = strlen((char *)newFn) + strlen((char *)srcFn) + 2;//1--��ӵ�1��'/'�ַ�
	tempFn = (u8*)gui_memin_malloc(sizeof(u8 ) * tempLength);
	if(tempFn == NULL) 
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memoery error!\r\n",__FILE__,__LINE__);
		return rtv;
	
	}
	gui_memset((u8*)tempFn,0,sizeof(u8 )*tempLength);
	strcpy((char *)tempFn,(const char *)newFn);
	strcat((char*)tempFn,(const char*)"/");	//���б��
	strcat((char*)tempFn,(const char*)srcFn);
	tempFn[tempLength - 1] = '\0';
	
	strcpy((char *)newFn,(const char *)tempFn);
	
	gui_memin_free(tempFn);//�ͷž�Ŀ¼���ڴ�
	tempFn = NULL;
	
	rtv = 0;
	return rtv;
}

//������һ��Ŀ¼
//����ֵ��0--�ɹ����أ���·�������Ѿ������ı䣬1-- ��ǰĿ¼Ϊ��Ŀ¼������ֵ--����
u8 goto_prev_fileRoot(u8 * srcFn)
{
	u8 *tempFn = NULL;
	u8 *newFn = NULL;
	u8 rtv = 0xff;
	u8 tempLength;
	u8 offSetPoint;
	u8 rootDepth = 0;
	u8 cnt = 0;
	if(srcFn == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error!\r\n",__FILE__,__LINE__);
		return rtv;
	}
	tempFn = srcFn;
	cnt = strlen((char *)srcFn);
	while(*tempFn != '\0')//�ҵ��ַ���ĩβ
	{
		if(*tempFn == '/')	rootDepth++;
		tempFn++;
	}
	if(rootDepth == 0 ) //�ļ�·�����,��Ŀ¼Ϊ��"0:/GCODE"
	{
		rtv = 1;
		PRINTUI_DEBUG("file:%s,line:%d -> first root \r\n",__FILE__,__LINE__);

	}
	else if(rootDepth >= 1)
	{
		while(*tempFn != '/' && cnt)//���ַ���ĩβ��ǰ��
		{
			tempFn--;
			cnt--;
		}
		if(cnt == 0) 
		{
			rtv = 0xff;	
			PRINTUI_DEBUG("search end\r\n");
		}
		else
		{
			offSetPoint = tempFn - srcFn;//�õ��ַ�����һ���ַ��뵹����һ��Ŀ¼��λ��ƫ���ֽ���
			tempLength = offSetPoint + 1;//2--�ַ�����һ���ַ� + '\0'�ַ�ռ�õ��ֽ�
			
			newFn = (u8*)gui_memin_malloc(sizeof(u8 ) * tempLength);
			strncpy((char *)newFn,(const char *)srcFn,tempLength);
			newFn[tempLength - 1] ='\0';
			strcpy((char *)srcFn,(const char *)newFn);
					
			gui_memin_free(newFn);//�ͷž�Ŀ¼���ڴ�
			newFn = NULL;			
			rtv = 0;		
		
		}
	}
	return rtv;
}
//��ȡ��ӡ�ļ����֣���ص��ڴ�Ҫ�ڽ�����ӡ������ ���� SD���γ��� �ͷ�
static u8 *pickup_print_file_name(u8 *path, u8 *fn)
{
	
	u8 *printFileName = NULL;
	u8 length;
	if(path == NULL || fn == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error!\r\n",__FILE__,__LINE__);
	}
	else
	{
		length = strlen((char *)path) + strlen((char *)fn) + 2;//2--'/' + '\0'�ַ�
		printFileName = (u8*)gui_memin_malloc(sizeof(u8 ) * length);
		if(printFileName == NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
		}
		else
		{
			strcpy((char *)printFileName,(const char *)path);
			strcat((char*)printFileName,(const char*)"/");	//���б��
			strcat((char*)printFileName,(const char*)fn);
			printFileName[length - 1] = '\0';
		}
		
	}
	return printFileName;
}

void free_printFileName(u8 *fileN)
{
	if(fileN != NULL)
	{
		gui_memin_free(fileN);
		fileN = NULL;
		printFileName = NULL;
		PRINTUI_DEBUG("free print file name OK\r\n");
	}
}
//����ɨ�账��
//����ֵ��0--�л����ܽ��棬1--������һ��Ŀ¼ 2--ѡ�д�ӡ�ļ� ����ֵ--δ����
u8  exacute_button_even(_file_page_ctrl_block_s *pFile)
{

	
	u8 isRootChanged = 0xff;
	u8 isBackPrev = 0xff;
	u16 actualItem;//��Ѱ��ǰĿ¼�õ��ĵ����ļ�����
	u8 rtv = 0XFF;
	u8 *fileAttrubeTab =NULL;//�ļ����Ա�
	u8 **fileNameTab = NULL;
	
	_printUI_selected_button_e PRINT_SELECTED_BUTTON;
					

	actualItem = pFile->atualReadItems;//���ʵ�ʵĻͼ�����
	fileAttrubeTab = pFile->FileNameListAttributeTab;
	fileNameTab = pFile->FileNameListTab;


	curPageCnt = pFile->fileGuiAttr->pageCnt;//��õ�ǰҳ���
	display_file_page_icon(pFile);//��ʾ����
	PRINT_SELECTED_BUTTON = (_printUI_selected_button_e)gui_touch_chk(printUIdev);
	
	switch (PRINT_SELECTED_BUTTON)
		{
			case IS_PRINT_UP_ICON://��һҳ
			PRINT_SAVED_BUTTON = IS_PRINT_UP_ICON;
			pFile->selectedButtonID = IS_PRINT_UP_ICON;
			if(curPageCnt > 1 )
			{
				curPageCnt--;
				pFile->fileGuiAttr->pageCnt = curPageCnt;
				pFile->readItemsOffSet -= (pFile->fileGuiAttr->activeButtonNum + pFile->atualReadItems);//���ļ�ƫ�Ʒ���Ϊ��һҳ��һ��Ԫ��ƫ��ֵ,��ҳԪ�� + ��һҳԪ�� = Ҫ������
				scan_root_file(pFile);//�����ļ�/�ļ����б��ļ����Ե�
			}
			PRINTUI_DEBUG("->page up\r\n");
			break;
				
			case IS_PRINT_DOWN_ICON://��һҳ
			
			PRINT_SAVED_BUTTON = IS_PRINT_DOWN_ICON;
			pFile->selectedButtonID = IS_PRINT_DOWN_ICON;
			if(curPageCnt < pFile->fileGuiAttr->TotalPage)
			{
				curPageCnt++;	
			pFile->fileGuiAttr->pageCnt = curPageCnt;
			scan_root_file(pFile);//�����ļ�/�ļ����б��ļ����Ե�
				
			}
			PRINTUI_DEBUG("->page down\r\n");
				break;
				
			case IS_PRINT_BACK_ICON://����
			pFile->selectedButtonID = IS_PRINT_BACK_ICON;
			isBackPrev = goto_prev_fileRoot(pFile->path);
			if(isBackPrev == 0)//������һ��Ŀ¼�ɹ�
			{
				pFile->fileDepth--;//�ļ�Ƕ����ȼ��٣��ڸ�Ŀ¼������
				
				redraw_fileSystemUI_flag = IS_TRUE;
				pFile->readItemsOffSet = 0;//�л�������߷�����һ��Ŀ¼�����ļ�/�ļ���ƫ������
				get_current_root_page_items(pFile);//��õ�ǰĿ¼���ܵ��ļ�/�ļ�����Ŀ,����ǰĿ¼�仯ʱ����Ҫ����
				scan_root_file(pFile);//�����ļ�/�ļ����б��ļ����Ե�
				
			}
			else if(isBackPrev == 1)//��ǰĿ¼Ϊ��Ŀ¼�����ز���Ϊ��һ���ܽ���
			{
				pFile->readItemsOffSet = 0;//�л�������߷�����һ��Ŀ¼�����ļ�/�ļ���ƫ������
				rtv = 0;
				pFile->fileDepth = 0;
			}
			
			curPageCnt = 1;
			pFile->fileGuiAttr->pageCnt = curPageCnt;//��λҳ�������
			PRINTUI_DEBUG("->back\r\n");
				break;
				
			case IS_PRINT_FIRST_ICON://��һͼ��
			if(actualItem < 1) return rtv;
			pFile->selectedButtonID = IS_PRINT_FIRST_ICON;
			if(fileAttrubeTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode �ļ�
			{
				pFile->destFileType = IS_GCODE_FILE;
				
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);//��һ��Ŀ¼����

			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);
		 		
		PRINTUI_DEBUG("->1 page \r\n");			
				break;
				
			case IS_PRINT_SECOND_ICON://�ڶ�ͼ��
			if(actualItem < 2) return rtv;
			pFile->selectedButtonID = IS_PRINT_SECOND_ICON;
			if(fileAttrubeTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode �ļ�
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);//��һ��Ŀ¼����
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->2 page \r\n");			
				break;
				
			case IS_PRINT_THIRD_ICON://����ͼ��
			if(actualItem < 3) return rtv;
			pFile->selectedButtonID = IS_PRINT_THIRD_ICON;
			if(fileAttrubeTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode �ļ�
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);//��һ��Ŀ¼����
			
			}
			
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);			
			PRINTUI_DEBUG("->3 page \r\n");			
				break;
				
			case IS_PRINT_FORTH_ICON://����ͼ��
			if(actualItem < 4) return rtv;
			pFile->selectedButtonID = IS_PRINT_FORTH_ICON;
			//�ж��ļ�����
			if(fileAttrubeTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode �ļ�
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);//��һ��Ŀ¼����
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->4 page \r\n");			
				break;
				
			case IS_PRINT_FIFTH_ICON://����ͼ��
			if(actualItem < 5) return rtv;//�жϱ�����Ѱ��ǰĿ¼���ļ�/�ļ��и����Ƿ�С��5��
			pFile->selectedButtonID = IS_PRINT_FIFTH_ICON;
			//�ж��ļ�����
			if(fileAttrubeTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode �ļ�
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG(" selected fn:%s\r\n",fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);//��һ��Ŀ¼����
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->5 page \r\n");			
				break;				
				
			default:
				break;
		}
		if(isRootChanged == 0)//�ж��Ƿ�ɹ�������һ��Ŀ¼
		{
			PRINTUI_DEBUG("next root OK\r\n");
			rtv = 1;
			curPageCnt = 1;//��λҳ�������
			pFile->fileGuiAttr->pageCnt = curPageCnt;//����ҳ�������
			pFile->readItemsOffSet = 0;//���ļ�ƫ������
			pFile->fileDepth++;//�ļ�Ƕ��������ӣ��ڸ�Ŀ¼������
			redraw_fileSystemUI_flag = IS_TRUE;//�����»滭�ļ�ϵͳ����
		   get_current_root_page_items(pFile);//��õ�ǰĿ¼���ܵ��ļ�/�ļ�����Ŀ,����ǰĿ¼�仯ʱ����Ҫ����
			scan_root_file(pFile);//�����ļ�/�ļ����б��ļ����Ե�
		}

	if(PRINT_SAVED_BUTTON != NONE_PRINT_ICON)
	{
		
		gui_fill_rectangle_DMA(printUIdev->icon[PRINT_SAVED_BUTTON].x, printUIdev->icon[PRINT_SAVED_BUTTON].y, printUIdev->icon[PRINT_SAVED_BUTTON].width, printUIdev->icon[PRINT_SAVED_BUTTON].height,BLACK);
		display_bmp_bin_to_LCD((u8*)printUIdev->icon[PRINT_SAVED_BUTTON].path, printUIdev->icon[PRINT_SAVED_BUTTON].x , printUIdev->icon[PRINT_SAVED_BUTTON].y, printUIdev->icon[PRINT_SAVED_BUTTON].width, printUIdev->icon[PRINT_SAVED_BUTTON].height, printUIdev->icon[PRINT_SAVED_BUTTON].color);
		PRINT_SAVED_BUTTON = NONE_PRINT_ICON;		
	}
	return rtv;
}


static void display_processbar(uint32_t length, uint32_t pos)
{
    static uint32_t bar_update_time = 0;

    if( bar_update_time <= millis())
    {
        bar_update_time = millis() + 1000;

        prgb->totallen = length;
        prgb->curpos = pos;
        if( pos >= length)//�ļ�ȡ��󣬱��ֹͣ
        {
            card.FILE_STATE = PS_PRINT_OVER_NOR;

        }
        progressbar_setpos(prgb);
    }


}

void printui_init(void)
{
	if(hasGotoConfirmPrintFileScreen == 0)//δ����gcodeȷ�Ͻ��棬���ȡ����ȷ���ܽ�����һ��Ŀ¼����
	{
		init_file_page_ctrl_block((u8 *)PRINT_FILE_ROOT);
		get_current_root_page_items(curFilePageCtrlBlock);//��õ�ǰĿ¼���ܵ��ļ�/�ļ�����Ŀ,����ǰĿ¼�仯ʱ����Ҫ����
		scan_root_file(curFilePageCtrlBlock);//�����ļ�/�ļ����б��ļ����Ե�
	}
	
}

/*
*********************************************************************************************************
*	�� �� ��: print_screen
*	����˵��: ��ӡ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void print_screen(void)
{
	u8 res;

	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		printui_init();
		card.FILE_STATE = PS_NO;
	}

	
	res = exacute_button_even(curFilePageCtrlBlock);//������ʾ����

	if(res == 0)//�˳��ļ�ѡ�����
	{
		free_flag = IS_TRUE;
		hasGotoConfirmPrintFileScreen = 0;
		currentMenu = main_screen;
	}
	else if(res == 2)//ѡ��gcode�ļ�
	{
		redraw_screen_flag = IS_TRUE;
		currentMenu = print_confirm_screen;
		PRINTUI_DEBUG("goto confirm gcode file \r\n");
	}

	if(SD_CD)	
		{
			free_flag = IS_TRUE;
			sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;
			currentMenu = sd_card_absence_in_select_file_remind_screen;
			PRINTUI_DEBUG("sd unplug screen \r\n");
		}
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		
		free_file_page_ctrl_block(curFilePageCtrlBlock);//�ͷ��ļ����ƿ��ڴ�
		PRINTUI_DEBUG("free printUIdev \r\n");
	}
	

}

//��gcode�ļ�ѡ������У�SD���γ�����ʾ��Ϣ
void sd_card_absence_in_select_file_remind_screen(void)
{
	u8 SELECTED_BUTTON = 0xff;
	u8 isOver;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	uint8_t font = 16;  
	uint8_t j;
	uint16_t xStart = 2;
	uint16_t yStart = 5;
	if(redraw_screen_flag == IS_TRUE)
	{
		uint16_t bmp_between_offx;
		uint16_t bmp_between_offy;
	
		uint8_t num = SD_UNPLUG_UI_ICON_NUM;
		 BACK_COLOR = BLACK;
		POINT_COLOR = BLUE;
		redraw_screen_flag = IS_FALSE;
		LCD_Clear_DMA(BLACK);

	Show_Str(xStart,yStart,lcddev.width,font,(u8 *)sdCardUnplug_title[LANGUAGE],font,1);	
	POINT_COLOR = RED;
	 font = 24;
	 Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor1[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
	 Show_Str(xStart,yStart +2* font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor2[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
	 Show_Str(xStart,yStart + 3*font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor3[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
	 font = 16;
		//Ϊ����������������ڴ�
		sdCardDev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( sdCardDev == 0)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		sdCardDev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*num);
		
		if( sdCardDev->icon == 0)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
		
		bmp_between_offx = PICTURE_X_SIZE + 2;
		bmp_between_offy = PICTURE_Y_SIZE + 2;
		sdCardDev->status = 0x3F ;
		
		

		for(j = 0; j < num ; j++)
		{
			
			
			sdCardDev->icon[j].x = bmp_sart_offx + j*3*bmp_between_offx  ;
			sdCardDev->icon[j].y = bmp_sart_offy + bmp_between_offy ;


			sdCardDev->icon[j].width = PICTURE_X_SIZE;
			sdCardDev->icon[j].height = PICTURE_Y_SIZE;
			sdCardDev->icon[j].path = (u8*)sdCardUnplugIcon_path[LANGUAGE][j];

		}

			

		sdCardDev->totalIcon = num;

		//δ�����ϵ�����gcode�ļ���δѡ��
		if( (sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_SELECT_FILE && eeprom_is_power_off_happen() != PW_OFF_HAPPEN) || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_CONFIRM_FILE) 
		{
			bsp_DelayMS(2000);

				currentMenu = main_screen;
				sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//��λ��־
				redraw_screen_flag = IS_TRUE;
				myfree(SRAMIN,sdCardDev->icon);
				myfree(SRAMIN,sdCardDev);
				POINT_COLOR = BLUE;
				hasGotoConfirmPrintFileScreen = 0;
				free_file_page_ctrl_block(curFilePageCtrlBlock);//�ͷ��ļ����ƿ��ڴ�
				PRINTUI_DEBUG("free  curFilePageCtrlBlock\r\n");
				PRINTUI_DEBUG("free sdCardDev \r\n");	
				return;
		
		}
		
	
		j = SD_UNPLUG_UI_STOP_BT_ID;
		display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);			
			
		sdCardIconDispFlag = 0;
		
	}


	//��ʾͼ��
	if(SD_CD == 1 || (SD_CD == 0 && isPrintFileExist == FALSE))//SD��δ���� ���� SD���Ѳ��뵫�Ǵ�ӡ�ļ�ȱʧ
	{
		sdCardIconDispFlag++;
		if(sdCardIconDispFlag  == 1)
		{
			POINT_COLOR = RED;
			 font = 24;
			 gui_fill_rectangle_DMA(xStart,yStart + font,lcddev.width,3*font,BLACK);//����
			 Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor1[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
			 Show_Str(xStart,yStart + 2 *font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor2[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
			Show_Str(xStart,yStart + 3* font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor3[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
			
			j = SD_UNPLUG_UI_BACK_BT_ID;
			gui_fill_rectangle_DMA(sdCardDev->icon[j].x, sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height,BLACK);
			
			j = SD_UNPLUG_UI_STOP_BT_ID;
			display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);		
			
		}
		sdCardIconDispFlag = 2;
	}
	else if(SD_CD == 0 && isPrintFileExist == TRUE)//��ʾ������Ϣ
	{
		sdCardIconDispFlag++;
		if(sdCardIconDispFlag  == 3)
		{
			POINT_COLOR = BLUE;
			 font = 24;
			 gui_fill_rectangle_DMA(xStart,yStart + font,lcddev.width,3*font,BLACK);//����
			Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardplug_remind_infor1[LANGUAGE],font,1);//��ʾ��ʾ��Ϣ
			j = SD_UNPLUG_UI_STOP_BT_ID;
			gui_fill_rectangle_DMA(sdCardDev->icon[j].x, sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height,BLACK);
			j = SD_UNPLUG_UI_BACK_BT_ID;
			display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);		
		}
		sdCardIconDispFlag = 0;
	}		
//////////////////////////////////////////////////////////////////////////////////

	SELECTED_BUTTON = gui_touch_chk(sdCardDev);	

//ִ�ж�Ӧ����	
//	isOver = is_print_over();
//	if(isOver == 0)
//	{
//		bsp_DelayMS(2000);
//		free_flag = IS_TRUE;
//		currentMenu = main_screen;
//		PRINTUI_DEBUG("print is over,back mainUI\r\n");	
//	}
	 if(SD_CD == 1 || (SD_CD == 0 && isPrintFileExist == FALSE))//SD ���γ�,ȥ������ͼ�꣬��ʾֹͣͼ��
	{
		
		if(sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_FILE || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_OPTION || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_MORE)
		{
			if(SELECTED_BUTTON == SD_UNPLUG_UI_STOP_BT_ID)//ֹͣ��ǰ��ӡ	
			{		
				currentMenu = stop_screen;
				free_flag = IS_TRUE;
				
				PRINTUI_DEBUG("stop\r\n");	
			}			
		}
		else//��δѡ��gcode�ļ�,���ص�������
		{
			bsp_DelayMS(3000);
			free_flag = IS_TRUE;
			currentMenu = main_screen;
			PRINTUI_DEBUG("delay back main\r\n");	
		}
	
	}
	else if(SD_CD == 0 && isPrintFileExist == TRUE) //SD������,ȥ��ֹͣͼ�꣬��ʾ����ͼ��
	{
		
		if(SELECTED_BUTTON == SD_UNPLUG_UI_BACK_BT_ID)	//���ش�ӡ����
		{
						
			if( sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_SELECT_FILE && eeprom_is_power_off_happen() == PW_OFF_HAPPEN  ) { currentMenu = start_print_screen;}
			
			else if( sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_FILE || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_OPTION || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_MORE)  currentMenu = gcode_print_option_screen;
			
			else //��ѡ���ļ�����/ȷ��gcodeѡ�����
			{
				currentMenu = main_screen;
			
				
			}
			if(card.FILE_STATE == PS_PRINT_OVER_NOR) currentMenu = main_screen;
			
			
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("back\r\n");		
		}

	}
////////////////////////////////////////////////////////

		
		
	
	if(free_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,sdCardDev->icon);
		myfree(SRAMIN,sdCardDev);
		POINT_COLOR = BLUE;
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//��λ��־
		hasGotoConfirmPrintFileScreen = 0;
	
		PRINTUI_DEBUG("free sdCardDev \r\n");
	}
	
}






//��ӡ�ļ�ȷ��
 void print_confirm_screen(void)
{
    uint8_t num = 2;//������
    uint8_t i;
    uint8_t selx;

    uint8_t free_flag = 0;
	uint8_t font = 16;
	uint16_t xStart = 2;
	uint16_t yStart = 5;
	uint8_t fileDepth;
	uint8_t *pathName = NULL;
	uint8_t *fileName = NULL;
	_file_page_ctrl_block_s *pFile = curFilePageCtrlBlock;
	uint8_t fileType;
	uint8_t offSet;
	uint8_t length;
	uint8_t setLength;
	uint8_t cnt = 0;
	if(pFile ==NULL ) 
	{
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error!\r\n",__FILE__,__LINE__);
		return;
	}
    if( redraw_screen_flag == IS_TRUE )
    {
    redraw_screen_flag = IS_FALSE;
	BACK_COLOR = BLACK;
    POINT_COLOR = WHITE;
	fileDepth = pFile->fileDepth;
	pathName = get_current_root_file_name(pFile->path);//��ñ�Ŀ¼���֣���ʾ�������ͷ��ڴ�

	fileType = pFile->destFileType;

	LCD_Clear_DMA(BLACK);//����
	//��ʾ������
		offSet = 2;
		
		length = strlen((char *)print_confirm_title[LANGUAGE]);
		Show_Str(xStart,yStart,lcddev.width,font,(u8 *)print_confirm_title[LANGUAGE],font,1);//��ʾ��������Ϣ
		xStart += offSet;
		LCD_ShowString(xStart + length*(font/2),yStart ,lcddev.width,font,font,(u8 *)PRINT_FILE_ROOT);//��ʾ"0:"
		
		if(fileDepth == 1)//��һ��Ƕ��
		{

			LCD_ShowString(xStart + length*(font/2) + 2*(font/2),yStart ,lcddev.width,font,font,(u8 *)"/");	
			if(pathName != NULL)
			Show_Str(xStart + length*(font/2)  + 2*(font/2) + font/2 ,yStart,lcddev.width,font,(u8 *)pathName,font,1);
		}
		if(fileDepth > 1 )//ȷ�����ļ�����·����ȴ���1
		{

			LCD_ShowString(xStart + length*(font/2) + 2*(font/2) ,yStart,lcddev.width,font,font,(u8 *)"/.../");	
			if(pathName != NULL)
			Show_Str(xStart + length*(font/2) + 2*(font/2) + 5*(font/2),yStart,lcddev.width,font,(u8 *)pathName,font,1);
		}
		free_current_root_file_name_memory(pathName);
	
		xStart = 2;
	yStart += 2*font;
	gui_show_strmid(xStart,yStart,lcddev.width,font,POINT_COLOR,font,(u8 *)print_confirm_infor[LANGUAGE],1);//��ʾѡ����ʾ��Ϣ
	
	//��ʾ��ӡ�ļ���	
	offSet = 10;
	xStart = 40;
	yStart += 2*font;
	setLength = 30;//������2������������ʾ����Ҫ2���ֽ�
		if(fileType == IS_GCODE_FILE)
		{	
			length = strlen((char *)pFile->destFileName);
			if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
			{
				fileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
				if(fileName == NULL)
				{	
					PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
					
				}
				else
				{
					
					strncpy((char *)fileName,(const char *)(pFile->destFileName),setLength);
					fileName[setLength] = '\0';
					
					Show_Str(xStart,yStart, lcddev.width, font,  (u8*)fileName, font, 0); 
					gui_memin_free(fileName);
					
					fileName = pFile->destFileName + setLength;
					Show_Str(xStart + offSet,yStart + font, lcddev.width, font,  (u8*)fileName, font, 0); 
				}						
			}
			else 
			Show_Str(xStart,yStart, lcddev.width, font,  (u8*)pFile->destFileName, font, 0); //��ʾgcode�ļ�
		}
		
		
		
        screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
        if(screen_key_group)//���������ɹ�
        {

            uint16_t botton_width = 80;
            uint16_t botton_height = 40;

			xStart = 40;
			yStart = 100 + font;
			
            for( i = 0; i < num; i++)//����2������
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
            PRINTUI_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
            return;
        }
		

    }

//�������
    selx = screen_key_chk(screen_key_group, &in_obj, num);
    if(selx & BUTTON_ACTION)
    {
        switch(selx & BUTTON_SELECTION_MASK)
        {
        case YES://ȷ����ӡ�󣬻�ȡ��ӡ�ļ������ͷ�pFile�ṹ���ڴ�
           
            free_flag = 1;
			printFileName = pickup_print_file_name(pFile->path,pFile->destFileName);//�ϳɴ�ӡ�ļ���
			PRINTUI_DEBUG("file:%s\r\n",printFileName);
			
			
			selx = 1;
			while(selx)
			{
				selx = f_open(&card.fgcode, (const TCHAR*)printFileName, FA_READ);//��gcode�ļ�
				cnt++;
				if (selx != FR_OK && cnt > 3) 
				{
					
					PRINTUI_DEBUG("file:%s,line:%d -> read gcode error\r\n",__FILE__,__LINE__);
					 break;
				}


			}
			if(selx == FR_OK)
			{
				 currentMenu = start_print_screen;
				hasGotoConfirmPrintFileScreen = 0;
				free_file_page_ctrl_block(pFile);//���ļ�ҳ����ƿ��ڴ�
				isPrintFileExist = TRUE;
				file_pos = 0;//�ļ�ָ������
				fil_size = f_size(&card.fgcode);//��¼�ļ���С
				card.FILE_STATE = PS_PIRNT;//���ڴ�ӡ
				powerDownPrintTime = 0;
				starttime = millis();//��ʼ��ʱ
				card_startFileprint();
				feedmultiply=100;//����ΪĬ���ٶ�
			}
			else//�ļ���ʧ�ܣ�������һ��Ŀ¼
			{
				
				free_printFileName(printFileName);//���ļ����ڴ�
				isPrintFileExist = FALSE;
				hasGotoConfirmPrintFileScreen = 1;
				 currentMenu = print_screen;
				 redraw_fileSystemUI_flag = IS_TRUE;
			}


			
            PRINTUI_DEBUG("YES\r\n");
            break;
        case NO://ȡ��--����ѡ���ļ����棬��Ŀ¼Ƕ��Ϊ����ý���Ŀ¼
			hasGotoConfirmPrintFileScreen = 1;
			redraw_fileSystemUI_flag = IS_TRUE;
			 currentMenu = print_screen;
			
            free_flag = 1;
            PRINTUI_DEBUG("NO\r\n");
            break;
        default:
            break;
        }
    }
	
	if(SD_CD)	
		{
			free_flag = 1;
			sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_CONFIRM_FILE;
			currentMenu = sd_card_absence_in_select_file_remind_screen;

			PRINTUI_DEBUG("sd unplug screen \r\n");
		}
	
    if(free_flag == 1) //�ͷ��ڴ�
    {
        redraw_screen_flag = IS_TRUE;
        for(i = 0; i < num; i++)
        {
            btn_delete(screen_key_group[i]);
        }
        gui_memin_free(screen_key_group);

    }
	


	
}

/**********************************��ʼ��ӡgcode����**************************************/
void start_print_screen(void)
{
	u8  SELECTED_BUTTON = 0XFF;
	u8 printOver = 1;

	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		start_print_screen_init();
		bsp_ClearKey();//����IO��⻺������
		
	}
	if(card.FILE_STATE != PS_PRINT_OVER_NOR)//δ������ӡ��ɣ������Ƿ�������ӡ��ɣ������ظ�����option/backͼ��
	{
		printOver = is_print_over();//�ж��Ƿ�������ӡ����
	}
	
	if(printOver == 0 || optionBackBtSet == 1)
	{
	//���ݴ�ӡ���״̬���������һ��ͼ��Ϊ���ؿؼ�
		gui_fill_rectangle_DMA(startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].x, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].y, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].width, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].height,BLACK);
		display_bmp_bin_to_LCD((u8*)startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].path, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].x , startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].y, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].width, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].height, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].color);
		pause_start_cnt = 0;
		isOption2moreUiSelected = NO;
		optionBackBtSet = 0;
//		free_printFileName(printFileName);//������ӡ���������ӡ�ļ����ڴ�
	}

	SELECTED_BUTTON = gui_touch_chk(startPintdev);
	
	if(SELECTED_BUTTON == GCODE_PRINT_OPTION_BUTTON)	//���һ�����һ��ͼ�걻����
	{
		if(card.FILE_STATE == PS_PRINT_OVER_NOR )//���ؿؼ�,������ӡ����
		{
			free_flag = IS_TRUE;
			currentMenu = main_screen;
			
			free_printFileName(printFileName);
			
			PRINTUI_DEBUG("back selected \r\n");
	
		}
		else//option �ؼ�
		{	
			free_flag = IS_TRUE;
			
			currentMenu = gcode_print_option_screen;
			PRINTUI_DEBUG("option selected \r\n");		
		}
	}
	
	
	update_printUI_parameter(&printParam);
	update_print_time(&printTime);
	
	display_processbar(fil_size, file_pos);

	filaState.filaIOState = check_fila_run_out();
	if(filaState.filaIOState == FILA_STATE_OUT)
	{
		free_flag = IS_TRUE;
		currentMenu =fila_run_out_detet_screen;
		PRINTUI_DEBUG("goto fila run out screen \r\n");
	}

	
	if(SD_CD)	
	{
		free_flag = IS_TRUE;
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_PRINT_FILE;
		currentMenu = sd_card_absence_in_select_file_remind_screen;
		
		PRINTUI_DEBUG("sd unplug screen \r\n");
	}
	
	if(free_flag == IS_TRUE)
	{
		
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,startPintdev->icon);
		myfree(SRAMIN,startPintdev);
		progressbar_delete(prgb);
		PRINTUI_DEBUG("free startPintdev \r\n");
	}
}	

//�ж��Ƿ��ӡ���
//����ֵ0--��ӡ���
u8 is_print_over(void)
{
	u8 rtv = 0xff;
	if( file_pos >= fil_size )//gcode�ļ���ȡ��ϣ���������滹δִ�����
	{
		
		card.FILE_STATE = PS_PRINT_OVER_NOR;
		rtv = 0;
		feedmultiply=100;//����ΪĬ���ٶ�
	}
	return rtv;
}
void test_prag_bar(void)
{
	 static uint32_t print_update_time = 0;
	 if( print_update_time <= millis())
	 {
		print_update_time = millis() + 1000;
	        //��ʾ��ӡ������
        if(prgb)
        {
 
            prgb->curpos++;
			if(prgb->curpos >=  prgb->totallen) prgb->curpos = 0;
             progressbar_setpos(prgb);//����ť
        }
	
		
	}
	 
}
void start_print_screen_init(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint8_t *dispFileName = NULL;
	uint8_t *fileName = NULL;


	uint16_t xStart;
	uint16_t yStart;
	uint16_t offSet;
	uint16_t width;
	uint16_t hight;
	uint16_t setLength;
	uint16_t length;
	uint16_t titleLength;
	
	LCD_Clear_DMA(BLACK);
	xStart = 2;
	yStart = 5;
	offSet = 16;
	setLength = 30;//������2������������ʾ����Ҫ2���ֽ�
	width = lcddev.width;
	hight = font;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	
	titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//��ñ�������ʾ��Ϣ����
	Show_Str(xStart,yStart,width,hight,(u8 *)gcode_print_title[LANGUAGE],font,1);	
	fileName = get_current_root_file_name(printFileName);//���gcode�ļ���


			length = strlen((char *)fileName);
			if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
			{
				dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
				if(dispFileName == NULL)
				{	
					PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
					
				}
				else
				{
					
					strncpy((char *)dispFileName,(const char *)fileName,setLength);
					dispFileName[setLength] = '\0';
					
					Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)dispFileName, font, 1); 
					gui_memin_free(dispFileName);//�ͷ��ڴ�
					
					xStart = 2;
					dispFileName = fileName + setLength;
					Show_Str(xStart,yStart + font, width,hight,(u8*)dispFileName, font, 1); 
				}						
			}
			else 
			Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)fileName, font, 1); //��ʾgcode�ļ�

	free_current_root_file_name_memory(fileName);//�ͷ��ڴ�
	
	
	//Ϊ����������������ڴ�
	startPintdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( startPintdev == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	startPintdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*GCODE_PRINT_ICON_NUM);
	
	if( startPintdev->icon == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	
	
	xStart = 10;
	yStart = yStart + 2*font + offSet;
	width = 300;
	hight = 40;
	prgb = progressbar_creat(xStart, yStart, width, hight, 0x61); //����������
	if(prgb == NULL) 
	{
	
		PRINTUI_DEBUG("file:%s line:%d  memory error!", __FILE__, __LINE__);   //����ʧ��.
		return;
	}
				//��ʼ��ʾʱ������
	printTime.x = xStart;
	printTime.y = yStart + hight + offSet;
	printTime.hour = 0;
	printTime.minute = 0;
	printTime.second = 0;
	print_time_init(&printTime);

	
	offSet = 2;
	bmp_between_offx = PICTURE_X_SIZE + offSet;
	bmp_between_offy = PICTURE_Y_SIZE + offSet;
	startPintdev->status = 0x3F ;
	xStart = bmp_sart_offx;
	yStart = bmp_sart_offy + bmp_between_offy ;
	printParam.x = 2 + font;
	printParam.y = 208;
	printParam.bedCurTemp = degBed();
	printParam.bedDestTemp = degTargetBed();
	printParam.etr0CurTemp = degHotend0();
	printParam.etr0DestTemp = degTargetHotend0();
	printParam.fanSpeed = fanSpeed;

		for(j=0;j<GCODE_PRINT_ICON_NUM;j++)
		{
			if(j== GCODE_PRINT_ICON_NUM - 1)
			{
			
			startPintdev->icon[j].x = xStart + (j-1)*bmp_between_offx  ;
			startPintdev->icon[j].y = yStart ;
			startPintdev->icon[j].width = PICTURE_X_SIZE;
			startPintdev->icon[j].height = PICTURE_Y_SIZE;

			}
			else 
			{
			startPintdev->icon[j].x = xStart + j*bmp_between_offx  ;
			startPintdev->icon[j].y = yStart ;
			startPintdev->icon[j].width = PICTURE_X_SIZE;
			startPintdev->icon[j].height = PICTURE_Y_SIZE;
			
			}
			
			
			startPintdev->icon[j].path = (u8*)gcodePrintui_icon_path_tbl[LANGUAGE][j];

		}
	
	
	startPintdev->totalIcon = GCODE_PRINT_ICON_NUM;
	
		for(j=0; j < startPintdev->totalIcon - 1; j++)//��4��ͼ��
		{
			display_bmp_bin_to_LCD((u8*)startPintdev->icon[j].path, startPintdev->icon[j].x , startPintdev->icon[j].y, startPintdev->icon[j].width, startPintdev->icon[j].height, startPintdev->icon[j].color);
			
		}	
		
	set_printUI_paramter(&printParam);
}

void update_printUI_parameter(_parameter_state_s *parState)
{
	get_printUI_parameter(parState);
	set_printUI_paramter(parState);
	
}
void  get_printUI_parameter(_parameter_state_s *parState)
{
		
		parState->bedCurTemp = current_temperature_bed;
		parState->bedDestTemp = degTargetBed();
		
		parState->etr0CurTemp = current_temperature[0];
		parState->etr0DestTemp = degTargetHotend0();
		
		parState->fanSpeed = fanSpeed;

}
//��ʼ��ӡ������ʾ
void  set_printUI_paramter(_parameter_state_s *parState)
{
	uint16_t xStart;
	uint16_t yStart;
	uint16_t offSet;
	uint8_t fanSpeed;
	uint16_t bedCurTemp;
	uint16_t bedDestTemp;
	uint16_t etr0CurTemp;
	uint16_t etr0DestTemp;
	uint16_t width,hight;
	uint16_t f_size = 16;
	uint32_t static printParaStateTime = 0;
	
	width = lcddev.width;
	 hight = f_size;
	 
	xStart = parState->x;
	yStart = parState->y;
	bedCurTemp = parState->bedCurTemp;
	bedDestTemp = parState->bedDestTemp;
	
	etr0CurTemp = parState->etr0CurTemp;
	etr0DestTemp = parState->etr0DestTemp;
	
	fanSpeed = parState->fanSpeed;
	BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;

	
		 //����
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		gui_fill_rectangle_DMA(xStart, yStart,width -PICTURE_X_SIZE - 4 - f_size,hight,BACK_COLOR);
	//��ʾ�ȴ��¶�
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
    LCD_ShowString(xStart + 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num(xStart + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, bedDestTemp, 1); 
	
	offSet = 1 + PICTURE_X_SIZE ;
	//��ʾ����ת��
	gui_show_num(xStart + offSet + 2 * (f_size / 2), yStart, 3, POINT_COLOR, f_size, fanSpeed, 1); 
//	LCD_ShowString(xStart + offSet+ 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"%");
	
	offSet += 1 + PICTURE_X_SIZE;
	//��ʾ����ͷ�¶�
	gui_show_num(xStart + offSet, yStart, 3, POINT_COLOR, f_size, etr0CurTemp, 1); 
    LCD_ShowString(xStart + offSet+ 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num(xStart + offSet + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, etr0DestTemp, 1); 
		
	 }


}
void  display_heat_paramter(_parameter_state_s *parState)
{
	uint16_t xStart;
	uint16_t yStart;
	uint16_t offSet;
	uint16_t bedCurTemp;
	uint16_t bedDestTemp;
	uint16_t etr0CurTemp;
	uint16_t etr0DestTemp;
	uint16_t width,hight;
	uint16_t f_size = 24;
	uint32_t static printParaStateTime = 0;
	
	width = lcddev.width;
	 hight = f_size;
	 
	xStart = parState->x;
	yStart = parState->y;
	bedCurTemp = parState->bedCurTemp;
	bedDestTemp = parState->bedDestTemp;
	
	etr0CurTemp = parState->etr0CurTemp;
	etr0DestTemp = parState->etr0DestTemp;
	
	fanSpeed = parState->fanSpeed;
	BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;

	
		 //����
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		gui_fill_rectangle_DMA(xStart, yStart,8*(f_size/2),hight,BACK_COLOR);
		gui_fill_rectangle_DMA(xStart + 160 , yStart,8*(f_size/2),hight,BACK_COLOR);
	//��ʾ�ȴ��¶�
	POINT_COLOR = GREEN;
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
	POINT_COLOR = WHITE;
    LCD_ShowString(xStart + 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num(xStart + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, bedDestTemp, 1); 
	POINT_COLOR = YELLOW;


	offSet =  160 + 43 - 3;
	//��ʾ����ͷ�¶�
	POINT_COLOR = GREEN;
	gui_show_num( offSet, yStart, 3, POINT_COLOR, f_size, etr0CurTemp, 1); 
	POINT_COLOR = WHITE;
    LCD_ShowString(offSet+ 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num( offSet + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, etr0DestTemp, 1); 
	POINT_COLOR = YELLOW;	
	 }
	POINT_COLOR = WHITE;

}

void display_current_print_parameter(_parameter_state_s *parState)
{
	get_printUI_parameter(parState);
	set_current_print_paramter(parState);
}
//option������ʾ������Ϣ
void  set_current_print_paramter(_parameter_state_s *parState)
{
	uint16_t xStart;
	uint16_t yStart;
	uint16_t offSet;
	uint8_t fanSpeed;
	uint8_t length1;
	uint8_t length2;
	uint8_t length3;
	uint16_t bedCurTemp;

	uint16_t etr0CurTemp;

	uint16_t width,hight;
	uint16_t f_size = 16;
	uint32_t static printParaStateTime = 0;
	
	width = lcddev.width;
	 hight = f_size;
	 
	xStart = parState->x;
	yStart = parState->y;
	bedCurTemp = parState->bedCurTemp;

	
	etr0CurTemp = parState->etr0CurTemp;

	
	fanSpeed = parState->fanSpeed;
	BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;

	
		 //����
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		length1 = strlen((char *)bedInfor[LANGUAGE]);
		length2 = strlen((char *)extrudInfor[LANGUAGE]) ;
		
		offSet = (length1 )* (f_size / 2) ;
		width = 3*(f_size / 2);
		yStart += 10;
		gui_fill_rectangle_DMA(xStart + offSet, yStart,width,hight ,BACK_COLOR);//���ȴ���ʾ
		
		offSet = (length1 + 5 + length2)* (f_size / 2) ;
		gui_fill_rectangle_DMA(xStart + offSet, yStart,width,hight,BACK_COLOR);//�強����ʾ
		
	//��ʾ�ȴ��¶�
	width = (length1 )* (f_size / 2);
	
	Show_Str(xStart , yStart , width, hight, (u8*)bedInfor[LANGUAGE],f_size,1); 
	xStart += width;
	gui_show_num(xStart , yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
   
	
	offSet = 5* (f_size / 2) ;
	
	//��ʾ����ͷ�¶�
	width = (length2 )* (f_size / 2);
	xStart += offSet;
	Show_Str(xStart , yStart , width, hight, (u8*)extrudInfor[LANGUAGE],f_size, 1);
	xStart += width;
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, etr0CurTemp, 1); 
	
	

	//��ʾ����ת��
	offSet = f_size ;
	length3 = strlen((char *)fanInfor[LANGUAGE]);
	offSet = (length3  )* (f_size / 2) ;
	width = 3*(f_size / 2);
	xStart = parState->x ;
	yStart += 2*f_size;
	gui_fill_rectangle_DMA(xStart + offSet, yStart,width,hight,BACK_COLOR);

	width = (length3 )* (f_size / 2);
	Show_Str(xStart , yStart , width, hight,  (u8*)fanInfor[LANGUAGE],f_size,1); 
	
	xStart += width;
	gui_show_num( xStart, yStart, 3, POINT_COLOR, f_size, fanSpeed, 1); 
  
		
	 }

}
/*************************************option �ؼ�����*******************************************/
void gcode_print_option_screen(void)
{
	u8  SELECTED_BUTTON = 0XFF;
	u8 saveButton = 0xff;
	u8 isPrintOver = 0xff;

	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		gcode_print_option_screen_init();
	}

	SELECTED_BUTTON = gui_touch_chk(optiondev);
	
	
	switch ( SELECTED_BUTTON )
	{

		case GCODE_OPTION_STOP_BUTTON_ID://ֹͣ��ӡ
			saveButton = GCODE_OPTION_STOP_BUTTON_ID;
			currentMenu = stop_screen;
			free_flag = IS_TRUE;

				PRINTUI_DEBUG("stop \r\n");
			break;
			
		case GCODE_OPTION_PAUSE_BUTTON_ID:


			//ת��ͼ��->resume
			pause_start_cnt++;
			if(pause_start_cnt % 2 == 1)//���ƴ�ӡ����ͣ�����ط��ذ���
			{
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].height,BLACK);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_START_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].color);
				IS_PAUSE_BT = NO;
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].height,BLACK);

				if(card.FILE_STATE == PS_PIRNT)//ֻ�д��ڴ�ӡ�����е���ͣ��������
					{
					card.FILE_STATE = PS_PAUSE;//��ͣ
					
					card_pauseSDPrint();
					}
				
			}
			else if(pause_start_cnt % 2 == 0)//�ָ���ӡ,��ʾ���ذ���
			{
				pause_start_cnt = 0;
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height,BLACK);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].color);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].color);

				IS_PAUSE_BT = YES;
				if(card.FILE_STATE == PS_PAUSE)//ֻ�д�����ͣ״̬�£��ָ���ӡ��������
				{
					card.FILE_STATE = PS_PIRNT;//��ӡ
				}
				

				
			}
			PRINTUI_DEBUG("pause/start \r\n");	
			break;		
		case GCODE_OPTION_MORE_BUTTON_ID:
		
			free_flag = IS_TRUE;
			currentMenu = gcode_print_option_to_more_screen;
			isOption2moreUiSelected = YES;//��־����more����Ԥ������,��ӡ��ɸ�λ
			PRINTUI_DEBUG("more \r\n");	
			break;		
		case GCODE_OPTION_BACK_BUTTON_ID:
		if(IS_PAUSE_BT == YES)//����ӡ�����ڴ�ӡ״̬--��ʾ��ͣͼ��ʱ���ż�ⷵ�ذ����¼�
		{
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("back \r\n");	
			currentMenu = start_print_screen;
		
		}
		break;		
			
		default:
			break;			
	}

	display_current_print_parameter(&printParam);
	update_print_time(&printTime);
	
	display_processbar(fil_size, file_pos);
	
	

	
	
	check_is_pause_exacute();
	
	isPrintOver = is_print_over();//�ж��Ƿ�������ӡ���
	if(isPrintOver == 0)
	{
		free_flag = IS_TRUE;
		PRINTUI_DEBUG("print over \r\n");	
		
		currentMenu = start_print_screen;
		optionBackBtSet = 1;//start_print_screen����ѡ��ͼ���л�Ϊ ����ͼ��
//		free_printFileName(printFileName);
	}
	
	if(saveButton != 0xff)
	{
		
		gui_fill_rectangle_DMA(optiondev->icon[saveButton].x, optiondev->icon[saveButton].y, optiondev->icon[saveButton].width, optiondev->icon[saveButton].height,BLACK);
		display_bmp_bin_to_LCD((u8*)optiondev->icon[saveButton].path, optiondev->icon[saveButton].x , optiondev->icon[saveButton].y, optiondev->icon[saveButton].width, optiondev->icon[saveButton].height, optiondev->icon[saveButton].color);
		saveButton = 0xff;		
	}

	filaState.filaIOState = check_fila_run_out();
	if(filaState.filaIOState == FILA_STATE_OUT)
	{
		free_flag = IS_TRUE;
		currentMenu =fila_run_out_detet_screen;
		PRINTUI_DEBUG("goto fila run out screen \r\n");
	}
	if(SD_CD)	
	{
		free_flag = IS_TRUE;
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_PRINT_OPTION;
		currentMenu = sd_card_absence_in_select_file_remind_screen;
		PRINTUI_DEBUG("sd unplug screen \r\n");
	}
	

	
	if(free_flag == IS_TRUE)
	{
		
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,optiondev->icon);
		myfree(SRAMIN,optiondev);
		progressbar_delete(prgb);
		PRINTUI_DEBUG("free optiondev \r\n");
	}


}
void stop_screen(void)
{
	uint8_t num = 2;//������
	uint8_t i;
	uint8_t selx;
	uint8_t font = 16;
	uint8_t length;
	uint8_t setLength;
	uint8_t *fileName = NULL;
	uint16_t  xStart,yStart;
	uint8_t free_flag = 0;
	
	xStart = 2;
	yStart = 24;
	if( redraw_screen_flag == IS_TRUE )
    {
		redraw_screen_flag = IS_FALSE;
		BACK_COLOR = BLACK;
		POINT_COLOR = BLUE;
		
		
		LCD_Clear_DMA(BLACK);//����
		Show_Str(xStart,yStart,lcddev.width,font,(u8 *)print_stop_infor[LANGUAGE],font,1);//��ʾ��������Ϣ
		
		POINT_COLOR = WHITE;
		yStart += 2*font;
		setLength = 30;//������2������������ʾ����Ҫ2���ֽ�
		length = strlen((char *)printFileName);
		if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
		{
			fileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
			if(fileName == NULL)
			{	
				PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
				
			}
			else
			{
				
				strncpy((char *)fileName,(const char *)(printFileName),setLength);
				fileName[setLength] = '\0';
				
				Show_Str(xStart,yStart, lcddev.width, font,  (u8*)fileName, font, 0); 
				gui_memin_free(fileName);
				
				fileName = printFileName + setLength;
				Show_Str(xStart + font,yStart + font, lcddev.width, font,  (u8*)fileName, font, 0); 
			}						
		}
		else 
		Show_Str(xStart,yStart, lcddev.width, font,  (u8*)printFileName, font, 0); //��ʾgcode�ļ�

			screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
			if(screen_key_group)//���������ɹ�
			{

				uint16_t botton_width = 80;
				uint16_t botton_height = 40;

				xStart = 40;
				yStart = 100 + font;
				
				for( i = 0; i < num; i++)//����2������
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
				PRINTUI_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
				return;
			}
	}

	//�������
    selx = screen_key_chk(screen_key_group, &in_obj, num);
    if(selx & BUTTON_ACTION)
    {
        switch(selx & BUTTON_SELECTION_MASK)
        {
        case YES://ֹͣ��ӡ
            currentMenu = main_screen;
            free_flag = 1;

            PRINTUI_DEBUG("YES\r\n");
			
			card.FILE_STATE = PS_STOP;
            quickStop();//ֹͣ�滮����
            card_closefile();//�ر��ļ�
//��������ջ����־
			clear_cmd_buf();

            stoptime = millis() - starttime; //��������ӡ��������¼��ʱ����ʱ��
           
            line_no = 0;//gcode �����к�����

			clear_filaTime_s();
            force_sdCard_pause();//sdCard��������ֹͣ
            pause_start_cnt = 0;
			filaState.filaPause = FALSE;//��λ��־
			isPauseStart = 0;//������ͣ/��ӡִ�б�־
			paueStartFlag = 0;
			IS_PAUSE_BT = YES;//��λgcode_print_screen ���ذ�������־
            //�¶�����
            //x��y���ԭ�㣬z�����Ƶ����λ��
            menu_action_gcode("M104 S0.0");//�����ȼ���ͷ�¶�
            menu_action_gcode("M140 S0.0");//�����ȴ��¶�		
			setTargetHotend(0.0,0);		
			setTargetBed(0.0);			
			menu_action_gcode("G1 X0");//X���ԭ��
            menu_action_gcode("M84");//����ϵ�
	
			free_printFileName(printFileName);
			sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//��λ��־
			feedmultiply=100;//����ΪĬ���ٶ�
			if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)//�ϵ�������׼�������У�ֹͣ��ӡ
			{
				eeprom_clear_power_off_status();	
			}
            break;
        case NO://ȡ��--���ش�ӡ����
			if(SD_CD && sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_OPTION)
			{
				currentMenu = sd_card_absence_in_select_file_remind_screen;			
			}
			else currentMenu = gcode_print_option_screen;
            
            free_flag = 1;
            PRINTUI_DEBUG("NO\r\n");
            break;
        default:
            break;
        }
    }
	

	
    if(free_flag == 1) //�ͷ��ڴ�
    {
        redraw_screen_flag = IS_TRUE;
        for(i = 0; i < num; i++)
        {
            btn_delete(screen_key_group[i]);
        }
        gui_memin_free(screen_key_group);

    }

}


void gcode_print_option_screen_init(void)
{
	uint16_t i,j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint8_t *dispFileName = NULL;
	uint8_t *fileName = NULL;


	uint16_t xStart;
	uint16_t yStart;
	uint16_t offSet;
	uint16_t width;
	uint16_t hight;
	uint16_t setLength;
	uint16_t length;
	uint16_t titleLength;
	
	LCD_Clear_DMA(BLACK);
	xStart = 2;
	yStart = 5;
	offSet = 16;
	setLength = 30;//������2������������ʾ����Ҫ2���ֽ�
	width = lcddev.width;
	hight = font;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	
	titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//��ñ�������ʾ��Ϣ����
	Show_Str(xStart,yStart,width,hight,(u8 *)gcode_print_title[LANGUAGE],font,1);	
	fileName = get_current_root_file_name(printFileName);//���gcode�ļ���


			length = strlen((char *)fileName);
			if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
			{
				dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
				if(dispFileName == NULL)
				{	
					PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
					
				}
				else
				{
					
					strncpy((char *)dispFileName,(const char *)fileName,setLength);
					dispFileName[setLength] = '\0';
					
					Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)dispFileName, font, 1); 
					gui_memin_free(dispFileName);//�ͷ��ڴ�
					
					xStart = 2;
					dispFileName = fileName + setLength;
					Show_Str(xStart,yStart + font, width,hight,(u8*)dispFileName, font, 1); 
				}						
			}
			else 
			Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)fileName, font, 1); //��ʾgcode�ļ�

	free_current_root_file_name_memory(fileName);//�ͷ��ڴ�
	
	

	optiondev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( optiondev == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	optiondev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*GCODE_OPTION_ICON_NUM);
	
	if( optiondev->icon == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	
	
	xStart = 10;
	yStart = yStart + 2*font + offSet;
	width = 150;
	hight = 40;
	prgb = progressbar_creat(xStart, yStart, width, hight, 0x61); //����������
	if(prgb == NULL) 
	{
	
		PRINTUI_DEBUG("file:%s line:%d  memory error!", __FILE__, __LINE__);   //����ʧ��.
		return;
	}
	
	xStart -= 4;				//��ʼ��ʾʱ������
	printTime.x = xStart + width + font ;
	printTime.y = yStart ;

	print_time_init(&printTime);
	
	printParam.x = xStart + width + font;
	printParam.y = yStart + font;
	printParam.bedCurTemp = degBed();

	printParam.etr0CurTemp = degHotend0();
	
	printParam.fanSpeed = fanSpeed;


	offSet = 2;
	bmp_between_offx = PICTURE_X_SIZE + offSet;
	bmp_between_offy = PICTURE_Y_SIZE + offSet;
	
	optiondev->status = 0x3F ;
	xStart = bmp_sart_offx;
	yStart = bmp_sart_offy + bmp_between_offy ;
		for(j=0;j<GCODE_OPTION_ICON_NUM;j++)
		{

			if(j== GCODE_OPTION_ICON_NUM - 1)
			{
			
				optiondev->icon[j].x = xStart + bmp_between_offx  ;
				optiondev->icon[j].y = yStart ;
			}
			else 
			{
				optiondev->icon[j].x = xStart + j*bmp_between_offx  ;
				optiondev->icon[j].y = yStart ;
			}
				optiondev->icon[j].width = PICTURE_X_SIZE;
				optiondev->icon[j].height = PICTURE_Y_SIZE;
				optiondev->icon[j].path = (u8*)optionui_icon_path_tbl[LANGUAGE][j];
			
		}
	
	
	optiondev->totalIcon = GCODE_OPTION_ICON_NUM;
	
		for(j=0; j < optiondev->totalIcon - 1; j++)//
		{
		
			if(j == GCODE_OPTION_PAUSE_BUTTON_ID)
			{
				if(pause_start_cnt % 2 == 1 || filaState.filaPause == TRUE)//������ͣ/��ʼ ͼ��Ϊ ��ʼͼ�꣬SD���β��ʼ�����
				{
					if(filaState.filaPause == TRUE) pause_start_cnt = 1;
					display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_START_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].color);
					
				}
				else 
					display_bmp_bin_to_LCD((u8*)optiondev->icon[j].path, optiondev->icon[j].x , optiondev->icon[j].y, optiondev->icon[j].width, optiondev->icon[j].height, optiondev->icon[j].color);		
			}
			
			else if(j == GCODE_OPTION_BACK_BUTTON_ID)
			{
				if(pause_start_cnt % 2 == 0)//��ʾ��ͣͼ��
					display_bmp_bin_to_LCD((u8*)optiondev->icon[j].path, optiondev->icon[j].x , optiondev->icon[j].y, optiondev->icon[j].width, optiondev->icon[j].height, optiondev->icon[j].color);
			}
						
			else 
			display_bmp_bin_to_LCD((u8*)optiondev->icon[j].path, optiondev->icon[j].x , optiondev->icon[j].y, optiondev->icon[j].width, optiondev->icon[j].height, optiondev->icon[j].color);	
		}	
		
	set_current_print_paramter(&printParam);
}





void gcode_print_option_to_more_screen(void)
{

	u8  SELECTED_BUTTON = 0XFF; 
	u8 isPause = TRUE;
	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		gcode_print_option_to_more_screen_int();
	}

	isPause =  StopPrint_remiderUI();
	SELECTED_BUTTON = gui_touch_chk(optionToMoredev);
	
	switch ( SELECTED_BUTTON )
	{
/**************************��ҳ�ĵ�һ��ͼ��******************************/	
		case GCODE_OPTION_TO_MORE_PREHEAT_BT_ID:
		currentMenu = preheat_screen;
		free_flag = IS_TRUE;
				PRINTUI_DEBUG("goto preheat ui\r\n");
				
			break;
			
		case GCODE_OPTION_TO_MORE_FAN_BT_ID:
		currentMenu = fan_screen;
		free_flag = IS_TRUE;
				PRINTUI_DEBUG("goto fan ui\r\n");
				
			break;			
					
		case GCODE_OPTION_TO_MORE_CHANGE_FILA_BT_ID:
		if(isPause == 1)//��ӡ����ͣ
		{
			currentMenu = change_fila_screen;//����
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("change fila ui\r\n");				
		}
				
			break;			


		case GCODE_OPTION_TO_MORE_FILA_BT_ID:
		if(isPause == 1)//��ӡ����ͣ
		{
			currentMenu = fila_screen;//��ӡ�����н�����
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("print fila ui\r\n");			
		}
		break;
			
		case GCODE_OPTION_TO_MORE_SPEED_BT_ID://��ӡ�ٶ�����
			
		currentMenu = speed_screen;	
		free_flag = IS_TRUE;
		PRINTUI_DEBUG("speed set ui\r\n");	
		break;
			
		case GCODE_OPTION_TO_MORE_BACK_BT_ID:
		currentMenu = gcode_print_option_screen;
		free_flag = IS_TRUE;
		isOption2moreUiSelected = NO;
				PRINTUI_DEBUG("back to option ui\r\n");
			break;
			
		default:
			break;				
	}
	
	filaState.filaIOState = check_fila_run_out();
	if(filaState.filaIOState == FILA_STATE_OUT)
	{
		free_flag = IS_TRUE;
		currentMenu =fila_run_out_detet_screen;
		PRINTUI_DEBUG("goto fila run out screen \r\n");
	}



	check_is_pause_exacute();//����ӡͷ�Ƿ�ִ�п���ֹͣ
	if(SD_CD)	
	{
		free_flag = IS_TRUE;
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_PRINT_MORE;
		currentMenu = sd_card_absence_in_select_file_remind_screen;		
		PRINTUI_DEBUG("sd unplug screen \r\n");
	}
	
	if(free_flag == IS_TRUE)
	{
		
		redraw_screen_flag = IS_TRUE;
		myfree(SRAMIN,optionToMoredev->icon);
		myfree(SRAMIN,optionToMoredev);
		PRINTUI_DEBUG("free optionToMoredev \r\n");
	}

	

}
void gcode_print_option_to_more_screen_int(void)
{
	uint16_t j;

	uint16_t bmp_between_offx;
	uint16_t bmp_between_offy;
	uint8_t font = 16;
	uint8_t rtv;
	uint8_t dispIconNum;
	uint16_t x,y;
	x = 2;
	y = 5;
	LCD_Clear_DMA(BLACK);
Show_Str(x,y,lcddev.width,font,(u8 *)optToMoreUI_title[LANGUAGE],font,1);	

	//Ϊ����������������ڴ�
	optionToMoredev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
	
	if( optionToMoredev == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	optionToMoredev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*GCODE_OPTION_TO_MORE_ICON_NUM);
	
	if( optionToMoredev->icon == 0)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}
	
	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	optionToMoredev->status = 0x3F ;
	
	

		for(j=0;j<GCODE_OPTION_TO_MORE_ICON_NUM;j++)
		{

			if (j < 4)
			{
	
				optionToMoredev->icon[j].y = bmp_sart_offy ;
				optionToMoredev->icon[j].x = bmp_sart_offx + j*bmp_between_offx  ;
			}
			else 
			{	
				if(j == GCODE_OPTION_TO_MORE_BACK_BT_ID) 	optionToMoredev->icon[j].x = bmp_sart_offx + 3*bmp_between_offx  ;
				else optionToMoredev->icon[j].x = bmp_sart_offx + (j - 4)*bmp_between_offx  ;
				
				optionToMoredev->icon[j].y = bmp_sart_offy + bmp_between_offy;
				
			}
			
			optionToMoredev->icon[j].width = PICTURE_X_SIZE;
			optionToMoredev->icon[j].height = PICTURE_Y_SIZE;
			optionToMoredev->icon[j].path = (u8*)opMoreui_icon_path_tbl[LANGUAGE][j];

		}

	optionToMoredev->totalIcon = GCODE_OPTION_TO_MORE_ICON_NUM;

	rtv =  StopPrint_remiderUI();
	if(rtv == 0) 
	{
		PRINTUI_DEBUG("only disp 3 icons\r\n");
		dispIconNum = 3;//ֻ��ʾԤ�� + ����ͼ�� + ��ӡ�ٶ�����
	}
	
	else dispIconNum = optionToMoredev->totalIcon;
	
		for(j=0; j < dispIconNum; j++)
		{
			display_bmp_bin_to_LCD((u8*)optionToMoredev->icon[j].path, optionToMoredev->icon[j].x , optionToMoredev->icon[j].y, optionToMoredev->icon[j].width, optionToMoredev->icon[j].height, optionToMoredev->icon[j].color);
		}
		
	if(rtv == 0)	
		{
			j = GCODE_OPTION_TO_MORE_BACK_BT_ID;
			display_bmp_bin_to_LCD((u8*)optionToMoredev->icon[j].path, optionToMoredev->icon[j].x , optionToMoredev->icon[j].y, optionToMoredev->icon[j].width, optionToMoredev->icon[j].height, optionToMoredev->icon[j].color);
		}

}

/**********************************/

void check_is_pause_exacute(void)
{
    u8 i;
   // static u8 first_run = 0;


	if(card.FILE_STATE == PS_PAUSE && filaState.filaPause == FALSE )//��ͣ��ӡ,δ����������ͣ,
	{
		isPauseStart ++;
		st_synchronize_no_lcd();
		if(isPauseStart == 1)//��ֻ֤����һ�������ͣ��λ��
		{
			
				//�ȴ�ֹͣ
			if((current_position[0] != destination[0]) ||
					(current_position[1] != destination[1]) ||
					(current_position[2] != destination[2]) ||
					(current_position[3] != destination[3]) ||
					(block_buffer_head != block_buffer_tail) ||  buflen )//�����Ϊ��Ϊֹ
			{
				isPauseStart = 0;
				return;
			}			
			  	//ȷ��֮ǰ������gcode��ִ�����
				for( i = 0 ; i < NUM_AXIS; i++)
				{
					saved_position[i] = current_position[i];//���浱ǰλ��
					temp_posistion[i] = current_position[i];
					PRINTUI_DEBUG("des1[%d] = %.7f ,cur1[%d]=%.7f\r\n",i,destination[i],i,current_position[i]);
				}
				 
				temp_posistion[X_AXIS] = 0.0f;//x ���ԭ�㣬����᲻��
				PRINT_STATE = HAS_EXACUTE_PAUSE;
				Cmd_MoveAxisCommand(temp_posistion); 
				
			
		}
		else isPauseStart = 2;
	}

    if(card.FILE_STATE == PS_PIRNT && (isPauseStart >= 1 || filaState.filaPause == TRUE))//�ָ���ӡ
    {
		if (isPauseStart >= 1 ) isPauseStart = 0 ;
        
		else if(filaState.filaPause == TRUE) 
			{
				filaState.filaPause = FALSE;//��λ��־

			}
			
		
        Cmd_MoveAxisCommand(saved_position);
		PRINT_STATE = NORMAL_PLAY;//ȷ����ӡͷ�ص���ͣλ�ú��ٸ��±�־���Է��ڴ˹����жϵ�
        PRINTUI_DEBUG("-------------------------------------- resume to print\r\n");
        card_startFileprint();//��ӡ
	
    }

}
void fila_run_out_pause(void)
{
    u8 i;

	if(card.FILE_STATE == PS_PIRNT && filaState.filaPause == FALSE)//��ͣ��ӡ,ȷ�����Ϻ�ִ������Ϻ󣬲ſ�ʼ��һ�ּ��
	{
		
		
		card_pauseSDPrint();//ֹͣSD������
		
		st_synchronize_no_lcd();		
				//�ȴ�ֹͣ
			if((current_position[0] != destination[0]) ||
					(current_position[1] != destination[1]) ||
					(current_position[2] != destination[2]) ||
					(current_position[3] != destination[3]) ||
					(block_buffer_head != block_buffer_tail))
			{
				
				return;
			}
			
					
			  	//ȷ��֮ǰ������gcode��ִ�����
				for( i = 0 ; i < NUM_AXIS; i++)
				{
					saved_position[i] = current_position[i];//���浱ǰλ��
					temp_posistion[i] = current_position[i];
					printf("des1[%d] = %.7f ,cur1[%d]=%.7f\r\n",i,destination[i],i,current_position[i]);
				}
				temp_posistion[X_AXIS] = 0.0f;//x ���ԭ�㣬����᲻��

				PRINT_STATE = HAS_EXACUTE_PAUSE;
				Cmd_MoveAxisCommand(temp_posistion); 
				card.FILE_STATE = PS_PAUSE;//��ͣ
				filaState.filaPause = TRUE;//��־����������ͣ
	}	
}







/*************************************/
