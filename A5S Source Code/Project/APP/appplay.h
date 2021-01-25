#ifndef __APPPLAY_H
#define __APPPLAY_H 	
#include "common.h"

//APP-����Ӧ�ó��� ����	   

//��ͼ��/ͼƬ·��
extern u8*const appplay_icospath_tbl[16];//icos��·����
////////////////////////////////////////////////////////////////////////////////////////////
#define OS_TICKS_PER_SEC  200

//APPͼ���������
__packed typedef struct _m_app_icos
{										    
	u16 x;			//ͼ�����꼰�ߴ�
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//ͼ��·��
	u8 * name;		//ͼ������
}m_app_icos;

//APP������
typedef struct _m_app_dev
{										    
	u8 selico;					//��ǰѡ�е�ͼ��.
								//0~15,��ѡ�е�ͼ����	 
								//����,û���κ�ͼ�걻ѡ��	  
	m_app_icos icos[16];		//�ܹ�16��ͼ��
}m_app_dev;
		 

u8 app_play(void);
u8 Remote_Play(u8* caption);

#endif























