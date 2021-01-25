#include "fontupd.h"
#include "ff.h"	  
#include "w25qxx.h"   
#include "lcd.h"  
#include "string.h"
#include "malloc.h"
#include "bsp_timer.h"
#include "bsp_usart.h"
#include "guix.h"
//////////////////////////////////////////////////////////////////////////////////	 
//�ֿ�����ռ�õ�����������С(3���ֿ�+unigbk��+�ֿ���Ϣ=3238700�ֽ�,Լռ791��W25QXX����)
#define FONTSECSIZE	 	791
//�ֿ�����ʼ��ַ 
#define FONTINFOADDR 	1024*1024*12 					//WarShip STM32F103 V3�Ǵ�12M��ַ�Ժ�ʼ����ֿ�
														//ǰ��12M��fatfsռ����.
														//12M�Ժ����3���ֿ�+UNIGBK.BIN,�ܴ�С3.09M,���ֿ�ռ����,���ܶ�!
														//15.10M�Ժ�,�û���������ʹ��.����������100K�ֽڱȽϺ�.
														
//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

//�ֿ����ڴ����е�·��
u8*const GBK24_PATH="/SYSTEM/FONT/GBK24.FON";		//GBK24�Ĵ��λ��
u8*const GBK16_PATH="/SYSTEM/FONT/GBK16.FON";		//GBK16�Ĵ��λ��
u8*const GBK12_PATH="/SYSTEM/FONT/GBK12.FON";		//GBK12�Ĵ��λ��
u8*const UNIGBK_PATH="/SYSTEM/FONT/UNIGBK.BIN";		//UNIGBK.BIN�Ĵ��λ��




//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size);//��ʾ��ֵ
	}
	return 0;					    
} 
//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
//����ֵ:0,�ɹ�;����,ʧ��.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u32 prabDate;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMIN,4096);				//����4096���ֽڿռ�
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//���ļ�ʧ��  
 	if(rval==0)	 
	{
		POINT_COLOR = WHITE;
		switch(fx)
		{
			case 0:												//����UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize=fftemp->fsize;					//UNIGBK��С
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK֮�󣬽���GBK12�ֿ�
				ftinfo.gbk12size=fftemp->fsize;					//GBK12�ֿ��С
				flashaddr=ftinfo.f12addr;						//GBK12����ʼ��ַ
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12֮�󣬽���GBK16�ֿ�
				ftinfo.gbk16size=fftemp->fsize;					//GBK16�ֿ��С
				flashaddr=ftinfo.f16addr;						//GBK16����ʼ��ַ
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gkb24size=fftemp->fsize;					//GBK24�ֿ��С
				flashaddr=ftinfo.f24addr;						//GBK24����ʼ��ַ
				break;
		} 
			
		while(res==FR_OK)//��ѭ��ִ��
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//��ȡ����	 
			if(res!=FR_OK)break;								//ִ�д���
			W25QXX_Write(tempbuf,offx+flashaddr,4096);		//��0��ʼд��4096������  
	  		offx+=bread;	  
		//	fupd_prog(x,y,size,fftemp->fsize,offx);	 			//������ʾ
			LCD_Show_Bar(x,y,size,120,offx,fftemp->fsize,BLUE,BLACK);
			LCD_ShowNum(x + 120 + 2, y,((float)offx)/fftemp->fsize*100,3,size);//%
			LCD_ShowString(x + 120 + 2 + 3*size/2,y,100,size,size,(u8 *)"%");
			prabDate = ((float)offx)/fftemp->fsize*100;
			DBG("->update font prab:%d%%\r\n",prabDate);
			if(bread!=4096)break;								//������.
	 	} 	
		f_close(fftemp);	
	}			 
	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
	myfree(SRAMIN,tempbuf);	//�ͷ��ڴ�
	return res;
} 
//ȷ�ϴ�����Դ
//src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
u8 check_disk_num(const u8* src)
{
	u8 ret = 0xff;

		if( *src == '0')
		{
			ret = 0;
		}
		else if(*src == '1') ret = 1;
		else if(*src == '2') ret = 2;
return ret;
	
}

//���������ļ�,UNIGBK,GBK12,GBK16,GBK24һ�����
//x,y:��ʾ��Ϣ����ʾ��ַ
//size:�����С
//src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
//��ʾ��Ϣ�����С										  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	  
u8 update_font(u16 x,u16 y,u8 size,u8* src)
{	
	u8 *pname;
	u32 *buf;
	u8 res=0;	
	u8 disk_num;
 	u16 i,j;
	u16 width,height;
	u32 prabDate;
	FIL *fftemp;
	u8 rval=0; 
	res=0XFF;		
	ftinfo.fontok=0XFF;
	pname=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�  
	buf=mymalloc(SRAMIN,4096);	//����4K�ֽ��ڴ�  
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return 5;	//�ڴ�����ʧ��
	}
	disk_num = check_disk_num(src);
	//�Ȳ����ļ��Ƿ����� 
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)UNIGBK_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<4;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK12_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<5;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK16_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<6;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK24_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<7;//���ļ�ʧ��   
	myfree(SRAMIN,fftemp);//�ͷ��ڴ�
	width = lcddev.width;
	height = size;
	if(rval==0)//�ֿ��ļ�������.
	{  
		POINT_COLOR = WHITE;
		LCD_ShowString(x,y,width,height,size,(u8 *)"Erasing sectors... ");//��ʾ���ڲ�������	
		POINT_COLOR = BLUE;	
		DBG("->Erasing sectors...\r\n");
		for(i=0;i<FONTSECSIZE;i++)	//�Ȳ����ֿ�����,���д���ٶ�
		{
			//fupd_prog(x+20*size/2,y,size,FONTSECSIZE,i);//������ʾ
			LCD_Show_Bar(x+20*size/2,y,size,120,i+1,FONTSECSIZE,BLUE,BLACK);
			LCD_ShowNum(x+20*size/2 + 120 + 2, y,((float)i+1)/FONTSECSIZE*100,3,size);//%
			LCD_ShowString(x+20*size/2 + 120 + 2 + 3*size/2,y,100,size,size,(u8 *)"%");
			prabDate = ((float)i+1)/FONTSECSIZE*100;
			DBG("->prab : %d%%\r\n",prabDate);
			W25QXX_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//������������������
			for(j=0;j<1024;j++)//У������
			{
				if(buf[j]!=0XFFFFFFFF)break;//��Ҫ����  	  
			}
			if(j!=1024)W25QXX_Erase_Sector((FONTINFOADDR/4096)+i);	//��Ҫ����������
		}

		myfree(SRAMIN,buf);
		POINT_COLOR = WHITE;
		LCD_ShowString(x,y,width,height,size,(u8 *)"Updating UNIGBK.BIN");	
		DBG("->Updating UNIGBK.BIN\r\n");	
	
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,0);	//����UNIGBK.BIN
		if(res){myfree(SRAMIN,pname);return 1;}
	
		LCD_ShowString(x,y,width,height,size,(u8 *)"Updating GBK12.BIN  ");
		DBG("->Updating GBK12.BIN\r\n");	
	
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK12_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,1);	//����GBK12.FON
		if(res){myfree(SRAMIN,pname);return 2;}
	
		LCD_ShowString(x,y,width,height,size,(u8 *)"Updating GBK16.BIN  ");
		DBG("->Updating GBK16.BIN\r\n");
		
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK16_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,2);	//����GBK16.FON
		if(res){myfree(SRAMIN,pname);return 3;}
		
		LCD_ShowString(x,y,width,height,size,(u8 *)"Updating GBK24.BIN  ");
		DBG("->Updating GBK24.BIN\r\n");
		
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK24_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,3);	//����GBK24.FON
		if(res){myfree(SRAMIN,pname);return 4;}
		//ȫ�����º���
		ftinfo.fontok=0XAA;
		W25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//�����ֿ���Ϣ
		POINT_COLOR = BLUE;
	}
	else
	{
	 POINT_COLOR = RED;

		switch (disk_num)
		{
			case 0:	LCD_ShowString(x,y,width, height,size,(u8 *)"FONT files are lost,please ");
				LCD_ShowString(x,y + size,width, height,size,(u8 *)"check the SD card!");
				break;
			case 1:LCD_ShowString(x,y,width, height,size,(u8 *)"FONT files lost,please");
			LCD_ShowString(x,y + size,width, height,size,(u8 *)"check the Extern FLASH!");
				break;
			case 2:LCD_ShowString(x,y,width, height,size,(u8 *)"FONT files lost,please ");
			LCD_ShowString(x,y + size,width, height,size,(u8 *)"check the U disk!");
				break;				
			default:
				break;
		}
	bsp_DelayMS(2000);
	gui_fill_rectangle_DMA(x,y ,width, height,BACK_COLOR);
	gui_fill_rectangle_DMA(x,y + size,width, height,BACK_COLOR);
	LCD_ShowString(x,y,width, height,size,(u8 *)"Plug in a SD card again!");
	bsp_DelayMS(2000);
	gui_fill_rectangle_DMA(x,y ,width, height,BACK_COLOR);
	bsp_DelayMS(1500);
	POINT_COLOR = BLUE;	
	DBG("<@>font is lost\r\n");
	}
	 
	myfree(SRAMIN,pname);//�ͷ��ڴ� 
	myfree(SRAMIN,buf);
	return rval;//�޴���.			 
} 
//��ʼ������
//����ֵ:0,�ֿ����.
//		 ����,�ֿⶪʧ
u8 font_init(void)
{		
	u8 t=0;
	W25QXX_Init();  
	while(t<10)//������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ���
	{
		t++;
		W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//����ftinfo�ṹ������
		if(ftinfo.fontok==0XAA)break;
		bsp_DelayMS(20);
	}
	if(ftinfo.fontok!=0XAA)return 1;
	return 0;		    
}




























