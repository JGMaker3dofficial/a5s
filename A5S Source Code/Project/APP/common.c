/*
*********************************************************************************************************
*
*	模块名称 : GUI应用公共模块
*	文件名称 : common.c
*	版    本 : V1.0
*	说    明 : 用户GUI界面共有模块，用来管理一些共有信息
*
*
*********************************************************************************************************
*/


/**********************包含头文件*************************/

#include "common.h"




/**********************************************/
//GUI支持的语言种类数目
//系统语言种类数


_languageSet_e	LANGUAGE = ENGLISH;//语言默认为英文

 
//PC2LCD2002字体取模方法:逐列式,顺向(高位在前),阴码.C51格式.																		    
//特殊字体:
//数码管字体:ASCII集+℃(' '+95)
//普通字体:ASCII集
char *const APP_ASCII_S6030="1:/SYSTEM/COMMON/fonts60.fon";	//数码管字体60*30大数字字体路径 
char *const APP_ASCII_5427="1:/SYSTEM/COMMON/font54.fon";		//普通字体54*27大数字字体路径 
char *const APP_ASCII_3618="1:/SYSTEM/COMMON/font36.fon";		//普通字体36*18大数字字体路径
char *const APP_ASCII_2814="1:/SYSTEM/COMMON/font28.fon";		//普通字体28*14大数字字体路径 


u8* asc2_s6030=0;	//数码管字体60*30大字体点阵集
u8* asc2_5427=0;	//普通字体54*27大字体点阵集
u8* asc2_3618=0;	//普通字体36*18大字体点阵集
u8* asc2_2814=0;	//普通字体28*14大字体点阵集

char *const gui_english_icon_path_tbl[]=
{
	"1:/SYSTEM/PICTURE/en_logo.bin",//开机界面

//主页
	"1:/SYSTEM/PICTURE/en_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/en_bmp_mov.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zero.bin",
	"1:/SYSTEM/PICTURE/en_bmp_printing.bin",
	"1:/SYSTEM/PICTURE/en_bmp_extru1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling.bin",
	"1:/SYSTEM/PICTURE/en_bmp_set.bin",
	"1:/SYSTEM/PICTURE/en_bmp_language.bin",
	
//预热界面	
	"1:/SYSTEM/PICTURE/en_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/en_bmp_Dec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step1_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step5_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step10_degree.bin",
	"1:/SYSTEM/PICTURE/en_bmp_manual_off.bin",//清零温度设置
	"1:/SYSTEM/PICTURE/en_bmp_return.bin",//返回
	"1:/SYSTEM/PICTURE/en_bmp_bed.bin",
	
//移动界面	
	"1:/SYSTEM/PICTURE/en_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_xDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_yAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_yDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zAdd.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zDec.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move0_1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step_move10.bin",

//回原点界面	
	"1:/SYSTEM/PICTURE/en_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroX.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroY.bin",
	"1:/SYSTEM/PICTURE/en_bmp_zeroZ.bin",

//打印文件选择界面	
	"1:/SYSTEM/PICTURE/en_bmp_file.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fileSys.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pageDown.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pageUp.bin",
	
//打印gcode界面	
	"1:/SYSTEM/PICTURE/en_bmp_bed_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_extru1_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fan_no_words.bin",
	"1:/SYSTEM/PICTURE/en_bmp_menu.bin",
	"1:/SYSTEM/PICTURE/en_bmp_pause.bin",
	"1:/SYSTEM/PICTURE/en_bmp_resume.bin",
	"1:/SYSTEM/PICTURE/en_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/en_bmp_more.bin",
//进退料界面	
	"1:/SYSTEM/PICTURE/en_bmp_in.bin",
	"1:/SYSTEM/PICTURE/en_bmp_out.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step1_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step5_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_step10_mm.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_high.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_normal.bin",
	"1:/SYSTEM/PICTURE/en_bmp_speed_slow.bin",

//调平界面
	"1:/SYSTEM/PICTURE/en_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling2.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling3.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling4.bin",
	"1:/SYSTEM/PICTURE/en_bmp_leveling5.bin",


//设置界面
	"1:/SYSTEM/PICTURE/en_bmp_about.bin",
	"1:/SYSTEM/PICTURE/en_bmp_filamentchange.bin",//换料
	"1:/SYSTEM/PICTURE/en_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/en_bmp_fontUpdt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_iconUpdt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_lcdcalibt.bin",
	"1:/SYSTEM/PICTURE/en_bmp_motor_off.bin",
	
	"1:/SYSTEM/PICTURE/en_bmp_speed.bin",//速度图标
	"1:/SYSTEM/PICTURE/en_bmp_speed1.bin",//速度步进1
	"1:/SYSTEM/PICTURE/en_bmp_speed5.bin",//速度步进5
	"1:/SYSTEM/PICTURE/en_bmp_speed10.bin",//速度步进10	

#if EN_QR_CODE==1		
	"1:/SYSTEM/PICTURE/en_about_infor.bin",//二维码
#endif
};
char *const gui_chinese_icon_path_tbl[]=
{
	"1:/SYSTEM/PICTURE/ch_logo.bin",//开机界面

//主页
	"1:/SYSTEM/PICTURE/ch_bmp_preHeat.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_mov.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zero.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_printing.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_extru1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_set.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_language.bin",
	
//预热界面	
	"1:/SYSTEM/PICTURE/ch_bmp_Add.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_Dec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step1_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step5_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step10_degree.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_manual_off.bin",//清零温度设置
	"1:/SYSTEM/PICTURE/ch_bmp_return.bin",//返回
	"1:/SYSTEM/PICTURE/ch_bmp_bed.bin",
	
//移动界面	
	"1:/SYSTEM/PICTURE/ch_bmp_xAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_xDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_yAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_yDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zAdd.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zDec.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move0_1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step_move10.bin",

//回原点界面	
	"1:/SYSTEM/PICTURE/ch_bmp_zeroA.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroX.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroY.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_zeroZ.bin",

//打印文件选择界面	
	"1:/SYSTEM/PICTURE/ch_bmp_file.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fileSys.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pageDown.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pageUp.bin",
	
//打印gcode界面	
	"1:/SYSTEM/PICTURE/ch_bmp_bed_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_extru1_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fan_no_words.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_menu.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_pause.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_resume.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_stop.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_more.bin",
//进退料界面	
	"1:/SYSTEM/PICTURE/ch_bmp_in.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_out.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step1_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step5_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_step10_mm.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_high.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_normal.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_speed_slow.bin",

//调平界面
	"1:/SYSTEM/PICTURE/ch_bmp_leveling1.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling2.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling3.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling4.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_leveling5.bin",

//设置界面
	"1:/SYSTEM/PICTURE/ch_bmp_about.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_filamentchange.bin",//换料
	"1:/SYSTEM/PICTURE/ch_bmp_fan.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_fontUpdt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_iconUpdt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_lcdcalibt.bin",
	"1:/SYSTEM/PICTURE/ch_bmp_motor_off.bin",
	
	"1:/SYSTEM/PICTURE/ch_bmp_speed.bin",//速度图标
	
	"1:/SYSTEM/PICTURE/ch_bmp_speed1.bin",//速度步进1
	"1:/SYSTEM/PICTURE/ch_bmp_speed5.bin",//速度步进5
	"1:/SYSTEM/PICTURE/ch_bmp_speed10.bin",//速度步进10

#if EN_QR_CODE==1	
	"1:/SYSTEM/PICTURE/ch_about_infor.bin",//二维码
#endif

};

//ALIENTEK logo 图标(18*24大小)
//PCtoLCD2002取模方式:阴码,逐行式,顺向


//读取背景色
//x,y,width,height:背景色读取范围
//ctbl:背景色存放指针
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u16 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			ctbl[ccnt]=gui_phy.read_point(x0,y0);//读取颜色
			ccnt++;
		}
	}
}  
//恢复背景色
//x,y,width,height:背景色还原范围
//ctbl:背景色存放指针
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u16 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			gui_phy.draw_point(x0,y0,ctbl[ccnt]);//读取颜色
			ccnt++;
		}
	}
}
//2色条
//x,y,width,height:坐标及尺寸.
//mode:	设置分界线
//	    [3]:右边分界线
//		[2]:左边分界线
//		[1]:下边分界线
//		[0]:上边分界线
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,LIGHTBLUE);  			//填充底部颜色(浅蓝色)	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,GRAYBLUE); 	//填充底部颜色(灰蓝色)
	if(mode&0x01)gui_draw_hline(x,y,width,DARKBLUE);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,DARKBLUE);
	if(mode&0x04)gui_draw_vline(x,y,height,DARKBLUE);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,DARKBLUE);
} 
//得到数字的位数
//num:数字
//dir:0,从高位到低位.1,从低位到高位.
//返回值:数字的位数.(最大为10位)
//注:0,的位数为0位.
u8 app_get_numlen(long long num,u8 dir)
{
#define MAX_NUM_LENTH		10		//最大的数字长度
	u8 i=0,j;
	u8 temp=0;  
	if(dir==0)//从高到底
	{
		i=MAX_NUM_LENTH-1;
		while(1)
		{
			temp=(num/gui_pow(10,i))%10;
			if(temp||i==0)break;
			i--;
		}
	}else	//从低到高
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
 
//显示单色图标
//x,y,width,height:坐标及尺寸.
//icosbase:点整位置
//color:画点的颜色.
//bkcolor:背景色
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor)
{
	u16 rsize;
	u16 i,j;
	u8 temp;
	u16 t=0;
	u16 x0=x;//保留x的位置
	rsize=width/8+((width%8)?1:0);//每行的字节数
 	for(i=0;i<rsize*height;i++)
	{
		temp=icosbase[i];
		for(j=0;j<8;j++)
		{
	        if(temp&0x80)gui_phy.draw_point(x,y,color);  
			else gui_phy.draw_point(x,y,bkcolor);  	 
			temp<<=1;
			x++;
			t++;			//宽度计数器
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
//显示一个浮点数(支持负数)
//注意:这里的坐标是从右到左显示的.
//x,y:开始显示的坐标
//num:数字	   
//flen:小数的位数.0,没有小数部分;1,有一位小数;2,有两位小数;其他以此类推.
//clen:要清除的位数(清除前一次的痕迹)
//font:字体
//color:字体颜色.
//bkcolor:背景色
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor)
{			   
	u8 offpos=0;
	u8 ilen=0;						//整数部分长度和小数部分的长度
	u8 negative=0;					//负数标记
	u16 maxlen=(u16)clen*(font/2);	//清除的长度
	gui_fill_rectangle(x-maxlen,y,maxlen,font,bkcolor);//清除之前的痕迹
	if(num<0) 
	{
		num=-num;
  		negative=1;
	}
	ilen=app_get_numlen(num,0);	//得到总位数
	gui_phy.back_color=bkcolor;
	if(flen)
	{
 		gui_show_num(x-(font/2)*flen,y,flen,color,font,num,0X80);//显示小数部分
		gui_show_ptchar(x-(font/2)*(flen+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'.',0);//显示小数点
		offpos=2+flen;
	} 
	if(ilen<=flen)ilen=1;
 	else 
	{
		offpos=ilen+1;
		ilen=ilen-flen;	//得到整数部分的长度.
	}
	num=num/gui_pow(10,flen);//得到整数部分
	gui_show_num(x-(font/2)*offpos,y,ilen,color,font,num,0X80);	//填充0显示 
	if(negative)gui_show_ptchar(x-(font/2)*(offpos+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'-',0);//显示符号		 
} 			  
					   
//topname:浏览的时候要显示的名字		 				 
//mode:
//[0]:0,不显示上方色条;1,显示上方色条
//[1]:0,不显示下方色条;1,显示下方色条
//[2]:0,不显示名字;1,显示名字
//[3~7]:保留
//返回值:无	 						  
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//下分界线
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//上分界线
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,topname,1);	  
}  
//在一个区域中间显示数字
//x,y,width,height:区域
//num:要显示的数字
//len:位数
//size:字体尺寸
//ptcolor,bkcolor:画笔颜色以及背景色   
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor)
{
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;//数字长度
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);//显示这个数字
}
//画一条平滑过渡的彩色线(或矩形)
//以中间为间隔,两边展开
//x,y,width,height:线条的坐标尺寸
//sergb,mrgb:起止颜色和中间颜色
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb)
{	  
	gui_draw_smooth_rectangle(x,y,width/2,height,sergb,mrgb);	   		//前半段渐变
	gui_draw_smooth_rectangle(x+width/2,y,width/2,height,mrgb,sergb);   //后半段渐变
}      

//判断触摸屏当前值是不是在某个区域内
//tp:触摸屏
//x,y,width,height:坐标和尺寸 
//返回值:0,不在区域内;1,在区域内.
u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height)
{						 	 
	if(tp->x[0]<=(x+width)&&tp->x[0]>=x&&tp->y[0]<=(y+height)&&tp->y[0]>=y)return 1;
	else return 0;							 	
}
//显示条目
//x,y,itemwidth,itemheight:条目坐标及尺寸
//name:条目名字
//icopath:图标路径
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor)
{
  	gui_fill_rectangle(x,y,itemwidth,itemheight,bkcolor);	//填充背景色
	gui_show_ptstr(x+5,y+(itemheight-16)/2,x+itemwidth-10-APP_ITEM_ICO_SIZE-5,y+itemheight,0,color,16,name,1);	//显示条目名字
	if(icopath)minibmp_decode(icopath,x+itemwidth-10-APP_ITEM_ICO_SIZE,y+(itemheight-APP_ITEM_ICO_SIZE)/2,APP_ITEM_ICO_SIZE,APP_ITEM_ICO_SIZE,0,0);			//解码APP_ITEM_ICO_SIZE*APP_ITEM_ICO_SIZE的bmp图片
}		 

//将磁盘路径按要求修改
//des:目标缓存区
//diskx:新的磁盘编号
//src:原路径
void app_system_file_chgdisk(char *des,char *diskx,char *src)
{ 
	//static u8 t=0;
	strcpy((char*)des,(const char*)src);
	des[0]=diskx[0]; 
	//printf("path[%d]:%s\r\n",t++,des);	//打印文件名
}
//系统总共需要的图标/图片/系统文件有141个,其中SPB部分占20个.其余121个为APP各功能占用
//这还不包括SYSTEM/FONT文件夹内的字体文件.
//141个文件总大小:1,033,804 字节(0.98MB)
//3个字体文件总大小:1,514,984 字节(1.44MB)
//检测所有的系统文件是否正常
//diskx:磁盘路径."0",SD卡;"1",SPI FLASH
//返回值:0,正常
//    其他,错误代码

u8 app_system_file_check(char* diskx)
{
	u8 i;
	u8 rval=0;
	u8 res=0;
	u32 enPictureNum;
	u32 chPictureNum;
	FIL *f_check;
	char *path;
	f_check=(FIL *)gui_memin_malloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	path=gui_memin_malloc(200);						//为path申请内存 
	if(!f_check||!path)rval=0XFF;//申请失败
	while(rval==0)
	{	
	
		enPictureNum = COUNT_ARRY_LENGTH(gui_english_icon_path_tbl);//获得数组长度
		
		//检测GUI所有图标
 		for(i=0;i<enPictureNum;i++)
		{
			
			app_system_file_chgdisk(path,diskx,gui_english_icon_path_tbl[i]); 
			//DBG("*****file:%s\r\n",path);
	 		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
			if(res)break;		//打开失败   
			f_close(f_check);    //关闭文件  
		}
		rval+=i;
		if(i<enPictureNum){rval++;break;}   
		
		
		chPictureNum = COUNT_ARRY_LENGTH(gui_chinese_icon_path_tbl);//获得数组长度
		//检测GUI所有图标
 		for(i=0;i<chPictureNum;i++)
		{
			
			app_system_file_chgdisk(path,diskx,gui_chinese_icon_path_tbl[i]); 
			//DBG("*****file:%s\r\n",path);
	 		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
			if(res)break;		//打开失败   
			f_close(f_check);    //关闭文件  
		}
		rval+=i;
		if(i<chPictureNum){rval++;break;}   
		
	
/*****************************检查字体文件*****************************************/	
#if 0
		app_system_file_chgdisk(path,diskx,APP_ASCII_S6030); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
		if(res)break;			//打开失败      
    	f_close(f_check);		//关闭文件     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_5427); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
		if(res)break;			//打开失败      
    	f_close(f_check);		//关闭文件     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_3618); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
		if(res)break;			//打开失败      
    	f_close(f_check);		//关闭文件     
		rval++;	      
		
		app_system_file_chgdisk(path,diskx,APP_ASCII_2814); 
		res=f_open(f_check,(const TCHAR*)path,FA_READ);//只读方式打开文件
		if(res)break;			//打开失败      
    	f_close(f_check);		//关闭文件     
		rval++;	      
#endif
		
		DBG("\r\ntotal system files:%d\r\n",rval);
		rval=0;
		break;    
	}
	gui_memin_free(f_check);//释放内存 
	gui_memin_free(path);	//释放内存
	return rval; 
}

////////////////////////////////////////////////////////////////////////////////////////////
//一下代码仅用于实现SYSTEM文件夹更新
char *const  APP_SYSTEM_COMMONPATH="0:/SYSTEM/COMMON";		//APP文件夹路径
char *const  APP_SYSTEM_FONTPATH="0:/SYSTEM/FONT";		//FONT文件夹路径
char *const  GUI_PICTURE_DSTPATH="0:/SYSTEM/PICTURE";			//图片文件目录

char *const  APP_SYSTEM_DSTPATH="1:/SYSTEM";			//系统文件目标路径

//文件复制信息提示坐标 
//文件复制信息提示坐标和字体
static u16 cpdmsg_x;
static u16 cpdmsg_y;
static u8  cpdmsg_fsize;	


//系统启动的时候,用于显示更新进度
//*pname:更新文件名字
//pct:百分比
//mode:模式
//[0]:更新文件名
//[1]:更新百分比pct
//[2]:更新文件夹
//[3~7]:保留
//返回值:0,正常;
//       1,结束复制
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode)
{	
	u8 offset = 10;												
	if(mode&0X01)
	{
		gui_fill_rectangle_DMA(cpdmsg_x,cpdmsg_y,lcddev.width,cpdmsg_fsize,BLACK);	//填充底色
		LCD_ShowString(cpdmsg_x,cpdmsg_y,30*(cpdmsg_fsize/2),cpdmsg_fsize,cpdmsg_fsize,pname);	//显示文件名,最长16个字符宽度
		DBG("\r\nCopy File:%s\r\n",pname);  
	}
	if(mode&0X04)DBG("Copy Folder:%s\r\n",pname);
	if(mode&0X02)//更新百分比
	{
		
		LCD_Show_Bar(cpdmsg_x + offset,cpdmsg_y + cpdmsg_fsize + offset,cpdmsg_fsize,25*(cpdmsg_fsize/2),pct,100,GREEN,BLACK);
		LCD_ShowNum(cpdmsg_x+(25 + 2)*(cpdmsg_fsize/2),cpdmsg_y+ cpdmsg_fsize + offset,pct,3,cpdmsg_fsize);//显示数值
		LCD_ShowString(cpdmsg_x+(25+ 2 + 3)*(cpdmsg_fsize/2),cpdmsg_y+ cpdmsg_fsize + offset,lcddev.width,cpdmsg_fsize,cpdmsg_fsize,(u8*)"%");
		DBG("File Copyed:%d\r\n",pct);
	}
	return 0;	
}
//设置app_boot_cpdmsg的显示坐标
//x,y:坐标.
//fisze:字体大小
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize)
{
	cpdmsg_x=x;
	cpdmsg_y=y;
	cpdmsg_fsize=fsize;
}
//系统更新
//返回值:0,正常
//    其他,错误,0XFF强制退出了
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode))
{
	u32 totsize=0;
	u32 cpdsize=0;
	u8 res=0;
//	totsize+=exf_fdsize((u8*)APP_SYSTEM_COMMONPATH);//得到三个文件夹的总大小
//	totsize+=exf_fdsize((u8*)APP_SYSTEM_FONTPATH);
	totsize+=exf_fdsize((u8*)GUI_PICTURE_DSTPATH);

	DBG("totsize:%d\r\n",totsize);
	f_mkdir((const TCHAR *)APP_SYSTEM_DSTPATH);//强制创建目标文件夹
	
#if 0	
 	res=exf_fdcopy(fcpymsg,(u8*)APP_SYSTEM_COMMONPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//第一阶段复制出错
		else return 0XFF;		//强制退出
	}
	
	res=exf_fdcopy(fcpymsg,(u8*)APP_SYSTEM_FONTPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//第二阶段复制出错
		else return 0XFF;		//强制退出
	}
#endif
	
	
 	res=exf_fdcopy(fcpymsg,(u8*)GUI_PICTURE_DSTPATH,(u8*)APP_SYSTEM_DSTPATH,&totsize,&cpdsize,1);
	if(res)
	{
		if(res!=0xff)return 1;	//第三阶段复制出错
		else return 0XFF;		//强制退出
	}
	return 0;
}
//得到STM32的序列号
//sn0,sn1,sn2:3个固有序列号
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2)
{
	*sn0=*(vu32*)(0x1FFFF7E8);
	*sn1=*(vu32*)(0x1FFFF7EC);
	*sn2=*(vu32*)(0x1FFFF7F0);    
}
//打印SN
void app_usmart_getsn(void)
{
	u32 sn0,sn1,sn2;
	app_getstm32_sn(&sn0,&sn1,&sn2);
	printf("\r\nSerial Number:%X%X%X\r\n",sn0,sn1,sn2);
}

//设置语言
//输入：id--1,ENGLISH;	id--0,CHINESE
 void set_language(u8 id)
 {
	if(id == ENGLISH) LANGUAGE = ENGLISH;
	else LANGUAGE = CHINESE;
	 
 }










