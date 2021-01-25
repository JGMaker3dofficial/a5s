#ifndef __BUTTON_H
#define __BUTTON_H 			 
#include "guix.h"  
#include "sys.h"  

//��׼��ť�߿���ɫ����
#define STD_BTN_LTOC 	0XFFFF	    //��������ɫ
#define STD_BTN_LTIC 	0XDEFC		//��������ɫ
#define STD_BTN_RBOC 	0X8431		//��������ɫ
#define STD_BTN_RBIC 	0XA535		//��������ɫ
#define STD_BTN_INSC	0XEF5D	    //��ť�ڲ������ɫ
#define STD_BTN_DFLC	0XA535 		//δʹ�ܰ�ťcaption�����ɫ
#define STD_BTN_DFRC	0XFFFF		//δʹ�ܰ�ťcaption�ұ���ɫ
//Բ�ǰ�ť��ɫ����
#define ARC_BTN_RIMC	0X0000		//Բ�ǰ�ť�߿���ɫ
#define ARC_BTN_TP1C	0XD6BB		//��һ�е���ɫ
#define ARC_BTN_UPHC	0X8452		//�ϰ벿����ɫ
#define ARC_BTN_DNHC	0X52CD		//�°벿����ɫ
//��ť��ɫ����
#define BTN_DFU_BCFUC	0X0000		//Ĭ���ɿ�����ɫ
#define BTN_DFU_BCFDC	0X0000		//Ĭ�ϰ��µ���ɫ

//��ť״̬����
#define BTN_RELEASE 	0X00
#define BTN_PRESS   	0X01
#define BTN_INACTIVE   	0X02
//��ť���Ͷ���
#define BTN_TYPE_STD 	0X00
#define BTN_TYPE_PIC   	0X01
#define BTN_TYPE_ANG 	0X02
#define BTN_TYPE_TEXTA 	0X03
#define BTN_TYPE_TEXTB 	0X04
#define BTN_TYPE_SGICON_TEXT 	0X05

//��ť��Ӧ��־
#define BUTTON_ACTION	0X40	//(1<<6)

//��������ɨ��ѡ�񰴼�����
#define BUTTON_SELECTION_MASK	0X3F	//~(3<<6)

//�����ɿ�����
#define BUTTON_UP_MASK	0X7F

//�������±�־
#define BUTTON_DOWN	0X80

//����״̬���
#define BUTTON_CLEAR_STATE 0XFC
//��ť�ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//��ť��������
	u16 left;                       //��ť�������
	u16 width; 				  		//���
	u16 height;						//�߶�
	
	u8 id;							//��ťID
	u8 type;						//��ť����
									//[7]:0,ģʽA,������һ��״̬,�ɿ���һ��״̬.
									//	  1,ģʽB,ÿ����һ��,״̬�ı�һ��.��һ�°���,�ٰ�һ�µ���.
									//[6:4]:����
									//[3:0]:0,��׼��ť;1,ͼƬ��ť;2,�߽ǰ�ť;3,���ְ�ť(����͸��),4,���ְ�ť(������һ)5,��ɫͼ�갴ť����ʾ����;6.��ɫͼ����ʾ����
	u8 sta;							//��ť״̬
									//[7]:����״̬ 0,�ɿ�.1,����.(������ʵ�ʵ�TP״̬)
									//[6]:0,�˴ΰ�����Ч;1,�˴ΰ�����Ч.(����ʵ�ʵ�TP״̬����)
									//[5:2]:����
									//[1:0]:0,�����(�ɿ�);1,����;2,δ�������

	u8 *caption;					//��ť����
	u8 font;						//caption��������
	u8 arcbtnr;						//Բ�ǰ�ťʱԲ�ǵİ뾶 
	u16 caption_top;
	u16 caption_left;

	u16 bcfucolor; 				  	//button caption font up color
	u16 bcfdcolor; 				  	//button caption font down color
	
	u16 icon_top;
	u16 icon_left;
	u16 icon_width;
	u16 icon_height;
	
	
	
	u16 *bkctbl;					//�������ְ�ť:
									//����ɫ��(��ťΪ���ְ�ť��ʱ��ʹ��)
									//a,��Ϊ���ְ�ť(����͸��ʱ),���ڴ洢����ɫ
									//b,��Ϊ���ְ�ť(������һ��),bkctbl[0]:����ɿ�ʱ�ı���ɫ;bkctbl[1]:��Ű���ʱ�ı���ɫ.
									//���ڱ߽ǰ�ť:
									//bkctbl[0]:Բ�ǰ�ť�߿����ɫ
									//bkctbl[1]:Բ�ǰ�ť��һ�е���ɫ
									//bkctbl[2]:Բ�ǰ�ť�ϰ벿�ֵ���ɫ
									//bkctbl[3]:Բ�ǰ�ť�°벿�ֵ���ɫ	
									//���ڵ�ɫͼ����˵��
									//bkctbl[0]:�ɿ�ʱ�ı���ɫ
									//bkctbl[1]:����ʱ�ı���ɫ
									//bkctbl[2]:�ɿ�ʱ��ͼ����ɫ
									//bkctbl[3]:����ʱ��ͼ����ɫ
	u8 *picbtnpathu;				//ͼƬ��ť�ɿ�ʱ��ͼƬ·��
	u8 *picbtnpathd;		 		//ͼƬ��ť����ʱ��ͼƬ·��
}_btn_obj;

void btn_read_backcolor(_btn_obj * btnx);
void btn_recover_backcolor(_btn_obj * btnx);
_btn_obj * btn_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type);
void btn_delete(_btn_obj * btn_del);
void set_color(u16 up,u16 down);
u8 btn_check(_btn_obj * btnx,void * in_key);	//��ť�Ƿ񱻰��¼��
void btn_draw(_btn_obj * btnx);					//����ť
void btn_on_keyup(_btn_obj * btnx);				//��ť�ɿ�����
void btn_on_keydown(_btn_obj * btnx);			//��ť���²���
void btn_draw_stdbtn(_btn_obj * btnx);			//����׼��ť
void btn_press_hook(_btn_obj *btnx);
void btn_release_hook(_btn_obj *btnx);
void btn_draw_arcrectangle(u16 x,u16 y,u16 width,u16 height,u8 r,u8 mode,u16 upcolor,u16 downcolor);
void btn_draw_arcbtn(_btn_obj * btnx);
void btn_draw_picbtn(_btn_obj * btnx);
void btn_draw_textabtn(_btn_obj * btnx);
void btn_draw_textbbtn(_btn_obj * btnx);
void btn_draw_sgiconbtn(_btn_obj * btnx);

u8 screen_key_chk(_btn_obj **keydev, _in_obj * key,u8 num);

#endif
