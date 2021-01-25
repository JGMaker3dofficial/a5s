#ifndef __APPPLAY_H
#define __APPPLAY_H 	
#include "common.h"

//APP-其他应用程序 代码	   

//各图标/图片路径
extern u8*const appplay_icospath_tbl[16];//icos的路径表
////////////////////////////////////////////////////////////////////////////////////////////
#define OS_TICKS_PER_SEC  200

//APP图标参数管理
__packed typedef struct _m_app_icos
{										    
	u16 x;			//图标坐标及尺寸
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//图标路径
	u8 * name;		//图标名字
}m_app_icos;

//APP控制器
typedef struct _m_app_dev
{										    
	u8 selico;					//当前选中的图标.
								//0~15,被选中的图标编号	 
								//其他,没有任何图标被选中	  
	m_app_icos icos[16];		//总共16个图标
}m_app_dev;
		 

u8 app_play(void);
u8 Remote_Play(u8* caption);

#endif























