#include "appplay.h"

//APP-����Ӧ�ó��� ����	   
  

#define APPPLAY_EX_BACKCOLOR	0X0000			//�����ⲿ����ɫ
#define APPPLAY_IN_BACKCOLOR	0X8C51			//�����ڲ�����ɫ
#define APPPLAY_NAME_COLOR		0X001F			//��������ɫ

#define APPPLAY_ALPHA_VAL 		18 				//APPѡ��͸��������
#define APPPLAY_ALPHA_COLOR		WHITE			//APP͸��ɫ
	

//����ѡ���ĸ�ͼ��
//sel:0~15����ǰҳ��ѡ��ico
void appplay_set_sel(m_app_dev* appdev,u8 sel)
{
	if(sel>=16)return;//�Ƿ�������
	if(appdev->selico<16)
	{
		gui_fill_rectangle(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_IN_BACKCOLOR);//���֮ǰ��ͼƬ
		minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x+4,appdev->icos[appdev->selico].y+4,48,48,0,0);
 		gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+4+48+2,appdev->icos[appdev->selico].width,12,APPPLAY_NAME_COLOR,12,appdev->icos[appdev->selico].name,1);
	};				   
	appdev->selico=sel;
	gui_alphablend_area(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_ALPHA_COLOR,APPPLAY_ALPHA_VAL);
	minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x+4,appdev->icos[appdev->selico].y+4,48,48,0,0);
	gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+4+48+2,appdev->icos[appdev->selico].width,12,APPPLAY_NAME_COLOR,12,appdev->icos[appdev->selico].name,1);
}
//������ɨ��
//����ֵ:0~15,��Ч����.����,��Ч
u8 appplay_tpscan(m_app_dev *appdev)
{
	static u8 firsttpd=0;	//���������־,��ֹһ�ΰ���,��η���
	u8 i=0XFF;
	tp_dev.scan(0);//ɨ��																		 
	if((tp_dev.sta&TP_PRES_DOWN))//�а���������
	{
		if(firsttpd==0)//��һ�δ���?
		{
			firsttpd=1;	  		//����Ѿ������˴˴ΰ���
			for(i=0;i<16;i++)
			{
				if((tp_dev.x[0]>appdev->icos[i].x)&&(tp_dev.x[0]<appdev->icos[i].x+appdev->icos[i].width)
				 &&(tp_dev.y[0]>appdev->icos[i].y)&&(tp_dev.y[0]<appdev->icos[i].y+appdev->icos[i].height))//��������
				{
					break;//�õ�ѡ�еı��	
				}
			}
		}
	}else firsttpd=0;//�ɿ���
	return i;
}

///////////////////////////////////////////////////////////////////////////////////












