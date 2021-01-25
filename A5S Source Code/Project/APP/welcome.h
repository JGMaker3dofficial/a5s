
/*
*********************************************************************************************************
*
*	ģ������ : ��������ģ��
*	�ļ����� : welcome.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/
#ifndef _WELCOME_H
#define _WELCOME_H

/**********************����ͷ�ļ�*************************/
#include "sys.h"
/**********************************/


/**********************����ͷģ��***********************/
#define EXTRUDER_XOFF	160
#define EXTRUDER_YOFF	2
#define EXTRUDER_W	35
#define EXTRUDER_H	35

#define EXTRUDER_TEM_XOFF	(EXTRUDER_XOFF + EXTRUDER_W + 2)
#define EXTRUDER_TEM_YOFF	EXTRUDER_YOFF


/**********************�ȴ�ģ��***********************/
#define BED_XOFF	( EXTRUDER_XOFF + 80)
#define BED_YOFF	2
#define BED_W	40
#define BED_H	35

#define BED_TEM_XOFF	(BED_XOFF + BED_W + 2)
#define BED_TEM_YOFF	BED_YOFF

#define LOGO_NUM	2



/**********************��������*************************/
extern char *const logo_icos_path_tbl[];

/**********************************/




/**********************��������*************************/
void welcome_screen(void);
void display_temp(void);
/**********************************/



#endif
