#include "pvd.h"
#include "temperature.h"
#include "Dlion.h"
#include "gcodeplayer.h"
#include "planner.h"
#include "gcodeplayer.h"
#include "print.h"
#include "debug.h"  

volatile u8 paueStartFlag ;//断电时是否处于暂停/恢复打印过程中标志，1--yes，0--No


//PA0--EXTI0检测外部掉电，触发--下降沿
void power_vol_down_detect_init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(POWER_DOWN_DETECT_PORT_CLK, ENABLE);
  
  /* Configure PA.00 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = POWER_DOWN_DETECT_IO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(POWER_DOWN_DETECT_PORT, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(POWER_DOWN_DETECT_EXTI_SOURCE, POWER_DOWN_DETECT_EXTI_IO);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = POWER_DOWN_DETECT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//优先级最高
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void POWER_DOWN_DETECT_INT(void)
{
	u32 k;
	u8 res;
  if(EXTI_GetITStatus(POWER_DOWN_DETECT_EXTI_LINE) != RESET)
  {
      /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(POWER_DOWN_DETECT_EXTI_LINE);
	
	DBG("#power down\r\n");
	power_down_callBack();

//	if(eeprom_is_power_off_happen() == PW_OFF_NOT_HAPPEN || card.FILE_STATE == PS_NO)
	{
			LCD_LED=0;				//关背光

		
//			RESET_USER = 0;
//			for(k = 0; k < 12500; k++);
//			//for(k = 0; k < 500; k++);
//			RESET_USER = 1;
		
			SoftReset();
	}
  }
 
}

void power_down_callBack(void)
{
	u8 i;
	u8 unExcuteBlockCount;
	u8 unExcuteCardBuffCount;

	u8 fanValue;
	u16 tempFnLength;
	u16 fileNameLength;
	u32 saveLineNum;
	
	u32 printTime;

	u32 k;
	int rvt = -1;
	float position[NUM_AXIS];
	float percet;
	float pvdBedTemp;
	float pvdExt0Temp;
	
	u8 *tempFileName = NULL;
	u8 dontExacuteGcodeNum;
	u8 tempDontExacuteGcodeNum;
	
	fileNameLength = strlen((const char*)printFileName) + 1;//加上'\0'
	
	tempFileName = mymalloc(SRAMIN,fileNameLength);
	if(tempFileName==NULL) return;
	
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN) return;//上一次断电续打未完成，本次断电不保存参数
	
	percet = ((float)file_pos/fil_size)*100;//断电续打大于预定的百分数才有效
	if(percet<5.0 || file_pos>=fil_size || card.FILE_STATE == PS_STOP) return;//已开始打印，但未打印完，且打印机状态不处于 停止打印
	
	
	if(PRINT_STATE ==HAS_EXACUTE_PAUSE)//断电时处于暂停阶段，可能是暂停靠边过程中断电，可能是恢复打印过程中断电
	{
		Stepper_SaveGetPositions(bck_position);
	}
	else if(PRINT_STATE == NORMAL_PLAY)
	{
		Stepper_GetPositions(bck_position);
	}

	eeprom_save_axis_position(bck_position);	
	unExcuteBlockCount = Plan_GetBlocksInBufFromInterrupt();
	plan_init();		
	
	unExcuteCardBuffCount = buflen;
	
	if(paueStartFlag == 1 ) unExcuteBlockCount = 0;//除去暂停/恢复打印的那条用户手动添加的gcode指令
	
	saveLineNum = line_no - unExcuteBlockCount - unExcuteCardBuffCount;
	dontExacuteGcodeNum = unExcuteBlockCount + unExcuteCardBuffCount;
//			if(saveLineNum >= 2)
//			saveLineNum -=1;//断电前一条gcode

	file_pos = sdCardUnplug.firstFilePos;//保存gcode行首的文件位置
	eeprom_save_not_exacute_gcode_num(dontExacuteGcodeNum);
	
	eeprom_save_print_percentage(&saveLineNum, &file_pos);//保存断电时执行的gcode命令行号
	eeprom_save_print_time(&escape_time);//保存打印时间
	eeprom_save_file_name((u8*)printFileName,&fileNameLength);//保存文件名

	if(filaTime.heating == TRUE || filaTime.firstSet == TRUE)	
	{
		pvdBedTemp = filaTime.bck_bedTep;
		pvdExt0Temp = filaTime.bck_ex0Tep;
	}
	else 	
	{
		pvdBedTemp = current_temperature_bed;
		pvdExt0Temp =current_temperature[0];
	}

	eeprom_save_temp(&pvdBedTemp, &pvdExt0Temp);//保存温度
	eeprom_save_print_fan(fanSpeed);
	
	
	//读保存的参数供校验使用
	eeprom_get_not_exacute_gcode_num(&tempDontExacuteGcodeNum);
	eeprom_get_axis_position(position);	
	eeprom_get_print_percentage(&line_no_test, &file_pos_test);
	eeprom_get_print_time(&printTime);
	eeprom_get_temp(&bck_bed_temperature, &bck_extruder_temperature);
	eeprom_get_print_fan(&fanValue);
	eeprom_get_file_name(tempFileName,&tempFnLength);
	

	//校验
	if(bck_bed_temperature != pvdBedTemp || bck_extruder_temperature != pvdExt0Temp ||
	line_no_test != saveLineNum || file_pos_test != file_pos || 
	escape_time != printTime || fanValue != fanSpeed || 
	tempFnLength != fileNameLength || dontExacuteGcodeNum != tempDontExacuteGcodeNum)
	{
		return;
	}
	
	for(i = 0 ; i < NUM_AXIS ; i++)
	{
		if(bck_position[i] != position[i])
		{
			return;
		}
	}
	
	rvt = strcmp((const char *)printFileName,(const char *)tempFileName);
	if(rvt == 0) 
	{
		LCD_LED=0;				//关背光
		eeprom_save_power_off_status();
//		RESET_USER = 0;
//		for(k = 0; k < 12500; k++);
//		RESET_USER = 1;
	}
	
}
