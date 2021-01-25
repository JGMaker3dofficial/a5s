/*
*********************************************************************************************************
*
*	模块名称 : 备份RAM模块
*	文件名称 : backup_sram.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __BACKUP_SRAM_H
#define __BACKUP_SRAM_H

#include "sys.h"
#include "Configuration.h"

#define BKP_DR_NUMBER              42	//高容量对应的backup register 数量

#define	PW_OFF_STATUS_CACHE_ADDR					BKP_DR1
#define	PW_OFF_STATUS_VALUE_SET						0X55
#define	PW_OFF_STATUS_VALUE_RESET					0XAE

#define	EXTRUDER_TEMPERATURE_CACHE_L16_ADDR			BKP_DR3
#define	EXTRUDER_TEMPERATURE_CACHE_H16_ADDR			BKP_DR4

#define	BED_TEMPERATURE_CACHE_L16_ADDR				BKP_DR5
#define	BED_TEMPERATURE_CACHE_H16_ADDR				BKP_DR6


#define	X_POS_CACHE_L16_ADDR						BKP_DR7
#define	X_POS_CACHE_H16_ADDR						BKP_DR8


#define	Y_POS_CACHE_L16_ADDR						BKP_DR9
#define	Y_POS_CACHE_H16_ADDR						BKP_DR10


#define	Z_POS_CACHE_L16_ADDR						BKP_DR11
#define	Z_POS_CACHE_H16_ADDR						BKP_DR12

#define	E0_POS_CACHE_L16_ADDR						BKP_DR13
#define	E0_POS_CACHE_H16_ADDR						BKP_DR14


#define	LINE_NUM_CACHE_L16_ADDR						BKP_DR15
#define	LINE_NUM_CACHE_H16_ADDR						BKP_DR16

#define	FILE_POS_CACHE_L16_ADDR						BKP_DR17
#define	FILE_POS_CACHE_H16_ADDR						BKP_DR18

#define	FILE_NAME_LENGTH_CACHE_L16_ADDR				BKP_DR19
//#define	FILE_NAME_LENGTH_CACHE_H16_ADDR				BKP_DR20

#define PRINT_TIME_CACHE_L16_ADDR			BKP_DR21
#define PRINT_TIME_CACHE_H16_ADDR			BKP_DR22

#define PRINT_FAN_CACHE_L16_ADDR			BKP_DR23

#define	FILE_NAME_CACHE_ADDR					BKP_DR24//24~42 共19*2=38bytes
#define	BCK_CACHE_ADDR_OFFSET						4

//每次写一个16bits 的数据
#define WR_BKSRAM(addr,data)		*(__IO uint32_t *) (BKP_BASE + addr) = data
#define RD_BKSRAM(addr)				*((__IO uint32_t *) (BKP_BASE + addr))



typedef enum 
{
	PW_OFF_NOT_HAPPEN,
	PW_OFF_HAPPEN,
}_PW_OFF_E;



extern float bck_extruder_temperature ;
extern float bck_bed_temperature;
extern float bck_position[NUM_AXIS];
extern uint32_t line_no_test;
extern uint32_t file_pos_test;
extern uint32_t powerDownLineNum;
extern uint8_t  dontExctpPdLineNum;
extern uint32_t bckGcodeHeadPos;
/* 供外部调用的函数声明 */
void init_backup_domain(void);


_PW_OFF_E is_power_off_happen(void);
void save_power_off_status(void);
void clear_power_off_status(void);

void save_temperature(const float * const cur_bed_temp, const float * const cur_extr_temp);
void get_temperature(float *cur_bed_temp, float *cur_extr_temp);

void  save_print_percentage(const uint32_t * const line_no, const uint32_t * const file_pos);
void get_print_percentage(uint32_t *line_no, uint32_t *file_pos);

void save_file_name(const uint8_t * const pName, const uint16_t * const name_length);
void get_file_name(uint8_t *pName,  uint16_t *name_length);

void save_axis_position(const float * const current_position);
void get_axis_position(float * current_position);
void get_power_down_infor_test(void);

void get_file_name_length(uint16_t *name_length);
void save_print_time(uint32_t *misc);
void get_print_time(uint32_t *misc);
void save_print_fan(uint16_t fanValue);
void get_print_fan(uint16_t *fanValue);
void get_power_down_infor(void);
void eeprom_get_file_name_length(uint16_t *name_length);






_PW_OFF_E eeprom_is_power_off_happen(void);
void eeprom_save_power_off_status(void);
void eeprom_clear_power_off_status(void);
void eeprom_save_temp(const float * const cur_bed_temp, const float * const cur_extr_temp);
void eeprom_get_temp(float *cur_bed_temp, float *cur_extr_temp);
void eeprom_save_print_percentage(const uint32_t * const line_no, const uint32_t * const file_pos);
void eeprom_get_print_percentage(uint32_t *line_no, uint32_t *file_pos);
void eeprom_save_file_name(const uint8_t * const pName, const uint16_t * const name_length);
void eeprom_get_file_name(uint8_t *pName,  uint16_t *name_length);
void eeprom_save_axis_position(const float * const current_position);
void eeprom_get_axis_position(float * current_position);
void Stepper_GetPositions(float *countPos);
void Stepper_SaveGetPositions(float *countPos);
void eeprom_save_print_time(uint32_t *misc);
void eeprom_get_print_time(uint32_t *misc);
void eeprom_save_print_fan(uint8_t fanValue);
void eeprom_get_print_fan(uint8_t *fanValue);
void eeprom_save_not_exacute_gcode_num(uint8_t gcdoeNum);
void eeprom_get_not_exacute_gcode_num(uint8_t *gcdoeNum);

void eeprom_set_z_up_flag(void);
u8 eeprom_get_z_up_flag(void);
void eeprom_clear_z_up_flag(void);


#endif

/**************************************************************/
