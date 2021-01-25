/*
*********************************************************************************************************
*
*	模块名称 : 主程序入口
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 串口通信例子，和PC机超级终端软件进行交互
*	修改记录 :
*		版本号  日期       作者    说明

*
 使用SD卡前建议格式化：格式化选项（文件系统--FAT32（默认） 分配单元大小--默认）
*
*********************************************************************************************************
*/

#include "bsp.h"				/* 底层硬件驱动 */

static void PrintfHelp(void);
volatile u8 system_task_return = 0;
volatile u8 forceAdjust = 0;
void pw_InitLed(void);
void test_pw_switch(void);
char *const updateRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"请稍等...",
	"Please Wait..."
};
char *const sdRemidInfor[GUI_LANGUAGE_NUM] = 
{
	"请插入储存卡.",
	"Please plug in a SD card"
};
char *const companyInfor[GUI_LANGUAGE_NUM] = 
{
	"深圳极光尔沃3D打印机",
	"JGAURORA 3D Printer"
};

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
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
    	ST固件库中的启动文件已经执行了 SystemInit() 函数，该函数在 system_stm32f4xx.c 文件，主要功能是
    配置CPU系统的时钟，内部Flash访问时序，配置FSMC用于外部SRAM
    */
    bsp_Init();		/* 硬件初始化 */

    PrintfHelp();	/* 打印操作提示 */
	


REINIT://重新初始化
    
    POINT_COLOR = BLUE;
    BACK_COLOR = BLACK;
    j = 0;
/////////////////////////////////////////////////////////////////////////
//显示版权信息
    print_fw_infor();
    sprintf((char*)verbuf, "LCD ID:%04X", lcddev.id);		//LCD ID打印到verbuf里面
    DBG("->%s\r\n", verbuf);
	
    /********************************************EEPROM-AT24C64 存在检测 + 保存信息检查**********************************************/
	//24C02检测
	if(AT24CXX_Check())//失败
	{

		DBG("<@>Don't find AT24CXX IC\r\n");
	}
	else DBG("<#>EEPROM is OK!");
	
	//开始硬件检测初始化

	if(W25QXX_ReadID() != W25Q128) //检测不到W25Q128
	{

		DBG("<@>Don't find W28Q128 IC\r\n");
	}
	else DBG("<#>external FLASH IC is OK!\r\n");

	res = font_init();

	/***********************************语言设置********************************************/	
	wanteSwitchLan = AT24CXX_ReadOneByte(LANGUAGE_STATE_ADDR);
	if(wanteSwitchLan == LANGUAGE_STATE_EN)	languageID = ENGLISH;
	else if(wanteSwitchLan == LANGUAGE_STATE_CH) 	languageID = CHINESE;
	else {languageID = ENGLISH;}//默认的语言为英语

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
    //检查 FLASH SD card的文件系统
    res = f_mount(fs[0], "0:", 1); 		//挂载SD卡
    ret = f_mount(fs[1], "1:", 1); 		//挂载挂载FLASH.
    if( ret == FR_OK )//FLASH挂载成功
    {
		DBG("<#>FLASH mount FATS OK!\r\n");
        if( res != FR_OK) //SD卡挂载失败
        {
			DBG("<@>SD card mount FATS fail!\r\n");
            temp = AT24CXX_ReadOneByte(SYSTEM_STATE_ADDR);
            if( temp != SYSTEM_STATE_VALUE )//上次开机没有更新了所有系统文件
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
		if( temp != SYSTEM_STATE_VALUE )//上次开机没有更新了所有系统文件
		{
			DBG("->Flash Disk Formatting...\r\n");
			res = f_mkfs("1:", 1, 4096); //格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
			if(res == 0)
			{
				DBG("<#>Flash Disk Format OK\r\n");
			}
			else DBG("<#>Flash Disk Format fail\r\n");
			
			bsp_DelayMS(100);
			goto REINIT;
			
		}
    }
	
  /********************************************更新字库文件*************************************/
	ypos = offset;
	fsize = 16;
	xpos = 5;
	ypos += 2*fsize;
FONT_UPDATE:
	
    if(font_init() || wanteUpdateFont != ST_FONT_STATE_VALUE)	//检测字体:1)本身字库不存在，2)用户强制更新字库
    {

		DBG("<@>wait...\r\n");
        if(update_font(xpos, ypos  , fsize, (u8*)"0:") != 0) //从SD卡更新
        {
			DBG("<@>Font updated from SD card is fail...\r\n");
			bsp_DelayMS(100);
#if 0
            if(update_font(xpos, ypos  , fsize, (u8*)"1:") != 0) //从SPI FLASH更新
            {
				bsp_DelayMS(100);
				DBG("<@>Font updated from FLASH is fail...\r\n");
                goto FONT_UPDATE;
            }
#endif
			
        }
        //成功更新
		  AT24CXX_WriteOneByte(ST_FONT_STATE_ADDR, ST_FONT_STATE_VALUE);//更新字库成功
		  temp = AT24CXX_ReadOneByte(ST_FONT_STATE_ADDR);
		  if(temp == ST_FONT_STATE_VALUE)	 DBG("<#>Font update OK!\r\n");
		  else DBG("<@>Font update fail!\r\n");
		  
		 
    }
    /**********************************************************************/



  /********************************************更新图标文件************************************/	
		ypos = offset;
		j = 0;
		fsize = 16;
		xpos = 5;
		ypos += 3*fsize;	
		    //系统文件检测
		if(app_system_file_check("1") || wanteUpdateIcon != SYSTEM_STATE_VALUE)//检查FLASH中的文件,1)文件缺失,2)用户强制更新
		{
			bsp_DelayMS(50);
			app_boot_cpdmsg_set(xpos , ypos, fsize);		//设置到坐标
			if(app_system_file_check("0"))					//检查SD卡系统文件完整性
			{
				DBG("<@>SD card files are lost!\r\n");
			}
			else//SD card文件完整
			{
				DBG("<#>SD card files are OK!\r\n");
				POINT_COLOR = WHITE;
				while(app_system_update(app_boot_cpdmsg))	//SD卡更新
				{
					j++;
					bsp_DelayMS(100);
					
					if(j > 1 )	
					{
						break;
					}
				}		
			}



			if(app_system_file_check("1"))//更新了一次，再检测，如果还有不全，说明SD卡文件就不全！
			{

				DBG("<@>files update fail!\r\n");
			}
		AT24CXX_WriteOneByte(SYSTEM_STATE_ADDR, SYSTEM_STATE_VALUE);//更新文件成功
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
*	函 数 名: PrintfHelp
*	功能说明: 打印操作提示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
    DBG("help\r\n");
}



/**************************/
