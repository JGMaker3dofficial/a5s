/*
*********************************************************************************************************
*
*	模块名称 : 备份RAM模块
*	文件名称 : backup_sram.c
*	版    本 : V1.0
*	说    明 : 用于备份内存中重要的数据
*

*********************************************************************************************************
*/


#include "backup_sram.h"
#include "24cxx.h" 
#include "Dlion.h"
#include "temperature.h"
#include "gcodeplayer.h"

float bck_extruder_temperature = {0.0};
float bck_bed_temperature = 0;

float bck_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };

uint32_t line_no_test;
uint32_t file_pos_test;

uint32_t powerDownLineNum;//断电时执行gcode指令的行号
uint8_t  dontExctpPdLineNum;//断电时命令缓冲区+ Planner执行缓冲区中gcode码数目
uint32_t bckGcodeHeadPos;//断电时正在读取的gcode行的行首文件位置
void init_backup_domain(void)
{
    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Enable write access to Backup domain */
    PWR_BackupAccessCmd(ENABLE);


}



_PW_OFF_E is_power_off_happen(void)
{
    uint16_t read_data;
    read_data = BKP_ReadBackupRegister(PW_OFF_STATUS_CACHE_ADDR);
    return ((read_data == PW_OFF_STATUS_VALUE_SET) ? PW_OFF_HAPPEN : PW_OFF_NOT_HAPPEN);

}



void save_power_off_status(void)
{
    BKP_WriteBackupRegister(PW_OFF_STATUS_CACHE_ADDR, PW_OFF_STATUS_VALUE_SET);
}

void clear_power_off_status(void)
{
    BKP_WriteBackupRegister(PW_OFF_STATUS_CACHE_ADDR, PW_OFF_STATUS_VALUE_RESET);
}



//把cur_bed_temp、cur_extr_temp 温度值写入backup registers
void save_temperature(const float * const cur_bed_temp, const float * const cur_extr_temp)
{
    uint16_t  arry[2];
    uint16_t  i;
    for(i = 0 ; i < 2 ; i++ )
    {
        arry[i] = ((uint16_t *) cur_bed_temp)[i];
    }
    BKP_WriteBackupRegister(BED_TEMPERATURE_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(BED_TEMPERATURE_CACHE_H16_ADDR, arry[1]);

    for(i = 0 ; i < 2 ; i++ )
    {
        arry[i] = ((uint16_t *) cur_extr_temp)[i];
    }
    BKP_WriteBackupRegister(EXTRUDER_TEMPERATURE_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(EXTRUDER_TEMPERATURE_CACHE_H16_ADDR, arry[1]);


}

//从backup registers取得温度值，赋值给cur_bed_temp、cur_extr_temp
void get_temperature(float *cur_bed_temp, float *cur_extr_temp)
{
    uint16_t  arry[2];
    uint16_t  i;

    arry[0] = BKP_ReadBackupRegister(BED_TEMPERATURE_CACHE_L16_ADDR);
    arry[1] = BKP_ReadBackupRegister(BED_TEMPERATURE_CACHE_H16_ADDR);
    for(i = 0 ; i < 2 ; i++ )
    {
        ((uint16_t *) cur_bed_temp)[i] = arry[i];
    }

    arry[0] = BKP_ReadBackupRegister(EXTRUDER_TEMPERATURE_CACHE_L16_ADDR);
    arry[1] = BKP_ReadBackupRegister(EXTRUDER_TEMPERATURE_CACHE_H16_ADDR);
    for(i = 0 ; i < 2 ; i++ )
    {
        ((uint16_t *) cur_extr_temp)[i] = arry[i];
    }

}

//把gcode码文件中的行号、文件读指针写入backup registers
void  save_print_percentage(const uint32_t * const line_no, const uint32_t * const file_pos)
{
    uint16_t  arry[2];
    uint16_t  i;
    for(i = 0 ; i < 2 ; i++ )
    {
        arry[i] = ((uint16_t *) line_no)[i];
    }
    BKP_WriteBackupRegister(LINE_NUM_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(LINE_NUM_CACHE_H16_ADDR, arry[1]);

    for(i = 0 ; i < 2 ; i++ )
    {
        arry[i] = ((uint16_t *) file_pos)[i];
    }
    BKP_WriteBackupRegister(FILE_POS_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(FILE_POS_CACHE_H16_ADDR, arry[1]);


}

//从backup registers中取得gcode码文件中的行号、文件读指针
void get_print_percentage(uint32_t *line_no, uint32_t *file_pos)
{
    uint16_t  arry[2];
    uint16_t  i;

    arry[0] = BKP_ReadBackupRegister(LINE_NUM_CACHE_L16_ADDR);
    arry[1] = BKP_ReadBackupRegister(LINE_NUM_CACHE_H16_ADDR);
    for(i = 0 ; i < 2 ; i++ )
    {
        ((uint16_t *) line_no)[i] = arry[i];
    }

    arry[0] = BKP_ReadBackupRegister(FILE_POS_CACHE_L16_ADDR);
    arry[1] = BKP_ReadBackupRegister(FILE_POS_CACHE_H16_ADDR);
    for(i = 0 ; i < 2 ; i++ )
    {
        ((uint16_t *) file_pos)[i] = arry[i];
    }

}

void save_print_time(uint32_t *misc)
{
	uint16_t  arry[2];
    uint16_t  i;
	for(i = 0 ; i < 2 ; i++ )
    {
        arry[i] = ((uint16_t *) misc)[i];
    }
	BKP_WriteBackupRegister(PRINT_TIME_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(PRINT_TIME_CACHE_H16_ADDR, arry[1]);

}

void get_print_time(uint32_t *misc)
{
    uint16_t  arry[2];
    uint16_t  i;

    arry[0] = BKP_ReadBackupRegister(PRINT_TIME_CACHE_L16_ADDR);
    arry[1] = BKP_ReadBackupRegister(PRINT_TIME_CACHE_H16_ADDR);
    for(i = 0 ; i < 2 ; i++ )
    {
        ((uint16_t *) misc)[i] = arry[i];
    }
	
}

void save_print_fan(uint16_t fanValue)
{
	uint16_t  temp;
	temp = fanValue;
	BKP_WriteBackupRegister(PRINT_FAN_CACHE_L16_ADDR, temp);

}
void get_print_fan(uint16_t *fanValue)
{
	uint16_t  temp;
	temp = BKP_ReadBackupRegister(PRINT_FAN_CACHE_L16_ADDR);
	*fanValue = temp;
}


//保存打印的文件名、文件名长度到backup registers
void save_file_name(const uint8_t * const pName, const uint16_t * const name_length)
{
    uint8_t  i, k;
    uint16_t  wdata;

    BKP_WriteBackupRegister(FILE_NAME_LENGTH_CACHE_L16_ADDR, *name_length);

    if(*name_length > 42)	{
        DBG("name length is overflow \r\n");
        return;
    }
    for(i = 0 ; i < *name_length / 2 ; i++ )
    {
        for(k = 0; k < 2 ; k++ )
        {
            ((uint8_t *) &wdata)[k] = pName[2 * i + k];
        }
        BKP_WriteBackupRegister(FILE_NAME_CACHE_ADDR + i * BCK_CACHE_ADDR_OFFSET, wdata);
    }

    if(*name_length % 2 )
    {
        wdata = pName[*name_length - 1];
        BKP_WriteBackupRegister(FILE_NAME_CACHE_ADDR + i * BCK_CACHE_ADDR_OFFSET, wdata);
    }

}
//从backup registers 获得 断电时文件名、文件名长度信息
void get_file_name(uint8_t *pName,  uint16_t *name_length)
{
	uint8_t  i;
	uint16_t  rdata;
	
	*name_length = BKP_ReadBackupRegister(FILE_NAME_LENGTH_CACHE_L16_ADDR);
	
	 for(i = 0 ; i < *name_length / 2 ; i++ )
	 {
		rdata = BKP_ReadBackupRegister(FILE_NAME_CACHE_ADDR + i * BCK_CACHE_ADDR_OFFSET);
		pName[2*i + 0] = rdata;
		pName[2*i + 1] = rdata>>8;
	 }
	 if(*name_length % 2 )
	 {
		pName[*name_length - 1] = BKP_ReadBackupRegister(FILE_NAME_CACHE_ADDR + i * BCK_CACHE_ADDR_OFFSET);
	 }
	
}
void get_file_name_length(uint16_t *name_length)
{
	*name_length = BKP_ReadBackupRegister(FILE_NAME_LENGTH_CACHE_L16_ADDR);
}
//保存x,y,z,e0 轴的当前位置到backup registers
void save_axis_position(const float * const current_position)
{
    uint16_t  arry[8];
    uint8_t  i, k;
    for(k = 0 ; k < NUM_AXIS ; k++ )
    {
        for(i = 0 ; i < 2 ; i++ )
        {
            arry[2*k + i] = ((uint16_t *) &current_position[k])[i];
        }
    }
    BKP_WriteBackupRegister(X_POS_CACHE_L16_ADDR, arry[0]);
    BKP_WriteBackupRegister(X_POS_CACHE_H16_ADDR, arry[1]);

    BKP_WriteBackupRegister(Y_POS_CACHE_L16_ADDR, arry[2]);
    BKP_WriteBackupRegister(Y_POS_CACHE_H16_ADDR, arry[3]);

    BKP_WriteBackupRegister(Z_POS_CACHE_L16_ADDR, arry[4]);
    BKP_WriteBackupRegister(Z_POS_CACHE_H16_ADDR, arry[5]);

    BKP_WriteBackupRegister(E0_POS_CACHE_L16_ADDR, arry[6]);
    BKP_WriteBackupRegister(E0_POS_CACHE_H16_ADDR, arry[7]);
}

//从backup registers 获得断电时x,y,z,e0 轴的当前位置
void get_axis_position(float * current_position)
{
	uint16_t  arry[2];
	arry[0] = BKP_ReadBackupRegister(X_POS_CACHE_L16_ADDR);
	arry[1] = BKP_ReadBackupRegister(X_POS_CACHE_H16_ADDR);
	((uint16_t *)&current_position[0])[0] = arry[0];
	((uint16_t *)&current_position[0])[1] = arry[1];

	arry[0] = BKP_ReadBackupRegister(Y_POS_CACHE_L16_ADDR);
	arry[1] = BKP_ReadBackupRegister(Y_POS_CACHE_H16_ADDR);
	((uint16_t *)&current_position[1])[0] = arry[0];
	((uint16_t *)&current_position[1])[1] = arry[1];	
	
	arry[0] = BKP_ReadBackupRegister(Z_POS_CACHE_L16_ADDR);
	arry[1] = BKP_ReadBackupRegister(Z_POS_CACHE_H16_ADDR);
	((uint16_t *)&current_position[2])[0] = arry[0];
	((uint16_t *)&current_position[2])[1] = arry[1];	
	
	arry[0] = BKP_ReadBackupRegister(E0_POS_CACHE_L16_ADDR);
	arry[1] = BKP_ReadBackupRegister(E0_POS_CACHE_H16_ADDR);
	((uint16_t *)&current_position[3])[0] = arry[0];
	((uint16_t *)&current_position[3])[1] = arry[1];
	
	
	
}
#if 0
void get_power_down_infor_test(void)
{
		float extru_temp_test;
		float bed_temp_test;
		float bckup_pos[4];
		
		uint32_t line_no,file_pos;
		char fileName[50];
		char fNTest[50];
		uint16_t fileNameLength;
		uint16_t fNTestLength;
		uint8_t fanValue;
		uint32_t printTime;

		
		uint8_t i;
#if 0	
	if(is_power_off_happen() == PW_OFF_HAPPEN)
	{

	
		clear_power_off_status();//清断电保存标志，放在gcode解析函数中
	
		DBG("# power off happen\r\n");
		DBG("# power off information follow...\r\n");
		
		get_temperature(&bed_temp_test , &extru_temp_test);
		
		get_print_percentage(&line_no , &file_pos);
		
		get_file_name((u8 *)fileName , &fileNameLength);
		
		get_axis_position(bckup_pos);
		get_print_fan(&fanValue);
		
		DBG("# bed Temp:%.3f , extru Temp:%.3f\r\n",bed_temp_test,extru_temp_test);
		
		DBG("# line_no :%d ,file_pos :%d\r\n",line_no,file_pos);
		
		DBG("# file name : %s,fileNameLength :%d\r\n",fileName,fileNameLength);
		DBG("# fan :%d \r\n",fanValue);
		
		for(i=0 ; i< NUM_AXIS ; i++)
		DBG("# bckup_pos[%d]:%.3f\r\n",i,bckup_pos[i]);
DBG("**************\r\n");	
	}
#endif

//sram-->eeprom transfer
#if 1
//读取

	
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)
	{
		eeprom_clear_power_off_status();
		
		DBG("# power off happen\r\n");
		DBG("# power off information follow...\r\n");
		eeprom_get_temp(&bck_bed_temperature, &bck_extruder_temperature);
		eeprom_get_print_percentage(&line_no_test, &file_pos_test);
		eeprom_get_file_name((u8*)fNTest,&fNTestLength);
		eeprom_get_axis_position(bck_position);	
		eeprom_get_print_time(&printTime);
		eeprom_get_print_fan(&fanValue);
		
		DBG("# bed Temp:%.3f , extru Temp:%.3f\r\n",bck_bed_temperature,bck_extruder_temperature);
		
		DBG("# line_no :%d ,file_pos :%d\r\n",line_no_test,file_pos_test);
		
		DBG("# file name : %s,fileNameLength :%d\r\n",fNTest,fNTestLength);
		DBG("# time : %d,fan :%d\r\n",printTime,fanValue);
		
		for(i=0 ; i< NUM_AXIS ; i++)
		DBG("# bckup_pos[%d]:%.3f\r\n",i,bck_position[i]);

	}
#endif
		


}


void get_power_down_infor(void)
{
		float extru_temp_test;
		float bed_temp_test;
		float bckup_pos[4];
		
		uint32_t line_no,file_pos;
		u8 *fileName = NULL;
	
		uint16_t fileNameLength;
		
		uint8_t fanValue;
		uint32_t printTime;

		
		uint8_t i;
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)
	{
		eeprom_clear_power_off_status();
		
		eeprom_get_file_name_length(&fileNameLength);//获得文件名长度
		fileName = mymalloc(SRAMIN,fileNameLength);
		if(fileName)
		{
			DBG("# power off happen\r\n");
			DBG("# power off information follow...\r\n");
			eeprom_get_temp(&bck_bed_temperature, &bck_extruder_temperature);
			eeprom_get_print_percentage(&line_no_test, &file_pos_test);
			
			eeprom_get_file_name((u8*)fileName,&fileNameLength);
			eeprom_get_axis_position(bck_position);	
			eeprom_get_print_time(&printTime);
			eeprom_get_print_fan(&fanValue);
			
			DBG("# bed Temp:%.3f , extru Temp:%.3f\r\n",bck_bed_temperature,bck_extruder_temperature);
			
			DBG("# line_no :%d ,file_pos :%d\r\n",line_no_test,file_pos_test);
			
			DBG("# file name : %s,fileNameLength :%d\r\n",fileName,fileNameLength);
			DBG("# time : %d,fan :%d\r\n",printTime,fanValue);
			
			for(i=0 ; i< NUM_AXIS ; i++)
			DBG("# bckup_pos[%d]:%.3f\r\n",i,bck_position[i]);
			
			myfree(SRAMIN,fileName);
		}
	}
		
	
}
#endif
/************SRAM-> EEPROM backup***********/

_PW_OFF_E eeprom_is_power_off_happen(void)
{
    uint8_t read_data;
    read_data = AT24CXX_ReadOneByte(POWER_OFF_FLAG_BASE_ADDR);
    return ((read_data == POWER_OFF_FLAG_VAL_SET) ? PW_OFF_HAPPEN : PW_OFF_NOT_HAPPEN);

}
void eeprom_save_power_off_status(void)
{
	AT24CXX_WriteOneByte(POWER_OFF_FLAG_BASE_ADDR,POWER_OFF_FLAG_VAL_SET);
}
void eeprom_clear_power_off_status(void)
{
	AT24CXX_WriteOneByte(POWER_OFF_FLAG_BASE_ADDR,POWER_OFF_FLAG_VAL_RESET);
}

void eeprom_save_temp(const float * const cur_bed_temp, const float * const cur_extr_temp)
{

	AT24CXX_Write(POWER_OFF_BED_BASE_ADDR,(uint8_t *)cur_bed_temp,4);
	AT24CXX_Write(POWER_OFF_EXTUDER_BASE_ADDR,(uint8_t *)cur_extr_temp,4);
}

void eeprom_get_temp(float *cur_bed_temp, float *cur_extr_temp)
{
	AT24CXX_Read(POWER_OFF_BED_BASE_ADDR,(uint8_t *)cur_bed_temp,4);
	AT24CXX_Read(POWER_OFF_EXTUDER_BASE_ADDR,(uint8_t *)cur_extr_temp,4);
	
}


void eeprom_save_print_percentage(const uint32_t * const line_no, const uint32_t * const file_pos)
{
	AT24CXX_Write(POWER_OFF_FILE_LINE_NUM_BASE_ADDR,(uint8_t *)line_no,4);
	AT24CXX_Write(POWER_OFF_FILE_POS_BASE_ADDR,(uint8_t *)file_pos,4);
}

void eeprom_get_print_percentage(uint32_t *line_no, uint32_t *file_pos)
{
	AT24CXX_Read(POWER_OFF_FILE_LINE_NUM_BASE_ADDR,(uint8_t *)line_no,4);
	AT24CXX_Read(POWER_OFF_FILE_POS_BASE_ADDR,(uint8_t *)file_pos,4);
}

void eeprom_save_file_name(const uint8_t * const pName, const uint16_t * const name_length)
{
	uint16_t buff;
	buff = *name_length;
	AT24CXX_Write(POWER_OFF_FN_LENGTH_BASE_ADDR,(uint8_t *)name_length,2);
	AT24CXX_Write(POWER_OFF_FILE_NAME_BASE_ADDR,(uint8_t *)pName,buff);
}
void eeprom_get_file_name(uint8_t *pName,  uint16_t *name_length)
{
	uint16_t buff;
	AT24CXX_Read(POWER_OFF_FN_LENGTH_BASE_ADDR,(uint8_t *)name_length,2);
	
	buff = *name_length;
	AT24CXX_Read(POWER_OFF_FILE_NAME_BASE_ADDR,(uint8_t *)pName,buff);

}
void eeprom_get_file_name_length(uint16_t *name_length)
{
		AT24CXX_Read(POWER_OFF_FN_LENGTH_BASE_ADDR,(uint8_t *)name_length,2);
}

void eeprom_save_axis_position(const float * const current_position)
{
	AT24CXX_Write(POWER_OFF_X_BASE_ADDR,(uint8_t *)&current_position[X_AXIS],4);
	AT24CXX_Write(POWER_OFF_Y_BASE_ADDR,(uint8_t *)&current_position[Y_AXIS],4);
	AT24CXX_Write(POWER_OFF_Z_BASE_ADDR,(uint8_t *)&current_position[Z_AXIS],4);
	AT24CXX_Write(POWER_OFF_E_BASE_ADDR,(uint8_t *)&current_position[E_AXIS],4);
}

void eeprom_get_axis_position(float * current_position)
{

	AT24CXX_Read(POWER_OFF_X_BASE_ADDR,(uint8_t *)&current_position[X_AXIS],4);
	AT24CXX_Read(POWER_OFF_Y_BASE_ADDR,(uint8_t *)&current_position[Y_AXIS],4);
	AT24CXX_Read(POWER_OFF_Z_BASE_ADDR,(uint8_t *)&current_position[Z_AXIS],4);
	AT24CXX_Read(POWER_OFF_E_BASE_ADDR,(uint8_t *)&current_position[E_AXIS],4);
}

void eeprom_save_print_time(uint32_t *misc)
{
	AT24CXX_Write(POWER_OFF_PRINT_TIME_BASE_ADDR,(uint8_t *)misc,4);
}

void eeprom_get_print_time(uint32_t *misc)
{
	AT24CXX_Read(POWER_OFF_PRINT_TIME_BASE_ADDR,(uint8_t *)misc,4);
}

void eeprom_save_print_fan(uint8_t fanValue)
{
	AT24CXX_Write(POWER_OFF_FAN__BASE_ADDR,(uint8_t *)&fanValue,1);
}

void eeprom_get_print_fan(uint8_t *fanValue)
{
	AT24CXX_Read(POWER_OFF_FAN__BASE_ADDR,(uint8_t *)fanValue,1);
}

void eeprom_save_not_exacute_gcode_num(uint8_t gcdoeNum)
{
	AT24CXX_Write(POWER_OFF_NOT_EXACUTE_GCODE_BASE_ADDR,(uint8_t *)&gcdoeNum,1);
}

void eeprom_get_not_exacute_gcode_num(uint8_t *gcdoeNum)
{
	AT24CXX_Read(POWER_OFF_NOT_EXACUTE_GCODE_BASE_ADDR,(uint8_t *)gcdoeNum,1);
}


void eeprom_set_z_up_flag(void)
{
	AT24CXX_WriteOneByte(POWER_OFF_Z_UP_FLAG_ADDR,POWER_OFF_Z_UP_FLAG_SET);
}
u8 eeprom_get_z_up_flag(void)
{
	 uint8_t read_data;
    read_data = AT24CXX_ReadOneByte(POWER_OFF_Z_UP_FLAG_ADDR);
	return read_data;
}

void eeprom_clear_z_up_flag(void)
{
	AT24CXX_WriteOneByte(POWER_OFF_Z_UP_FLAG_ADDR,POWER_OFF_Z_UP_FLAG_RESET);
}





/****************************************************************
 * @FunName	: Stepper_GetPositions()
 * @Brief	: Get all axis position.
 * @Para	: None.
 * @Return	: None.
****************************************************************/
void Stepper_GetPositions(float *countPos)
{
	//CRITICAL_SECTION_START;
	countPos[X_AXIS] = current_position[X_AXIS];
	countPos[Y_AXIS] = current_position[Y_AXIS];
	countPos[Z_AXIS] = current_position[Z_AXIS];
	countPos[E_AXIS] = current_position[E_AXIS];
	//CRITICAL_SECTION_END;
}

void Stepper_SaveGetPositions(float *countPos)
{
	//CRITICAL_SECTION_START;
	countPos[X_AXIS] = saved_position[X_AXIS];
	countPos[Y_AXIS] = saved_position[Y_AXIS];
	countPos[Z_AXIS] = saved_position[Z_AXIS];
	countPos[E_AXIS] = saved_position[E_AXIS];
	//CRITICAL_SECTION_END;
}


