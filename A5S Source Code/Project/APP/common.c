/*
*********************************************************************************************************
*
*	ģ������ : GUIӦ�ù���ģ��
*	�ļ����� : common.c
*	��    �� : V1.0
*	˵    �� : �û�GUI���湲��ģ�飬��������һЩ������Ϣ
*
*
*********************************************************************************************************
*/


/**********************����ͷ�ļ�*************************/

#include "common.h"




/**********************************************/
//GUI֧�ֵ�����������Ŀ
//ϵͳ����������


_languageSet_e	LANGUAGE = ENGLISH;//����Ĭ��ΪӢ��

 
//PC2LCD2002����ȡģ����:����ʽ,˳��(��λ��ǰ),����.C51��ʽ.																		    
//��������:
//���������:ASCII��+��(' '+95)
//��ͨ����:ASCII��
char *const APP_ASCII_S6030="1:/SYSTEM/COMMON/fonts60.fon";	//���������60*30����������·�� 
char *const APP_ASCII_5427="1:/SYSTEM/COMMON/font54.fon";		//��ͨ����54*27����������·�� 
char *const APP_ASCII_3618="1:/SYSTEM/COMMON/font36.fon";		//��ͨ����36*18����������·��
char *const APP_ASCII_2814="1:/SYSTEM/COMMON/font28.fon";		//��ͨ����28*14����������·�� 


u8* asc2_s6030=0;	//���������60*30���������
u8* asc2_5427=0;	//��ͨ����54*27���������
u8* asc2_3618=0;	//��ͨ����36*18���������
u8* asc2_2814=0;	//��ͨ����28*14���������

char *const gui_english_icon_path_tbl[]=
{
	"1:/SYSTEM/PICTURE/en_logo.bin",//��������

//��ҳ
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/en_bmp_mov.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zero.bin",
	"1:/SYSTEM/PICTURE/en_bmp_printing.bin",
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling.bin",
	"1:/SYSTEM/PICTURE/en_bmp_set.bin",
	"1:/SYSTEM/PICTURE/en_bmp_language.bin",
	
//Ԥ�Ƚ���	
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step1_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step5_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step10_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",//�����¶�����
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",//����
	"1:/SYSTEM/PICTURE/en_bmp_bed.bin",
	
//�ƶ�����	
	"1:/SYSTEM/PICTURE/en_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_xDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_yAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_yDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move0_1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move10.bin",

//��ԭ�����	
	"1:/SYSTEM/PICTURE/en_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroX.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroY.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroZ.bin",

//��ӡ�ļ�ѡ�����	
	"1:/SYSTEM/PICTURE/en_bmp_file.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fileSys.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pageDown.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pageUp.bin",
	
//��ӡgcode����	
	"1:/SYSTEM/PICTURE/en_bmp_bed_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_extru1_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fan_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_menu.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pause.bin",
	"1:/SYSTEM/PICTURE/en_bmp_resume.bin",
	"1:/SYSTEM/PICTURE/en_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/en_bmp_more.bin",
//�����Ͻ���	
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step1_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step5_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step10_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_high.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_normal.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_slow.bin",

//��ƽ����
	"1:/SYSTEM/PICTURE/en_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling2.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling3.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling4.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling5.bin",


//���ý���
	"1:/SYSTEM/PICTURE/en_bmp_about.bin",
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",//����
	"1:/SYSTEM/PICTURE/en_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fontUpdt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_iconUpdt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_lcdcalibt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_motor_off.bin",
	
	"1:/SYSTEM/PICTURE/en_bmp_speed.bin",//�ٶ�ͼ��
	"1:/SYSTEM/PICTURE/en_bmp_speed1.bin",//�ٶȲ���1
	"1:/SYSTEM/PICTURE/en_bmp_speed5.bin",//�ٶȲ���5
	"1:/SYSTEM/PICTURE/en_bmp_speed10.bin",//�ٶȲ���10	

#if EN_QR_CODE==1		
	"1:/SYSTEM/PICTURE/en_about_infor.bin",//��ά��
#endif
};
char *const gui_chinese_icon_path_tbl[]=
{
	"1:/SYSTEM/PICTURE/ch_logo.bin",//��������

//��ҳ
	"1:/SYSTEM/PICTURE/ch_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_mov.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zero.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_printing.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_set.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_language.bin",
	
//Ԥ�Ƚ���	
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step1_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step5_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step10_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",//�����¶�����
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",//����
	"1:/SYSTEM/PICTURE/ch_bmp_bed.bin",
	
//�ƶ�����	
	"1:/SYSTEM/PICTURE/ch_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_xDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_yAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_yDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move0_1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move10.bin",

//��ԭ�����	
	"1:/SYSTEM/PICTURE/ch_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroX.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroY.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroZ.bin",

//��ӡ�ļ�ѡ�����	
	"1:/SYSTEM/PICTURE/ch_bmp_file.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fileSys.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pageDown.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pageUp.bin",
	
//��ӡgcode����	
	"1:/SYSTEM/PICTURE/ch_bmp_bed_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_extru1_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fan_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_menu.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pause.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_resume.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_more.bin",
//�����Ͻ���	
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step1_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step5_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step10_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_high.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_normal.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_slow.bin",

//��ƽ����
	"1:/SYSTEM/PICTURE/ch_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling2.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling3.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling4.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling5.bin",

//���ý���
	"1:/SYSTEM/PICTURE/ch_bmp_about.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",//����
	"1:/SYSTEM/PICTURE/ch_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fontUpdt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_iconUpdt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_lcdcalibt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_motor_off.bin",
	
	"1:/SYSTEM/PICTURE/ch_bmp_speed.bin",//�ٶ�ͼ��
	
	"1:/SYSTEM/PICTURE/ch_bmp_speed1.bin",//�ٶȲ���1
	"1:/SYSTEM/PICTURE/ch_bmp_speed5.bin",//�ٶȲ���5
	"1:/SYSTEM/PICTURE/ch_bmp_speed10.bin",//�ٶȲ���10

#if EN_QR_CODE==1	
	"1:/SYSTEM/PICTURE/ch_about_infor.bin",//��ά��
#endif

};

//ALIENTEK logo ͼ��(18*24��С)
//PCtoLCD2002ȡģ��ʽ:����,����ʽ,˳��


//��ȡ����ɫ
//x,y,width,height:����ɫ��ȡ��Χ
//ctbl:����ɫ���ָ��
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u16 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			ctbl[ccnt]=gui_phy.read_point(x0,y0);//��ȡ��ɫ
			ccnt++;
		}
	}
}  
//�ָ�����ɫ
//x,y,width,height:����ɫ��ԭ��Χ
//ctbl:����ɫ���ָ��
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u16 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			gui_phy.draw_point(x0,y0,ctbl[ccnt]);//��ȡ��ɫ
			ccnt++;
		}
	}
}
//2ɫ��
//x,y,width,height:���꼰�ߴ�.
//mode:	���÷ֽ���
//	    [3]:�ұ߷ֽ���
//		[2]:��߷ֽ���
//		[1]:�±߷ֽ���
//		[0]:�ϱ߷ֽ���
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,LIGHTBLUE);  			//���ײ���ɫ(ǳ��ɫ)	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,GRAYBLUE); 	//���ײ���ɫ(����ɫ)
	if(mode&0x01)gui_draw_hline(x,y,width,DARKBLUE);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,DARKBLUE);
	if(mode&0x04)gui_draw_vline(x,y,height,DARKBLUE);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,DARKBLUE);
} 
//�õ����ֵ�λ��
//num:����
//dir:0,�Ӹ�λ����λ.1,�ӵ�λ����λ.
//����ֵ:���ֵ�λ��.(���Ϊ10λ)
//ע:0,��λ��Ϊ0λ.
u8 app_get_numlen(long long num,u8 dir)
{
#define MAX_NUM_LENTH		10		//�������ֳ���
	u8 i=0,j;
	u8 temp=0;  
	if(dir==0)//�Ӹߵ���
	{
		i=MAX_NUM_LENTH-1;
		while(1)
		{
			temp=(num/gui_pow(10,i))%10;
			if(temp||i==0)break;
			i--;
		}
	}else	//�ӵ͵���
	{
		j=0;
 		while(1)
		{
			if(num%10)
			{
				i=app_get_numlen(num,0);    
				return i;
			}
			if(j==(MAX_NUM_LENTH-1))break;	 
			num/=10;
			j++;
		}
	}
	if(i)return i+1;
	else if(temp)return 1;
	else return 0;	
}
 
//��ʾ��ɫͼ��
//x,y,width,height:���꼰�ߴ�.
//icosbase:����λ��
//color:�������ɫ.
//bkcolor:����ɫ
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor)
{
	u16 rsize;
	u16 i,j;
	u8 temp;
	u16 t=0;
	u16 x0=x;//����x��λ��
	rsize=width/8+((width%8)?1:0);//ÿ�е��ֽ���
 	for(i=0;i<rsize*height;i++)
	{
		temp=icosbase[i];
		for(j=0;j<8;j++)
		{
	        if(temp&0x80)gui_phy.draw_point(x,y,color);  
			else gui_phy.draw_point(x,y,bkcolor);  	 
			temp<<=1;
			x++;
			t++;			//��ȼ�����
			if(t==width)
			{
				t=0;
				x=x0;
				y++;  
				break;
			}
		}
	}				
} 
//��ʾһ��������(֧�ָ���)
//ע��:����������Ǵ��ҵ�����ʾ��.
//x,y:��ʼ��ʾ������
//num:����	   
//flen:С����λ��.0,û��С������;1,��һλС��;2,����λС��;�����Դ�����.
//clen:Ҫ�����λ��(���ǰһ�εĺۼ�)
//font:����
//color:������ɫ.
//bkcolor:����ɫ
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor)
{			   
	u8 offpos=0;
	u8 ilen=0;						//�������ֳ��Ⱥ�С�����ֵĳ���
	u8 negative=0;					//�������
	u16 maxlen=(u16)clen*(font/2);	//����ĳ���
	gui_fill_rectangle(x-maxlen,y,maxlen,font,bkcolor);//���֮ǰ�ĺۼ�
	if(num<0) 
	{
		num=-num;
  		negative=1;
	}
	ilen=app_get_numlen(num,0);	//�õ���λ��
	gui_phy.back_color=bkcolor;
	if(flen)
	{
 		gui_show_num(x-(font/2)*flen,y,flen,color,font,num,0X80);//��ʾС������
		gui_show_ptchar(x-(font/2)*(flen+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'.',0);//��ʾС����
		offpos=2+flen;
	} 
	if(ilen<=flen)ilen=1;
 	else 
	{
		offpos=ilen+1;
		ilen=ilen-flen;	//�õ��������ֵĳ���.
	}
	num=num/gui_pow(10,flen);//�õ���������
	gui_show_num(x-(font/2)*offpos,y,ilen,color,font,num,0X80);	//���0��ʾ 
	if(negative)gui_show_ptchar(x-(font/2)*(offpos+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'-',0);//��ʾ����		 
} 			  
					   
//topname:�����ʱ��Ҫ��ʾ������		 				 
//mode:
//[0]:0,����ʾ�Ϸ�ɫ��;1,��ʾ�Ϸ�ɫ��
//[1]:0,����ʾ�·�ɫ��;1,��ʾ�·�ɫ��
//[2]:0,����ʾ����;1,��ʾ����
//[3~7]:����
//����ֵ:��	 						  
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//�·ֽ���
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//�Ϸֽ���
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,topname,1);	  
}  
//��һ�������м���ʾ����
//x,y,width,height:����
//num:Ҫ��ʾ������
//len:λ��
//size:����ߴ�
//ptcolor,bkcolor:������ɫ�Լ�����ɫ   
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor)
{
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;//���ֳ���
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);//��ʾ�������
}
//��һ��ƽ�����ɵĲ�ɫ��(�����)
//���м�Ϊ���,����չ��
//x,y,width,height:����������ߴ�
//sergb,mrgb:��ֹ��ɫ���м���ɫ
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb)
{	  
	gui_draw_smooth_rectangle(x,y,width/2,height,sergb,mrgb);	   		//ǰ��ν���
	gui_draw_smooth_rectangle(x+width/2,y,width/2,height,mrgb,sergb);   //���ν���
}      

//�жϴ�������ǰֵ�ǲ�����ĳ��������
//tp:������
//x,y,width,height:����ͳߴ� 
//����ֵ:0,����������;1,��������.
u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height)
{						 	 
	if(tp->x[0]<=(x+width)&&tp->x[0]>=x&&tp->y[0]<=(y+height)&&tp->y[0]>=y)return 1;
	else return 0;							 	
}
//��ʾ��Ŀ
//x,y,itemwidth,itemheight:��Ŀ���꼰�ߴ�
//name:��Ŀ����
//icopath:ͼ��·��
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor)
{
  	gui_fill_rectangle(x,y,itemwidth,itemheight,bkcolor);	//��䱳��ɫ
	gui_show_ptstr(x+5,y+(itemheight-16)/2,x+itemwidth-10-APP_ITEM_ICO_SIZE-5,y+itemheight,0,color,16,name,1);	//��ʾ��Ŀ����
	if(icopath)minibmp_decode(icopath,x+itemwidth-10-APP_ITEM_ICO_SIZE,y+(itemheight-APP_ITEM_ICO_SIZE)/2,APP_ITEM_ICO_SIZE,APP_ITEM_ICO_SIZE,0,0);			//����APP_ITEM_ICO_SIZE*APP_ITEM_ICO_SIZE��bmpͼƬ
}		 

//������·����Ҫ���޸�
//des:Ŀ�껺����
//diskx:�µĴ��̱��
//src:ԭ·��
void app_system_file_chgdisk(char *des,char *diskx,char *src)
{ 
	//static u8 t=0;
	strcpy((char*)des,(const char*)src);
	des[0]=diskx[0]; 
	//printf("path[%d]:%s\r\n",t++,des);	//��ӡ�ļ���
}
//ϵͳ�ܹ���Ҫ��ͼ��/ͼƬ/ϵͳ�ļ���141��,����SPB����ռ20��.����121��ΪAPP������ռ��
//�⻹������SYSTEM/FONT�ļ����ڵ������ļ�.
//141���ļ��ܴ�С:1,033,804 �ֽ�(0.98MB)
//3�������ļ��ܴ�С:1,514,984 �ֽ�(1.44MB)
//������е�ϵͳ�ļ��Ƿ�����
//diskx:����·��."0",SD��;"1",SPI FLASH
//����ֵ:0,����
//    ����,�������

u8 app_system_file_check(char* diskx)
{
	u8 i;
	u8 rval=0;
	u8 res=0;
	u32 enPictureNum;
	u32 chPictureNum;
	FIL *f_check;
	char *path;
	f_check=(FIL *)gui_memin_malloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	path=gui_memin_malloc(200);						//Ϊpath�����ڴ� 
	if(!f_check||!path)rval=0XFF;//����ʧ��
	while(rval==0)
	{	
	
		enPictureNum = COUNT_ARRY_LENGTH(gui_english_icon_path_tbl);//������鳤��
		
		//���GUI����ͼ��
 		for(i=0;i<enPictureNum;i++)
		{
			
			app_system_file_chgdisk(path,diskx,gui_english_icon_path_tbl[i]); 
			//DBG("*****file:%s\r\n",path);
	 		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
			if(res)break;		//��ʧ��   
			f_close(f_check);    //�ر��ļ�  
		}
		rval+=i;
		if(i<enPictureNum){rval++;break;}   
		
		
		chPictureNum = COUNT_ARRY_LENGTH(gui_chinese_icon_path_tbl);//������鳤��
		//���GUI����ͼ��
 		for(i=0;i<chPictureNum;i++)
		{
			
			app_system_file_chgdisk(path,diskx,gui_chinese_icon_path_tbl[i]); 
			//DBG("*****file:%s\r\n",path);
	 		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
			if(res)break;		//��ʧ��   
			f_close(f_check);    //�ر��ļ�  
		}
		rval+=i;
		if(i<chPictureNum){rval++;break;}   
		
	
/*****************************��������ļ�*****************************************/	
#if 0
		app_system_file_chgdisk(path,diskx,APP_ASCII_S6030); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
		if(res)break;			//��ʧ��      
    	f_close(f_check);		//�ر��ļ�     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_5427); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
		if(res)break;			//��ʧ��      
    	f_close(f_check);		//�ر��ļ�     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_3618); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
		if(res)break;			//��ʧ��      
    	f_close(f_check);		//�ر��ļ�     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_2814); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//ֻ����ʽ���ļ�
		if(res)break;			//��ʧ��      
    	f_close(f_check);		//�ر��ļ�     
		rval++;	      
#endif
		
		DBG("\r\ntotal system files:%d\r\n",rval);
		rval=0;
		break;    
	}
	gui_memin_free(f_check);//�ͷ��ڴ� 
	gui_memin_free(path);	//�ͷ��ڴ�
	return rval; 
}

////////////////////////////////////////////////////////////////////////////////////////////
//һ�´��������ʵ��SYSTEM�ļ��и���
char *const  APP_SYSTEM_COMMONPATH="0:/SYSTEM/COMMON";		//APP�ļ���·��
char *const  APP_SYSTEM_FONTPATH="0:/SYSTEM/FONT";		//FONT�ļ���·��
char *const  GUI_PICTURE_DSTPATH="0:/SYSTEM/PICTURE";			//ͼƬ�ļ�Ŀ¼

char *const  APP_SYSTEM_DSTPATH="1:/SYSTEM";			//ϵͳ�ļ�Ŀ��·��

//�ļ�������Ϣ��ʾ���� 
//�ļ�������Ϣ��ʾ���������
static u16 cpdmsg_x;
static u16 cpdmsg_y;
static u8  cpdmsg_fsize;	


//ϵͳ������ʱ��,������ʾ���½���
//*pname:�����ļ�����
//pct:�ٷֱ�
//mode:ģʽ
//[0]:�����ļ���
//[1]:���°ٷֱ�pct
//[2]:�����ļ���
//[3~7]:����
//����ֵ:0,����;
//       1,��������
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode)
{	
	u8 offset = 10;												
	if(mode&0X01)
	{
		gui_fill_rectangle_DMA(cpdmsg_x,cpdmsg_y,lcddev.width,cpdmsg_fsize,BLACK);	//����ɫ
		LCD_ShowString(cpdmsg_x,cpdmsg_y,30*(cpdmsg_fsize/2),cpdmsg_fsize,cpdmsg_fsize,pname);	//��ʾ�ļ���,�16���ַ����
		DBG("\r\nCopy File:%s\r\n",pname);  
	}
	if(mode&0X04)DBG("Copy Folder:%s\r\n",pname);
	if(mode&0X02)//���°ٷֱ�
	{
		
		LCD_Show_Bar(cpdmsg_x + offset,cpdmsg_y + cpdmsg_fsize + offset,cpdmsg_fsize,25*(cpdmsg_fsize/2),pct,100,GREEN,BLACK);
		LCD_ShowNum(cpdmsg_x+(25 + 2)*(cpdmsg_fsize/2),cpdmsg_y+ cpdmsg_fsize + offset,pct,3,cpdmsg_fsize);//��ʾ��ֵ
		LCD_ShowString(cpdmsg_x+(25+ 2 + 3)*(cpdmsg_fsize/2),cpdmsg_y+ cpdmsg_fsize + offset,lcddev.width,cpdmsg_fsize,cpdmsg_fsize,(u8*)"%");
		DBG("File Copyed:%d\r\n",pct);
	}
	return 0;	
}
//����app_boot_cpdmsg����ʾ����
//x,y:����.
//fisze:�����С
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize)
{
	cpdmsg_x=x;
	cpdmsg_y=y;
	cpdmsg_fsize=fsize;
}
//ϵͳ����
//����ֵ:0,����
//    ����,����,0XFFǿ���˳���
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode))
{
	u32 totsize=0;
	u32 cpdsize=0;
	u8 res=0;
//	totsize+=exf_fdsize((u8*)APP_SYSTEM_COMMONPATH);//�õ������ļ��е��ܴ�С
//	totsize+=exf_fdsize((u8*)APP_SYSTEM_FONTPATH);
	totsize+=exf_fdsize((u8*)GUI_PICTURE_DSTPATH);

	DBG("totsize:%d\r\n",totsize);
	f_mkdir((const TCHAR *)APP_SYSTEM_DSTPATH);//ǿ�ƴ���Ŀ���ļ���
	
#if 0	
 	res=exf_fdcopy(fcpymsg,(u8*)APP_SYSTEM_COMMONPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//��һ�׶θ��Ƴ���
		else return 0XFF;		//ǿ���˳�
	}
	
	res=exf_fdcopy(fcpymsg,(u8*)APP_SYSTEM_FONTPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//�ڶ��׶θ��Ƴ���
		else return 0XFF;		//ǿ���˳�
	}
#endif
	
	
 	res=exf_fdcopy(fcpymsg,(u8*)GUI_PICTURE_DSTPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//�����׶θ��Ƴ���
		else return 0XFF;		//ǿ���˳�
	}
	return 0;
}
//�õ�STM32�����к�
//sn0,sn1,sn2:3���������к�
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2)
{
	*sn0=*(vu32*)(0x1FFFF7E8);
	*sn1=*(vu32*)(0x1FFFF7EC);
	*sn2=*(vu32*)(0x1FFFF7F0);    
}
//��ӡSN
void app_usmart_getsn(void)
{
	u32 sn0,sn1,sn2;
	app_getstm32_sn(&sn0,&sn1,&sn2);
	printf("\r\nSerial Number:%X%X%X\r\n",sn0,sn1,sn2);
}

//��������
//���룺id--1,ENGLISH;	id--0,CHINESE
 void set_language(u8 id)
 {
	if(id == ENGLISH) LANGUAGE = ENGLISH;
	else LANGUAGE = CHINESE;
	 
 }










