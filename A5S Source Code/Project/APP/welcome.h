
/*
*********************************************************************************************************
*
*	模块名称 : 开机界面模块
*	文件名称 : welcome.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/
#ifndef _WELCOME_H
#define _WELCOME_H

/**********************包含头文件*************************/
#include "sys.h"
/**********************************/


/**********************挤出头模块***********************/
#define EXTRUDER_XOFF	160
#define EXTRUDER_YOFF	2
#define EXTRUDER_W	35
#define EXTRUDER_H	35

#define EXTRUDER_TEM_XOFF	(EXTRUDER_XOFF + EXTRUDER_W + 2)
#define EXTRUDER_TEM_YOFF	EXTRUDER_YOFF


/**********************热床模块***********************/
#define BED_XOFF	( EXTRUDER_XOFF + 80)
#define BED_YOFF	2
#define BED_W	40
#define BED_H	35

#define BED_TEM_XOFF	(BED_XOFF + BED_W + 2)
#define BED_TEM_YOFF	BED_YOFF

#define LOGO_NUM	2



/**********************导出变量*************************/
extern char *const logo_icos_path_tbl[];

/**********************************/




/**********************导出函数*************************/
void welcome_screen(void);
void display_temp(void);
/**********************************/



#endif
