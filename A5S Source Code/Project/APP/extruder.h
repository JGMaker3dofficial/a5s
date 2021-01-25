

#ifndef __EXTRUDER_H
#define __EXTRUDER_H 	 

/**********************包含头文件*************************/
#include "app_gui.h"

/*****************************************/
typedef enum 
{
	NONE_FILAMENT_SPEED = 0XFF,
	FILAMENT_SPEED_SLOW = 0,
	FILAMENT_SPEED_NORMAL = 1,
	FILAMENT_SPEED_FAST = 2,
}_filament_speed_step_e;


typedef enum 
{
	UNIT_NONE = 0XFF,
	UNIT_MM = 0,
	UNIT_CM = 1,
}_lenght_unit_e;

void extru_screen(void);
void exacute_manualFilament(u8 dirId, u8 speedID, u8 lengthID);
void clear_heatSt(void);


#endif
