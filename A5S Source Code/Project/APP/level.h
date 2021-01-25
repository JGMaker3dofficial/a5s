

#ifndef __LEVEL_H
#define __LEVEL_H 	 

/**********************包含头文件*************************/
#include "app_gui.h"

/*****************************************/
#define A5S_USE_LEVEL_BEEP	0 //美国版定制的A5S 添加调平感应器



void level_screen(void);
void level_first_point_callBack(void);
void level_second_point_callBack(void);
void level_third_point_callBack(void);
void level_fourth_point_callBack(void);
void level_fifth_point_callBack(void);
void level_z_home(float hight);
#endif
