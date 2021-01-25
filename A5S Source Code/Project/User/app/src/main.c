/*
*********************************************************************************************************
*
*	ģ������ : ���������
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ����ͨ�����ӣ���PC�������ն�������н���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��

*
 ʹ��SD��ǰ�����ʽ������ʽ��ѡ��ļ�ϵͳ--FAT32��Ĭ�ϣ� ���䵥Ԫ��С--Ĭ�ϣ�
*
*********************************************************************************************************
*/

#include "bsp.h"				/* �ײ�Ӳ������ */

static void PrintfHelp(void);
volatile u8 system_task_return = 0;
volatile u8 forceAdjust = 0;
void pw_InitLed(void);
void test_pw_switch(void);
char *const updateRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"���Ե�...",
	"Please Wait..."
};
char *const sdRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"����봢�濨.",
	"Please plug in a SD card"
};
char *const companyInfor[GUI_LANGUAGE_NUM] = 
{
	"���ڼ������3D��ӡ��",
	"JGAURORA 3D Printer"
};

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
    uint8_t i = 0;
    u16 ypos = 0;
    u16 xpos = 0;
    u16 j = 0;
    u16 temp = 0;
	u16 offset;
    u8 res;
    u8 ret;
    u8 verbuf[12];
    u8 fsize = 24;
	u8 wanteUpdateFont = FALSE;
	u8 wanteUpdateIcon = FALSE;
	u8 wanteSwitchLan = FALSE;
	u8 languageID ;
	u8 changeLAN = TRUE;

#ifdef IS_APP
	SCB->VTOR = FLASH_APP1_ADDR;
#endif
	
    /*
    	ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
    ����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
    */
    bsp_Init();		/* Ӳ����ʼ�� */

    PrintfHelp();	/* ��ӡ������ʾ */
	


REINIT://���³�ʼ��
    
    POINT_COLOR = BLUE;
    BACK_COLOR = BLACK;
    j = 0;
/////////////////////////////////////////////////////////////////////////
//��ʾ��Ȩ��Ϣ
    print_fw_infor();
    sprintf((char*)verbuf, "LCD ID:%04X", lcddev.id);		//LCD ID��ӡ��verbuf����
    DBG("->%s\r\n", verbuf);
	
    /********************************************EEPROM-AT24C64 ���ڼ�� + ������Ϣ���**********************************************/
	//24C02���
	if(AT24CXX_Check())//ʧ��
	{

		DBG("<@>Don't find AT24CXX IC\r\n");
	}
	else DBG("<#>EEPROM is OK!");
	
	//��ʼӲ������ʼ��

	if(W25QXX_ReadID() != W25Q128) //��ⲻ��W25Q128
	{

		DBG("<@>Don't find W28Q128 IC\r\n");
	}
	else DBG("<#>external FLASH IC is OK!\r\n");

	res = font_init();

	/***********************************��������********************************************/	
	wanteSwitchLan = AT24CXX_ReadOneByte(LANGUAGE_STATE_ADDR);
	if(wanteSwitchLan == LANGUAGE_STATE_EN)	languageID = ENGLISH;
	else if(wanteSwitchLan == LANGUAGE_STATE_CH) 	languageID = CHINESE;
	else {languageID = ENGLISH;}//Ĭ�ϵ�����ΪӢ��

	DBG("->language id :%d\r\n",languageID);
	
	set_language(languageID);

/*******************************************************************************/	

	wanteUpdateFont = AT24CXX_ReadOneByte(ST_FONT_STATE_ADDR);
	wanteUpdateIcon = AT24CXX_ReadOneByte(SYSTEM_STATE_ADDR);
	if(wanteUpdateFont != ST_FONT_STATE_VALUE || wanteUpdateIcon != SYSTEM_STATE_VALUE)
	{
		xpos = 5;
		ypos = 2;
		fsize = 24;
	#if ZHONG_XING==0
		//Show_Str(xpos ,ypos,lcddev.width,fsize,(u8 *)companyInfor[LANGUAGE],fsize,1);
	#endif
		ypos = ypos + 2*fsize;
		Show_Str(xpos ,ypos,lcddev.width,fsize,(u8 *)sdRemidInfor[LANGUAGE],fsize,1);
		ypos += fsize;
		Show_Str(xpos ,ypos,lcddev.width,fsize,(u8 *)updateRemidInfor[LANGUAGE],fsize,1);
		ypos += fsize;
	}
	offset = 2 + fsize*4;


    /***************************************************************************************/
    //��� FLASH SD card���ļ�ϵͳ
    res = f_mount(fs[0], "0:", 1); 		//����SD��
    ret = f_mount(fs[1], "1:", 1); 		//���ع���FLASH.
    if( ret == FR_OK )//FLASH���سɹ�
    {
		DBG("<#>FLASH mount FATS OK!\r\n");
        if( res != FR_OK) //SD������ʧ��
        {
			DBG("<@>SD card mount FATS fail!\r\n");
            temp = AT24CXX_ReadOneByte(SYSTEM_STATE_ADDR);
            if( temp != SYSTEM_STATE_VALUE )//�ϴο���û�и���������ϵͳ�ļ�
            {
                bsp_DelayMS(100);
                goto REINIT;
            }
        }
		else DBG("<#>SD card mount FATS OK!\r\n");

    }

    else
    {
		DBG("<@>FLASH mount FATS fail!\r\n");
		temp = AT24CXX_ReadOneByte(SYSTEM_STATE_ADDR);
		if( temp != SYSTEM_STATE_VALUE )//�ϴο���û�и���������ϵͳ�ļ�
		{
			DBG("->Flash Disk Formatting...\r\n");
			res = f_mkfs("1:", 1, 4096); //��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
			if(res == 0)
			{
				DBG("<#>Flash Disk Format OK\r\n");
			}
			else DBG("<#>Flash Disk Format fail\r\n");
			
			bsp_DelayMS(100);
			goto REINIT;
			
		}
    }
	
  /********************************************�����ֿ��ļ�*************************************/
	ypos = offset;
	fsize = 16;
	xpos = 5;
	ypos += 2*fsize;
FONT_UPDATE:
	
    if(font_init() || wanteUpdateFont != ST_FONT_STATE_VALUE)	//�������:1)�����ֿⲻ���ڣ�2)�û�ǿ�Ƹ����ֿ�
    {

		DBG("<@>wait...\r\n");
        if(update_font(xpos, ypos  , fsize, (u8*)"0:") != 0) //��SD������
        {
			DBG("<@>Font updated from SD card is fail...\r\n");
			bsp_DelayMS(100);
#if 0
            if(update_font(xpos, ypos  , fsize, (u8*)"1:") != 0) //��SPI FLASH����
            {
				bsp_DelayMS(100);
				DBG("<@>Font updated from FLASH is fail...\r\n");
                goto FONT_UPDATE;
            }
#endif
			
        }
        //�ɹ�����
		  AT24CXX_WriteOneByte(ST_FONT_STATE_ADDR, ST_FONT_STATE_VALUE);//�����ֿ�ɹ�
		  temp = AT24CXX_ReadOneByte(ST_FONT_STATE_ADDR);
		  if(temp == ST_FONT_STATE_VALUE)	 DBG("<#>Font update OK!\r\n");
		  else DBG("<@>Font update fail!\r\n");
		  
		 
    }
    /**********************************************************************/



  /********************************************����ͼ���ļ�************************************/	
		ypos = offset;
		j = 0;
		fsize = 16;
		xpos = 5;
		ypos += 3*fsize;	
		    //ϵͳ�ļ����
		if(app_system_file_check("1") || wanteUpdateIcon != SYSTEM_STATE_VALUE)//���FLASH�е��ļ�,1)�ļ�ȱʧ,2)�û�ǿ�Ƹ���
		{
			bsp_DelayMS(50);
			app_boot_cpdmsg_set(xpos , ypos, fsize);		//���õ�����
			if(app_system_file_check("0"))					//���SD��ϵͳ�ļ�������
			{
				DBG("<@>SD card files are lost!\r\n");
			}
			else//SD card�ļ�����
			{
				DBG("<#>SD card files are OK!\r\n");
				POINT_COLOR = WHITE;
				while(app_system_update(app_boot_cpdmsg))	//SD������
				{
					j++;
					bsp_DelayMS(100);
					
					if(j > 1 )	
					{
						break;
					}
				}		
			}



			if(app_system_file_check("1"))//������һ�Σ��ټ�⣬������в�ȫ��˵��SD���ļ��Ͳ�ȫ��
			{

				DBG("<@>files update fail!\r\n");
			}
		AT24CXX_WriteOneByte(SYSTEM_STATE_ADDR, SYSTEM_STATE_VALUE);//�����ļ��ɹ�
		temp = AT24CXX_ReadOneByte(SYSTEM_STATE_ADDR);
		if(temp == SYSTEM_STATE_VALUE)	DBG("<#>files update OK!\r\n");
		else DBG("<@>files update fail!\r\n");
		}
		else DBG("<#>files are OK!\r\n");
/*******************************************************************************/
	
	
	
    welcome_screen();
	power_down_print_init();
    setup();
    loop();

}

/*
*********************************************************************************************************
*	�� �� ��: PrintfHelp
*	����˵��: ��ӡ������ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
    DBG("help\r\n");
}



/**************************/
