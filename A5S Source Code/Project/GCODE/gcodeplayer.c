#include "gcodeplayer.h"
#include "Dlion.h"
#include "sdio_sdcard.h"
#include "bsp_usart.h"
#include "exfuns.h"
#include "malloc.h"
#include "string.h"
#include "language.h"
#include "temperature.h"
#include "stepper.h"
#include "bsp.h"









#ifdef SDSUPPORT
/**********************调试宏定义**************************/

#define SD_DEBUG_EN

#ifdef SD_DEBUG_EN

#define SD_DEBUG	DBG

#else
#define SD_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



/**********************全局变量**************************/
//用于记录当前打印的gcode文件的文件行号、文件读取位置和文件大小信息
uint32_t fil_size;//文件大小
volatile uint32_t  line_no;//gcode码行号,断电续打用到
uint32_t file_pos;//读gcode的文件指针
uint32_t user_line_no;//用户发送的gcode命令
_PRINT_STATE print_state = PS_NO;
_sdCardUnplugCheck_s sdCardUnplug;
uint8_t isPrintFileExist = TRUE;//默认要打印的文件存在SD卡中，用来标志SD卡拔插时卡内没有正在打印的文件
/************************************************/





/*
*********************************************************************************************************
*	函 数 名: gcode_ls
*	功能说明: gcode文件列表
*	形    参: path--指定路径
*	返回值：无
*********************************************************************************************************
*/
void gcode_ls(u8 * path)
{
    u8 res;
    u8 *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    fileinfo.lfsize = _MAX_LFN * 2 + 1;
    fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);
#endif

    res = f_opendir(&dir, (const TCHAR*)path);
    if (res == FR_OK)
    {

        while(1)
        {
            res = f_readdir(&dir, &fileinfo);
            if (res != FR_OK || fileinfo.fname[0] == 0) break;
            //if (fileinfo.fname[0] == '.') continue;
#if _USE_LFN
            fn = (u8*)( *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
#else
            fn = (u8*)( fileinfo.fname);
#endif
            //	SD_DEBUG("%s/", path);
            res = f_typetell(fn);
            if((res & 0XF0) == 0X60)
            {   SD_DEBUG("%s\n",  fn);
            }
        }
    }
    myfree(SRAMIN, fileinfo.lfname);
    //  return res;
}

/*
*********************************************************************************************************
*	函 数 名: card_ls
*	功能说明: 打印SDcard中的gcode文件列表
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_ls(void)
{
    if(card.lsAction == LS_Count)
// nrFiles=0;
        card.lsAction = LS_SerialPrint;
    gcode_ls("0:/GCODE");
}

/*
*********************************************************************************************************
*	函 数 名: card_initsd
*	功能说明: SDcard初始化
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_initsd(void)
{   card.cardOK = FALSE;
    SERIAL_ECHO_START;
    if(SD_Init())
    {   SD_DEBUG(MSG_SD_INIT_FAIL);
    }
    else
    {   card.cardOK = TRUE;
        SD_DEBUG(MSG_SD_CARD_OK);
    }
}

/*
*********************************************************************************************************
*	函 数 名: card_release
*	功能说明: 释放SDcard
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_release(void)
{   card.sdprinting = FALSE;
    card.cardOK = FALSE;
}

/*
*********************************************************************************************************
*	函 数 名: card_openFile
*	功能说明: 在SDcard中打开指定的gcode文件
*	形    参: fname--文件名
read--读写控制标志
@ 0--以写的方式打开文件
@ 1--以读的方式打开文件
*	返回值：无
*********************************************************************************************************
*/
void card_openFile(char* fname, u8 read)
{
    u8 res;
    u8 *pname;

    pname = mymalloc(SRAMIN, _MAX_LFN * 2 + 1);
    if(!card.cardOK)
        return;
// file.close();
// SD_DEBUG("--");
    card.sdprinting = FALSE;

    if(read)
    {
//        strcpy((char*)pname, "0:/GCODE/");
//        strcat((char*)pname, (const char*)fname);
        res = f_open(&card.fgcode, (const TCHAR*)pname, FA_READ);
        if (res == FR_OK)
        {
            SD_DEBUG(MSG_SD_FILE_OPENED);
            SD_DEBUG("%s\r\n", fname);
            SD_DEBUG(MSG_SD_SIZE);
            SD_DEBUG("%ld", f_size(&card.fgcode));
            SD_DEBUG("\n");
//      card.sdpos = 0;
            SD_DEBUG(MSG_SD_FILE_SELECTED);
            //lcd_setstatus(fname);
        }
        else
        {
            SD_DEBUG(MSG_SD_OPEN_FILE_FAIL);
            SD_DEBUG("%s\r\n", fname);
            SD_DEBUG(".");
        }
        SD_DEBUG("\n");
    }
    else
    {   res = f_open(&card.fgcode, (const TCHAR*)fname, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
            SD_DEBUG(MSG_SD_OPEN_FILE_FAIL);
            SD_DEBUG("%s\r\n", fname);
            SD_DEBUG(".");
        }
        else
        {
            card.saving = TRUE;
            SD_DEBUG(MSG_SD_WRITE_TO_FILE);
            SD_DEBUG("%s\r\n", fname);
            // lcd_setstatus(fname);
        }
        SD_DEBUG("\n");
    }
    myfree(SRAMIN, pname);
    //myfree(SRAMIN,fgcode);
}

/*
*********************************************************************************************************
*	函 数 名: card_removeFile
*	功能说明: 删除SDcard中指定的gcode文件
*	形    参: fname--文件名
*	返回值：无
*********************************************************************************************************
*/
void card_removeFile(char* fname)
{   u8 res;
    res = f_unlink(fname);
    if (res == FR_OK)
    {
        SD_DEBUG("File deleted:");
        SD_DEBUG("%s\r\n", fname);
//      card.sdpos = 0;
    }
    else
    {
        SD_DEBUG("Deletion failed, File: ");
        SD_DEBUG("%s\r\n", fname);
        SD_DEBUG(".\n");
    }
}
/*
*********************************************************************************************************
*	函 数 名: card_startFileprint
*	功能说明: 开始SDcard打印gcode，置位标志后，SDcard中的gcode文件在get_command()中被读取到命令缓冲区中
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_startFileprint(void)
{
    if(card.cardOK)
    {
        card.sdprinting = TRUE;
		SD_DEBUG("start file print\r\n");
    }

}

u8 read_card_print_flag(void)
{
	u8 rtv;
	rtv = card.sdprinting ;
	return rtv;
}
/*
*********************************************************************************************************
*	函 数 名: card_pauseSDPrint
*	功能说明: SDcard暂停打印gcode，复位标志后，SDcard中的gcode文件在get_command()中不被读取到命令缓冲区中
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_pauseSDPrint(void)
{
    if(card.sdprinting)
    {
        card.sdprinting = FALSE;
    }
}

/*
*********************************************************************************************************
*	函 数 名: force_sdCard_pause
*	功能说明: 强制使SDcard暂停打印gcode，复位标志后，SDcard中的gcode文件在get_command()中不被读取到命令缓冲区中
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void force_sdCard_pause(void)
{
    card.sdprinting = FALSE;

}

/*
*********************************************************************************************************
*	函 数 名: force_sdCard_start
*	功能说明: 强制使SDcard打印gcode，置位标志后，SDcard中的gcode文件在get_command()中被读取到命令缓冲区中
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void force_sdCard_start(void)
{
    card.sdprinting = TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: card_setIndex
*	功能说明: 设置SDcard读/写gcode文件的文件指针位置
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_setIndex(long index)
{   //card.sdpos = index;
    f_lseek(&card.fgcode, index);
}


/*
*********************************************************************************************************
*	函 数 名: card_getStatus
*	功能说明: 获取SDcard状态
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_getStatus(void)
{   if(card.cardOK)
    {
        SD_DEBUG(MSG_SD_PRINTING_BYTE);
        SD_DEBUG("%d", f_tell(&card.fgcode));
        SD_DEBUG("/");
        SD_DEBUG("%d", f_size(&card.fgcode));
    }
    else {
        SD_DEBUG(MSG_SD_NOT_PRINTING);
    }
    SD_DEBUG("\n");
}

/*
*********************************************************************************************************
*	函 数 名: card_closefile
*	功能说明: 关闭SDcard读取的gcode文件
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_closefile(void)
{
    f_close(&card.fgcode);
    card.saving = FALSE;
    line_no = 0;
}

void card_write_command(char *buf)
{
}

/*
*********************************************************************************************************
*	函 数 名: card_checkautostart
*	功能说明: 检测SD卡是否在位
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_checkautostart()
{
    if((!card.cardOK) && (!SD_CD))
    {
        card_initsd();
				sdCardUnplug.unplugFlag = TRUE;//标志插入
        if(!card.cardOK) //fail
            return;
    }

}

/*
*********************************************************************************************************
*	函 数 名: card_printingHasFinished
*	功能说明: 从SDcard中读取gcode文件结束后执行收尾工作
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void card_printingHasFinished(void)
{
    st_synchronize();
    quickStop();
    card_closefile();
    starttime = 0;
    card.sdprinting = FALSE;
    if(SD_FINISHED_STEPPERRELEASE)
    {
        //  finishAndDisableSteppers();
        enquecommand((char *) PSTR(SD_FINISHED_RELEASECOMMAND));
    }
    autotempShutdown();
//	 SD_DEBUG("5.\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: card_eof
*	功能说明: 判断从SDcard中读取文件是否结束
*	形    参: 无
*	返回值：真或假
*********************************************************************************************************
*/
u8 card_eof(void)
{   return f_eof(&card.fgcode);
}

/*
*********************************************************************************************************
*	函 数 名: card_get
*	功能说明: 从SDcard中获取gcode文件的一个字节
*	形    参: 无
*	返回值：被读取的字节
*********************************************************************************************************
*/
int16_t card_get(void)
{   //card.sdpos = f_tell(&card.fgcode);
	
    return  file_read();
}

/*
*********************************************************************************************************
*	函 数 名: file_read
*	功能说明: gcode文件每次被读取一个字节，用在get_command()中
*	形    参: 无
*	返回值：被读取的字节
*********************************************************************************************************
*/

int16_t file_read(void)
{   u8 buffer[128];
    u8 res;
    UINT br;
    res = f_read(&card.fgcode, buffer, 1, &br);
    if (res == FR_OK )
    {   // SD_DEBUG("%s",buffer);
		file_pos ++;//读文件指引加1	
        return *buffer;//
    }
	else
    {   
	SD_DEBUG("f_read() fail .. \r\n");
	return -1;
    }
}
//处理sdCard被意外拔插
void deal_with_unplug_sdCard(void)
{
	u8 isReadCard;
	u8 isFilaOut;
	isFilaOut = bsp_GetKeyState(FILA_IO_ID);
	//DBG("io:%d\r\n",isFilaOut);
	if((SD_CD && sdCardUnplug.unplugFlag != TRUE && isFilaOut == 0) || (stopDuringPrintFlag == TRUE && SD_CD == 0 && sdCardUnplug.unplugFlag != TRUE))//unplug,正常打印过程中
	{
		isReadCard = read_card_print_flag();
		if(isReadCard == TRUE || line_no >= 1)//在读卡过程中,确保打印机已经动作
		{
			card_pauseSDPrint();
			sdCardUnplug.saveLineNo = line_no;
			sdCardUnplug.saveFpos = file_pos;
			sdCardUnplug.unplugFlag = TRUE;
			f_close(&card.fgcode);//关闭文件

			DBG("-> ##unplug sd \r\n");
		}
	

	}
	if((isPrintFileExist == FALSE || SD_CD)&& eeprom_is_power_off_happen() == PW_OFF_HAPPEN && sdCardUnplug.unplugFlag != TRUE && isFilaOut == 0)//断电续打开机时SD卡未插入
	{
		sdCardUnplug.unplugFlag = TRUE;
		DBG("-> #@powerD sd lost \r\n");
	}
}	
//虽然有拔插卡出来程序，但是不建议无故拔插SD卡，否则会导致读卡异常，因为SD卡电源未切断
void deal_with_sdCard_plugAgain(void)
{


	u8 fmountF = 0xff;
	u8 fopenfileF = 0xff;
	u8 cnt = 0;
	u8 res = 1;
	if(sdCardUnplug.unplugFlag == TRUE && SD_CD == 0)//确保SD卡插入
	{
		
			
		DBG("->come in\r\n");
		fmountF = f_mount(NULL, "0:", 1); 		//卸载SD卡
		if(fmountF != FR_OK )	DBG("->## un mount Fat fail\r\n");
		bsp_DelayMS(5);
		
		fmountF = f_mount(fs[0], "0:", 1);
		if(fmountF != FR_OK )	DBG("->## mount Fat fail\r\n");
		
		if(card.FILE_STATE == PS_NO	|| card.FILE_STATE == PS_STOP	)//打印文件未选择完毕拔插卡
		{
			DBG("->printer is in pickup state\r\n");
			sdCardUnplug.unplugFlag = FALSE;
			return;
		}			
			cnt = 0;		
			while(fopenfileF)
			{
				if(printFileName != NULL)//文件名存在
				{
					fopenfileF = f_open(&card.fgcode, (const TCHAR*)printFileName, FA_READ |FA_OPEN_EXISTING);
					cnt++;
				}
				else {DBG("->## print file lost \r\n"); break;}
				
			
				if(fopenfileF != FR_OK && cnt >=3 )
				{
					DBG("->## print file lost \r\n");
					
					isPrintFileExist = FALSE;//不存在打印文件
					break;
				}
				bsp_DelayMS(5);
			}

			
			if(fopenfileF == FR_OK)//打开文件OK
			{
				if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)
				{
					reach_power_down_file_pos(dontExctpPdLineNum,bckGcodeHeadPos,&file_pos);
					line_no = powerDownLineNum;		
				}
				else
				{
					file_pos = sdCardUnplug.firstFilePos;
					card_setIndex(file_pos);	
					
					if(pause_start_cnt % 2 == 0)//暂停按键按下后，拔插SD卡不能立即打印
						card_startFileprint();					
				}
				fil_size = f_size(&card.fgcode);//记录文件大小
				//设置文件读指针
				
				sdCardUnplug.unplugFlag = FALSE;//挂载fats + 读取到打印文件 才认为该次拔出初始化完成
				isPrintFileExist = TRUE;
				
				DBG("-> plug again OK \r\n");			
			}		
			if(stopDuringPrintFlag == TRUE)
			{
				stopDuringPrintFlag = FALSE;//reset flag	
				DBG("->rese stopDuringPrintFlag\r\n");		
			}
	}

		

	  

}

/*
*********************************************************************************************************
*	函 数 名: get_precent_file_printed
*	功能说明: 获取gcode文件 打印百分比
*	形    参: 无
*	返回值：percents
*********************************************************************************************************
*/
uint8_t get_precent_file_printed(void)
{
    uint8_t percents = 0;
    if(line_no) {
        uint32_t size_unit = fil_size / 100;
        percents = file_pos / size_unit;
        SD_DEBUG("percents[%02d]\r\n", percents);
    }
    return percents;
}

/*
*********************************************************************************************************
*	函 数 名: get_size_file_printed
*	功能说明: 获取gcode文件 文件指针位置
*	形    参: 无
*	返回值：line_no
*********************************************************************************************************
*/
uint32_t get_size_file_printed(void)
{
    return file_pos;
}

/*
*********************************************************************************************************
*	函 数 名: get_line_no_file_printed
*	功能说明: 获取gcode文件已经打印到的行号
*	形    参: 无
*	返回值：line_no
*********************************************************************************************************
*/

uint32_t get_line_no_file_printed(void)
{
    return line_no;
}

/*
*********************************************************************************************************
*	函 数 名: get_size_of_file
*	功能说明: 获取文件的大小
*	形    参: 无
*	返回值：fil_size
*********************************************************************************************************
*/
uint32_t get_size_of_file(void)
{
    return fil_size;
}

/*
*********************************************************************************************************
*	函 数 名: menu_action_gcode
*	功能说明: 把用户命令送到命令缓冲区里面排列
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void menu_action_gcode(char* pgcode)
{
    enquecommand(pgcode);
}
/******************gcode 分散命令控制********************/

/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_allHome
*	功能说明: 用户命令x、y、z轴回原点
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void user_cmd_move_allHome(void)
{
    enable_x();
    enable_y();
    enable_z();
    enable_e0();
    menu_action_gcode(MOVE_GO_HOME);
}


/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_xHome
*	功能说明: 用户命令x轴回原点
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/

void user_cmd_move_xHome(void)
{
    enable_x();
    menu_action_gcode(MOVE_GO_HOME_X);
}

/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_yHome
*	功能说明: 用户命令y轴回原点
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/

void user_cmd_move_yHome(void)
{
    enable_y();
    menu_action_gcode(MOVE_GO_HOME_Y);
}

/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_zHome
*	功能说明: 用户命令z轴回原点
*	形    参: 无
*	返回值：无
*********************************************************************************************************
*/
void user_cmd_move_zHome(void)
{
    enable_z();
    menu_action_gcode(MOVE_GO_HOME_Z);
}
/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_x
*	功能说明: 用户移动x轴命令
*	形    参: dir--移动方向
@ POSITIVE_DIR--正方向
@ NEGTIVE_DIR--负方向


scale--每次移动范围
@ MM01--移动0.1mm
@ MM1--移动1mm
@ MM10--移动10mm
*	返 回 值: 无
*********************************************************************************************************
*/

void user_cmd_move_x(DIR_E dir, MOVE_SCALE scale)
{
    enable_x();
    menu_action_gcode(USING_RELATIVE_POSITION);
    switch (dir)
    {
    case POSITIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_X_INC_0_1MM_CMD);
            break;
        case MM1:
		
            menu_action_gcode(MOVE_X_INC_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_X_INC_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    case NEGTIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_X_SUB_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_X_SUB_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_X_SUB_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
    menu_action_gcode(USING_ABSOLUTE_POSITION);
}
/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_y
*	功能说明: 用户移动y轴命令
*	形    参: dir--移动方向
@ POSITIVE_DIR--正方向
@ NEGTIVE_DIR--负方向


scale--每次移动范围
@ MM01--移动0.1mm
@ MM1--移动1mm
@ MM10--移动10mm
*	返 回 值: 无
*********************************************************************************************************
*/

void user_cmd_move_y(DIR_E dir, MOVE_SCALE scale)
{
    enable_y();
    menu_action_gcode(USING_RELATIVE_POSITION);
    switch (dir)
    {
    case POSITIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_Y_INC_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_Y_INC_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_Y_INC_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    case NEGTIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_Y_SUB_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_Y_SUB_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_Y_SUB_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
    menu_action_gcode(USING_ABSOLUTE_POSITION);
}
/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_z
*	功能说明: 用户移动Z轴命令
*	形    参: dir--移动方向
@ POSITIVE_DIR--正方向
@ NEGTIVE_DIR--负方向


scale--每次移动范围
@ MM01--移动0.1mm
@ MM1--移动1mm
@ MM10--移动10mm
*	返 回 值: 无
*********************************************************************************************************
*/
void user_cmd_move_z(DIR_E dir, MOVE_SCALE scale)
{
    enable_z();
    menu_action_gcode(USING_RELATIVE_POSITION);
    switch (dir)
    {
    case POSITIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_Z_INC_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_Z_INC_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_Z_INC_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    case NEGTIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_Z_SUB_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_Z_SUB_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_Z_SUB_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
    menu_action_gcode(USING_ABSOLUTE_POSITION);
}

/*
*********************************************************************************************************
*	函 数 名: user_cmd_move_e
*	功能说明: 用户移动e轴命令
*	形    参: dir--移动方向
@ POSITIVE_DIR--正方向
@ NEGTIVE_DIR--负方向


scale--每次移动范围
@ MM01--移动0.1mm
@ MM1--移动1mm
@ MM10--移动10mm
*	返 回 值: 无
*********************************************************************************************************
*/
void user_cmd_move_e(DIR_E dir, MOVE_SCALE scale)
{
    enable_e0();
    menu_action_gcode(USING_RELATIVE_POSITION);
    switch (dir)
    {
    case POSITIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_E_INC_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_E_INC_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_E_INC_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    case NEGTIVE_DIR:
        switch (scale)
        {
        case MM01:
            menu_action_gcode(MOVE_E_SUB_0_1MM_CMD);
            break;
        case MM1:
            menu_action_gcode(MOVE_E_SUB_1MM_CMD);
            break;
        case MM10:
            menu_action_gcode(MOVE_E_SUB_10MM_CMD);
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
    menu_action_gcode(USING_ABSOLUTE_POSITION);

}


//挤出电机挤出动作，以绝对位置操作
void user_cmd_move_extru(int32_t length, _filament_speed_step_e speed)
{
	 char e_move_length[32];
	u32 setSpeed;
	int32_t setLength = length;
    enable_e0();

	switch (speed)
		{
			case FILAMENT_SPEED_SLOW:
			setSpeed = MOVE_E1_SPEED_SLOW;
			
				break;
				
			case FILAMENT_SPEED_NORMAL:
			setSpeed = MOVE_E1_SPEED_NORMAL;
				break;
				
			case FILAMENT_SPEED_FAST:
			setSpeed = MOVE_E1_SPEED_FAST;
				break;				
				
			default:
				break;
		}

	sprintf(e_move_length, "G1 E%d %d",setLength, setSpeed);
	  menu_action_gcode(e_move_length);		

}

//找出断电时命令缓冲区执行的gcode在文件的位置
/**
输入参数：

cmdLineNo -- 断电时命令缓冲区 + planner 缓冲区的gcode码个数
pdFilePosHead -- 断电时读取gcode码的行号的行首位置

输出参数：
desFilePos -- 断电续打gcode文件读取位置
**/
void reach_power_down_file_pos(u8 cmdLineNo,u32 pdFilePosHead,u32 *desFilePos)
{
	u32 cnt = 0;
	UINT br;

	int headPos = pdFilePosHead;
	u8  res;
	u8 data;
	u8 i ;
	i = 2*(cmdLineNo + 1);
	card_setIndex(headPos);//设置读文件指针为断电时是
	
	//控制文件读到断电时执行的gcode码的上一条gcode的末尾 
	while(i)
	{
		res = f_read(&card.fgcode, &data, 1, &br);//
		if (res == FR_OK )
		{   
			if((data == '\n' || data == '\r') &&  cnt < 70)
			{
				
				i--;
				cnt = 0;
				DBG("#i:%d\r\n",i);
			}
			else if(data == ';')//遇到注释行，本行无效
			{
				i += 2;
				cnt = 0;
			}
			
			headPos--;//读文件减1,往前读文件
			if(headPos < 0 )	{DBG("#@2read data fault\r\n");return;}
		   card_setIndex(headPos);//设置文件指针
		   cnt++;
		   if(cnt > 70 ) {DBG("#data overflow\r\n");return;}
		   DBG("#data:%c\r\n",data);		
		}
		else 
		{
			DBG("#@1read data fault\r\n");
			return;
		}
		
	}
	headPos += 2;
	card_setIndex(headPos);//设置文件指针
	*desFilePos = headPos;
	
}






#endif //SDSUPPORT
