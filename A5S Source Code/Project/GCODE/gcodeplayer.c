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
/**********************���Ժ궨��**************************/

#define SD_DEBUG_EN

#ifdef SD_DEBUG_EN

#define SD_DEBUG	DBG

#else
#define SD_DEBUG(format,...)	((void) 0)

#endif
/************************************************/



/**********************ȫ�ֱ���**************************/
//���ڼ�¼��ǰ��ӡ��gcode�ļ����ļ��кš��ļ���ȡλ�ú��ļ���С��Ϣ
uint32_t fil_size;//�ļ���С
volatile uint32_t  line_no;//gcode���к�,�ϵ������õ�
uint32_t file_pos;//��gcode���ļ�ָ��
uint32_t user_line_no;//�û����͵�gcode����
_PRINT_STATE print_state = PS_NO;
_sdCardUnplugCheck_s sdCardUnplug;
uint8_t isPrintFileExist = TRUE;//Ĭ��Ҫ��ӡ���ļ�����SD���У�������־SD���β�ʱ����û�����ڴ�ӡ���ļ�
/************************************************/





/*
*********************************************************************************************************
*	�� �� ��: gcode_ls
*	����˵��: gcode�ļ��б�
*	��    ��: path--ָ��·��
*	����ֵ����
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
*	�� �� ��: card_ls
*	����˵��: ��ӡSDcard�е�gcode�ļ��б�
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_initsd
*	����˵��: SDcard��ʼ��
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_release
*	����˵��: �ͷ�SDcard
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void card_release(void)
{   card.sdprinting = FALSE;
    card.cardOK = FALSE;
}

/*
*********************************************************************************************************
*	�� �� ��: card_openFile
*	����˵��: ��SDcard�д�ָ����gcode�ļ�
*	��    ��: fname--�ļ���
read--��д���Ʊ�־
@ 0--��д�ķ�ʽ���ļ�
@ 1--�Զ��ķ�ʽ���ļ�
*	����ֵ����
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
*	�� �� ��: card_removeFile
*	����˵��: ɾ��SDcard��ָ����gcode�ļ�
*	��    ��: fname--�ļ���
*	����ֵ����
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
*	�� �� ��: card_startFileprint
*	����˵��: ��ʼSDcard��ӡgcode����λ��־��SDcard�е�gcode�ļ���get_command()�б���ȡ�����������
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_pauseSDPrint
*	����˵��: SDcard��ͣ��ӡgcode����λ��־��SDcard�е�gcode�ļ���get_command()�в�����ȡ�����������
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: force_sdCard_pause
*	����˵��: ǿ��ʹSDcard��ͣ��ӡgcode����λ��־��SDcard�е�gcode�ļ���get_command()�в�����ȡ�����������
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void force_sdCard_pause(void)
{
    card.sdprinting = FALSE;

}

/*
*********************************************************************************************************
*	�� �� ��: force_sdCard_start
*	����˵��: ǿ��ʹSDcard��ӡgcode����λ��־��SDcard�е�gcode�ļ���get_command()�б���ȡ�����������
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void force_sdCard_start(void)
{
    card.sdprinting = TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: card_setIndex
*	����˵��: ����SDcard��/дgcode�ļ����ļ�ָ��λ��
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void card_setIndex(long index)
{   //card.sdpos = index;
    f_lseek(&card.fgcode, index);
}


/*
*********************************************************************************************************
*	�� �� ��: card_getStatus
*	����˵��: ��ȡSDcard״̬
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_closefile
*	����˵��: �ر�SDcard��ȡ��gcode�ļ�
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_checkautostart
*	����˵��: ���SD���Ƿ���λ
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void card_checkautostart()
{
    if((!card.cardOK) && (!SD_CD))
    {
        card_initsd();
				sdCardUnplug.unplugFlag = TRUE;//��־����
        if(!card.cardOK) //fail
            return;
    }

}

/*
*********************************************************************************************************
*	�� �� ��: card_printingHasFinished
*	����˵��: ��SDcard�ж�ȡgcode�ļ�������ִ����β����
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: card_eof
*	����˵��: �жϴ�SDcard�ж�ȡ�ļ��Ƿ����
*	��    ��: ��
*	����ֵ������
*********************************************************************************************************
*/
u8 card_eof(void)
{   return f_eof(&card.fgcode);
}

/*
*********************************************************************************************************
*	�� �� ��: card_get
*	����˵��: ��SDcard�л�ȡgcode�ļ���һ���ֽ�
*	��    ��: ��
*	����ֵ������ȡ���ֽ�
*********************************************************************************************************
*/
int16_t card_get(void)
{   //card.sdpos = f_tell(&card.fgcode);
	
    return  file_read();
}

/*
*********************************************************************************************************
*	�� �� ��: file_read
*	����˵��: gcode�ļ�ÿ�α���ȡһ���ֽڣ�����get_command()��
*	��    ��: ��
*	����ֵ������ȡ���ֽ�
*********************************************************************************************************
*/

int16_t file_read(void)
{   u8 buffer[128];
    u8 res;
    UINT br;
    res = f_read(&card.fgcode, buffer, 1, &br);
    if (res == FR_OK )
    {   // SD_DEBUG("%s",buffer);
		file_pos ++;//���ļ�ָ����1	
        return *buffer;//
    }
	else
    {   
	SD_DEBUG("f_read() fail .. \r\n");
	return -1;
    }
}
//����sdCard������β�
void deal_with_unplug_sdCard(void)
{
	u8 isReadCard;
	u8 isFilaOut;
	isFilaOut = bsp_GetKeyState(FILA_IO_ID);
	//DBG("io:%d\r\n",isFilaOut);
	if((SD_CD && sdCardUnplug.unplugFlag != TRUE && isFilaOut == 0) || (stopDuringPrintFlag == TRUE && SD_CD == 0 && sdCardUnplug.unplugFlag != TRUE))//unplug,������ӡ������
	{
		isReadCard = read_card_print_flag();
		if(isReadCard == TRUE || line_no >= 1)//�ڶ���������,ȷ����ӡ���Ѿ�����
		{
			card_pauseSDPrint();
			sdCardUnplug.saveLineNo = line_no;
			sdCardUnplug.saveFpos = file_pos;
			sdCardUnplug.unplugFlag = TRUE;
			f_close(&card.fgcode);//�ر��ļ�

			DBG("-> ##unplug sd \r\n");
		}
	

	}
	if((isPrintFileExist == FALSE || SD_CD)&& eeprom_is_power_off_happen() == PW_OFF_HAPPEN && sdCardUnplug.unplugFlag != TRUE && isFilaOut == 0)//�ϵ����򿪻�ʱSD��δ����
	{
		sdCardUnplug.unplugFlag = TRUE;
		DBG("-> #@powerD sd lost \r\n");
	}
}	
//��Ȼ�аβ忨�������򣬵��ǲ������޹ʰβ�SD��������ᵼ�¶����쳣����ΪSD����Դδ�ж�
void deal_with_sdCard_plugAgain(void)
{


	u8 fmountF = 0xff;
	u8 fopenfileF = 0xff;
	u8 cnt = 0;
	u8 res = 1;
	if(sdCardUnplug.unplugFlag == TRUE && SD_CD == 0)//ȷ��SD������
	{
		
			
		DBG("->come in\r\n");
		fmountF = f_mount(NULL, "0:", 1); 		//ж��SD��
		if(fmountF != FR_OK )	DBG("->## un mount Fat fail\r\n");
		bsp_DelayMS(5);
		
		fmountF = f_mount(fs[0], "0:", 1);
		if(fmountF != FR_OK )	DBG("->## mount Fat fail\r\n");
		
		if(card.FILE_STATE == PS_NO	|| card.FILE_STATE == PS_STOP	)//��ӡ�ļ�δѡ����ϰβ忨
		{
			DBG("->printer is in pickup state\r\n");
			sdCardUnplug.unplugFlag = FALSE;
			return;
		}			
			cnt = 0;		
			while(fopenfileF)
			{
				if(printFileName != NULL)//�ļ�������
				{
					fopenfileF = f_open(&card.fgcode, (const TCHAR*)printFileName, FA_READ |FA_OPEN_EXISTING);
					cnt++;
				}
				else {DBG("->## print file lost \r\n"); break;}
				
			
				if(fopenfileF != FR_OK && cnt >=3 )
				{
					DBG("->## print file lost \r\n");
					
					isPrintFileExist = FALSE;//�����ڴ�ӡ�ļ�
					break;
				}
				bsp_DelayMS(5);
			}

			
			if(fopenfileF == FR_OK)//���ļ�OK
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
					
					if(pause_start_cnt % 2 == 0)//��ͣ�������º󣬰β�SD������������ӡ
						card_startFileprint();					
				}
				fil_size = f_size(&card.fgcode);//��¼�ļ���С
				//�����ļ���ָ��
				
				sdCardUnplug.unplugFlag = FALSE;//����fats + ��ȡ����ӡ�ļ� ����Ϊ�ôΰγ���ʼ�����
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
*	�� �� ��: get_precent_file_printed
*	����˵��: ��ȡgcode�ļ� ��ӡ�ٷֱ�
*	��    ��: ��
*	����ֵ��percents
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
*	�� �� ��: get_size_file_printed
*	����˵��: ��ȡgcode�ļ� �ļ�ָ��λ��
*	��    ��: ��
*	����ֵ��line_no
*********************************************************************************************************
*/
uint32_t get_size_file_printed(void)
{
    return file_pos;
}

/*
*********************************************************************************************************
*	�� �� ��: get_line_no_file_printed
*	����˵��: ��ȡgcode�ļ��Ѿ���ӡ�����к�
*	��    ��: ��
*	����ֵ��line_no
*********************************************************************************************************
*/

uint32_t get_line_no_file_printed(void)
{
    return line_no;
}

/*
*********************************************************************************************************
*	�� �� ��: get_size_of_file
*	����˵��: ��ȡ�ļ��Ĵ�С
*	��    ��: ��
*	����ֵ��fil_size
*********************************************************************************************************
*/
uint32_t get_size_of_file(void)
{
    return fil_size;
}

/*
*********************************************************************************************************
*	�� �� ��: menu_action_gcode
*	����˵��: ���û������͵����������������
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void menu_action_gcode(char* pgcode)
{
    enquecommand(pgcode);
}
/******************gcode ��ɢ�������********************/

/*
*********************************************************************************************************
*	�� �� ��: user_cmd_move_allHome
*	����˵��: �û�����x��y��z���ԭ��
*	��    ��: ��
*	����ֵ����
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
*	�� �� ��: user_cmd_move_xHome
*	����˵��: �û�����x���ԭ��
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/

void user_cmd_move_xHome(void)
{
    enable_x();
    menu_action_gcode(MOVE_GO_HOME_X);
}

/*
*********************************************************************************************************
*	�� �� ��: user_cmd_move_yHome
*	����˵��: �û�����y���ԭ��
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/

void user_cmd_move_yHome(void)
{
    enable_y();
    menu_action_gcode(MOVE_GO_HOME_Y);
}

/*
*********************************************************************************************************
*	�� �� ��: user_cmd_move_zHome
*	����˵��: �û�����z���ԭ��
*	��    ��: ��
*	����ֵ����
*********************************************************************************************************
*/
void user_cmd_move_zHome(void)
{
    enable_z();
    menu_action_gcode(MOVE_GO_HOME_Z);
}
/*
*********************************************************************************************************
*	�� �� ��: user_cmd_move_x
*	����˵��: �û��ƶ�x������
*	��    ��: dir--�ƶ�����
@ POSITIVE_DIR--������
@ NEGTIVE_DIR--������


scale--ÿ���ƶ���Χ
@ MM01--�ƶ�0.1mm
@ MM1--�ƶ�1mm
@ MM10--�ƶ�10mm
*	�� �� ֵ: ��
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
*	�� �� ��: user_cmd_move_y
*	����˵��: �û��ƶ�y������
*	��    ��: dir--�ƶ�����
@ POSITIVE_DIR--������
@ NEGTIVE_DIR--������


scale--ÿ���ƶ���Χ
@ MM01--�ƶ�0.1mm
@ MM1--�ƶ�1mm
@ MM10--�ƶ�10mm
*	�� �� ֵ: ��
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
*	�� �� ��: user_cmd_move_z
*	����˵��: �û��ƶ�Z������
*	��    ��: dir--�ƶ�����
@ POSITIVE_DIR--������
@ NEGTIVE_DIR--������


scale--ÿ���ƶ���Χ
@ MM01--�ƶ�0.1mm
@ MM1--�ƶ�1mm
@ MM10--�ƶ�10mm
*	�� �� ֵ: ��
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
*	�� �� ��: user_cmd_move_e
*	����˵��: �û��ƶ�e������
*	��    ��: dir--�ƶ�����
@ POSITIVE_DIR--������
@ NEGTIVE_DIR--������


scale--ÿ���ƶ���Χ
@ MM01--�ƶ�0.1mm
@ MM1--�ƶ�1mm
@ MM10--�ƶ�10mm
*	�� �� ֵ: ��
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


//������������������Ծ���λ�ò���
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

//�ҳ��ϵ�ʱ�������ִ�е�gcode���ļ���λ��
/**
���������

cmdLineNo -- �ϵ�ʱ������� + planner ��������gcode�����
pdFilePosHead -- �ϵ�ʱ��ȡgcode����кŵ�����λ��

���������
desFilePos -- �ϵ�����gcode�ļ���ȡλ��
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
	card_setIndex(headPos);//���ö��ļ�ָ��Ϊ�ϵ�ʱ��
	
	//�����ļ������ϵ�ʱִ�е�gcode�����һ��gcode��ĩβ 
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
			else if(data == ';')//����ע���У�������Ч
			{
				i += 2;
				cnt = 0;
			}
			
			headPos--;//���ļ���1,��ǰ���ļ�
			if(headPos < 0 )	{DBG("#@2read data fault\r\n");return;}
		   card_setIndex(headPos);//�����ļ�ָ��
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
	card_setIndex(headPos);//�����ļ�ָ��
	*desFilePos = headPos;
	
}






#endif //SDSUPPORT
