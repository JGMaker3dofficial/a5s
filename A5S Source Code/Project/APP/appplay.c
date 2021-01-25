#include "appplay.h"

//APP-其他应用程序 代码	   
  

#define APPPLAY_EX_BACKCOLOR	0X0000			//窗体外部背景色
#define APPPLAY_IN_BACKCOLOR	0X8C51			//窗体内部背景色
#define APPPLAY_NAME_COLOR		0X001F			//程序名颜色

#define APPPLAY_ALPHA_VAL 		18 				//APP选中透明度设置
#define APPPLAY_ALPHA_COLOR		WHITE			//APP透明色
	

//设置选中哪个图标
//sel:0~15代表当前页的选中ico
void appplay_set_sel(m_app_dev* appdev,u8 sel)
{
	if(sel>=16)return;//非法的输入
	if(appdev->selico<16)
	{
		gui_fill_rectangle(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_IN_BACKCOLOR);//清除之前的图片
		minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x+4,appdev->icos[appdev->selico].y+4,48,48,0,0);
 		gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+4+48+2,appdev->icos[appdev->selico].width,12,APPPLAY_NAME_COLOR,12,appdev->icos[appdev->selico].name,1);
	};				   
	appdev->selico=sel;
	gui_alphablend_area(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_ALPHA_COLOR,APPPLAY_ALPHA_VAL);
	minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x+4,appdev->icos[appdev->selico].y+4,48,48,0,0);
	gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+4+48+2,appdev->icos[appdev->selico].width,12,APPPLAY_NAME_COLOR,12,appdev->icos[appdev->selico].name,1);
}
//触摸屏扫描
//返回值:0~15,有效按键.其他,无效
u8 appplay_tpscan(m_app_dev *appdev)
{
	static u8 firsttpd=0;	//按键处理标志,防止一次按下,多次返回
	u8 i=0XFF;
	tp_dev.scan(0);//扫描																		 
	if((tp_dev.sta&TP_PRES_DOWN))//有按键被按下
	{
		if(firsttpd==0)//第一次处理?
		{
			firsttpd=1;	  		//标记已经处理了此次按键
			for(i=0;i<16;i++)
			{
				if((tp_dev.x[0]>appdev->icos[i].x)&&(tp_dev.x[0]<appdev->icos[i].x+appdev->icos[i].width)
				 &&(tp_dev.y[0]>appdev->icos[i].y)&&(tp_dev.y[0]<appdev->icos[i].y+appdev->icos[i].height))//在区域内
				{
					break;//得到选中的编号	
				}
			}
		}
	}else firsttpd=0;//松开了
	return i;
}

///////////////////////////////////////////////////////////////////////////////////












