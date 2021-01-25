/*
*********************************************************************************************************
*
*	ģ������ : ������
*	�ļ����� : mainui.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/
#ifndef _MAINUI_H
#define _MAINUI_H


/**********************����ͷ�ļ�*************************/

#include "app_gui.h"
#include "common.h"
/**********************************/ 

	   
  
 /**********************ͼ����Ŀ����*************************/ 
#define HOME_BMP_NUMS	8	//������ͼ����� 
  

/**********************************/

#define MENU_DEPTH	3	//������ͼ����� 
  
  
  
 /**********************�����������*************************/   
#define MUI_BACK_COLOR		LIGHTBLUE		//MUIѡ��ͼ���ɫ
#define MUI_FONT_COLOR 		BLUE 			//MUI������ɫ

#define MUI_EX_BACKCOLOR	0X0000			//�����ⲿ����ɫ
#define MUI_IN_BACKCOLOR	0X8C51			//�����ڲ�����ɫ

#define BMP_FONT_SIZE	24			//����ͼ�����Һ�������ľ���
/**********************************/



/**********************��������*************************/
//��ͼ��/ͼƬ·��
extern  char *const mui_icon_path_tbl[GUI_LANGUAGE_NUM][HOME_BMP_NUMS]; //ͼ��·����


/**********************************/






/**********************************/

/**********************��������*************************/

/**********************************/



/**********************��������*************************/

void mui_load_icon(void);

uint8_t mui_touch_chk(void);

void main_screen(void);
uint8_t gui_touch_chk(_gui_dev *buttonID);
/**********************************/


#endif




















 



