/*
*********************************************************************************************************
*
*	模块名称 :打印界面模块
*	文件名称 : print.c
*	版    本 : V1.0
*	说    明 : 打印界面
*
*
*********************************************************************************************************
*/


/**********************包含头文件*************************/
#include "print.h"



#include "temperature.h"
#include "gcodeplayer.h"
#include "stepper.h"
#include "Dlion.h"
#include "planner.h"

#include "progressbar.h"

/*********************************************/

/**********************调试宏定义**************************/

#define PRINTUI_DEBUG_EN

#ifdef PRINTUI_DEBUG_EN

#define PRINTUI_DEBUG	DBG

#else
#define PRINTUI_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define PRINT_NUM_ROW	1//总共1行控件
#define PRINT_ROW_COUNT	4//每行用4个控件


#define PRINT_FILE_ICON_OFFSET  3


#define PRINT_FILE_ICON_NUM			5//每页最多显示5个
#define PRINT_FILE_FIX_ICON_NUM		3


#define PRINT_FILE_ROOT		"0:"//SD卡
#define FILE_NAME_MAX_LENGTH	512    //路径长度，当打开的文件目录嵌套的文件名达到这个数时，就不能正常嵌套下去了，根据应用来设置该值大小
#define DEST_FILE_NAME_MAX_LENGTH	100//选中的目标文件/文件夹长度，根据实际情况调整该值
#define PRINT_FOLDER_FILE_COLOR	 GRAY

#define GCODE_PRINT_ICON_NUM	5//gcode打印界面的图标数目
#define GCODE_PRINT_BACK_ICON_ID	4//gcode打印界面退回控件标号

#define GCODE_PRINT_OPTION_BUTTON	3//选择或者返回控件下标


/******************gcode 打印option 控件选择界面******************/
#define GCODE_OPTION_ICON_NUM			5 //pause 和 start 两个控件处于同一位置
#define GCODE_OPTION_STOP_BUTTON_ID		0
#define GCODE_OPTION_PAUSE_BUTTON_ID	1
#define GCODE_OPTION_MORE_BUTTON_ID		2
#define GCODE_OPTION_BACK_BUTTON_ID	 	3
#define GCODE_OPTION_START_BUTTON_ID	4



/**************************gcode打印 option->more 界面***************************/
#define GCODE_OPTION_TO_MORE_ICON_NUM			6
#define GCODE_OPTION_TO_MORE_PREHEAT_BT_ID			0
#define GCODE_OPTION_TO_MORE_FAN_BT_ID				1
#define GCODE_OPTION_TO_MORE_SPEED_BT_ID				2 //打印速度设置
#define GCODE_OPTION_TO_MORE_CHANGE_FILA_BT_ID				3
#define GCODE_OPTION_TO_MORE_FILA_BT_ID				4

#define GCODE_OPTION_TO_MORE_BACK_BT_ID				5


#define SD_UNPLUG_UI_ICON_NUM				2
#define SD_UNPLUG_UI_STOP_BT_ID				0
#define SD_UNPLUG_UI_BACK_BT_ID				1











/******************************************************************/
_gui_dev *printUIdev = NULL;
static uint16_t	bmp_sart_offx = 1;//第一排图标的左边第一个图片距离边框距离
static uint16_t bmp_sart_offy = 24 ;//第一排图标左边第一个图片距离标题栏距离
static uint8_t hasGotoConfirmPrintFileScreen = 0;//进入gcode文件确认界面标志
static uint8_t sdCardIconDispFlag = 0;//SD卡拔插提示界面切换图标控制标志
static	uint8_t	 isPauseStart ;	//用于暂停/打印检测执行操作标志
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
//icon的路径表,MOVE界面图标
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
	"是否打印这个模型",
	 "Are you sure to print the model?"
};

char *const print_stop_infor[GUI_LANGUAGE_NUM] = 
{
	"是否停止打印这个模型",
	"Sure to stop printing the model?"
};

char *const print_confirm_title[GUI_LANGUAGE_NUM] =
{
	"准备/选择",
	"Prepare/pickUp"
};

char *const gcode_print_title [GUI_LANGUAGE_NUM]= 
{
	"打印->",
	"Print->"
};

char *const gcode_print_time [GUI_LANGUAGE_NUM] = 
{
	"时间: ",
	"Time: "
};

char *const optToMoreUI_title[GUI_LANGUAGE_NUM] =
{
	"打印.../工具/更多",
	 "Print.../option/more"
};

char *const optToMoreFanUI_title[GUI_LANGUAGE_NUM]= 
{
	"打印.../工具/更多/风扇",
	"Print.../option/more/fan"
};

char *const optToMoreChangeFilaUI_title[GUI_LANGUAGE_NUM]= 
{
	"打印.../工具/更多/换料",
	"Print.../option/more/change filament"
};

char *const optToMoreFilaIOUI_title[GUI_LANGUAGE_NUM] = 
{
	"打印.../工具/更多/进退料",
	"Print.../option/more/filament in/out"
};





//SD卡拔出提示信息
char *const sdCardUnplug_title [GUI_LANGUAGE_NUM]=
{
	"准备/打印",
	 "Prepare/print"
};

char *const sdCardUnplug_remind_infor1 [GUI_LANGUAGE_NUM]= 
{
	"请检查存储卡",
	"Pls Check if the SD card"
};

char *const sdCardUnplug_remind_infor2 [GUI_LANGUAGE_NUM]= 
{
	"或者打印文件缺失",
	"or print file is lost"
};

char *const sdCardUnplug_remind_infor3[GUI_LANGUAGE_NUM] = 
{
	"->请再次插入存储卡",
	"->Then try again!"
};

char *const sdCardUnplug_remind_infor4 [GUI_LANGUAGE_NUM]=
{
	"->是否暂停打?",
	 "-> be sure stop print?"
};



char *const sdCardplug_remind_infor1[GUI_LANGUAGE_NUM] = 
{
	"->存储卡已插入,请返回",
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
	"1:/SYSTEM/PICTURE/ch_bmp_speed.bin",//2--打印速度设置
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",	//3
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",	//4
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",	//5
	},
	{
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",//0	    
	"1:/SYSTEM/PICTURE/en_bmp_fan.bin",	   //1	 
	"1:/SYSTEM/PICTURE/en_bmp_speed.bin",//2--打印速度设置
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",	//3
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",//4
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",	//5
	
	}
	
};  


char *const fanInfor[GUI_LANGUAGE_NUM] = 
{
	"风扇:",
	"FAN:"
};

char *const bedInfor[GUI_LANGUAGE_NUM] = 
{
	"热床:",
	"Bed:"
};
char *const extrudInfor[GUI_LANGUAGE_NUM] = 
{
	"挤出:",
	"Etru:"
};
_yes_no_e IS_PAUSE_BT = YES;
_print_time_s printTime;
_parameter_state_s printParam;
u8 pause_start_cnt = 0;//奇数--恢复图标，偶数--暂停图标，打印完成复位标志
//uint8_t isOption2moreUiPreheatSelected = NO;//gcode文件打印 界面->option界面->more界面中的预热触发标志,打印完成就复位该标志
//uint8_t isOption2moreUiFanSelected = NO;//gcode文件打印 界面->option界面->more界面中的风扇触发标志,打印完成就复位该标志
uint8_t optionBackBtSet = 0;//开始打印界面中的选择图标/返回图标切换标志位，1--切换到返回图标，0--切换到选择图标

uint8_t isOption2moreUiSelected = NO;//进入到打印界面->option界面->more界面标志，用于某些界面重复使用后，返回控制


/************************打印界面图标路径***********************/

/******************************************************************/

//static uint32_t filam_adj_update_time = 0;
 uint32_t escape_time;

uint8_t pause_op_counter ;
/**********************************************/

/********************************全局变量**********************/
_gui_dev *pintdev = NULL;
_gui_dev *startPintdev = NULL;
_gui_dev *optiondev = NULL;//gcode文件打印选择界面
_gui_dev *optionToMoredev = NULL;//gcode文件打印选择界面->more 界面
_gui_dev *sdCardDev = NULL;//SD card 拔出检测界面

_progressbar_obj* prgb = NULL;
_btn_obj **screen_key_group = NULL;




_file_page_ctrl_block_s *curFilePageCtrlBlock = NULL;


uint8_t  curPageCnt = 0;


float step_value[3] = {45.0, 190.0, 50.0}; //bed--45C，Extrude--190C，Fan--50%

float set_value[3] = {45.0, 190.0, 50.0}; //bed--45C，Extrude--190C，Fan--50%//preheatUI界面




uint8_t adj_value_flag = 0;//设定温度、风扇值的id标志
_GUI_TURE_OR_FALSE is_set_value = IS_FALSE;//标记设定热床、挤出头和风扇

//确定或者取消  
char *const CONFIRM_CAPTION_TBL[GUI_LANGUAGE_NUM][2]=
{
	{"确定","取消"},
	{"YES","NO"}
}; 


uint8_t *printFileName = NULL;//记录当前打印的文件名，包含目录
uint8_t *printFileNameNOPrefix = NULL;//记录当前打印的文件名,去掉前缀"0:/GCODE/"


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

	 //清屏
	 if( print_update_time <= millis())
	 {
		print_update_time = millis() + 300;
		
		if(card.FILE_STATE == PS_STOP || card.FILE_STATE == PS_PRINT_OVER_NOR ) 
		escape_time = stoptime / 1000;//正常打印结束 + 用户结束打印
		
        else if (card.FILE_STATE == PS_NO) 
		{
            escape_time = 0;
        }
        else escape_time = (millis() - starttime) / 1000;//ms->s exchage
		
		escape_time += powerDownPrintTime;//加上上次断电打印的时间
		
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
	 Show_Str(x_start,y_start,width,hight,(u8 *)gcode_print_time[LANGUAGE],f_size,1);//显示"Time: "
	 
    gui_show_num(x_start + length*(f_size / 2), y_start, 2, POINT_COLOR, f_size, hour, 1); //显示小时xx
    LCD_ShowString(x_start + length*(f_size / 2) + 3 * (f_size / 2), y_start , width, hight, f_size, (u8*)":"); //显示：
    gui_show_num(x_start +  length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2),  y_start, 2, POINT_COLOR, f_size, minute, 1); //显示分钟xx
    LCD_ShowString(x_start + length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2) + 3 * (f_size / 2), y_start,width, hight, f_size, (u8*)":"); //显示：
    gui_show_num(x_start +  length*(f_size / 2) + 3 * (f_size / 2) + 2 * (f_size / 2)+ 3 * (f_size / 2) +  2 * (f_size / 2),  y_start , 2, POINT_COLOR, f_size, second, 1); //显示秒xx	
	 
		
}

//文件页控制块创建，初始化时避免重复创建
_file_page_ctrl_block_s * create_file_page_ctrl_block(u8 * path)
{
	_file_page_ctrl_block_s * filePageCtrlBlock;
	u16 fileNameLength;
	u16 destFileNameLength;
	//_file_page_ctrl_block_s结构体
	filePageCtrlBlock=(_file_page_ctrl_block_s*)gui_memin_malloc(sizeof(_file_page_ctrl_block_s));	//分配内存
	if(filePageCtrlBlock==NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//内存分配不够.
	}
	gui_memset((u8*)filePageCtrlBlock,0,sizeof(_file_page_ctrl_block_s));//清零所有成员变量
	
#if _USE_LFN						
 	fileNameLength = _MAX_LFN * 4 + 1;
	destFileNameLength = _MAX_LFN * 2 + 1;
#else
	fileNameLength = FILE_NAME_MAX_LENGTH;
	destFileNameLength = DEST_FILE_NAME_MAX_LENGTH;
#endif	 	

	//遍历目录属性
	filePageCtrlBlock->path=(u8*)gui_memin_malloc(fileNameLength);	//分配内存
	if(filePageCtrlBlock->path==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//内存分配不够.

	}

	strcpy((char *)filePageCtrlBlock->path, (char *)path);
	
	filePageCtrlBlock->destFileName = (u8*)gui_memin_malloc(destFileNameLength);	//分配内存
	if(filePageCtrlBlock->destFileName==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock->path);
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//内存分配不够.
	}
	
	
	//文件\文件夹GUI属性
	filePageCtrlBlock->fileGuiAttr = (_file_gui_attribute_s*)gui_memin_malloc(sizeof(_file_gui_attribute_s));	//分配内存
	if(filePageCtrlBlock->fileGuiAttr==NULL)
	{
		
		gui_memin_free(filePageCtrlBlock->path);
		gui_memin_free(filePageCtrlBlock->destFileName);
		gui_memin_free(filePageCtrlBlock);
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//内存分配不够.

	}
	gui_memset((u8*)filePageCtrlBlock->fileGuiAttr,0,sizeof(_file_gui_attribute_s));//清零所有成员变量
	
	filePageCtrlBlock->fileGuiAttr->activeButtonNum = PRINT_FILE_ICON_NUM;
	filePageCtrlBlock->fileGuiAttr->fixButtonNum = PRINT_FILE_FIX_ICON_NUM;
	filePageCtrlBlock->fileGuiAttr->pageCnt = 1;//从第一页显示
	
	filePageCtrlBlock->fliter = 0;//所有文件过滤(包含文件夹)
	filePageCtrlBlock->fliterex = 0xffff;//不对小类进行过滤(所有小类均符合要求)
	filePageCtrlBlock->fileDepth = 0;
	filePageCtrlBlock->destFileType = NO_FILE_TYPE;
	
	filePageCtrlBlock->FileNameListAttributeTab = NULL;
	filePageCtrlBlock->FileNameListTab = NULL;	
	filePageCtrlBlock->atualReadItems = 0;
	filePageCtrlBlock->readItemsOffSet = 0;
	filePageCtrlBlock->selectedButtonID = NONE_PRINT_ICON;
	return filePageCtrlBlock;
}
//释放文件页面控制块内存
static void free_file_page_ctrl_block(_file_page_ctrl_block_s *pFile)
{
	u8 i;

	u8 fileNameListItems;
	if(pFile == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> parameter error\r\n",__FILE__,__LINE__);
		return;
	}
	if(pFile->fileGuiAttr != NULL)//文件GUI属性
	{
		fileNameListItems = pFile->fileGuiAttr->activeButtonNum;//获得文件名列表的项数
	}
	
	if(pFile->FileNameListAttributeTab != NULL)//文件/文件夹属性表
	{
		gui_memin_free(pFile->FileNameListAttributeTab);
	}
	
	if(pFile->path != NULL)//文件/文件夹目录
	{
		gui_memin_free(pFile->path);
	}
	
	if(pFile->FileNameListTab != NULL)//文件/文件夹名字列表
	{
		for(i = 0 ; i < fileNameListItems ; i++)
		{
			if(pFile->FileNameListTab[i] != NULL) 
			gui_memin_free(pFile->FileNameListTab[i]);//分项清零
		}
		gui_memin_free(pFile->FileNameListTab);//总项清零
	}
	
	if(pFile->fileGuiAttr != NULL)//文件GUI属性
	{
		gui_memin_free(pFile->fileGuiAttr);
	}

	if(pFile->destFileName != NULL)//选中的目标文件名
	{
		gui_memin_free(pFile->destFileName);
	}
	gui_memin_free(pFile);

	
	gui_memin_free(printUIdev->icon);
	gui_memin_free(printUIdev);
	printUIdev =NULL;
	pFile =  NULL;
	curFilePageCtrlBlock =  NULL;//确保指针为0
}
//初始化1个文件显示界面控制块
u8 init_file_page_ctrl_block(u8 * path)
{
	u8 rtv = 0;
	curFilePageCtrlBlock = create_file_page_ctrl_block(path);
	curFilePageCtrlBlock->fliter = FLBOX_FLT_GCODE;//文件大类过滤
	return rtv;
}

//盘寻当前目录，获得文件/文件夹数目
//返回值最大为0XFFFFFFFE,即当前目录的文件/文件夹数目不能超过所允许的最大值
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
	if(pFile == NULL)  //无效路径
	{
		totalItems = 0xFFFFFFFF;
		PRINTUI_DEBUG("file:%s,line:%d -> path error\r\n",__FILE__,__LINE__);
		return totalItems;
	}

	if(pFile->path == NULL)  //无效路径
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
	
	res=f_opendir(&filedir,(const TCHAR*)pFile->path); //打开一个目录
	if(res==FR_OK)		
	{
		
	}
	while(res==FR_OK)//遍历当前目录下的所有文件
	{
		
			res=f_readdir(&filedir,&fileinfo);//读取文件信息
			if(res!=FR_OK||fileinfo.fname[0]==0)
			{
				break;		//错误了/到末尾了,退出   
			}
			if (fileinfo.fname[0]=='.'&&fileinfo.fname[1]==0)continue;//忽略本级目录
			fn=(u8*)(*fileinfo.lfname?fileinfo.lfname:fileinfo.fname);
			
			
		if(fileinfo.fattrib & AM_DIR)//文件夹
		{
			totalItems++;
			folderCnt++;
			//DBG("->file:%s\r\n",fn);				
		}
		else if(fileinfo.fattrib & AM_ARC )//是一个归档文件且mark=1
		{
			type=f_typetell(fn);	//获得类型
			typeex=type&0XF;		//获得小类的编号
			type>>=4;				//获得大类的编号
			if(((1<<type) & pFile->fliter) && ((1<<typeex) & pFile->fliterex ) )//大小类型都正确，目标文件
			{
				totalItems++;
				fileCnt++;
				//DBG("->file:%s\r\n",fn);				
			}
			else continue;	//不需要的类型	
		}
		else continue;		//继续找下一个 	
	}
	 res=f_closedir(&filedir); //关掉目录
	gui_memin_free(fileinfo.lfname);
	
	totalPage = totalItems/pFile->fileGuiAttr->activeButtonNum ;//整页
	if(totalItems % pFile->fileGuiAttr->activeButtonNum )totalPage += 1;//不足一页按一页算
	
	pFile->fileGuiAttr->TotalPage = totalPage;
	
	DBG("->totalItems:%d = folderNum:%d,fileNum:%d\r\n",totalItems,folderCnt,fileCnt);	
	return totalItems;
	
}

//遍历指定目录下指定个数的文件/文件夹
//更新文件名列表、属性列表等，读文件偏移被更新
void scan_root_file(_file_page_ctrl_block_s *pFile)
{
	
	u8 res=0; 
 	DIR filedir;		
	FILINFO fileinfo;	 	   
	u16 type=0XFFFF;

	u16 typeex=0;
	u16 readItemOffSet;//读取文件偏移，因为切换界面时，要切换下一页的文件/文件夹，更新偏移
	u16 readCnt = 0;
	u16 aquireItems;//期望读取文件/文件夹数目
	u16 actualReadItems = 0;//实际读取的文件/文件夹数目
	
	u8 *path= NULL;//当前路径
	u8 *fn = NULL;   //This function is assuming non-Unicode cfg. 
	u32 findex;	 
	u16 fnLength;
	u16 folderCnt = 0;
	u16 fileCnt = 0;
	if(pFile == NULL)//文件指针有效性检测
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
		return ;			//内存分配不够.
	}

	readItemOffSet = pFile->readItemsOffSet;//获取读取文件偏移
	path = pFile->path;//获取当前目录
	

	aquireItems = pFile->fileGuiAttr->activeButtonNum;//获得一次遍历文件/文件夹数目
	if(pFile->FileNameListTab == NULL)//为aquireItems个文件/文件名的名字指针申请内存,第一次盘寻
	{
		pFile->FileNameListTab = (u8**)gui_memin_malloc(sizeof(u8 *) * aquireItems);	//为aquireItems 个 u8 型指针分配内存
		if(pFile->FileNameListTab==NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
			gui_memin_free(fileinfo.lfname);
			return ;			//内存分配不够.
		}		
	}
	//清零上一次盘寻的文件名列表
	gui_memset((u8**)pFile->FileNameListTab,0,sizeof(u8 *)*aquireItems);//清零所有成员变量,即p[0]=p[1]=p[2]=...=p[aquireItems - 1] =NULL;
	
	//还未为每个文件/文件夹的名字的具体储存长度申请内存，根据遍历得到的文件/文件名长度来申请内存

	
	if(pFile->FileNameListAttributeTab == NULL)//为aquireItems个文件/文件夹对应的属性申请内存，实际使用以读出的文件/文件夹数量为准,第一次盘寻
	{
		pFile->FileNameListAttributeTab = (u8*)gui_memin_malloc(sizeof(u8 ) * aquireItems);	//分配内存,FileNameListAttributeTab是 u8 * 类型，1个bytes--对应一个属性
		
		if(pFile->FileNameListAttributeTab==NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
			gui_memin_free(fileinfo.lfname);
			gui_memin_free(pFile->FileNameListTab);
			return ;			//内存分配不够.
		}
	}
	//清零上一次盘寻的文件属性表
	gui_memset((u8*)pFile->FileNameListAttributeTab,0,sizeof(u8) * aquireItems);//清零所有成员变量
	
	
	res=f_opendir(&filedir,(const TCHAR*)path); //打开一个目录
	if(res==FR_OK)
	{
		
	}
	while(res==FR_OK)//遍历当前目录下aquireItems个文件/文件夹
	{
						
			if(actualReadItems == aquireItems )//读到指定个数的文件/文件夹
			{
								
				break;
			}
			findex=filedir.index;//记录下此文件的index值

			res=f_readdir(&filedir,&fileinfo);//读取文件信息
			if(res!=FR_OK||fileinfo.fname[0]==0)
			{
			
			
				break;		//错误了/到末尾了,退出   
			}
			if (fileinfo.fname[0]=='.'&&fileinfo.fname[1]==0)continue;//忽略本级目录
			fn=(u8*)(*fileinfo.lfname?fileinfo.lfname:fileinfo.fname);
			
			fnLength = strlen((char *)fn) + 1;
						
			
			if(fileinfo.fattrib & AM_DIR)//文件夹
			{
				readCnt++;
				
				if(readCnt <= readItemOffSet) continue;//读取完上一页的文件/文件夹
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
							for(;actualReadItems > 0;actualReadItems--)gui_memin_free(pFile->FileNameListTab[actualReadItems - 1]);//actualReadItems先申请内存后再加1
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							return ;			//内存分配不够.
						}
					
					}				
				strcpy((char *)pFile->FileNameListTab[actualReadItems],(char *)fn);
				DBG("->folder:%s\r\n",pFile->FileNameListTab[actualReadItems]);	
				//为文件夹属性赋值
				pFile->FileNameListAttributeTab[actualReadItems] = IS_FOLDER;
				
				actualReadItems++;
			
			}
			else if(fileinfo.fattrib & AM_ARC )//是一个归档文件且mark=1
			{
				type=f_typetell(fn);	//获得类型
				typeex=type&0XF;		//获得小类的编号
				type>>=4;				//获得大类的编号
				if( ((1<<type) & pFile->fliter) && ((1<<typeex) & pFile->fliterex ))//大小类型都正确，目标文件
				{
					readCnt++;
					if(readCnt <= readItemOffSet) continue;//读取完上一页的文件/文件夹
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
							for(;actualReadItems > 0;actualReadItems--)gui_memin_free(pFile->FileNameListTab[actualReadItems - 1]);//actualReadItems先申请内存后再加1
							PRINTUI_DEBUG("file:%s,line:%d -> malloc fail\r\n",__FILE__,__LINE__);
							return ;			//内存分配不够.
						}
					
					}

					
					strcpy((char *)pFile->FileNameListTab[actualReadItems],(char *)fn);
					
					DBG("->file:%s\r\n",pFile->FileNameListTab[actualReadItems]);	
					//为文件属性赋值
					pFile->FileNameListAttributeTab[actualReadItems] = IS_GCODE_FILE;

						actualReadItems++;
						
				}
				else continue;	//不需要的类型	
			}
			else continue;		//继续找下一个 	
			
	}
	pFile->atualReadItems = actualReadItems;
	//翻页在外部实现
	pFile->readItemsOffSet +=  actualReadItems;//更新读取文件偏移，当要返回上一级目录时，文件偏移要清零
		 res=f_closedir(&filedir); //关掉目录
		gui_memin_free(fileinfo.lfname);
		DBG("->folderNum:%d,fileNum:%d\r\n",folderCnt,fileCnt);	
}
//获得本级目录名字,或者从带目录的打印文件名中剥离出不带目录的gcode文件
u8 * get_current_root_file_name(u8 *path)
{
	u8 pathLength;
	u8 *tempPath = path;
	u8 *fn = NULL;
	u8 fileNameLength = 0;
	pathLength = strlen((char *)tempPath) + 1;
	while(*tempPath != '\0')//找到字符串末尾
	{
		tempPath++;
	}
	
	while(*tempPath != '/' && pathLength)//从字符串末尾往前找
	{
		tempPath--;
		fileNameLength++;
		pathLength--;
	}
	//找到最后一级目录
	tempPath++;//指针指向最后一级目录
	
	fn = (u8*)gui_memin_malloc(fileNameLength );	//分配内存
	if(fn==NULL)
	{	
		PRINTUI_DEBUG("file:%s,line:%d -> filePageCtrlBlock->path malloc fail\r\n",__FILE__,__LINE__);
		return NULL;			//内存分配不够.
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
//显示文件/文件夹图标界面
void display_file_page_icon(_file_page_ctrl_block_s *pFile)
{


	if(pFile == NULL)//文件指针有效性检测
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
//要显示的文件\文件夹数量，文件\文件夹名字链表，对应的文件\文件夹属性表,GUI属性都存在的情况下
	
	draw_file_page_infor(pFile);
	pFile->selectedButtonID = NONE_PRINT_ICON;//处理完显示，复位控制按键
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
	
	static uint8_t timeCnt = 0;	//本界面进入次数计数器
	static uint8_t haveDrawed = 0;	//标志是否已经重绘过了
	
	font = 16;
	actIconNum = pFile->atualReadItems;//实际读取到的文件\文件夹个数
	fixIconNum = pFile->fileGuiAttr->fixButtonNum;
	fileAttr = pFile->FileNameListAttributeTab;
	fnList = pFile->FileNameListTab;
	curPage = pFile->fileGuiAttr->pageCnt;
	totalPage = pFile->fileGuiAttr->TotalPage;
	buttonID = pFile->selectedButtonID;
	

	fileDepth = pFile->fileDepth;
	if(redraw_fileSystemUI_flag == IS_TRUE)//从别的功能界面切换回文件系统界面
	{
		redraw_fileSystemUI_flag = IS_FALSE;
		timeCnt = 0;
	}
	if(redraw_fileSystemUI_flag == IS_FALSE)
	{
		timeCnt++;
	}
	
	if(timeCnt >= 2)//非第一次进入本界面
	{
		timeCnt = 2;
		
		if(curPage <= totalPage)//
		{			

			if(buttonID == IS_PRINT_DOWN_ICON || buttonID == IS_PRINT_UP_ICON)//控件--下一页/下一页被按下
			{
				needToDrawGui = 1;//需要重绘UI
				 timeCnt = 0;
				  haveDrawed = 1;
			}	
			
		}
		if(needToDrawGui != 1) //不需重绘
		{			
			return;
		}
	}
	if(timeCnt == 1 && haveDrawed == 1) //不需重绘
	{
		haveDrawed = 0;
		return;
	}
	
	BACK_COLOR = BLACK;
    POINT_COLOR = BLUE;
	iconNum =actIconNum + fixIconNum;
	desFileName = get_current_root_file_name(pFile->path);//获得本目录名字，显示结束后，释放内存
	LCD_Clear_DMA(BLACK);//清屏
	length = strlen((char*)print_confirm_title[LANGUAGE]);
	Show_Str(xOff,yOff,lcddev.width,font,(u8 *)print_confirm_title[LANGUAGE],font,1);	
	LCD_ShowNum(xOff + length*(font/2),yOff,curPage,2,font);
	LCD_ShowString(xOff + length*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/");	
	LCD_ShowNum(xOff + length*(font/2)+ 3*(font/2),yOff,totalPage,2,font);
	LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2),yOff,lcddev.width,font,font,(u8 *)PRINT_FILE_ROOT);
	if(fileDepth == 1)//第一层嵌套
	{
		xEnd = lcddev.width;
		yEnd = yOff + font;
		LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/");	
		if(desFileName != NULL)
		gui_show_ptstr(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2)+ 3*(font/2),yOff,xEnd,yEnd,dispOffSet,POINT_COLOR,font,(u8 *)desFileName,1);
	}
	if(fileDepth > 1 )//确认是文件夹且路径深度大于1
	{
		xEnd = lcddev.width;
		yEnd = yOff + font;
		LCD_ShowString(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2) + 2*(font/2),yOff,lcddev.width,font,font,(u8 *)"/.../");	
		if(desFileName != NULL)
		gui_show_ptstr(xOff + length*(font/2)+ 3*(font/2) + 3*(font/2)+ 2*(font/2) + 5*(font/2),yOff,xEnd,yEnd,dispOffSet,POINT_COLOR,font,(u8 *)desFileName,1);
	}
	
	free_current_root_file_name_memory(desFileName);
	
	if(printUIdev == NULL )	//为主界面控制器申请内存,第一次初始化 或者之前已经初始化后，现在再次初始化
	{
		printUIdev = (_gui_dev *) gui_memin_malloc(sizeof(_gui_dev));
		
		if( printUIdev == 0)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
			return;
		}
	}
	
	if(((u32)printUIdev->icon & RAM_ADDR) == RAM_ADDR)//之前已经申请过内存了
	{
		gui_memin_free(printUIdev->icon);
	}
	
	printUIdev->icon =(_gui_icon_info *) gui_memin_malloc(sizeof(_gui_icon_info)*iconNum);//每次都重新申请内存
		
	if( printUIdev->icon == NULL)
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memory error!\r\n",__FILE__,__LINE__);
		return;
	}

	bmp_between_offx = PICTURE_X_SIZE + 2;
	bmp_between_offy = PICTURE_Y_SIZE + 2;
	printUIdev->status = 0x3F ;
	
	//为固定的图标赋值,第二排后三个控制图标
			for(j=0;j < fixIconNum; j++)
			{			
				printUIdev->icon[j].x = bmp_sart_offx + j*bmp_between_offx + bmp_between_offx;
				printUIdev->icon[j].y = bmp_sart_offy + bmp_between_offy;
				printUIdev->icon[j].width = PICTURE_X_SIZE;
				printUIdev->icon[j].height = PICTURE_Y_SIZE;
				printUIdev->icon[j].path = (u8*)printui_icon_path_tbl[LANGUAGE][j];

			}
	
//为动态的图标赋值，动态图标下标号偏移为fixIconNum，即第一个动态图标对应的下标号为0+3=3，依此类推		
		for(i = 0,j=fixIconNum;j < actIconNum + fixIconNum ; j++,i++)
		{
			
			if(i == PRINT_FILE_ICON_NUM - 1)//活动的图标等于5个时，需要在第二排显示
			{
				if( j == actIconNum + fixIconNum - 1)//第二排第一个图标
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
			
			if(fileAttr[i] == 1)//gcode文件
			{
				printUIdev->icon[j].path = (u8*)printui_fileIcon_path[LANGUAGE];	
			}
			else printUIdev->icon[j].path = (u8*)printui_folderIcon_path[LANGUAGE];//文件夹
			
			printUIdev->icon[j].width = PICTURE_X_SIZE;
			printUIdev->icon[j].height = PICTURE_Y_SIZE;
			

		}

	printUIdev->totalIcon = iconNum;
	strOff = PICTURE_Y_SIZE - 2*font;
	POINT_COLOR = GREEN;
		for(i = 0,j=fixIconNum;j < actIconNum + fixIconNum ; j++,i++)//先画活动图标
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

//打开下一级目录
//返回值：0--成功返回，即路径名称已经发生改变，其他值--错误
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
	tempLength = strlen((char *)newFn) + strlen((char *)srcFn) + 2;//1--添加的1个'/'字符
	tempFn = (u8*)gui_memin_malloc(sizeof(u8 ) * tempLength);
	if(tempFn == NULL) 
	{
		PRINTUI_DEBUG("file:%s,line:%d -> memoery error!\r\n",__FILE__,__LINE__);
		return rtv;
	
	}
	gui_memset((u8*)tempFn,0,sizeof(u8 )*tempLength);
	strcpy((char *)tempFn,(const char *)newFn);
	strcat((char*)tempFn,(const char*)"/");	//添加斜杠
	strcat((char*)tempFn,(const char*)srcFn);
	tempFn[tempLength - 1] = '\0';
	
	strcpy((char *)newFn,(const char *)tempFn);
	
	gui_memin_free(tempFn);//释放旧目录的内存
	tempFn = NULL;
	
	rtv = 0;
	return rtv;
}

//返回上一级目录
//返回值：0--成功返回，即路径名称已经发生改变，1-- 当前目录为根目录，其他值--错误
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
	while(*tempFn != '\0')//找到字符串末尾
	{
		if(*tempFn == '/')	rootDepth++;
		tempFn++;
	}
	if(rootDepth == 0 ) //文件路径检查,根目录为："0:/GCODE"
	{
		rtv = 1;
		PRINTUI_DEBUG("file:%s,line:%d -> first root \r\n",__FILE__,__LINE__);

	}
	else if(rootDepth >= 1)
	{
		while(*tempFn != '/' && cnt)//从字符串末尾往前找
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
			offSetPoint = tempFn - srcFn;//得到字符串第一个字符与倒数第一层目录的位置偏移字节数
			tempLength = offSetPoint + 1;//2--字符串第一个字符 + '\0'字符占用的字节
			
			newFn = (u8*)gui_memin_malloc(sizeof(u8 ) * tempLength);
			strncpy((char *)newFn,(const char *)srcFn,tempLength);
			newFn[tempLength - 1] ='\0';
			strcpy((char *)srcFn,(const char *)newFn);
					
			gui_memin_free(newFn);//释放旧目录的内存
			newFn = NULL;			
			rtv = 0;		
		
		}
	}
	return rtv;
}
//获取打印文件名字，相关的内存要在结束打印操作后 或者 SD卡拔出后 释放
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
		length = strlen((char *)path) + strlen((char *)fn) + 2;//2--'/' + '\0'字符
		printFileName = (u8*)gui_memin_malloc(sizeof(u8 ) * length);
		if(printFileName == NULL)
		{
			PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
		}
		else
		{
			strcpy((char *)printFileName,(const char *)path);
			strcat((char*)printFileName,(const char*)"/");	//添加斜杠
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
//按键扫描处理
//返回值：0--切换功能界面，1--进入下一级目录 2--选中打印文件 其他值--未定义
u8  exacute_button_even(_file_page_ctrl_block_s *pFile)
{

	
	u8 isRootChanged = 0xff;
	u8 isBackPrev = 0xff;
	u16 actualItem;//盘寻当前目录得到的单次文件个数
	u8 rtv = 0XFF;
	u8 *fileAttrubeTab =NULL;//文件属性表
	u8 **fileNameTab = NULL;
	
	_printUI_selected_button_e PRINT_SELECTED_BUTTON;
					

	actualItem = pFile->atualReadItems;//获得实际的活动图标个数
	fileAttrubeTab = pFile->FileNameListAttributeTab;
	fileNameTab = pFile->FileNameListTab;


	curPageCnt = pFile->fileGuiAttr->pageCnt;//获得当前页面号
	display_file_page_icon(pFile);//显示界面
	PRINT_SELECTED_BUTTON = (_printUI_selected_button_e)gui_touch_chk(printUIdev);
	
	switch (PRINT_SELECTED_BUTTON)
		{
			case IS_PRINT_UP_ICON://上一页
			PRINT_SAVED_BUTTON = IS_PRINT_UP_ICON;
			pFile->selectedButtonID = IS_PRINT_UP_ICON;
			if(curPageCnt > 1 )
			{
				curPageCnt--;
				pFile->fileGuiAttr->pageCnt = curPageCnt;
				pFile->readItemsOffSet -= (pFile->fileGuiAttr->activeButtonNum + pFile->atualReadItems);//读文件偏移返回为上一页第一个元素偏移值,本页元素 + 上一页元素 = 要减的数
				scan_root_file(pFile);//更新文件/文件名列表、文件属性等
			}
			PRINTUI_DEBUG("->page up\r\n");
			break;
				
			case IS_PRINT_DOWN_ICON://下一页
			
			PRINT_SAVED_BUTTON = IS_PRINT_DOWN_ICON;
			pFile->selectedButtonID = IS_PRINT_DOWN_ICON;
			if(curPageCnt < pFile->fileGuiAttr->TotalPage)
			{
				curPageCnt++;	
			pFile->fileGuiAttr->pageCnt = curPageCnt;
			scan_root_file(pFile);//更新文件/文件名列表、文件属性等
				
			}
			PRINTUI_DEBUG("->page down\r\n");
				break;
				
			case IS_PRINT_BACK_ICON://返回
			pFile->selectedButtonID = IS_PRINT_BACK_ICON;
			isBackPrev = goto_prev_fileRoot(pFile->path);
			if(isBackPrev == 0)//返回上一级目录成功
			{
				pFile->fileDepth--;//文件嵌套深度减少，在根目录基础上
				
				redraw_fileSystemUI_flag = IS_TRUE;
				pFile->readItemsOffSet = 0;//切换界面或者返回上一级目录，读文件/文件夹偏移清零
				get_current_root_page_items(pFile);//获得当前目录下总的文件/文件夹数目,当当前目录变化时，需要调用
				scan_root_file(pFile);//更新文件/文件名列表、文件属性等
				
			}
			else if(isBackPrev == 1)//当前目录为根目录，返回操作为上一功能界面
			{
				pFile->readItemsOffSet = 0;//切换界面或者返回上一级目录，读文件/文件夹偏移清零
				rtv = 0;
				pFile->fileDepth = 0;
			}
			
			curPageCnt = 1;
			pFile->fileGuiAttr->pageCnt = curPageCnt;//复位页面计数器
			PRINTUI_DEBUG("->back\r\n");
				break;
				
			case IS_PRINT_FIRST_ICON://第一图标
			if(actualItem < 1) return rtv;
			pFile->selectedButtonID = IS_PRINT_FIRST_ICON;
			if(fileAttrubeTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode 文件
			{
				pFile->destFileType = IS_GCODE_FILE;
				
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);//下一级目录名称

			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FIRST_ICON - PRINT_FILE_ICON_OFFSET]);
		 		
		PRINTUI_DEBUG("->1 page \r\n");			
				break;
				
			case IS_PRINT_SECOND_ICON://第二图标
			if(actualItem < 2) return rtv;
			pFile->selectedButtonID = IS_PRINT_SECOND_ICON;
			if(fileAttrubeTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode 文件
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);//下一级目录名称
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_SECOND_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->2 page \r\n");			
				break;
				
			case IS_PRINT_THIRD_ICON://第三图标
			if(actualItem < 3) return rtv;
			pFile->selectedButtonID = IS_PRINT_THIRD_ICON;
			if(fileAttrubeTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode 文件
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);//下一级目录名称
			
			}
			
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_THIRD_ICON - PRINT_FILE_ICON_OFFSET]);			
			PRINTUI_DEBUG("->3 page \r\n");			
				break;
				
			case IS_PRINT_FORTH_ICON://第四图标
			if(actualItem < 4) return rtv;
			pFile->selectedButtonID = IS_PRINT_FORTH_ICON;
			//判断文件属性
			if(fileAttrubeTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode 文件
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG("selected fn:%s\r\n",fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);//下一级目录名称
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FORTH_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->4 page \r\n");			
				break;
				
			case IS_PRINT_FIFTH_ICON://第五图标
			if(actualItem < 5) return rtv;//判断本次盘寻当前目录的文件/文件夹个数是否小于5个
			pFile->selectedButtonID = IS_PRINT_FIFTH_ICON;
			//判断文件属性
			if(fileAttrubeTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_GCODE_FILE)//gcode 文件
			{
				pFile->destFileType = IS_GCODE_FILE;
				PRINTUI_DEBUG(" selected fn:%s\r\n",fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);
				rtv = 2;
			}
			else if(fileAttrubeTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET] == IS_FOLDER)
			{
				pFile->destFileType = IS_FOLDER;
				isRootChanged = goto_next_fileRoot(pFile->path,fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);//下一级目录名称
			}
			strcpy((char *)pFile->destFileName,(const char *)fileNameTab[IS_PRINT_FIFTH_ICON - PRINT_FILE_ICON_OFFSET]);	
			PRINTUI_DEBUG("->5 page \r\n");			
				break;				
				
			default:
				break;
		}
		if(isRootChanged == 0)//判断是否成功进入下一级目录
		{
			PRINTUI_DEBUG("next root OK\r\n");
			rtv = 1;
			curPageCnt = 1;//复位页面计数器
			pFile->fileGuiAttr->pageCnt = curPageCnt;//清零页面计数器
			pFile->readItemsOffSet = 0;//读文件偏移清零
			pFile->fileDepth++;//文件嵌套深度增加，在根目录基础上
			redraw_fileSystemUI_flag = IS_TRUE;//续重新绘画文件系统界面
		   get_current_root_page_items(pFile);//获得当前目录下总的文件/文件夹数目,当当前目录变化时，需要调用
			scan_root_file(pFile);//更新文件/文件名列表、文件属性等
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
        if( pos >= length)//文件取完后，标记停止
        {
            card.FILE_STATE = PS_PRINT_OVER_NOR;

        }
        progressbar_setpos(prgb);
    }


}

void printui_init(void)
{
	if(hasGotoConfirmPrintFileScreen == 0)//未进入gcode确认界面，点击取消后，确保能进入上一层目录界面
	{
		init_file_page_ctrl_block((u8 *)PRINT_FILE_ROOT);
		get_current_root_page_items(curFilePageCtrlBlock);//获得当前目录下总的文件/文件夹数目,当当前目录变化时，需要调用
		scan_root_file(curFilePageCtrlBlock);//更新文件/文件名列表、文件属性等
	}
	
}

/*
*********************************************************************************************************
*	函 数 名: print_screen
*	功能说明: 打印界面
*	形    参: 无
*	返 回 值: 无
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

	
	res = exacute_button_even(curFilePageCtrlBlock);//包含显示程序

	if(res == 0)//退出文件选择界面
	{
		free_flag = IS_TRUE;
		hasGotoConfirmPrintFileScreen = 0;
		currentMenu = main_screen;
	}
	else if(res == 2)//选中gcode文件
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
		
		free_file_page_ctrl_block(curFilePageCtrlBlock);//释放文件控制块内存
		PRINTUI_DEBUG("free printUIdev \r\n");
	}
	

}

//在gcode文件选择过程中，SD卡拔出后提示信息
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
	 Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor1[LANGUAGE],font,1);//显示提示信息
	 Show_Str(xStart,yStart +2* font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor2[LANGUAGE],font,1);//显示提示信息
	 Show_Str(xStart,yStart + 3*font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor3[LANGUAGE],font,1);//显示提示信息
	 font = 16;
		//为主界面控制器申请内存
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

		//未发生断电续打，gcode文件尚未选好
		if( (sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_SELECT_FILE && eeprom_is_power_off_happen() != PW_OFF_HAPPEN) || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_CONFIRM_FILE) 
		{
			bsp_DelayMS(2000);

				currentMenu = main_screen;
				sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//复位标志
				redraw_screen_flag = IS_TRUE;
				myfree(SRAMIN,sdCardDev->icon);
				myfree(SRAMIN,sdCardDev);
				POINT_COLOR = BLUE;
				hasGotoConfirmPrintFileScreen = 0;
				free_file_page_ctrl_block(curFilePageCtrlBlock);//释放文件控制块内存
				PRINTUI_DEBUG("free  curFilePageCtrlBlock\r\n");
				PRINTUI_DEBUG("free sdCardDev \r\n");	
				return;
		
		}
		
	
		j = SD_UNPLUG_UI_STOP_BT_ID;
		display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);			
			
		sdCardIconDispFlag = 0;
		
	}


	//显示图标
	if(SD_CD == 1 || (SD_CD == 0 && isPrintFileExist == FALSE))//SD卡未插入 或者 SD卡已插入但是打印文件缺失
	{
		sdCardIconDispFlag++;
		if(sdCardIconDispFlag  == 1)
		{
			POINT_COLOR = RED;
			 font = 24;
			 gui_fill_rectangle_DMA(xStart,yStart + font,lcddev.width,3*font,BLACK);//清屏
			 Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor1[LANGUAGE],font,1);//显示提示信息
			 Show_Str(xStart,yStart + 2 *font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor2[LANGUAGE],font,1);//显示提示信息
			Show_Str(xStart,yStart + 3* font,lcddev.width,font,(u8 *)sdCardUnplug_remind_infor3[LANGUAGE],font,1);//显示提示信息
			
			j = SD_UNPLUG_UI_BACK_BT_ID;
			gui_fill_rectangle_DMA(sdCardDev->icon[j].x, sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height,BLACK);
			
			j = SD_UNPLUG_UI_STOP_BT_ID;
			display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);		
			
		}
		sdCardIconDispFlag = 2;
	}
	else if(SD_CD == 0 && isPrintFileExist == TRUE)//提示插入信息
	{
		sdCardIconDispFlag++;
		if(sdCardIconDispFlag  == 3)
		{
			POINT_COLOR = BLUE;
			 font = 24;
			 gui_fill_rectangle_DMA(xStart,yStart + font,lcddev.width,3*font,BLACK);//清屏
			Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)sdCardplug_remind_infor1[LANGUAGE],font,1);//显示提示信息
			j = SD_UNPLUG_UI_STOP_BT_ID;
			gui_fill_rectangle_DMA(sdCardDev->icon[j].x, sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height,BLACK);
			j = SD_UNPLUG_UI_BACK_BT_ID;
			display_bmp_bin_to_LCD((u8*)sdCardDev->icon[j].path, sdCardDev->icon[j].x , sdCardDev->icon[j].y, sdCardDev->icon[j].width, sdCardDev->icon[j].height, sdCardDev->icon[j].color);		
		}
		sdCardIconDispFlag = 0;
	}		
//////////////////////////////////////////////////////////////////////////////////

	SELECTED_BUTTON = gui_touch_chk(sdCardDev);	

//执行对应操作	
//	isOver = is_print_over();
//	if(isOver == 0)
//	{
//		bsp_DelayMS(2000);
//		free_flag = IS_TRUE;
//		currentMenu = main_screen;
//		PRINTUI_DEBUG("print is over,back mainUI\r\n");	
//	}
	 if(SD_CD == 1 || (SD_CD == 0 && isPrintFileExist == FALSE))//SD 卡拔出,去掉返回图标，显示停止图标
	{
		
		if(sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_FILE || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_OPTION || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_MORE)
		{
			if(SELECTED_BUTTON == SD_UNPLUG_UI_STOP_BT_ID)//停止当前打印	
			{		
				currentMenu = stop_screen;
				free_flag = IS_TRUE;
				
				PRINTUI_DEBUG("stop\r\n");	
			}			
		}
		else//还未选好gcode文件,返回到主界面
		{
			bsp_DelayMS(3000);
			free_flag = IS_TRUE;
			currentMenu = main_screen;
			PRINTUI_DEBUG("delay back main\r\n");	
		}
	
	}
	else if(SD_CD == 0 && isPrintFileExist == TRUE) //SD卡插入,去掉停止图标，显示返回图标
	{
		
		if(SELECTED_BUTTON == SD_UNPLUG_UI_BACK_BT_ID)	//返回打印界面
		{
						
			if( sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_SELECT_FILE && eeprom_is_power_off_happen() == PW_OFF_HAPPEN  ) { currentMenu = start_print_screen;}
			
			else if( sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_FILE || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_OPTION || sdCardUnplug.SD_CARD_UNPLUG_UI == UNPLUG_IN_PRINT_MORE)  currentMenu = gcode_print_option_screen;
			
			else //在选择文件界面/确认gcode选择界面
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
		sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//复位标志
		hasGotoConfirmPrintFileScreen = 0;
	
		PRINTUI_DEBUG("free sdCardDev \r\n");
	}
	
}






//打印文件确认
 void print_confirm_screen(void)
{
    uint8_t num = 2;//按键数
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
	pathName = get_current_root_file_name(pFile->path);//获得本目录名字，显示结束后，释放内存

	fileType = pFile->destFileType;

	LCD_Clear_DMA(BLACK);//清屏
	//显示标题栏
		offSet = 2;
		
		length = strlen((char *)print_confirm_title[LANGUAGE]);
		Show_Str(xStart,yStart,lcddev.width,font,(u8 *)print_confirm_title[LANGUAGE],font,1);//显示标题栏信息
		xStart += offSet;
		LCD_ShowString(xStart + length*(font/2),yStart ,lcddev.width,font,font,(u8 *)PRINT_FILE_ROOT);//显示"0:"
		
		if(fileDepth == 1)//第一层嵌套
		{

			LCD_ShowString(xStart + length*(font/2) + 2*(font/2),yStart ,lcddev.width,font,font,(u8 *)"/");	
			if(pathName != NULL)
			Show_Str(xStart + length*(font/2)  + 2*(font/2) + font/2 ,yStart,lcddev.width,font,(u8 *)pathName,font,1);
		}
		if(fileDepth > 1 )//确认是文件夹且路径深度大于1
		{

			LCD_ShowString(xStart + length*(font/2) + 2*(font/2) ,yStart,lcddev.width,font,font,(u8 *)"/.../");	
			if(pathName != NULL)
			Show_Str(xStart + length*(font/2) + 2*(font/2) + 5*(font/2),yStart,lcddev.width,font,(u8 *)pathName,font,1);
		}
		free_current_root_file_name_memory(pathName);
	
		xStart = 2;
	yStart += 2*font;
	gui_show_strmid(xStart,yStart,lcddev.width,font,POINT_COLOR,font,(u8 *)print_confirm_infor[LANGUAGE],1);//显示选择提示信息
	
	//显示打印文件名	
	offSet = 10;
	xStart = 40;
	yStart += 2*font;
	setLength = 30;//必须是2的整倍数，显示汉字要2个字节
		if(fileType == IS_GCODE_FILE)
		{	
			length = strlen((char *)pFile->destFileName);
			if(length >= setLength)//是否分段显示gcode文件名
			{
				fileName = (u8*)gui_memin_malloc(setLength + 1);	//分配内存
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
			Show_Str(xStart,yStart, lcddev.width, font,  (u8*)pFile->destFileName, font, 0); //显示gcode文件
		}
		
		
		
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
            PRINTUI_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
            return;
        }
		

    }

//按键检测
    selx = screen_key_chk(screen_key_group, &in_obj, num);
    if(selx & BUTTON_ACTION)
    {
        switch(selx & BUTTON_SELECTION_MASK)
        {
        case YES://确定打印后，获取打印文件名，释放pFile结构体内存
           
            free_flag = 1;
			printFileName = pickup_print_file_name(pFile->path,pFile->destFileName);//合成打印文件名
			PRINTUI_DEBUG("file:%s\r\n",printFileName);
			
			
			selx = 1;
			while(selx)
			{
				selx = f_open(&card.fgcode, (const TCHAR*)printFileName, FA_READ);//打开gcode文件
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
				free_file_page_ctrl_block(pFile);//清文件页面控制块内存
				isPrintFileExist = TRUE;
				file_pos = 0;//文件指引清零
				fil_size = f_size(&card.fgcode);//记录文件大小
				card.FILE_STATE = PS_PIRNT;//正在打印
				powerDownPrintTime = 0;
				starttime = millis();//开始计时
				card_startFileprint();
				feedmultiply=100;//设置为默认速度
			}
			else//文件打开失败，返回上一层目录
			{
				
				free_printFileName(printFileName);//清文件名内存
				isPrintFileExist = FALSE;
				hasGotoConfirmPrintFileScreen = 1;
				 currentMenu = print_screen;
				 redraw_fileSystemUI_flag = IS_TRUE;
			}


			
            PRINTUI_DEBUG("YES\r\n");
            break;
        case NO://取消--返回选择文件界面，且目录嵌套为进入该界面目录
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
	
    if(free_flag == 1) //释放内存
    {
        redraw_screen_flag = IS_TRUE;
        for(i = 0; i < num; i++)
        {
            btn_delete(screen_key_group[i]);
        }
        gui_memin_free(screen_key_group);

    }
	


	
}

/**********************************开始打印gcode界面**************************************/
void start_print_screen(void)
{
	u8  SELECTED_BUTTON = 0XFF;
	u8 printOver = 1;

	  _GUI_TURE_OR_FALSE free_flag = IS_FALSE;
	  
	if( redraw_screen_flag == IS_TRUE)
	{
		redraw_screen_flag = IS_FALSE;
		
		start_print_screen_init();
		bsp_ClearKey();//断料IO检测缓存清零
		
	}
	if(card.FILE_STATE != PS_PRINT_OVER_NOR)//未正常打印完成，则检查是否正常打印完成，避免重复更新option/back图标
	{
		printOver = is_print_over();//判断是否正常打印结束
	}
	
	if(printOver == 0 || optionBackBtSet == 1)
	{
	//根据打印完成状态，更新最后一个图标为返回控件
		gui_fill_rectangle_DMA(startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].x, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].y, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].width, startPintdev->icon[GCODE_PRINT_OPTION_BUTTON].height,BLACK);
		display_bmp_bin_to_LCD((u8*)startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].path, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].x , startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].y, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].width, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].height, startPintdev->icon[GCODE_PRINT_BACK_ICON_ID].color);
		pause_start_cnt = 0;
		isOption2moreUiSelected = NO;
		optionBackBtSet = 0;
//		free_printFileName(printFileName);//正常打印结束后，清打印文件名内存
	}

	SELECTED_BUTTON = gui_touch_chk(startPintdev);
	
	if(SELECTED_BUTTON == GCODE_PRINT_OPTION_BUTTON)	//最后一行最后一个图标被按下
	{
		if(card.FILE_STATE == PS_PRINT_OVER_NOR )//返回控件,正常打印结束
		{
			free_flag = IS_TRUE;
			currentMenu = main_screen;
			
			free_printFileName(printFileName);
			
			PRINTUI_DEBUG("back selected \r\n");
	
		}
		else//option 控件
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

//判断是否打印完成
//返回值0--打印完成
u8 is_print_over(void)
{
	u8 rtv = 0xff;
	if( file_pos >= fil_size )//gcode文件读取完毕，但是命令缓存还未执行完毕
	{
		
		card.FILE_STATE = PS_PRINT_OVER_NOR;
		rtv = 0;
		feedmultiply=100;//设置为默认速度
	}
	return rtv;
}
void test_prag_bar(void)
{
	 static uint32_t print_update_time = 0;
	 if( print_update_time <= millis())
	 {
		print_update_time = millis() + 1000;
	        //显示打印进度条
        if(prgb)
        {
 
            prgb->curpos++;
			if(prgb->curpos >=  prgb->totallen) prgb->curpos = 0;
             progressbar_setpos(prgb);//画按钮
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
	setLength = 30;//必须是2的整倍数，显示汉字要2个字节
	width = lcddev.width;
	hight = font;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	
	titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//获得标题栏提示信息长度
	Show_Str(xStart,yStart,width,hight,(u8 *)gcode_print_title[LANGUAGE],font,1);	
	fileName = get_current_root_file_name(printFileName);//获得gcode文件名


			length = strlen((char *)fileName);
			if(length >= setLength)//是否分段显示gcode文件名
			{
				dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//分配内存
				if(dispFileName == NULL)
				{	
					PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
					
				}
				else
				{
					
					strncpy((char *)dispFileName,(const char *)fileName,setLength);
					dispFileName[setLength] = '\0';
					
					Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)dispFileName, font, 1); 
					gui_memin_free(dispFileName);//释放内存
					
					xStart = 2;
					dispFileName = fileName + setLength;
					Show_Str(xStart,yStart + font, width,hight,(u8*)dispFileName, font, 1); 
				}						
			}
			else 
			Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)fileName, font, 1); //显示gcode文件

	free_current_root_file_name_memory(fileName);//释放内存
	
	
	//为主界面控制器申请内存
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
	prgb = progressbar_creat(xStart, yStart, width, hight, 0x61); //创建进度条
	if(prgb == NULL) 
	{
	
		PRINTUI_DEBUG("file:%s line:%d  memory error!", __FILE__, __LINE__);   //创建失败.
		return;
	}
				//初始显示时间设置
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
	
		for(j=0; j < startPintdev->totalIcon - 1; j++)//画4个图标
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
//开始打印界面显示
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

	
		 //清屏
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		gui_fill_rectangle_DMA(xStart, yStart,width -PICTURE_X_SIZE - 4 - f_size,hight,BACK_COLOR);
	//显示热床温度
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
    LCD_ShowString(xStart + 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num(xStart + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, bedDestTemp, 1); 
	
	offSet = 1 + PICTURE_X_SIZE ;
	//显示风扇转速
	gui_show_num(xStart + offSet + 2 * (f_size / 2), yStart, 3, POINT_COLOR, f_size, fanSpeed, 1); 
//	LCD_ShowString(xStart + offSet+ 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"%");
	
	offSet += 1 + PICTURE_X_SIZE;
	//显示挤出头温度
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

	
		 //清屏
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		gui_fill_rectangle_DMA(xStart, yStart,8*(f_size/2),hight,BACK_COLOR);
		gui_fill_rectangle_DMA(xStart + 160 , yStart,8*(f_size/2),hight,BACK_COLOR);
	//显示热床温度
	POINT_COLOR = GREEN;
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
	POINT_COLOR = WHITE;
    LCD_ShowString(xStart + 3 * (f_size / 2), yStart , width, hight, f_size, (u8*)"/"); 
    gui_show_num(xStart + 3 * (f_size / 2) + 1 * (f_size / 2),  yStart, 3, POINT_COLOR, f_size, bedDestTemp, 1); 
	POINT_COLOR = YELLOW;


	offSet =  160 + 43 - 3;
	//显示挤出头温度
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
//option界面显示参数信息
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

	
		 //清屏
	 if( printParaStateTime <= millis())
	 {
		printParaStateTime = millis() + 500;
		length1 = strlen((char *)bedInfor[LANGUAGE]);
		length2 = strlen((char *)extrudInfor[LANGUAGE]) ;
		
		offSet = (length1 )* (f_size / 2) ;
		width = 3*(f_size / 2);
		yStart += 10;
		gui_fill_rectangle_DMA(xStart + offSet, yStart,width,hight ,BACK_COLOR);//清热床显示
		
		offSet = (length1 + 5 + length2)* (f_size / 2) ;
		gui_fill_rectangle_DMA(xStart + offSet, yStart,width,hight,BACK_COLOR);//清挤出显示
		
	//显示热床温度
	width = (length1 )* (f_size / 2);
	
	Show_Str(xStart , yStart , width, hight, (u8*)bedInfor[LANGUAGE],f_size,1); 
	xStart += width;
	gui_show_num(xStart , yStart, 3, POINT_COLOR, f_size, bedCurTemp, 1); 
   
	
	offSet = 5* (f_size / 2) ;
	
	//显示挤出头温度
	width = (length2 )* (f_size / 2);
	xStart += offSet;
	Show_Str(xStart , yStart , width, hight, (u8*)extrudInfor[LANGUAGE],f_size, 1);
	xStart += width;
	gui_show_num(xStart, yStart, 3, POINT_COLOR, f_size, etr0CurTemp, 1); 
	
	

	//显示风扇转速
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
/*************************************option 控件界面*******************************************/
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

		case GCODE_OPTION_STOP_BUTTON_ID://停止打印
			saveButton = GCODE_OPTION_STOP_BUTTON_ID;
			currentMenu = stop_screen;
			free_flag = IS_TRUE;

				PRINTUI_DEBUG("stop \r\n");
			break;
			
		case GCODE_OPTION_PAUSE_BUTTON_ID:


			//转换图标->resume
			pause_start_cnt++;
			if(pause_start_cnt % 2 == 1)//控制打印机暂停，隐藏返回按键
			{
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].height,BLACK);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_START_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].color);
				IS_PAUSE_BT = NO;
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].height,BLACK);

				if(card.FILE_STATE == PS_PIRNT)//只有处于打印过程中的暂停才有意义
					{
					card.FILE_STATE = PS_PAUSE;//暂停
					
					card_pauseSDPrint();
					}
				
			}
			else if(pause_start_cnt % 2 == 0)//恢复打印,显示返回按键
			{
				pause_start_cnt = 0;
				gui_fill_rectangle_DMA(optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height,BLACK);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_PAUSE_BUTTON_ID].color);
				display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_BACK_BUTTON_ID].color);

				IS_PAUSE_BT = YES;
				if(card.FILE_STATE == PS_PAUSE)//只有处于暂停状态下，恢复打印才有意义
				{
					card.FILE_STATE = PS_PIRNT;//打印
				}
				

				
			}
			PRINTUI_DEBUG("pause/start \r\n");	
			break;		
		case GCODE_OPTION_MORE_BUTTON_ID:
		
			free_flag = IS_TRUE;
			currentMenu = gcode_print_option_to_more_screen;
			isOption2moreUiSelected = YES;//标志进入more界面预热设置,打印完成复位
			PRINTUI_DEBUG("more \r\n");	
			break;		
		case GCODE_OPTION_BACK_BUTTON_ID:
		if(IS_PAUSE_BT == YES)//当打印机处于打印状态--显示暂停图标时，才检测返回按键事件
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
	
	isPrintOver = is_print_over();//判断是否正常打印完成
	if(isPrintOver == 0)
	{
		free_flag = IS_TRUE;
		PRINTUI_DEBUG("print over \r\n");	
		
		currentMenu = start_print_screen;
		optionBackBtSet = 1;//start_print_screen界面选择图标切换为 返回图标
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
	uint8_t num = 2;//按键数
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
		
		
		LCD_Clear_DMA(BLACK);//清屏
		Show_Str(xStart,yStart,lcddev.width,font,(u8 *)print_stop_infor[LANGUAGE],font,1);//显示标题栏信息
		
		POINT_COLOR = WHITE;
		yStart += 2*font;
		setLength = 30;//必须是2的整倍数，显示汉字要2个字节
		length = strlen((char *)printFileName);
		if(length >= setLength)//是否分段显示gcode文件名
		{
			fileName = (u8*)gui_memin_malloc(setLength + 1);	//分配内存
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
		Show_Str(xStart,yStart, lcddev.width, font,  (u8*)printFileName, font, 0); //显示gcode文件

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
				PRINTUI_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
				return;
			}
	}

	//按键检测
    selx = screen_key_chk(screen_key_group, &in_obj, num);
    if(selx & BUTTON_ACTION)
    {
        switch(selx & BUTTON_SELECTION_MASK)
        {
        case YES://停止打印
            currentMenu = main_screen;
            free_flag = 1;

            PRINTUI_DEBUG("YES\r\n");
			
			card.FILE_STATE = PS_STOP;
            quickStop();//停止规划动作
            card_closefile();//关闭文件
//清命令接收缓存标志
			clear_cmd_buf();

            stoptime = millis() - starttime; //非正常打印结束，记录此时结束时间
           
            line_no = 0;//gcode 命令行号清零

			clear_filaTime_s();
            force_sdCard_pause();//sdCard解析命令停止
            pause_start_cnt = 0;
			filaState.filaPause = FALSE;//复位标志
			isPauseStart = 0;//清零暂停/打印执行标志
			paueStartFlag = 0;
			IS_PAUSE_BT = YES;//复位gcode_print_screen 返回按键检测标志
            //温度置零
            //x、y轴回原点，z轴下移到最大位置
            menu_action_gcode("M104 S0.0");//设置热挤出头温度
            menu_action_gcode("M140 S0.0");//设置热床温度		
			setTargetHotend(0.0,0);		
			setTargetBed(0.0);			
			menu_action_gcode("G1 X0");//X轴回原点
            menu_action_gcode("M84");//电机断电
	
			free_printFileName(printFileName);
			sdCardUnplug.SD_CARD_UNPLUG_UI = UNPLUG_IN_SELECT_FILE;//复位标志
			feedmultiply=100;//设置为默认速度
			if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)//断电续打还在准备过程中，停止打印
			{
				eeprom_clear_power_off_status();	
			}
            break;
        case NO://取消--返回打印界面
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
	

	
    if(free_flag == 1) //释放内存
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
	setLength = 30;//必须是2的整倍数，显示汉字要2个字节
	width = lcddev.width;
	hight = font;
	 BACK_COLOR = BLACK;
	POINT_COLOR = WHITE;
	
	titleLength = strlen((char *)gcode_print_title[LANGUAGE]);//获得标题栏提示信息长度
	Show_Str(xStart,yStart,width,hight,(u8 *)gcode_print_title[LANGUAGE],font,1);	
	fileName = get_current_root_file_name(printFileName);//获得gcode文件名


			length = strlen((char *)fileName);
			if(length >= setLength)//是否分段显示gcode文件名
			{
				dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//分配内存
				if(dispFileName == NULL)
				{	
					PRINTUI_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
					
				}
				else
				{
					
					strncpy((char *)dispFileName,(const char *)fileName,setLength);
					dispFileName[setLength] = '\0';
					
					Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)dispFileName, font, 1); 
					gui_memin_free(dispFileName);//释放内存
					
					xStart = 2;
					dispFileName = fileName + setLength;
					Show_Str(xStart,yStart + font, width,hight,(u8*)dispFileName, font, 1); 
				}						
			}
			else 
			Show_Str(xStart + titleLength*(font/2),yStart,width,hight,(u8*)fileName, font, 1); //显示gcode文件

	free_current_root_file_name_memory(fileName);//释放内存
	
	

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
	prgb = progressbar_creat(xStart, yStart, width, hight, 0x61); //创建进度条
	if(prgb == NULL) 
	{
	
		PRINTUI_DEBUG("file:%s line:%d  memory error!", __FILE__, __LINE__);   //创建失败.
		return;
	}
	
	xStart -= 4;				//初始显示时间设置
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
				if(pause_start_cnt % 2 == 1 || filaState.filaPause == TRUE)//更换暂停/开始 图标为 开始图标，SD卡拔插初始化完成
				{
					if(filaState.filaPause == TRUE) pause_start_cnt = 1;
					display_bmp_bin_to_LCD((u8*)optiondev->icon[GCODE_OPTION_START_BUTTON_ID].path, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].x , optiondev->icon[GCODE_OPTION_START_BUTTON_ID].y, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].width, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].height, optiondev->icon[GCODE_OPTION_START_BUTTON_ID].color);
					
				}
				else 
					display_bmp_bin_to_LCD((u8*)optiondev->icon[j].path, optiondev->icon[j].x , optiondev->icon[j].y, optiondev->icon[j].width, optiondev->icon[j].height, optiondev->icon[j].color);		
			}
			
			else if(j == GCODE_OPTION_BACK_BUTTON_ID)
			{
				if(pause_start_cnt % 2 == 0)//显示暂停图标
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
/**************************主页的第一排图标******************************/	
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
		if(isPause == 1)//打印机暂停
		{
			currentMenu = change_fila_screen;//换料
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("change fila ui\r\n");				
		}
				
			break;			


		case GCODE_OPTION_TO_MORE_FILA_BT_ID:
		if(isPause == 1)//打印机暂停
		{
			currentMenu = fila_screen;//打印过程中进退料
			free_flag = IS_TRUE;
			PRINTUI_DEBUG("print fila ui\r\n");			
		}
		break;
			
		case GCODE_OPTION_TO_MORE_SPEED_BT_ID://打印速度设置
			
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



	check_is_pause_exacute();//检查打印头是否执行靠边停止
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

	//为主界面控制器申请内存
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
		dispIconNum = 3;//只显示预热 + 风扇图标 + 打印速度设置
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


	if(card.FILE_STATE == PS_PAUSE && filaState.filaPause == FALSE )//暂停打印,未发生断料暂停,
	{
		isPauseStart ++;
		st_synchronize_no_lcd();
		if(isPauseStart == 1)//保证只保存一次最近暂停的位置
		{
			
				//等待停止
			if((current_position[0] != destination[0]) ||
					(current_position[1] != destination[1]) ||
					(current_position[2] != destination[2]) ||
					(current_position[3] != destination[3]) ||
					(block_buffer_head != block_buffer_tail) ||  buflen )//命令缓冲为空为止
			{
				isPauseStart = 0;
				return;
			}			
			  	//确认之前的所有gcode码执行完毕
				for( i = 0 ; i < NUM_AXIS; i++)
				{
					saved_position[i] = current_position[i];//保存当前位置
					temp_posistion[i] = current_position[i];
					PRINTUI_DEBUG("des1[%d] = %.7f ,cur1[%d]=%.7f\r\n",i,destination[i],i,current_position[i]);
				}
				 
				temp_posistion[X_AXIS] = 0.0f;//x 轴回原点，别的轴不动
				PRINT_STATE = HAS_EXACUTE_PAUSE;
				Cmd_MoveAxisCommand(temp_posistion); 
				
			
		}
		else isPauseStart = 2;
	}

    if(card.FILE_STATE == PS_PIRNT && (isPauseStart >= 1 || filaState.filaPause == TRUE))//恢复打印
    {
		if (isPauseStart >= 1 ) isPauseStart = 0 ;
        
		else if(filaState.filaPause == TRUE) 
			{
				filaState.filaPause = FALSE;//复位标志

			}
			
		
        Cmd_MoveAxisCommand(saved_position);
		PRINT_STATE = NORMAL_PLAY;//确保打印头回到暂停位置后，再更新标志，以防在此过程中断电
        PRINTUI_DEBUG("-------------------------------------- resume to print\r\n");
        card_startFileprint();//打印
	
    }

}
void fila_run_out_pause(void)
{
    u8 i;

	if(card.FILE_STATE == PS_PIRNT && filaState.filaPause == FALSE)//暂停打印,确保断料后，执行完进料后，才开始下一轮检测
	{
		
		
		card_pauseSDPrint();//停止SD卡解码
		
		st_synchronize_no_lcd();		
				//等待停止
			if((current_position[0] != destination[0]) ||
					(current_position[1] != destination[1]) ||
					(current_position[2] != destination[2]) ||
					(current_position[3] != destination[3]) ||
					(block_buffer_head != block_buffer_tail))
			{
				
				return;
			}
			
					
			  	//确认之前的所有gcode码执行完毕
				for( i = 0 ; i < NUM_AXIS; i++)
				{
					saved_position[i] = current_position[i];//保存当前位置
					temp_posistion[i] = current_position[i];
					printf("des1[%d] = %.7f ,cur1[%d]=%.7f\r\n",i,destination[i],i,current_position[i]);
				}
				temp_posistion[X_AXIS] = 0.0f;//x 轴回原点，别的轴不动

				PRINT_STATE = HAS_EXACUTE_PAUSE;
				Cmd_MoveAxisCommand(temp_posistion); 
				card.FILE_STATE = PS_PAUSE;//暂停
				filaState.filaPause = TRUE;//标志发生断料暂停
	}	
}







/*************************************/
