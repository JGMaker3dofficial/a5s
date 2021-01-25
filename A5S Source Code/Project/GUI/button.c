#include "button.h"
#include "piclib.h"
#include "bmp.h"

/***********************************
	���ܣ�ͨ�������ж���Ļ�������ĸ�ͼ���λ��
	����1�_btn_obj *keydev��  һ�������µ�����ͼ�꣬�Լ�ÿ��ͼ��Ĳ���
	����2��_in_obj * key��       ͨ��ÿ�ε�ѭ��ɨ��ó���ǰ������״̬
	����3��num                   ÿ��������ͼ��ĸ���
  ���أ� ioc_key��             �ڼ���ͼ�걻������
************************************/
u8 screen_key_chk(_btn_obj **keydev, _in_obj * key,u8 num)
{static u8 screen_key=0x00;//ͼ�갴��
		u8 i;
	if(key->ksta&0X01)//����������    	//ksta�������豸(����)��״̬ [7:1],����;[0],0,�ɿ�;1,����.
		{
			if ((screen_key & BUTTON_DOWN)==0)//֮ǰδ����
			{	
				for(i=0;i<num;i++)
					{
						if((key->y> keydev[i]->top )&&(key->y< keydev[i]->top + keydev[i]->height )
						&&(key->x> keydev[i]->left )&&(key->x< keydev[i]->left + keydev[i]->width ))//��������
						{	screen_key=BUTTON_DOWN;	//��ǰ���
							screen_key|=i;	//��¼��ֵ
							keydev[i]->sta &= BUTTON_CLEAR_STATE;
							keydev[i]->sta|=BTN_PRESS;//����
							btn_draw(keydev[i]);
							break;//�õ�ѡ�еı��	
						}
					}			
			}
	  }
	else//������Ļ�ɿ�
	{
		if(screen_key & BUTTON_DOWN)//֮ǰ����
		{ screen_key &= BUTTON_UP_MASK;  //�ɿ���־
			screen_key|=BUTTON_ACTION;	//��־Ϊ��Ҫ��Ӧ
			keydev[screen_key & BUTTON_SELECTION_MASK]->sta &= BUTTON_CLEAR_STATE;
			keydev[screen_key & BUTTON_SELECTION_MASK]->sta|=BTN_RELEASE;	//�ɿ�
			btn_draw(keydev[screen_key & BUTTON_SELECTION_MASK]);
		}
		else
		{
			screen_key=0x00;//
		}
	}
	return screen_key;
		
}

//��ȡ����ɫ
//btnx:��ťָ��
void btn_read_backcolor(_btn_obj * btnx)
{
	u16 x,y,ccnt;
	ccnt=0;
	for(y=btnx->top;y<btnx->top+btnx->height;y++)
	{
		for(x=btnx->left;x<btnx->left+btnx->width;x++)
		{
			btnx->bkctbl[ccnt]=gui_phy.read_point(x,y);//��ȡ��ɫ
			ccnt++;
		}
	}

}
//�ָ�����ɫ
//btnx:��ťָ��
void btn_recover_backcolor(_btn_obj * btnx)
{
	u16 x,y,ccnt;
	ccnt=0;	    
	for(y=btnx->top;y<btnx->top+btnx->height;y++)
	{
		for(x=btnx->left;x<btnx->left+btnx->width;x++)
		{
			gui_phy.draw_point(x,y,btnx->bkctbl[ccnt]);//����	
			ccnt++;
		}
	}  
}

//������ť
//top,left,width,height:����.
//id: ����id
//type:��������
//[7]:0,ģʽA,������һ��״̬,�ɿ���һ��״̬.
//	  1,ģʽB,ÿ����һ��,״̬�ı�һ��.��һ�°���,�ٰ�һ�µ���.
//[6:4]:����
//[3:0]:0,��׼��ť;1,ͼƬ��ť;2,�߽ǰ�ť;3,���ְ�ť(����͸��);4,���ְ�ť(������һ);5,��ɫͼ�갴ť����ʾ����;6.��ɫͼ����ʾ����
_btn_obj * btn_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type)
{
 	_btn_obj * btn_crt;
	if(width<6||height<6)return NULL;//�ߴ粻��̫С		  
	btn_crt=(_btn_obj*)gui_memin_malloc(sizeof(_btn_obj));//�����ڴ�
	if(btn_crt==NULL)return NULL;//�ڴ���䲻��.
	btn_crt->top=top;
	btn_crt->left=left;
	btn_crt->width=width;
	btn_crt->height=height;
	btn_crt->id=id;
	btn_crt->type=type;
	btn_crt->sta=0;
	btn_crt->caption=NULL;       //Ĭ�ϲ���ʾ����
	btn_crt->font=16;					//Ĭ��Ϊ16����
	btn_crt->arcbtnr=3;					//Ĭ��Բ�ǵİ뾶
	btn_crt->bcfucolor=BTN_DFU_BCFUC;	//Ĭ���ɿ�����ɫ
	btn_crt->bcfdcolor=BTN_DFU_BCFDC;	//Ĭ�ϰ��µ���ɫ
	btn_crt->caption_top=0;			//�����ڰ�ť��ƫ��λ��
	btn_crt->caption_left=0;			//�����ڰ�ť��ƫ��λ��

	btn_crt->icon_top=0;					//ͼ���ڰ�ť��ƫ��λ��
	btn_crt->icon_left=0;				//ͼ���ڰ�ť��ƫ��λ��
	btn_crt->icon_width=width;				//ͼ����
	btn_crt->icon_height=height;			//ͼ��߶�
	
	btn_crt->picbtnpathu=NULL;//Ĭ��·��Ϊ��
	btn_crt->picbtnpathd=NULL;//Ĭ��·��Ϊ��   
	if((type&0X0f)>=BTN_TYPE_ANG)//�߽ǰ�ť/���ְ�ť
	{
		switch(type&0X0f)
		{
			case BTN_TYPE_ANG://�߽ǰ�ť
				btn_crt->bkctbl=(u16*)gui_memin_malloc(4*2);//����8���ֽ�
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//�ͷ�֮ǰ������ڴ�
					return NULL;		//�ڴ���䲻��.
				}
				btn_crt->bkctbl[0]=ARC_BTN_RIMC;//�߿���ɫ
				btn_crt->bkctbl[1]=ARC_BTN_TP1C;//��һ�е���ɫ				
				btn_crt->bkctbl[2]=ARC_BTN_UPHC;//�ϰ벿����ɫ
				btn_crt->bkctbl[3]=ARC_BTN_DNHC;//�°벿����ɫ				
				break;
			case BTN_TYPE_TEXTA://����͸�������ְ�ť
				btn_crt->bkctbl=(u16*)gui_memin_malloc(width*height*2);//Ҫ���������ı���ɫ��
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//�ͷ�֮ǰ������ڴ�
					return NULL;		//�ڴ���䲻��.
				}
				btn_read_backcolor(btn_crt);//��ȡȫ������ɫ
				break;
			case BTN_TYPE_TEXTB://������һ�����ְ�ť
				btn_crt->bkctbl=(u16*)gui_memin_malloc(2*2);//����4���ֽ�
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//�ͷ�֮ǰ������ڴ�
					return NULL;		//�ڴ���䲻��.
				}
				btn_crt->bkctbl[0]=0XFFFF;//Ĭ���ɿ�Ϊ��ɫ
				btn_crt->bkctbl[1]=0X0000;//Ĭ�ϰ���Ϊ��ɫ				
				break;
			case BTN_TYPE_SGICON_TEXT://��ɫͼ��
				btn_crt->bkctbl=(u16*)gui_memin_malloc(4*2);//����8���ֽ�
				if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//�ͷ�֮ǰ������ڴ�
					return NULL;		//�ڴ���䲻��.
				}
				break;
		} 
	}else btn_crt->bkctbl=NULL;//�Ա�׼��ť��ͼƬ��ť�����Ч.
    return btn_crt;
}
//ɾ����ť
//btn_del:Ҫɾ���İ���
void btn_delete(_btn_obj * btn_del)
{
	if(btn_del==NULL)return;//�Ƿ��ĵ�ַ,ֱ���˳�
	if((btn_del->type&0X0f)==BTN_TYPE_TEXTA)btn_recover_backcolor(btn_del);//�ָ�����ɫ
	gui_memin_free(btn_del->bkctbl);
	gui_memin_free(btn_del);
}
//���	
//btnx:����ָ��
//in_key:���밴��ָ��
//����ֵ:
//0,û��һ����Ч�Ĳ���	
//1,��һ����Ч����		 					   
u8 btn_check(_btn_obj * btnx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
//	u8 sta;
	u8 btnok=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
			if(btnx->top<key->y&&key->y<(btnx->top+btnx->height)&&btnx->left<key->x&&key->x<(btnx->left+btnx->width))//�ڰ����ڲ�
			{
				if((btnx->sta&0X80)==0)	//�ϴ�����״̬Ϊ�ɿ�
				{
					btnx->sta|=1<<6;	//��Ǵ˴ΰ�����Ч
					
					btnx->sta&=~(0X03);
					btnx->sta|=BTN_PRESS;//����
					
					btn_draw(btnx);//����ť  
					btnok=1;
 				}
				btnx->sta|=0X80;//��ǰ���		   
			}
			else 
			{
				btnx->sta&=0X7F;//����ɿ�
				if((btnx->sta&0x03)==BTN_PRESS)		//֮ǰ�а���������
				{
				if(key->ksta&0x01)				//��ʱ�������ǰ���״̬,��Ч!!
					{
	 				//	btnx->sta=0;					
					//	btn_draw(btnx);				//�ָ���ť 
					}else if((btnx->type&0X80)==0)	//���������ɿ�
					{
						btnx->sta&=~(0X03);
						btnx->sta|=BTN_RELEASE;	//�ɿ�
						btn_draw(btnx);			//����ť 
						btnok=1;  
					}
				}
			}
			
			break;
		case IN_TYPE_KEY:	//��������
			break;
		case IN_TYPE_JOYPAD://�ֱ�����
			break;
		case IN_TYPE_MOUSE:	//�������
			break;
		default:
			break;
	}
	return btnok;
}

//����׼��ť
//btnx:��ť
void btn_draw_stdbtn(_btn_obj * btnx)
{										  
	u8 sta;
	sta=btnx->sta&0x03;
	gui_fill_rectangle(btnx->left+2,btnx->top+2,btnx->width-4,btnx->height-4,STD_BTN_INSC);//����ڲ�
	switch(sta)
	{
		case BTN_RELEASE://�ɿ�
		case BTN_INACTIVE://δ����
			gui_draw_vline(btnx->left,btnx->top,btnx->height,STD_BTN_LTOC);			//������
		 	gui_draw_hline(btnx->left,btnx->top,btnx->width,STD_BTN_LTOC);			//������
			gui_draw_vline(btnx->left+1,btnx->top+1,btnx->height-2,STD_BTN_LTIC);	//������
		 	gui_draw_hline(btnx->left+1,btnx->top+1,btnx->width-2,STD_BTN_LTIC);	//������
		
			gui_draw_vline(btnx->left+btnx->width-1,btnx->top,btnx->height,STD_BTN_RBOC);		//������
		 	gui_draw_hline(btnx->left,btnx->top+btnx->height-1,btnx->width,STD_BTN_RBOC);		//������
			gui_draw_vline(btnx->left+btnx->width-2,btnx->top+1,btnx->height-2,STD_BTN_RBIC);	//������
		 	gui_draw_hline(btnx->left+1,btnx->top+btnx->height-2,btnx->width-2,STD_BTN_RBIC);	//������
 			if(sta==0)gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption,1);//��ʾcaption
			else//δ����
			{
				gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,STD_BTN_DFRC,btnx->font,btnx->caption,1);
				gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,STD_BTN_DFLC,btnx->font,btnx->caption,1);
 			}
			break;
		case BTN_PRESS://����
		  	gui_draw_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,GUI_COLOR_BLACK);		//���
		  	gui_draw_rectangle(btnx->left+1,btnx->top+1,btnx->width-2,btnx->height-2,STD_BTN_RBOC);	//�ڿ�
 			gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption,1);//��ʾcaption
			break;
	}	    		
} 
//��ͼƬ��ť
//btnx:��ť
void btn_draw_picbtn(_btn_obj * btnx)
{
	u8 sta;
 	sta=btnx->sta&0x03;
 	switch(sta)
	{
		case BTN_RELEASE://�ɿ�
		case BTN_INACTIVE://δ����	
 			minibmp_decode(btnx->picbtnpathu,btnx->left,btnx->top,btnx->width,btnx->height,0,0);
 			break;
		case BTN_PRESS://����,�����ڰ���ͼƬʱ,��ʾ���µ�ͼƬ.��������ʱ,���ɿ�ͼƬ����alphablending
			if(btnx->picbtnpathd!=NULL)
			{
 				minibmp_decode(btnx->picbtnpathd,btnx->left,btnx->top,btnx->width,btnx->height,0,0);
 			}else 
			{
				minibmp_decode(btnx->picbtnpathu,btnx->left,btnx->top,btnx->width,btnx->height,0xffff,0x90);
			}
			break;
	}	    		
}  


//��Բ�ǰ�ť
//btnx:��ť
void btn_draw_arcbtn(_btn_obj * btnx)
{
	u16 angcolor=0,upcolor=0,downcolor=0;	
	u8 sta;
 	if(btnx->arcbtnr>btnx->width/2||btnx->arcbtnr>btnx->height/2)return;//������Χ��
	sta=btnx->sta&0x03;											 			
	switch(sta)
	{
		case BTN_RELEASE://����(�ɿ�)
			angcolor=btnx->bkctbl[1];
			upcolor=btnx->bkctbl[2];
			downcolor=btnx->bkctbl[3];
			break;
		case BTN_PRESS://����
	  		angcolor=gui_alpha_blend565(btnx->bkctbl[2],GUI_COLOR_WHITE,16);
	 		upcolor=angcolor; 
 			downcolor=gui_alpha_blend565(btnx->bkctbl[3],GUI_COLOR_WHITE,16); 
			break;
		case BTN_INACTIVE://���ְ�ťû��δ����״̬
			return;
	} 
  gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,1,upcolor,downcolor);//���Բ�ǰ�ť
  gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,0,btnx->bkctbl[0],btnx->bkctbl[0]);//��Բ�Ǳ߿�
	gui_draw_hline (btnx->left+btnx->arcbtnr,btnx->top+1,btnx->width-2*btnx->arcbtnr,angcolor);//��һ��		   
 	//��ʾcaption
	if(sta==0)gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption,1);
	else gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption,1);		   
}
//������͸�����ְ�ť
//btnx:��ť
void btn_draw_textabtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;	  
	switch(sta)
	{
		case BTN_RELEASE://����(�ɿ�)
    		btn_recover_backcolor(btnx);//����ɫ
		    gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption,1);
			break;
		case BTN_PRESS://����   
    		btn_recover_backcolor(btnx);//����ɫ
		    gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption,1);
			break;
		case BTN_INACTIVE://���ְ�ťû��δ����״̬
			return;
	} 
}

//��������һ���ְ�ť
//btnx:��ť
void btn_draw_textbbtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;	  
	switch(sta)
	{
		case BTN_RELEASE://����(�ɿ�)		
			if(btnx->arcbtnr==0)
			{		gui_fill_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bkctbl[0]);//��� 
			}
			else
			{		gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,1,btnx->bkctbl[0],btnx->bkctbl[0]);
			}
			gui_show_strmid(btnx->left+btnx->caption_left,btnx->top+btnx->caption_top,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfucolor,btnx->font,btnx->caption,1);
			break;
		case BTN_PRESS://����  
			if(btnx->arcbtnr==0)
			{		gui_fill_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bkctbl[1]);//��� 
			}
			else
			{		gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,1,btnx->bkctbl[1],btnx->bkctbl[1]);
			}			
			gui_show_strmid(btnx->left+btnx->caption_left+1,btnx->top+btnx->caption_top+1,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfdcolor,btnx->font,btnx->caption,1);
			break;
		case BTN_INACTIVE://���ְ�ťû��δ����״̬
			return;
	} 
}
//����ɫͼ�갴ť//20150912����ģʽB������type�ĵ�7λ�����º�̧��һ��״̬���͵�6λ�����´�����ĸ�ͼƬ��
//����ɫͼ�갴ť
//btnx:��ť
void btn_draw_sgiconbtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;	  
	if((btnx->type&0x80)!=0x80)	 //ģʽA
  {
			switch(sta)
			{
				case BTN_RELEASE://����(�ɿ�)width; 				  		//���u16 height;
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathu,btnx->bkctbl[2],btnx->bkctbl[0]);//20150909
						if(btnx->arcbtnr)
						gui_show_strmid(btnx->left+btnx->caption_left,btnx->top+btnx->caption_top,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfucolor,btnx->font,btnx->caption,1);
					break;
				case BTN_PRESS://����   
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathd,btnx->bkctbl[3],btnx->bkctbl[1]);//20150909
						if(btnx->arcbtnr)
						gui_show_strmid(btnx->left+btnx->caption_left,btnx->top+btnx->caption_top,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfdcolor,btnx->font,btnx->caption,1);
					break;
				case BTN_INACTIVE://���ְ�ťû��δ����״̬
					return;
			} 
	}
	else// ģʽB
	{	    switch(sta)
			{
				case BTN_RELEASE://����(�ɿ�)width; 				  		//���u16 height;
					 if((btnx->type&0x40)!=0x40)
					 {	
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathu,btnx->bkctbl[2],btnx->bkctbl[1]);//20150909
					 }
					 else	
					 {		
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathd,btnx->bkctbl[2],btnx->bkctbl[1]);//20150909
					 }
					 if(btnx->arcbtnr)//
						gui_show_strmid(btnx->left+btnx->caption_left,btnx->top+btnx->caption_top,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfucolor,btnx->font,btnx->caption,1);
					break;
				case BTN_PRESS://���� 
           if((btnx->type&0x40)!=0x40)
					 {	
						 btnx->type|=0x40;
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathd,btnx->bkctbl[3],btnx->bkctbl[1]);//20150909
					 }
					 else	
					 {		
						 btnx->type&=~0x40;
						gui_draw_single_color_icos(btnx->left+btnx->icon_left,btnx->top+btnx->icon_top,btnx->icon_width,btnx->icon_height,btnx->picbtnpathu,btnx->bkctbl[3],btnx->bkctbl[1]);//20150909
					 }
					 if(btnx->arcbtnr)
						gui_show_strmid(btnx->left+btnx->caption_left,btnx->top+btnx->caption_top,btnx->width-btnx->caption_left,btnx->height-btnx->caption_top,btnx->bcfdcolor,btnx->font,btnx->caption,1);
					break;
				case BTN_INACTIVE://���ְ�ťû��δ����״̬
					return;
			}
	}
}

//����ť
//btnx:��ť
void btn_draw(_btn_obj * btnx) 	
{
	if(btnx==NULL)return;//��Ч,ֱ���˳�
	switch(btnx->type&0x0f)
	{
		case BTN_TYPE_STD://��׼��ť
			btn_draw_stdbtn(btnx);
			break;
		case BTN_TYPE_PIC://ͼƬ��ť
			btn_draw_picbtn(btnx);
			break;
		case BTN_TYPE_ANG://�߽ǰ�ť
			btn_draw_arcbtn(btnx);
			break;
		case BTN_TYPE_TEXTA://���ְ�ť(����͸��)
			btn_draw_textabtn(btnx);
			break;
		case BTN_TYPE_TEXTB://���ְ�ť(������һ)
			btn_draw_textbbtn(btnx);
			break;
		case BTN_TYPE_SGICON_TEXT://����ɫͼ�갴ť�ɴ�����
			btn_draw_sgiconbtn(btnx);
			break;

 	}
	if(btnx->sta&(1<<6))
	{
		if((btnx->sta&0x03)==BTN_PRESS)btn_press_hook(btnx);//�������µĹ��Ӻ���
		else if((btnx->sta&0x03)==BTN_RELEASE)btn_release_hook(btnx);//�����ɿ�ʱ�Ĺ��Ӻ��� 
	}	
}	

//��ť�ɿ����Ӻ���
//btnx:��ťָ��
void btn_release_hook(_btn_obj *btnx)
{
	//printf("button release %d status:%d\r\n",btnx->id,btnx->sta);
}
//��ť���¹��Ӻ���
//btnx:��ťָ��
void btn_press_hook(_btn_obj *btnx)
{
	//printf("button press %d status:%d\r\n",btnx->id,btnx->sta);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
 















