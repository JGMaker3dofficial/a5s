
#include "Dlion.h"
#include "bsp_timer.h"
//#include "ultralcd.h"
#include "planner.h"
#include "stepper.h"
#include "temperature.h"
#include "motion_control.h"
//#include "cardreader.h"
#include "watchdog.h"
#include "ConfigurationStore.h"
#include "language.h"
//#include "pins_arduino.h"
#include "bsp_usart.h"
#include "beep.h"
#include "gcodeplayer.h"
#include "app_gui.h"
#include "sdio_sdcard.h"
#include "gcodeplayer.h"
#include "print.h"
#include "watchdog.h"
#define VERSION_STRING  "1.0.0"

// look here for descriptions of gcodes: http://linuxcnc.org/handbook/gcode/g-code.html
// http://objects.reprap.org/wiki/Mendel_User_Manual:_RepRapGCodes

//Implemented Codes
//-------------------
// G0  -> G1
// G1  - Coordinated Movement X Y Z E
// G2  - CW ARC
// G3  - CCW ARC
// G4  - Dwell S<seconds> or P<milliseconds>
// G10 - retract filament according to settings of M207
// G11 - retract recover filament according to settings of M208
// G28 - Home all Axis
// G90 - Use Absolute Coordinates
// G91 - Use Relative Coordinates
// G92 - Set current position to cordinates given

//RepRap M Codes
// M0   - Unconditional stop - Wait for user to press a button on the LCD (Only if ULTRA_LCD is enabled)
// M1   - Same as M0
// M104 - Set extruder target temp
// M105 - Read current temp
// M106 - Fan on
// M107 - Fan off
// M109 - Wait for extruder current temp to reach target temp.
// M114 - Display current position

//Custom M Codes
// M15  - update flash data (font data ; icon data and so on)
// M16  - screen_adjust
// M17  - Enable/Power all stepper motors
// M18  - Disable all stepper motors; same as M84
// M20  - List SD card
// M21  - Init SD card
// M22  - Release SD card
// M23  - Select SD file (M23 filename.g)
// M24  - Start/resume SD print
// M25  - Pause SD print
// M26  - Set SD position in bytes (M26 S12345)
// M27  - Report SD print status
// M28  - Start SD write (M28 filename.g)
// M29  - Stop SD write
// M30  - Delete file from SD (M30 filename.g)
// M31  - Output time since last M109 or SD card start to serial
// M42  - Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
// M80  - Turn on Power Supply
// M81  - Turn off Power Supply
// M82  - Set E codes absolute (default)
// M83  - Set E codes relative while in Absolute Coordinates (G90) mode
// M84  - Disable steppers until next move,
//        or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
// M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
// M92  - Set axis_steps_per_unit - same syntax as G92
// M110 - Set the current line number. (Used by host printing)
// M114 - Output current position to serial port
// M115 - Capabilities string
// M117 - display message
// M119 - Output Endstop status to serial port
// M126 - Solenoid Air Valve Open (BariCUDA support by jmil)
// M127 - Solenoid Air Valve Closed (BariCUDA vent to atmospheric pressure by jmil)
// M128 - EtoP Open (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M129 - EtoP Closed (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M140 - Set bed target temp
// M190 - Wait for bed current temp to reach target temp.
// M200 - Set filament diameter
// M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
// M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000) Unused in Marlin!!
// M203 - Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E10000) in mm/sec
// M204 - Set default acceleration: S normal moves T filament only moves (M204 S3000 T7000) im mm/sec^2  also sets minimum segment time in ms (B20000) to prevent buffer underruns and M20 minimum feedrate
// M205 -  advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
// M206 - set additional homeing offset
// M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
// M208 - set recover=unretract length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
// M209 - S<1=TRUE/0=FALSE> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
// M218 - set hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>
// M220 S<factor in percent>- set speed factor override percentage
// M221 S<factor in percent>- set extrude factor override percentage
// M240 - Trigger a camera to take a photograph
// M280 - set servo position absolute. P: servo index, S: angle or microseconds
// M300 - Play beepsound S<frequency Hz> P<duration ms>
// M301 - Set PID parameters P I and D
// M302 - Allow cold extrudes
// M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)
// M304 - Set bed PID parameters P I and D
// M400 - Finish all moves
// M500 - stores paramters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
// M503 - print the current settings (from memory not from eeprom)
// M540 - Use S[0|1] to enable or disable the stop SD card print on endstop hit (requires ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
// M600 - Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
// M907 - Set digital trimpot motor current using axis codes.
// M908 - Control digital trimpot directly.
// M350 - Set microstepping mode.
// M351 - Toggle MS1 MS2 pins directly.
// M928 - Start SD logging (M928 filename.g) - ended by M29
// M999 - Restart after being stopped by error

//Stepper Movement Variables

//===========================================================================
//=============================imported variables============================
//===========================================================================


//===========================================================================
//=============================public variables=============================
//===========================================================================
#ifdef SDSUPPORT
CardReader card;
#endif
float homing_feedrate[] = HOMING_FEEDRATE;
u8 axis_relative_modes[] = AXIS_RELATIVE_MODES;
volatile int feedmultiply=100; //100->1 200->2
int saved_feedmultiply;
int extrudemultiply=100; //100->1 200->2
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };

float saved_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };
float temp_posistion[NUM_AXIS]={0.0, 0.0, 0.0, 0.0};

float add_homeing[3]={0,0,0};
float min_pos[3] = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };
float max_pos[3] = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };

static const float  base_min_pos[3]   = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };  
static float  base_max_pos[3]   = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
static const float  base_home_pos[3]  = { X_HOME_POS, Y_HOME_POS, Z_HOME_POS };
static float  max_length[3]        = { X_MAX_LENGTH, Y_MAX_LENGTH, Z_MAX_LENGTH };
static const float  home_retract_mm[3]= { X_HOME_RETRACT_MM, Y_HOME_RETRACT_MM, Z_HOME_RETRACT_MM };
static const signed char home_dir[3]  = { X_HOME_DIR, Y_HOME_DIR, Z_HOME_DIR };

u8 readLineFlag = FALSE;//gcode line_no arrives to powerdown line_no flag,1--up to ,0--not arrive
u8 stopDuringPrintFlag = FALSE;//SD card read error during printing 
// Extruder offset, only in XY plane
#if EXTRUDERS > 1
float extruder_offset[2][EXTRUDERS] = {
#if defined(EXTRUDER_OFFSET_X) && defined(EXTRUDER_OFFSET_Y)
  EXTRUDER_OFFSET_X, EXTRUDER_OFFSET_Y
#endif
};
#endif
uint8_t active_extruder = 0;
int fanSpeed=0;
#ifdef BARICUDA
int ValvePressure=0;
int EtoPPressure=0;
#endif

#ifdef FWRETRACT
  u8 autoretract_enabled=TRUE;
  u8 retracted=FALSE;
  float retract_length=3, retract_feedrate=17*60, retract_zlift=0.8;
  float retract_recover_length=0, retract_recover_feedrate=8*60;
#endif
//===========================================================================
//=============================private variables=============================
//===========================================================================
const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};
 float destination[NUM_AXIS] = {  0.0, 0.0, 0.0, 0.0};
 
float offset[3] = {0.0, 0.0, 0.0};
static u8 home_all_axis = TRUE;
float feedrate = 1500.0, next_feedrate, saved_feedrate;
static long gcode_N, gcode_LastN, Stopped_gcode_LastN = 0;

volatile static u8 relative_mode = FALSE;  //Determines Absolute or Relative Coordinates

static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];
volatile static u8 fromsd[BUFSIZE];
static int bufindr = 0;
static int bufindw = 0;

volatile int buflen = 0;

//static int i = 0;
static char serial_char;
static int serial_count = 0;
static u8 comment_mode = FALSE;
static char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc

//const int sensitive_pins[] = SENSITIVE_PINS; // Sensitive pin list for M42

//static float tt = 0;
//static float bt = 0;

//Inactivity shutdown variables
static unsigned long previous_millis_cmd = 0;
static unsigned long max_inactive_time = 0;
static unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME*1000l;

unsigned long starttime=0;
unsigned long stoptime=0;
static u8 tmp_extruder;


u8 Stopped=FALSE;

//#if NUM_SERVOS > 0
//  Servo servos[NUM_SERVOS];
//#endif
//===========================================================================
//=============================ROUTINES=============================
//===========================================================================

void get_arc_coordinates(void);
u8 setTargetedHotend(int code);

static void Gcode_G0G1(void);
static void Gcode_G2(void);
static void Gcode_G3(void);
static void Gcode_G4(void);
static void Gcode_G10(void);
static void Gcode_G11(void);
static void Gcode_G28(void);
static void Gcode_G90(void);
static void Gcode_G91(void);
static void Gcode_G92(void);
static void Gcode_M0M1(void);
static void Gcode_M15(void);
static void Gcode_M16(void);
static void Gcode_M17(void);
static void Gcode_M19(void);
static void Gcode_M20(void);
static void Gcode_M21(void);
static void Gcode_M22(void);
static void Gcode_M23(void);
static void Gcode_M24(void);
static void Gcode_M25(void);
static void Gcode_M26(void);
static void Gcode_M27(void);
static void Gcode_M28(void);
static void Gcode_M29(void);
static void Gcode_M30(void);
static void Gcode_M31(void);
static void Gcode_M42(void);
static void Gcode_M104(void);
static void Gcode_M140(void);
static void Gcode_M105(void);
static void Gcode_M109(void);
static void Gcode_M190(void);
static void Gcode_M106(void);
static void Gcode_M107(void);
static void Gcode_M126(void);
static void Gcode_M127(void);
static void Gcode_M128(void);
static void Gcode_M129(void);
static void Gcode_M80(void);
static void Gcode_M81(void);
static void Gcode_M82(void);
static void Gcode_M83(void);
static void Gcode_M18M84(void);
static void Gcode_M85(void);
static void Gcode_M92(void);

static void Gcode_M115(void);
static void Gcode_M117(void);
static void Gcode_M114(void);
static void Gcode_M120(void);
static void Gcode_M121(void);
static void Gcode_M119(void);
static void Gcode_M201(void);
static void Gcode_M202(void);
static void Gcode_M203(void);
static void Gcode_M204(void);
static void Gcode_M205(void);
static void Gcode_M206(void);
static void Gcode_M207(void);
static void Gcode_M208(void);
static void Gcode_M209(void);
static void Gcode_M218(void);
static void Gcode_M220(void);
static void Gcode_M221(void);
static void Gcode_M280(void);
static void Gcode_M300(void);
static void Gcode_M301(void);
static void Gcode_M304(void);
static void Gcode_M240(void);
static void Gcode_M302(void);
static void Gcode_M303(void);
static void Gcode_M400(void);
static void Gcode_M500(void);
static void Gcode_M501(void);
static void Gcode_M502(void);
static void Gcode_M503(void);
static void Gcode_M540(void);
static void Gcode_M600(void);
static void Gcode_M907(void);
static void Gcode_M908(void);
static void Gcode_M350(void);
static void Gcode_M351(void);
static void Gcode_M999(void);
static void Gcode_T(void);
static void Gcode_Error(void);

static void Gcode_M110(void);





void enquecommand(const char *cmd)
{
  if(buflen < BUFSIZE)
  {
    //this is dangerous if a mixing of serial and this happsens
    strcpy(&(cmdbuffer[bufindw][0]),cmd);
   // SERIAL_ECHO_START;
 //   DLION_PRINTF("enqueing \"%s\"",cmdbuffer[bufindw]);
    bufindw= (bufindw + 1)%BUFSIZE;
    buflen += 1;    
    user_line_no++;
  }    

}


void setup(void)
{ 
	Config_PrintSettings();
	reset_acceleration_rates();
//    Config_RetrieveSettings();
//        max_pos[0] = X_MAX_POS;
//        max_pos[1] = Y_MAX_POS;
//        max_pos[2] = Z_MAX_POS;
//        base_max_pos[0] = X_MAX_POS;
//        base_max_pos[1] = Y_MAX_POS;
//        base_max_pos[2] = Z_MAX_POS;
//        max_length[0]     = X_MAX_LENGTH;
//        max_length[1]     = Y_MAX_LENGTH;
//        max_length[2]     = Z_MAX_LENGTH;

  st_init();    // Initialize stepper, this enables interrupts
  tp_init();    // Initialize 
  plan_init();  // Initialize planner;
  lcd_update();
 }

 void loop(void)
{

 while(1)
{ 
        if(buflen < (BUFSIZE-1))
         { 
             get_command();                                                                                         
         }
        #ifdef SDSUPPORT
         if(SD_CD)//实时检测SD卡是否插入,SD插入引脚接个滤波电容，滤掉10K及以上毛刺电压
         {
            //bsp_DelayMS(5);
            //if(SD_CD) 
            {
            card.cardOK = FALSE;
            }
         }
         
        
        deal_with_unplug_sdCard();//处理意外拔卡
        card_checkautostart(); 
        deal_with_sdCard_plugAgain();
        #endif
    
         if(buflen) 
         {
                #ifdef SDSUPPORT
                    if(card.saving)
                    {     
                        if(strstr(cmdbuffer[bufindr], PSTR("M29")) == NULL)
                        {
                            card_write_command(cmdbuffer[bufindr]);
                            DLION_PRINTF(MSG_OK);
                        }
                        else                                         
                        {
                            card_closefile();
                            DLION_PRINTF(MSG_FILE_SAVED);
                        }
                         DLION_PRINTF("\n");
                    }
                    else
                    {    
                            process_commands();      
                         //   DLION_PRINTF("%s\r\n",cmdbuffer[bufindr]);                            
                    }
                #else
                    process_commands();  
        
                #endif //SDSUPPORT
                    
                if(buflen > 0)                                                                                                       
                {
                    buflen = (buflen-1);
                    bufindr = (bufindr + 1)%BUFSIZE;
                }                    


         }
  //check heater every n milliseconds 
  manage_heater();
  manage_inactivity();
  checkHitEndstops();
    checkDTR();//20160228
  lcd_update();
  pdPrintProcess();
 

 }
}

 
void get_command(void)
{
	int16_t n;

	while( MYSERIAL_available() > 0  && buflen < BUFSIZE)
	{
		serial_char = MYSERIAL_read();	// 从缓冲区读取一个字节
		//		DLION_PRINTF(" serial_char: %c\n\r",serial_char);
		
		if(	// 若为 回车/换行/非注释中的冒号 或 缓存已满
			serial_char == '\n' ||
			serial_char == '\r' ||
			(serial_char == ':' && comment_mode == FALSE) ||
			serial_count >= (MAX_CMD_SIZE - 1) 
		)//sanse 冒号
		{
			if(!serial_count)  // 若之前还未收到一个字节，现在收到了回车/换行，说明是空行。 if empty line
			{
				comment_mode = FALSE; //for new command
				return;
			}
			
			cmdbuffer[bufindw][serial_count] = 0; // 写入字符串结束标志。terminate string
			DLION_DEBUG("RX: [%s]\r\n",cmdbuffer[bufindw]);

			// 若不是注释行
			if(!comment_mode)
			{
				comment_mode = FALSE; //for new command
				fromsd[bufindw] = FALSE;// 标记此命令不来自SD卡

				// M110 Nxxx 设置当前行号
				// N123 M110 N0*125
				// G1 X191.223 Y162.445 E984.40241【不带行号和校验的GCODE命令】
				// N4351 G1 X191.223 Y162.445 E984.40241*93【带行号和校验的GCODE命令】
				if(strchr(cmdbuffer[bufindw], 'N')!=NULL) // 若存在字符N                                                         
				{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'N');	// 记录字符N的位置

					// 按十进制获取字符N后面的数值
					gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
					
					// 若当前行号不等于上一行号 且 cmdbuffer中不包含M110
					if(gcode_N != gcode_LastN+1 && (strstr(cmdbuffer[bufindw], PSTR("M110"))==NULL) )  
					{
						DLION_DEBUG("ERR: gcode linenumber err, [%ld != %ld + 1]\r\n",gcode_N,gcode_LastN);
						/*--------------------------------------------------------
						Error:Line Number is not Last Line Number+1, Last Line: 0
						Resend: 1
						ok
						----------------------------------------------------------*/
						SERIAL_ERROR_START;
						DLION_PRINTF(MSG_ERR_LINE_NO);
						DLION_PRINTF("%ld\n",gcode_LastN);
						FlushSerialRequestResend();
						serial_count = 0;
						return;
					}

					if(strchr(cmdbuffer[bufindw], '*') != NULL)	// 若存在字符*
					{
						u8 checksum = 0;
						u8 count = 0;

						// *之前的内容求异或校验和
						while(cmdbuffer[bufindw][count] != '*') checksum = checksum^cmdbuffer[bufindw][count++];
						
						strchr_pointer = strchr(cmdbuffer[bufindw], '*');	// 获取*位置

						// 获取*后面的浮点数，与校验和比较，若不相等，则校验失败，输出错误信息
						if( (u8)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum)
						{                                                                 
							DLION_DEBUG("ERR: gcode checksum err, [%s]\r\n",cmdbuffer[bufindw]);
							/*--------------------------------------------------------
							Error:checksum mismatch, Last Line: 2
							Resend: 3
							ok
							----------------------------------------------------------*/
							SERIAL_ERROR_START;
							DLION_PRINTF(MSG_ERR_CHECKSUM_MISMATCH);
							DLION_PRINTF("%ld\n",gcode_LastN);
							FlushSerialRequestResend();
						
						#if 0
							DLION_DEBUG(" checksum: %d\r\n",checksum);
							count = 0;
							DLION_DEBUG(" '");
							while(cmdbuffer[bufindw][count] != '*')
							{
								DLION_DEBUG("%c",cmdbuffer[bufindw][count++]);
							}
							DLION_DEBUG(" '\r\n ");
							
							checksum = 0;
							count = 0;
							while(cmdbuffer[bufindw][count] != '*')
							{ 
								DLION_DEBUG("cmdbuffer:%d;",cmdbuffer[bufindw][count]);
								checksum = checksum^cmdbuffer[bufindw][count++];
								DLION_DEBUG(" checksum:%d \n\r",checksum);
							}
						#endif
							
							serial_count = 0;
							return;
						}
						//if no errors, continue parsing
					}
					else	// 若不存在字符*
					{
						DLION_DEBUG("RX: [%s]\r\n",cmdbuffer[bufindw]);
						
						if(strstr(cmdbuffer[bufindw], PSTR("M110"))!=NULL)
						{}
						else// 包含N且不包含*的命令，若也不是 M110 Nxx，则是不正常的命令格式
						{
							SERIAL_ERROR_START;
							DLION_PRINTF(MSG_ERR_NO_CHECKSUM);
							DLION_PRINTF("%ld",gcode_LastN);
							FlushSerialRequestResend();
							serial_count = 0;
							return;
						}
					}

					gcode_LastN = gcode_N;
					//if no errors, continue parsing
				}
				else //if(strchr(cmdbuffer[bufindw], 'N')==NULL)
				{	// 没收到N，依然收到*，不正常
					if((strchr(cmdbuffer[bufindw], '*') != NULL))                                                            
					{
					DLION_DEBUG("ERR: gcode format err, [%s]\r\n",cmdbuffer[bufindw]);
						SERIAL_ERROR_START;
						DLION_PRINTF(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
						DLION_PRINTF("%ld",gcode_LastN);
						serial_count = 0;
						return;
					}
				}

				// 若存在字符G
				if((strchr(cmdbuffer[bufindw], 'G') != NULL))                                                                
				{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'G');// 获取G的位置

					// 获取G后面的数值
					switch((int)((strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL))))
					{
						case 0:
						case 1:
						case 2:
						case 3:
							// If printer is stopped by an error the G[0-3] codes are ignored.
							// 但此次并未看到对G0/1/2/3的ignore操作 ？？？？？？？？？？？？？？？
							if(Stopped == FALSE) { 
							#ifdef SDSUPPORT
								if(card.saving)
									break;
							#endif //SDSUPPORT

							/*gcode都还没处理，此处不能返回OK*/
							//	DLION_PRINTF(MSG_OK);
							//	DLION_PRINTF("\n");
							}
							else {
								DLION_PRINTF(MSG_ERR_STOPPED);
							 // LCD_MESSAGEPGM(MSG_STOPPED);
							}
							break;
						default:
							break;
					}
				}

				// 若为 M112 - Emergency Stop
				if( strcmp(cmdbuffer[bufindw], "M112") == 0 )
				{
					kill();		//If command was e-stop process now
				}
				
				bufindw = (bufindw + 1)%BUFSIZE;	// buf写指针后移
				buflen += 1;//sanse
			}
			
			serial_count = 0; //clear buffer
		}

		else
		{
			if(serial_char == ';') comment_mode = TRUE;	// 若为分号，则标记此行之后的部分均为注释部分

			// 若不是注释，则把该字符存入 GCODE指令缓冲区
			if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
		}
	}


/*---------------------------------------------------------------------------------*/
  #ifdef SDSUPPORT   //sanse
  if(!card.sdprinting || serial_count!=0 )//确保启动读卡控制、新的一行gcode
        { 
    return;
   }
    
  sdCardUnplug.firstFilePos = file_pos;//新的一行gcode行首
  while( !card_eof()  && buflen < BUFSIZE ) 
  {    
    if(SD_CD)//确保在读取一行gcode过程中SD卡在位
    {
        
        serial_count = 0;
        return;
    }
    n=card_get();
    serial_char = (BYTE)n;
    
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       (serial_char == ':' && comment_mode == FALSE) ||
       serial_count >= (MAX_CMD_SIZE - 1)||
             n==-1)
    { 
      if(card_eof() )//文件读取完毕,
            {
        DLION_PRINTF(MSG_FILE_PRINTED);
            DLION_PRINTF("\n");
            stoptime = millis() - starttime; //正常打印结束，记录此时结束时间

        card_printingHasFinished();
        DLION_PRINTF("card print finished\r\n");
       // card_checkautostart(TRUE);

      }
      if(!serial_count)
      {
        comment_mode = FALSE; //for new command
        return; //if empty line
      }

       cmdbuffer[bufindw][serial_count] = 0; //terminate string
      
        line_no ++;

//      if(!comment_mode){
        fromsd[bufindw] = TRUE;//sanse
        buflen += 1;
        bufindw = (bufindw + 1)%BUFSIZE;
//      }
    
        DLION_PRINTF("->line_no: %d\r\n",line_no);

      comment_mode = FALSE; //for new command
      serial_count = 0; //clear buffer
    }
    else
    {
      if(serial_char == ';') comment_mode = TRUE;//;0x3b
      if(!comment_mode) 
      {
         cmdbuffer[bufindw][serial_count++] = serial_char;        
      }
      
    }
  }
  #endif //SDSUPPORT

}


 float code_value(void)
 {

  return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}

long code_value_long(void)
{
  return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

u8 code_seen(char code)
{
  strchr_pointer = strchr(cmdbuffer[bufindr], code);
  return (strchr_pointer != NULL);  //Return True if a character was found
}


void axis_is_at_home(int axis) 
{
  current_position[axis] = base_home_pos[axis] + add_homeing[axis];
  min_pos[axis] =          base_min_pos[axis] + add_homeing[axis];
  max_pos[axis] =          base_max_pos[axis] + add_homeing[axis];
}
#define HOMEAXIS_DO(LETTER) (( LETTER##_HOME_DIR==-1) || (LETTER##_HOME_DIR==1))
 void homeaxis(int axis) 
{
  if (axis==X_AXIS ? HOMEAXIS_DO(X) :
      axis==Y_AXIS ? HOMEAXIS_DO(Y) :
      axis==Z_AXIS ? HOMEAXIS_DO(Z) :
      0)     //
   {
    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    destination[axis] = 1.5 * max_length[axis] * home_dir[axis];
    feedrate = homing_feedrate[axis];
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    destination[axis] = -home_retract_mm[axis] * home_dir[axis];
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    destination[axis] = 2*home_retract_mm[axis] * home_dir[axis];
    feedrate = homing_feedrate[axis]/2 ;
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    axis_is_at_home(axis);
    destination[axis] = current_position[axis];
    feedrate = 0.0;
    endstops_hit_on_purpose();
  }
}
#define HOMEAXIS(LETTER) homeaxis(LETTER##_AXIS)




void process_commands(void)
{ 
//unsigned long codenum; //throw away variable
//  char *starpos = NULL;
//  int8_t i;
 // u8 cnt=0;
  if(code_seen('G'))
  {
    switch((int)code_value())
    {
    case 0: // G0 -> G1
    case 1: // G1
    Gcode_G0G1();
      break;
      
    case 2: // G2  - CW ARC
    Gcode_G2();
       break;
       
    case 3: // G3  - CCW ARC
    Gcode_G3();
       break;
       
    case 4: // G4 dwell
    Gcode_G4();
      break;
      

      case 10: // G10 retract
    Gcode_G10();
      break;
      
      case 11: // G11 retract_recover
      Gcode_G11();
      break;
      

      
    case 28: //G28 Home all Axis one at a time
    Gcode_G28();
      break;
      
    case 90: // G90
     Gcode_G90();
      break;
      
    case 91: // G91
    Gcode_G91();
      break;
      
    case 92: // G92
    Gcode_G92();
      break;
    }
  }
  else if(code_seen('M'))
  {    
  switch( (int)code_value() )
    {      
        case 0: // M0 - Unconditional stop - Wait for user button press on LCD
        case 1: // M1 - Conditional stop - Wait for user button press on LCD
        Gcode_M0M1();
            break;
            
            case 15:// M15  - update flash data (font data ; icon data and so on)    
        Gcode_M15();
            break;
            
            case 16:// M16  - screen_adjust
            Gcode_M16();
            break;
            
        case 17:
        Gcode_M17();
            break;
            

            case 19:
        Gcode_M19();
            break;
            
        case 20:
        Gcode_M20();
              break;
              
        case 21:     
              Gcode_M21();
              break;
              
        case 22: 
              Gcode_M22();
              break;
              
        case 23:
        Gcode_M23();
              break;
              
        case 24:
        Gcode_M24();
              break;
              
        case 25: 
             Gcode_M25();
              break;
              
        case 26: 
        Gcode_M26();
              break;
              
        case 27: 
              Gcode_M27();
              break;
              
        case 28:
        Gcode_M28();
              break;
              
        case 29:
        Gcode_M29();
              break;
              
        case 30: 
        Gcode_M30();
              break;

        case 31: 
        Gcode_M31();
              break;
              
        case 42: 
        Gcode_M42();
             break;
             
        case 104: // M104
        Gcode_M104();
              break;
              
        case 140: 
             Gcode_M140();
              break;
              
        case 105 : 
        Gcode_M105();
         //     break;
          return; // "ok" 已经打印了
		  
        case 109:
        Gcode_M109();
        break;
        
		case 110:
		Gcode_M110();
		break;

        case 190: // M190 - Wait for bed heater to reach target.
        Gcode_M190();
        break;

          case 106: //M106 Fan On
          Gcode_M106();
            break;
            
          case 107: //M107 Fan Off
        Gcode_M107();
            break;
            
          case 126: //M126 valve open
          Gcode_M126();
            break;
            
          case 127: //M127 valve closed
        Gcode_M127();
            break;
            
         case 128://M128 valve open
        Gcode_M128();
            break;
            
          case 129: //M129 valve closed
        Gcode_M129();
            break;
            
         case 80: // M80 - ATX Power On
        Gcode_M80();
            break;
            
         case 81: // M81 - ATX Power Off
         Gcode_M81();
             break;
            
         case 82:
           Gcode_M82();
            break;
         case 83:
          Gcode_M83();
            break;
            
         case 18: //compatibility
         case 84: // M84
        Gcode_M18M84();
             break;
             
          case 85: // M85
        Gcode_M85();
             break;
             
          case 92: // M92
        Gcode_M92();
             break;
             
          case 115: // M115
              Gcode_M115();
              break;
              
          case 117: // M117 display message        /////////////////////////////////////////////////////
            Gcode_M117();
              break;
              
          case 114: // M114     changed by tony
          Gcode_M114();
              break; 
              
           case 120: // M120
              Gcode_M120();
              break;
              
           case 121: // M121
              Gcode_M121();
              break;
              
           case 119: // M119
           Gcode_M119();
              break;
              
              //TODO: update for all axis, use for loop
          case 201: // M201     
            Gcode_M201();
              break;
              
          case 202: // M202
            Gcode_M202();
              break;
          case 203: // M203 max feedrate mm/sec
        Gcode_M203();
              break; 
          case 204: // M204 acclereration S normal moves T filmanent only moves
        Gcode_M204();
              break;
              
          case 205: //M205 advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk
            Gcode_M205();
            break;
          case 206: // M206 additional homeing offset
        Gcode_M206();
              break;
              

            case 207: //M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
        Gcode_M207();
            break;
            
            case 208: // M208 - set retract recover length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
            Gcode_M208();
            break;
            
            case 209: // M209 - S<1=TRUE/0=FALSE> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
            Gcode_M209();
            break;
            
            

            case 218: // M218 - set hotend offset (in mm), T<extruder_number> X<offset_on_X> Y<offset_on_Y>
            Gcode_M218();
                break;

           case 220: // M220 S<factor in percent>- set speed factor override percentage
            Gcode_M220();
              break;
              
           case 221: // M221 S<factor in percent>- set extrude factor override percentage
            Gcode_M221();
              break;
              

           case 280: // M280 - set servo position absolute. P: servo index, S: angle or microseconds
            Gcode_M280();
              break;


            case 300: // M300
            Gcode_M300();
            break;

       
        case 301: // M301
        Gcode_M301();
          break;
        
       
        case 304: // M304
        Gcode_M304();
          break;

        case 240: // M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/
        Gcode_M240();
            break;
            
            case 302: // allow cold extrudes
        Gcode_M302();    
            break;
            
            case 303: // M303 PID autotune
        Gcode_M303();
            break;
            
            case 400: // M400 finish all moves
        Gcode_M400();
            break;
            
            case 500: // M500 Store settings in EEPROM
        Gcode_M500();
            break;
            
            case 501: // M501 Read settings from EEPROM
        Gcode_M501();
            break;
            
            case 502: // M502 Revert to default settings
        Gcode_M502();
            break;
            
            case 503: // M503 print settings currently in memory
        Gcode_M503();
            break;
            

         case 540:
        Gcode_M540();
            break;
        


        case 600: //Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
        Gcode_M600();
        break;
        
        case 907: // M907 Set digital trimpot motor current using axis codes.
        Gcode_M907();
        break;
        
        case 908: // M908 Control digital trimpot directly.
        Gcode_M908();
        break;
        
        case 350: // M350 Set microstepping mode. Warning: Steps per unit remains unchanged. S code sets stepping mode for all drivers.
        Gcode_M350();
        break;
        
        case 351: // M351 Toggle MS1 MS2 pins directly, S# determines MS1 or MS2, X# sets the pin high/low.
        Gcode_M351();
        break;
        
    case 999: // M999: Restart after being stopped
    Gcode_M999();
        break;        
    
    }//end switch(int) code_value();

  }
   else if(code_seen('T'))
   {
    Gcode_T();
   }//end else if(code_seen('T'))
  else
  { 
    Gcode_Error();
  }
   ClearToSend();
}

void FlushSerialRequestResend()
{
  
  MYSERIAL_flush();
  DLION_PRINTF(MSG_RESEND);
  DLION_PRINTF("%d\n",gcode_LastN + 1);
  ClearToSend();
}

void ClearToSend()
{
  previous_millis_cmd = millis();
  #ifdef SDSUPPORT
//  if(fromsd[bufindr])
  //  return;
  #endif //SDSUPPORT
  
  DLION_PRINTF(MSG_OK);
  DLION_PRINTF("\n");
//printf("\r\n");
//  printf(MSG_OK);
//printf("\r\n");

}

void get_coordinates()
{ int8_t i;
  volatile u8 seen[4]={FALSE,FALSE,FALSE,FALSE};
  for( i=0; i < NUM_AXIS; i++) {
    if(code_seen(axis_codes[i]))
    {
      destination[i] = (float)code_value() + (axis_relative_modes[i] || relative_mode)*current_position[i];
      seen[i]=TRUE;
    }
    else destination[i] = current_position[i]; //Are these else lines really needed?
    #ifdef DLION_PRINTF 
    //DLION_PRINTF("des[%d]：%f \n",i,destination[i]);
    #endif

  }
  if(code_seen('F')) {
    next_feedrate = code_value();
    if(next_feedrate > 0.0) feedrate = next_feedrate;
  }
      #ifdef DLION_PRINTF 
    //DLION_PRINTF(" feedrate：%f \n",feedrate);
    #endif

  #ifdef FWRETRACT
  if(autoretract_enabled)
  if( !(seen[X_AXIS] || seen[Y_AXIS] || seen[Z_AXIS]) && seen[E_AXIS])
  {
    float echange=destination[E_AXIS]-current_position[E_AXIS];
    if(echange<-MIN_RETRACT) //retract
    {
      if(!retracted)
      {

      destination[Z_AXIS]+=retract_zlift; //not sure why chaninging current_position negatively does not work.
      //if slicer retracted by echange=-1mm and you want to retract 3mm, corrrectede=-2mm additionally
      float correctede=-echange-retract_length;
      //to generate the additional steps, not the destination is changed, but inversely the current position
      current_position[E_AXIS]+=-correctede;
      feedrate=retract_feedrate;
      retracted=TRUE;
      }

    }
    else
      if(echange>MIN_RETRACT) //retract_recover
    {
      if(retracted)
      {
      //current_position[Z_AXIS]+=-retract_zlift;
      //if slicer retracted_recovered by echange=+1mm and you want to retract_recover 3mm, corrrectede=2mm additionally
      float correctede=-echange+1*retract_length+retract_recover_length; //total unretract=retract_length+retract_recover_length[surplus]
      current_position[E_AXIS]+=correctede; //to generate the additional steps, not the destination is changed, but inversely the current position
      feedrate=retract_recover_feedrate;
      retracted=FALSE;
      }
    }

  }
  #endif //FWRETRACT
}
void get_arc_coordinates(void)
{
#ifdef SF_ARC_FIX
   u8 relative_mode_backup = relative_mode;
   relative_mode = TRUE;
#endif
   get_coordinates();
#ifdef SF_ARC_FIX
   relative_mode=relative_mode_backup;
#endif

   if(code_seen('I')) {
     offset[0] = code_value();
   }
   else {
     offset[0] = 0.0;
   }
   if(code_seen('J')) {
     offset[1] = code_value();
   }
   else {
     offset[1] = 0.0;
   }
}
void clamp_to_software_endstops(float target[3])
{
  if (min_software_endstops) {
    if (target[X_AXIS] < min_pos[X_AXIS]) target[X_AXIS] = min_pos[X_AXIS];
    if (target[Y_AXIS] < min_pos[Y_AXIS]) target[Y_AXIS] = min_pos[Y_AXIS];
    if (target[Z_AXIS] < min_pos[Z_AXIS]) target[Z_AXIS] = min_pos[Z_AXIS];
  }

  if (max_software_endstops) {
    if (target[X_AXIS] > max_pos[X_AXIS]) target[X_AXIS] = max_pos[X_AXIS];
    if (target[Y_AXIS] > max_pos[Y_AXIS]) target[Y_AXIS] = max_pos[Y_AXIS];
    if (target[Z_AXIS] > max_pos[Z_AXIS]) target[Z_AXIS] = max_pos[Z_AXIS];
  }
}

void prepare_move(void)
{  int8_t i;
  clamp_to_software_endstops(destination);
  previous_millis_cmd = millis();
  // Do not use feedmultiply for E or Z only moves
  if( (current_position[X_AXIS] == destination [X_AXIS]) && (current_position[Y_AXIS] == destination [Y_AXIS])) {
      plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
  }
  else {
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate*feedmultiply/60/100.0, active_extruder);
  }
  for(i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i];//提前更新当前位置，因为planer的命令缓冲区是以目标位置destination执行，所以，当该缓冲区执行完毕时，destination为当前位置，在打印界面，暂停触发时，就要保存目标位置
        #ifdef DLION_PRINTF 
    //DLION_PRINTF("cur[%d]：%f \n",i,current_position[i]);
    #endif
  }
}


void Cmd_MoveAxisCommand(float *pDestination)
{

    u8 i;
    destination[X_AXIS] = pDestination[X_AXIS];
    destination[Y_AXIS] = pDestination[Y_AXIS];
    destination[Z_AXIS] = pDestination[Z_AXIS];
    destination[E_AXIS] = pDestination[E_AXIS];

    feedrate = homing_feedrate[X_AXIS];

    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
    for (i = 0; i < NUM_AXIS; i++)    current_position[i] = destination[i];
    st_synchronize_no_lcd_pause_resume();
    
}

void prepare_arc_move(u8 isclockwise) {
    int8_t i;
  float r = hypot(offset[X_AXIS], offset[Y_AXIS]); // Compute arc radius for mc_arc

  // Trace the arc
  mc_arc(current_position, destination, offset, X_AXIS, Y_AXIS, Z_AXIS, feedrate*feedmultiply/60/100.0, r, isclockwise, active_extruder);

  // As far as the parser is concerned, the position is now == target. In reality the
  // motion control system might still be processing the action and the real tool position
  // in any intermediate location.
  for(i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i];
  }
  previous_millis_cmd = millis();
}

void manage_inactivity(void)
{
  if( (millis() - previous_millis_cmd) >  max_inactive_time )
    if(max_inactive_time)
      kill();
  if(stepper_inactive_time)  {
    if( (millis() - previous_millis_cmd) >  stepper_inactive_time )
    {
      if(blocks_queued() == FALSE ) //确保暂停打印过程中点击锁定
          {
          
//        disable_x();
//        disable_y();
//        disable_z();
//        disable_e0();
//        disable_e1();
     //   disable_e2();
      }
    }
  }
  check_axes_activity();
}

void kill(void)
{
  CRITICAL_SECTION_START; // Stop interrupts
	
  disable_heater();

  disable_x();
  disable_y();
  disable_z();
  disable_e0();
  disable_e1();
 // disable_e2();

//#if defined(PS_ON_PIN) && PS_ON_PIN > -1
//  pinMode(PS_ON_PIN,INPUT);
//#endif  
  SERIAL_ERROR_START;
  DLION_PRINTF(MSG_ERR_KILLED);
 // LCD_ALERTMESSAGEPGM(MSG_KILLED);
//  suicide();
	
  IWDG_Init(IWDG_Prescaler_64, 625);    //与分频数为64,重载值为625,溢出时间为1s       
  while(1) {
  /* Intentionally left empty */  
	  
//		printf("%d ", RCC_GetFlagStatus(RCC_FLAG_IWDGRST));
//		bsp_DelayMS(100);
  } // Wait for reset
}

void Stop(void)
{
  disable_heater();
  if(Stopped == FALSE) {

    Stopped = TRUE;
    Stopped_gcode_LastN = gcode_LastN; // Save last g_code for restart
    SERIAL_ERROR_START;
    DLION_PRINTF(MSG_ERR_STOPPED);
//    DLION_DEBUG(MSG_ERR_STOPPED);
  }
}

u8 IsStopped(void) 
{
    return Stopped; 
}
u8 setTargetedHotend(int code)
{
  tmp_extruder = active_extruder;
  if(code_seen('T')) 
    {
    tmp_extruder = code_value();
    if(tmp_extruder >= EXTRUDERS) 
        {
      SERIAL_ECHO_START;
      switch(code)
            {
        case 104:
          DLION_PRINTF(MSG_M104_INVALID_EXTRUDER);
          break;
        case 105:
          DLION_PRINTF(MSG_M105_INVALID_EXTRUDER);
          break;
        case 109:
          DLION_PRINTF(MSG_M109_INVALID_EXTRUDER);
          break;
        case 218:
          DLION_PRINTF(MSG_M218_INVALID_EXTRUDER);
          break;
      }
      DLION_PRINTF("%d",tmp_extruder);
      return TRUE;
    }
  }
  return FALSE ;
}

void clear_cmd_buf(void)
{
    buflen = 0;
    bufindr = 0;
    bufindw = 0;    
    DLION_PRINTF("clr cmd buf\r\n");
}

static void Gcode_G0G1(void)
{
    //  if(Stopped == FALSE) //added by tony
      {
        get_coordinates(); // For X Y Z E F
        prepare_move();
        //  DLION_PRINTF("\n");
        //ClearToSend();
//        return;
      }
}
static void Gcode_G2(void)
{
     // if(Stopped == FALSE) //added by tony
      {
        get_arc_coordinates();
        prepare_arc_move(TRUE);
      }
}

static void Gcode_G3(void)
{
    //  if(Stopped == FALSE) //added by tony
      {
        get_arc_coordinates();
        prepare_arc_move(FALSE);
      }
}

static void Gcode_G4(void)
{
unsigned long codenum; //throw away variable
     // LCD_MESSAGEPGM(MSG_DWELL); ///////////////////////////////////////////////////////////
      codenum = 0;
      if(code_seen('P')) codenum = code_value(); // milliseconds to wait
      if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait

      st_synchronize();
      codenum += millis();  // keep track of when we started waiting
      previous_millis_cmd = millis();
      while(millis()  < codenum ){
        manage_heater();
        manage_inactivity();
        lcd_update();    ///////////////////////////////////////////////////////////
      }
}


static void Gcode_G10(void)
{
 #ifdef FWRETRACT       //ONLY PARTIALLY TESTED
      if(!retracted)
      {
        destination[X_AXIS]=current_position[X_AXIS];
        destination[Y_AXIS]=current_position[Y_AXIS];
        destination[Z_AXIS]=current_position[Z_AXIS];
        current_position[Z_AXIS]+=-retract_zlift;
        destination[E_AXIS]=current_position[E_AXIS]-retract_length;
        feedrate=retract_feedrate;
        retracted=TRUE;
        prepare_move();
      }
#endif

}

static void Gcode_G11(void)
{
 #ifdef FWRETRACT       //ONLY PARTIALLY TESTED
      if(!retracted)
      {
        destination[X_AXIS]=current_position[X_AXIS];
        destination[Y_AXIS]=current_position[Y_AXIS];
        destination[Z_AXIS]=current_position[Z_AXIS];

        current_position[Z_AXIS]+=retract_zlift;
        current_position[E_AXIS]+=-retract_recover_length;
        feedrate=retract_recover_feedrate;
        retracted=FALSE;
        prepare_move();
      }
#endif
}


static void Gcode_G28(void)
{
    u8 i = 0;
      saved_feedrate = feedrate;
      saved_feedmultiply = feedmultiply;
      feedmultiply = 100;
      previous_millis_cmd = millis();

      enable_endstops(TRUE);

      for(i=0; i < NUM_AXIS; i++) {
        destination[i] = current_position[i];
      }
      feedrate = 0.0;
      home_all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2])));

      #if Z_HOME_DIR > 0                      // If homing away from BED do Z first
      if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
        HOMEAXIS(Z);
      }
      #endif

      #ifdef QUICK_HOME
      if((home_all_axis)||( code_seen(axis_codes[X_AXIS]) && code_seen(axis_codes[Y_AXIS])) )  //first diagonal move
      {
        current_position[X_AXIS] = 0;current_position[Y_AXIS] = 0;

        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = 1.5 * X_MAX_LENGTH * X_HOME_DIR;destination[Y_AXIS] = 1.5 * Y_MAX_LENGTH * Y_HOME_DIR;
        feedrate = homing_feedrate[X_AXIS];
        if(homing_feedrate[Y_AXIS]<feedrate)
          feedrate =homing_feedrate[Y_AXIS];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();

        axis_is_at_home(X_AXIS);
        axis_is_at_home(Y_AXIS);
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = current_position[X_AXIS];
        destination[Y_AXIS] = current_position[Y_AXIS];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        feedrate = 0.0;
        st_synchronize();
        endstops_hit_on_purpose();
      }
      #endif

      if((home_all_axis) || (code_seen(axis_codes[X_AXIS])))
      {
        HOMEAXIS(X);
      }

      if((home_all_axis) || (code_seen(axis_codes[Y_AXIS]))) {
        HOMEAXIS(Y);
      }

      #if Z_HOME_DIR < 0                      // If homing towards BED do Z last
      if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
        HOMEAXIS(Z);
      }
      #endif

      if(code_seen(axis_codes[X_AXIS]))
      {
        if(code_value_long() != 0) {
          current_position[X_AXIS]=code_value()+add_homeing[0];
        }
      }

      if(code_seen(axis_codes[Y_AXIS])) {
        if(code_value_long() != 0) {
          current_position[Y_AXIS]=code_value()+add_homeing[1];
        }
      }

      if(code_seen(axis_codes[Z_AXIS])) {
        if(code_value_long() != 0) {
          current_position[Z_AXIS]=code_value()+add_homeing[2];
        }
      }
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

      #ifdef ENDSTOPS_ONLY_FOR_HOMING
        enable_endstops(FALSE);
      #endif

      feedrate = saved_feedrate;
      feedmultiply = saved_feedmultiply;
      previous_millis_cmd = millis();
      endstops_hit_on_purpose();

}

static void Gcode_G90(void)
{
	relative_mode = FALSE;
	DLION_DEBUG("absolute mode\r\n");
}
static void Gcode_G91(void)
{
	relative_mode = TRUE;
	DLION_DEBUG("relative mode\r\n");
}
static void Gcode_G92(void)
{
    u8 i;
      if(!code_seen(axis_codes[E_AXIS]))
        st_synchronize();
      for(i=0; i < NUM_AXIS; i++) 
      {
        if(code_seen(axis_codes[i])) 
        {
           if(i == E_AXIS) 
           {
             current_position[i] = code_value();
             plan_set_e_position(current_position[E_AXIS]);
           }
           else 
           {
             current_position[i] = code_value()+add_homeing[i];
             plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
           }
        }
      }
}

static void Gcode_M0M1(void)
{
unsigned long codenum; //throw away variable
            
             #ifdef ULTIPANEL
             // LCD_MESSAGEPGM(MSG_USERWAIT);    /////////////////////////////////////////////////
              codenum = 0;
              if(code_seen('P')) codenum = code_value(); // milliseconds to wait
              if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait
        
              st_synchronize();
              previous_millis_cmd = millis();
              if (codenum > 0){
                codenum += millis();  // keep track of when we started waiting
                while(millis()  < codenum && !LCD_CLICKED){
                  manage_heater();
                  manage_inactivity();
                  lcd_update();     ///////////////////////////////////
                }
              }else{
                while(!LCD_CLICKED){
                  manage_heater();
                  manage_inactivity();
                  lcd_update();     ////////////////////////////////////////
                }
              }
           //   LCD_MESSAGEPGM(MSG_RESUMING); ////////////////////////////////
                 #endif
          
}
static void Gcode_M15(void)
{
    LCD_Clear(WHITE);        
    if(update_font(5,0,12,0)==0)
    {    LCD_ShowString(5,80,200,200,12, "UPDATE SUCCESS!");     
        bsp_DelayMS(500);                        
    }
}
static void Gcode_M16(void)
{
        LCD_Clear(WHITE);
        TP_Adjust();
    DLION_PRINTF("Adjust OK\r\n");
}
static void Gcode_M17(void)
{

    enable_x();
    enable_y();
    enable_z();
    enable_e0();
    enable_e1();
}

static void Gcode_M19(void)
{
#ifdef SDSUPPORT
#endif

}
static void Gcode_M20(void)
{
#ifdef SDSUPPORT
      DLION_PRINTF(MSG_BEGIN_FILE_LIST);
      DLION_PRINTF("\n");
      card_ls();
      DLION_PRINTF(MSG_END_FILE_LIST);
      DLION_PRINTF("\n");
#endif
}
static void Gcode_M21(void)
{
#ifdef SDSUPPORT
    card_initsd();    
#endif    
}
static void Gcode_M22(void)
{
#ifdef SDSUPPORT
    card_release();
#endif
}
static void Gcode_M23(void)
{
#ifdef SDSUPPORT
  char *starpos = NULL;
    DLION_PRINTF("--%s\n",strchr_pointer);
      starpos = (strchr(strchr_pointer + 4,'*'));
      if(starpos!=NULL)
        *(starpos-1)='\0';
      DLION_PRINTF("--%s\n",strchr_pointer + 4);
      card_openFile(strchr_pointer + 4,TRUE);    
#endif
}
static void Gcode_M24(void)
{
#ifdef SDSUPPORT
      card_startFileprint();
      starttime=millis();
#endif
}
static void Gcode_M25(void)
{
#ifdef SDSUPPORT
    card_pauseSDPrint();
#endif
}
static void Gcode_M26(void)
{
#ifdef SDSUPPORT
      if(card.cardOK && code_seen('S')) 
      {
        card_setIndex(code_value_long());
      }
#endif
}
static void Gcode_M27(void)
{
#ifdef SDSUPPORT
    card_getStatus();
#endif
}
static void Gcode_M28(void)
{
#ifdef SDSUPPORT
char *starpos = NULL;
      starpos = (strchr(strchr_pointer + 4,'*'));
     if(starpos != NULL){
        char* npos = strchr(cmdbuffer[bufindr], 'N');
        strchr_pointer = strchr(npos,' ') + 1;
       *(starpos-1) = '\0';
      }
      card_openFile(strchr_pointer+4,FALSE);
#endif
}
static void Gcode_M29(void)
{
#ifdef SDSUPPORT
  //processed in write to file routine above
  //card,saving = FALSE;
  
 #endif
}

static void Gcode_M30(void)
{
#ifdef SDSUPPORT
char *starpos = NULL;
    if (card.cardOK)
    {
        card_closefile();
        starpos = (strchr(strchr_pointer + 4,'*'));
        if(starpos != NULL){
          char* npos = strchr(cmdbuffer[bufindr], 'N');
          strchr_pointer = strchr(npos,' ') + 1;
          *(starpos-1) = '\0';
        }
        card_removeFile(strchr_pointer + 4);
    }
#endif
}    

static void Gcode_M31(void)
{

     // char time[30];
      unsigned long t=(stoptime-starttime)/1000;
      int sec,min;
            stoptime=millis();
      min=t/60;
      sec=t%60;
      DLION_PRINTF("%d min, %d sec", min, sec);
      SERIAL_ECHO_START;
     // DLION_PRINTF("%s\n",time);
      autotempShutdown();
        

}
static void Gcode_M42(void)
{

}
static void Gcode_M104(void)
{
      if(setTargetedHotend(104))
    {
    
      }
      if (code_seen('S')) 
        {    
            setTargetHotend(code_value(), tmp_extruder);    
        }
      setWatch();
}
static void Gcode_M140(void)
{
 if (code_seen('S')) setTargetBed(code_value());
}
static void Gcode_M105(void)
{
	// ok T:-15.00 /0.00 B:-15.00 /0.00 T0:-15.00 /0.00 T1:-15.00 /0.00 @:0 B@:0 @0:0 @1:0
	
	if(setTargetedHotend(105)){}
	
#if defined(TEMP_0_PIN)
	DLION_PRINTF("ok T:%.1f /%.1f",degHotend(tmp_extruder),degTargetHotend(tmp_extruder));//changed by tony

#if defined(TEMP_BED_PIN)
	DLION_PRINTF(" B:%.1f /%.1f",degBed(),degTargetBed());//changed by tony
#endif //GET_TEMP_AD_BED
	
#else
	SERIAL_ERROR_START;
	DLION_PRINTF(MSG_ERR_NO_THERMISTORS);
#endif

	DLION_PRINTF(" @:%d",getHeaterPower(tmp_extruder));
	DLION_PRINTF(" B@:%d\n",getHeaterPower(-1));
	
//	DLION_DEBUG("ok T:%.1f /%.1f",degHotend(tmp_extruder),degTargetHotend(tmp_extruder));
//	DLION_DEBUG(" B:%.1f /%.1f\r\n",degBed(),degTargetBed());
}
static void Gcode_M109(void)
{
    u8 target_direction;
    long residencyStart;
    unsigned long codenum;
    if(setTargetedHotend(109))return;

    #ifdef AUTOTEMP
         autotemp_enabled=FALSE;
    #endif
    if (code_seen('S')) setTargetHotend(code_value(), tmp_extruder);
    #ifdef AUTOTEMP
        if (code_seen('S')) autotemp_min=code_value();
        if (code_seen('B')) autotemp_max=code_value();    
        if (code_seen('F'))
        {
          autotemp_factor=code_value();
          autotemp_enabled=TRUE;
        }
    #endif
    setWatch();
    codenum = millis();
    /* See if we are heating up or cooling down */
    target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
    #ifdef TEMP_RESIDENCY_TIME
        residencyStart = -1;
    /* continue to loop until we have reached the target temp
      _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
        { 
              if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
              
              if( (millis() - codenum) > 1000UL )
                  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
//			DLION_PRINTF("T:%.1f E:%d ",degHotend(tmp_extruder),tmp_extruder);// 每秒打印一次挤出头温度
			DLION_PRINTF("T:%.1f E:%d B:%.1f ",degHotend(tmp_extruder),tmp_extruder,degBed());// 每秒打印一次挤出头温度
               
                    #ifdef TEMP_RESIDENCY_TIME
                      DLION_PRINTF(" W:");
                      if(residencyStart > -1)
                      {
                         codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
                         DLION_PRINTF("%ld\n", codenum );
                      }
                      else
                      {
                         DLION_PRINTF("?\n");
                      }
                    #else
                      DLION_PRINTF("\n");
                    #endif
                    codenum = millis();
                  }
                  manage_heater();
                  manage_inactivity();
                  lcd_update();////////////////////////////////////////////////
                 #ifdef TEMP_RESIDENCY_TIME
                    /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
                      or when current temp falls outside the hysteresis after target temp was reached */
                     if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
                          (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
                          (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
                      {
                        residencyStart = millis();
                      }
                #endif //TEMP_RESIDENCY_TIME
              }
    #else
        while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==FALSE)) ) 
              { if( (millis() - codenum) > 1000UL )
                  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
                    DLION_PRINTF("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
                 
                    #ifdef TEMP_RESIDENCY_TIME
                      DLION_PRINTF(" W:");
                      if(residencyStart > -1)
                      {
                         codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
                         DLION_PRINTF("%ld\n", codenum );
                      }
                      else
                      {
                         DLION_PRINTF("?\n");
                      }
                    #else
                      DLION_PRINTF("\n");
                    #endif
                    codenum = millis();
                  }
                  manage_heater();
                  manage_inactivity();
                          //  lcd_update();////////////////////////////////////////////////
                 #ifdef TEMP_RESIDENCY_TIME
                    /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
                      or when current temp falls outside the hysteresis after target temp was reached */
                     if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
                          (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
                          (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
                      {
                        residencyStart = millis();
                      }
                #endif //TEMP_RESIDENCY_TIME
              }
    #endif //TEMP_RESIDENCY_TIME
   // starttime=millis();//开始计时已经由外部打印确定
    previous_millis_cmd = millis();
}

static void Gcode_M190(void)
{
#if defined(TEMP_BED_PIN)
unsigned long codenum;
   // LCD_MESSAGEPGM(MSG_BED_HEATING);       //////////////////////////////////////////////////////
    if (code_seen('S')) setTargetBed(code_value());
    codenum = millis();
    while(isHeatingBed())
    {
		if( card.FILE_STATE == PS_STOP) break;//还没开始打印时，用户强制取消打印
		
      if(( millis() - codenum) > 1000 ) //Print Temp Reading every 1 second while heating up.
      {
        float tt=degHotend(active_extruder);
        DLION_PRINTF("T:%.1f E:%d B:%.1f\n",tt,active_extruder,degBed());
       
        codenum = millis();
      }
      manage_heater();
      manage_inactivity();
      lcd_update(); /////////////////////////////////////////////////
    }
   // LCD_MESSAGEPGM(MSG_BED_DONE);       //////////////////////////////////////////////////
    previous_millis_cmd = millis();
#endif

}

static void Gcode_M106(void)
{
           #if defined(FAN_PIN) 
                if (code_seen('S')){
                   fanSpeed=constrain(code_value(),0,255);
                  
            //       test(fanSpeed);
            //       soft_pwm[0]=fanSpeed;
                }
                else {
                  fanSpeed=255;
                 /// test(255);
                  //    soft_pwm[0]=255;
                }
                
           #endif //FAN_PIN
}
static void Gcode_M107(void)
{
    fanSpeed = 0;
}
static void Gcode_M126(void)
{
    #ifdef BARICUDA
    // PWM for HEATER_1_PIN
        #if defined(HEATER_1_PIN) 
        if (code_seen('S')){
             ValvePressure=constrain(code_value(),0,255);
          }
          else {
            ValvePressure=255;
          }
              
        #endif
    #endif

}
static void Gcode_M127(void)
{
    #ifdef BARICUDA
    // PWM for HEATER_1_PIN
        #if defined(HEATER_1_PIN) 
              ValvePressure = 0;
        #endif
    #endif
}
static void Gcode_M128(void)
{
    #ifdef BARICUDA
    // PWM for HEATER_1_PIN
    #if defined(HEATER_2_PIN) 
          if (code_seen('S'))
          {
             EtoPPressure=constrain(code_value(),0,255);
          }
          else 
          {
            EtoPPressure=255;
          }
        #endif
    #endif
}
static void Gcode_M129(void)
{
    #ifdef BARICUDA
    // PWM for HEATER_1_PIN
        #if defined(HEATER_2_PIN) 
              EtoPPressure = 0;
        #endif
    #endif
}
static void Gcode_M80(void)
{
     // SET_OUTPUT(PS_ON_PIN); //GND    ////////////////////////////////////////////////////
     //  WRITE(PS_ON_PIN, PS_ON_AWAKE);////////////////////////////////////////////////////
}
static void Gcode_M81(void)
{

}
static void Gcode_M82(void)
{
 axis_relative_modes[3] = FALSE;
}
static void Gcode_M83(void)
{
  axis_relative_modes[3] = TRUE;
}
static void Gcode_M18M84(void)
{
	if(code_seen('S'))
	{
	  stepper_inactive_time = code_value() * 1000;
	}
	else
	{
		u8 all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2]))|| (code_seen(axis_codes[3])));
		if(all_axis)
		{
		  st_synchronize();
		  disable_e0();
		  disable_e1();
		//  disable_e2();
		  finishAndDisableSteppers();
		}
		else
		{
		  st_synchronize();
		  if(code_seen('X')) disable_x();
		  if(code_seen('Y')) disable_y();
		  if(code_seen('Z')) disable_z();
		 // #if ((E0_ENABLE_PIN != X_ENABLE_PIN) && (E1_ENABLE_PIN != Y_ENABLE_PIN)) // Only enable on boards that have seperate ENABLE_PINS
			if(code_seen('E')) {
			  disable_e0();
			  disable_e1();
			//  disable_e2();
			}
		//  #endif
		}
	 }
}
static void Gcode_M85(void)
{
     code_seen('S');
     max_inactive_time = code_value() * 1000;
}
static void Gcode_M92(void)
{
    u8 i;
      for(i=0; i < NUM_AXIS; i++)
      {
        if(code_seen(axis_codes[i]))
        {
          if(i == 3) 
          { // E
            float value = code_value();
            if(value < 20.0) 
            {
              float factor = axis_steps_per_unit[i] / value; // increase e constants if M92 E14 is given for netfab.
              max_e_jerk *= factor;
              max_feedrate[i] *= factor;
              axis_steps_per_sqr_second[i] *= factor;
            }
            axis_steps_per_unit[i] = value;
          }
          else 
          {
            axis_steps_per_unit[i] = code_value();
          }
        }
      }
}


static void Gcode_M115(void)
{
	DLION_PRINTF(MSG_M115_REPORT);
}

static void Gcode_M117(void)
{
char *starpos = NULL;
  starpos = (strchr(strchr_pointer + 5,'*'));
  if(starpos!=NULL)
    *(starpos-1)='\0';
//   lcd_setstatus(strchr_pointer + 5);     //////////////////////////////////////////////////
}
static void Gcode_M114(void)
{
      DLION_PRINTF("X:%f Y:%f Z:%f E:%f",current_position[X_AXIS],current_position[Y_AXIS],current_position[Z_AXIS],current_position[E_AXIS]);            
    DLION_PRINTF(MSG_COUNT_X);
     DLION_PRINTF("%f Y:%f Z:%f\n",((float)st_get_position(X_AXIS))/axis_steps_per_unit[X_AXIS],((float)st_get_position(Y_AXIS))/axis_steps_per_unit[Y_AXIS],((float)st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS]);

}
static void Gcode_M120(void)
{
    enable_endstops(FALSE) ;
}
static void Gcode_M121(void)
{
    enable_endstops(TRUE) ;
}
static void Gcode_M119(void)
{
      DLION_PRINTF(MSG_M119_REPORT);
      DLION_PRINTF("\n");
      #if defined(X_MIN_PIN) 
        DLION_PRINTF(MSG_X_MIN);
        DLION_PRINTF(((X_MIN_PIN==X_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
       // (X_MIN_PIN==X_ENDSTOPS_INVERTING) ? (DLION_PRINTF(MSG_ENDSTOP_HIT)) : (DLION_PRINTF(MSG_ENDSTOP_OPEN));
      #endif
      #if defined(X_MAX_PIN)
        DLION_PRINTF(MSG_X_MAX);
        DLION_PRINTF(((X_MAX_PIN==X_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
      #endif
      #if defined(Y_MIN_PIN)
        DLION_PRINTF(MSG_Y_MIN);
        DLION_PRINTF(((Y_MIN_PIN^Y_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
      #endif
      #if defined(Y_MAX_PIN) 
        DLION_PRINTF(MSG_Y_MAX);
        DLION_PRINTF(((Y_MAX_PIN^Y_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
      #endif
      #if defined(Z_MIN_PIN) 
        DLION_PRINTF(MSG_Z_MIN);
        DLION_PRINTF(((Z_MIN_PIN^Z_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
      #endif
      #if defined(Z_MAX_PIN)
        DLION_PRINTF(MSG_Z_MAX);
        DLION_PRINTF(((Z_MAX_PIN^Z_ENDSTOPS_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
        DLION_PRINTF("\n");
      #endif

}
static void Gcode_M201(void)
{
    u8 i;
      for( i=0; i < NUM_AXIS; i++)
      {
        if(code_seen(axis_codes[i]))
        {
          max_acceleration_units_per_sq_second[i] = code_value();
        }
      }
      // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
      reset_acceleration_rates();
}
static void Gcode_M202(void)
{
    #if 0 // Not used for Sprinter/grbl gen6
    u8 i;
      for(i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) axis_travel_steps_per_sqr_second[i] = code_value() * axis_steps_per_unit[i];
      }
    #endif
}

static void Gcode_M203(void)
{
    u8 i;
      for( i=0; i < NUM_AXIS; i++) 
     {
        if(code_seen(axis_codes[i])) max_feedrate[i] = code_value();
      }
}
static void Gcode_M204(void)
{
    if(code_seen('S')) acceleration = code_value() ;
    if(code_seen('T')) retract_acceleration = code_value() ;
}

static void Gcode_M205(void)
{
  if(code_seen('S')) minimumfeedrate = code_value();
  if(code_seen('T')) mintravelfeedrate = code_value();
  if(code_seen('B')) minsegmenttime = code_value() ;
  if(code_seen('X')) max_xy_jerk = code_value() ;
  if(code_seen('Z')) max_z_jerk = code_value() ;
  if(code_seen('E')) max_e_jerk = code_value() ;
}
static void Gcode_M206(void)
{
    u8 i;
      for( i=0; i < 3; i++)
      {
        if(code_seen(axis_codes[i])) add_homeing[i] = code_value();
      }
}

static void Gcode_M207(void)
{
#ifdef FWRETRACT
  if(code_seen('S'))
  {
    retract_length = code_value() ;
  }
  if(code_seen('F'))
  {
    retract_feedrate = code_value() ;
  }
  if(code_seen('Z'))
  {
    retract_zlift = code_value() ;
  }
 #endif
}
static void Gcode_M208(void)
{
#ifdef FWRETRACT
    if(code_seen('S'))
    {
    retract_recover_length = code_value() ;
    }
    if(code_seen('F'))
    {
    retract_recover_feedrate = code_value() ;
    }
#endif
}
static void Gcode_M209(void)
{
#ifdef FWRETRACT
  if(code_seen('S'))
  {
    int t= code_value() ;
    switch(t)
    {
      case 0: autoretract_enabled=FALSE;retracted=FALSE;break;
      case 1: autoretract_enabled=TRUE;retracted=FALSE;break;
      default:
        SERIAL_ECHO_START;
        DLION_PRINTF(MSG_UNKNOWN_COMMAND);
        DLION_PRINTF("%d",cmdbuffer[bufindr]);
        DLION_PRINTF("\"");
    }
  }
#endif
}




static void Gcode_M218(void)
{
 #if EXTRUDERS > 1
      if(setTargetedHotend(218)){
        break;
      }
      if(code_seen('X'))
      {
        extruder_offset[X_AXIS][tmp_extruder] = code_value();
      }
      if(code_seen('Y'))
      {
        extruder_offset[Y_AXIS][tmp_extruder] = code_value();
      }
      SERIAL_ECHO_START;
      DLION_PRINTF(MSG_HOTEND_OFFSET);
      for(tmp_extruder = 0; tmp_extruder < EXTRUDERS; tmp_extruder++)
      {
         //SERIAL_ECHO(" ");
         DLION_PRINTF(" %f,%f",extruder_offset[X_AXIS][tmp_extruder],extruder_offset[Y_AXIS][tmp_extruder]);
        // SERIAL_ECHO(",");
       //  SERIAL_ECHO(extruder_offset[Y_AXIS][tmp_extruder]);
      }
      DLION_PRINTF("\n");
#endif
}


static void Gcode_M220(void)
{
      if(code_seen('S'))
      {
        feedmultiply = code_value() ;
      }
}
static void Gcode_M221(void)
{
  if(code_seen('S'))
  {
    extrudemultiply = code_value() ;
  }
}


static void Gcode_M280(void)
{
#if NUM_SERVOS > 0
    int servo_index = -1;
    int servo_position = 0;
    if (code_seen('P'))
      servo_index = code_value();
    if (code_seen('S')) {
      servo_position = code_value();
      if ((servo_index >= 0) && (servo_index < NUM_SERVOS)) {
        servos[servo_index].write(servo_position);
      }
      else {
      
      }
    }
    else if (servo_index >= 0) {
      
    }
#endif
}


static void Gcode_M300(void)
{
//  int beepS = 400;
  int beepP = 1000;
  //if(code_seen('S')) beepS = code_value();
  if(code_seen('P')) beepP = code_value();
    BEEP=1;  
    bsp_DelayMS(beepP);
    BEEP=0;

}
static void Gcode_M301(void)
{
#ifdef PIDTEMP
    if(code_seen('P')) Kp = code_value();
    if(code_seen('I')) Ki = scalePID_i(code_value());
    if(code_seen('D')) Kd = scalePID_d(code_value());

    #ifdef PID_ADD_EXTRUSION_RATE
    if(code_seen('C')) Kc = code_value();
    #endif

    updatePID();
    DLION_PRINTF(MSG_OK);
            DLION_PRINTF(" p:%f i:%f d:%f",Kp,unscalePID_i(Ki),unscalePID_d(Kd));

    #ifdef PID_ADD_EXTRUSION_RATE

    //Kc does not have scaling applied above, or in resetting defaults
    DLION_PRINTF(" c:%f",Kc);
    //  SERIAL_PROTOCOL(Kc);
    #endif
    DLION_PRINTF("\n");
#endif
}
static void Gcode_M304(void)
{
#ifdef PIDTEMPBED

    if(code_seen('P')) bedKp = code_value();
    if(code_seen('I')) bedKi = scalePID_i(code_value());
    if(code_seen('D')) bedKd = scalePID_d(code_value());

    updatePID();
    DLION_PRINTF(MSG_OK);
    DLION_PRINTF(" p:%f i:%f d:%f",Kp,unscalePID_i(bedKi,unscalePID_d(bedKd)));
    DLION_PRINTF("\n");
#endif //PIDTEMP
}
static void Gcode_M240(void)
{
#if defined(PHOTOGRAPH_PIN)

#endif
}
static void Gcode_M302(void)
{
  allow_cold_extrudes(TRUE);
}

static void Gcode_M303(void)
{
  float temp = 150.0;
  int e=0;
  int c=5;
  if (code_seen('E')) e=code_value();
    if (e<0)
      temp=70;
  if (code_seen('S')) temp=code_value();
  if (code_seen('C')) c=code_value();
  PID_autotune(temp, e, c);
}
static void Gcode_M400(void)
{
  st_synchronize();
}
static void Gcode_M500(void)
{
//Config_StoreSettings();////////////////////////////////////////////////////////////////////////////////////////
}
static void Gcode_M501(void)
{
//Config_RetrieveSettings();////////////////////////////////////////////////////////////////////////////////////
}
static void Gcode_M502(void)
{
//Config_ResetDefault();//////////////////////////////////////////////////////////////////////////////////////////
}
static void Gcode_M503(void)
{
    Config_PrintSettings();///////////////////////////////////////////////////////////////////////////////////////////
}
static void Gcode_M540(void)
{        
#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
    if(code_seen('S')) abort_on_endstop_hit = code_value() > 0;
#endif
}
static void Gcode_M600(void)
{
#ifdef FILAMENTCHANGEENABLE
    float target[4];
    float lastpos[4];
    target[X_AXIS]=current_position[X_AXIS];
    target[Y_AXIS]=current_position[Y_AXIS];
    target[Z_AXIS]=current_position[Z_AXIS];
    target[E_AXIS]=current_position[E_AXIS];
    lastpos[X_AXIS]=current_position[X_AXIS];
    lastpos[Y_AXIS]=current_position[Y_AXIS];
    lastpos[Z_AXIS]=current_position[Z_AXIS];
    lastpos[E_AXIS]=current_position[E_AXIS];
    //retract by E
    if(code_seen('E'))
    {
      target[E_AXIS]+= code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_FIRSTRETRACT
        target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT ;
      #endif
    }
    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);

    //lift Z
    if(code_seen('Z'))
    {
      target[Z_AXIS]+= code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_ZADD
        target[Z_AXIS]+= FILAMENTCHANGE_ZADD ;
      #endif
    }
    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);

    //move xy
    if(code_seen('X'))
    {
      target[X_AXIS]+= code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_XPOS
        target[X_AXIS]= FILAMENTCHANGE_XPOS ;
      #endif
    }
    if(code_seen('Y'))
    {
      target[Y_AXIS]= code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_YPOS
        target[Y_AXIS]= FILAMENTCHANGE_YPOS ;
      #endif
    }

    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);

    if(code_seen('L'))
    {
      target[E_AXIS]+= code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_FINALRETRACT
        target[E_AXIS]+= FILAMENTCHANGE_FINALRETRACT ;
      #endif
    }

    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);

    //finish moves
    st_synchronize();
    //disable extruder steppers so filament can be removed
    disable_e0();
    disable_e1();
    disable_e2();
    bsp_DelayMS(100);
  
    cnt=0;
     while(!lcd_clicked){
      cnt++;
       manage_heater();
       manage_inactivity();
       lcd_update();
       if(cnt==0)
       {
        beep();
       }
     }

    //return to normal
    if(code_seen('L'))
    {
      target[E_AXIS]+= -code_value();
    }
    else
    {
      #ifdef FILAMENTCHANGE_FINALRETRACT
        target[E_AXIS]+=(-1)*FILAMENTCHANGE_FINALRETRACT ;
      #endif
    }
    current_position[E_AXIS]=target[E_AXIS]; //the long retract of L is compensated by manual filament feeding
    plan_set_e_position(current_position[E_AXIS]);
    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //should do nothing
    plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //move xy back
    plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //move z back
    plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], lastpos[E_AXIS], feedrate/60, active_extruder); //final untretract

#endif
}

static void Gcode_M907(void)
{
    u8 i;
    for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) digipot_current(i,code_value());
    if(code_seen('B')) digipot_current(4,code_value());
    if(code_seen('S')) for(i=0;i<=4;i++) digipot_current(i,code_value());
}
static void Gcode_M908(void)
{
    uint8_t channel,current;
    if(code_seen('P')) channel=code_value();
    if(code_seen('S')) current=code_value();
    digipot_current(channel, current);
}

static void Gcode_M350(void)
{ 
    u8 i;
    if(code_seen('S')) for( i=0;i<=4;i++) microstep_mode(i,code_value());
    for( i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_mode(i,(uint8_t)code_value());
    if(code_seen('B')) microstep_mode(4,code_value());
    microstep_readings();
}
static void Gcode_M351(void)
{
    u8 i;
  if(code_seen('S')) switch((int)code_value())
  {
    case 1:
      for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,code_value(),-1,-1);
      if(code_seen('B')) microstep_ms(4,code_value(),-1,-1);
      break;
      
    case 2:
      for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,-1,code_value(),-1);
      if(code_seen('B')) microstep_ms(4,-1,code_value(),-1);
      break;
      
    case 3:
      for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,-1,-1,code_value());
      if(code_seen('B')) microstep_ms(4,-1,-1,code_value());
      break;
  }
  microstep_readings();

}
static void Gcode_M999(void)
{
  Stopped = FALSE;
//  lcd_reset_alert_level();//////////////////////////////////////
  gcode_LastN = Stopped_gcode_LastN;
  FlushSerialRequestResend();
}

static void Gcode_M110(void)
{
	// M110 N0
	// N123 M110 N0*125
	
	long line;
	char tmpstr[MAX_CMD_SIZE];
	char *p;
	
	strcpy(tmpstr, cmdbuffer[bufindr]);
	p = strchr(cmdbuffer[bufindr], 'M');	// p定位到字符M位置
	p = strchr(p, 'N');						// 从M位置开始查找N
	if(p==NULL)		// 若M后面没有N
		p = strchr(tmpstr, 'N');			// 从开头位置开始查找N
	
	line = strtod(p+1, NULL);				// 去N后面的数值
	gcode_LastN = line;
	DBG("Set gcode line number = %ld\r\n", line);
}



static void Gcode_T(void)
{
    tmp_extruder = code_value();
    if(tmp_extruder >= EXTRUDERS) 
    {
      SERIAL_ECHO_START;
      DLION_PRINTF("T%d",tmp_extruder);
     // SERIAL_ECHO(tmp_extruder);
      DLION_PRINTF(MSG_INVALID_EXTRUDER);
    }
    else 
    {
    volatile u8 make_move = FALSE;
      if(code_seen('F')) 
      {
        make_move = TRUE;
        next_feedrate = code_value();
        if(next_feedrate > 0.0) {
          feedrate = next_feedrate;
        }
      }
      #if EXTRUDERS > 1
      if(tmp_extruder != active_extruder) 
      {
        // Save current position to return to after applying extruder offset
        memcpy(destination, current_position, sizeof(destination));
        // Offset extruder (only by XY)
        for(i = 0; i < 2; i++) 
                    {
           current_position[i] = current_position[i] -
                                 extruder_offset[i][active_extruder] +
                                 extruder_offset[i][tmp_extruder];
        }
        // Set the new active extruder and position
        active_extruder = tmp_extruder;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        // Move to the old position if 'F' was in the parameters
        if(make_move && Stopped == FALSE) {
           prepare_move();
        }
      }    //end   if(tmp_extruder != active_extruder) 
       #endif
      SERIAL_ECHO_START;
      DLION_PRINTF(MSG_ACTIVE_EXTRUDER);
      DLION_PRINTF("%d",active_extruder);
      DLION_PRINTF("\n");
    }
}
static void Gcode_Error(void)
{
    SERIAL_ECHO_START;
    DLION_PRINTF(MSG_UNKNOWN_COMMAND);
    DLION_PRINTF("%s",cmdbuffer[bufindr]);
    DLION_PRINTF("\"");
}
//if Rsense unplug again,must send a restart move cmd,ie,set Stopped = FALSE;
u8 Get_Move_State(void)
{

    return Stopped ;

}