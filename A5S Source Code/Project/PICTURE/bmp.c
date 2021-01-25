#include "piclib.h"
#include "bmp.h"
#include "string.h"
#include "dma.h"
//ͼƬ���� ��������-bmp���벿��	   

//����˵�� 
//V1.1 20140722
//�޸�minibmp_decode����,ʹͼƬ���趨�������������ʾ
//////////////////////////////////////////////////////////////////////////////////

//��ʹ���ڴ����
#if BMP_USE_MALLOC == 0	
FIL f_bfile;
u8 bmpreadbuf[BMP_DBUF_SIZE];
#endif 				    

//��׼��bmp����,����filename���BMP�ļ�	
//�ٶȱȽ���.��Ҫ	
//filename:����·�����ļ���	       	  			  
//����ֵ:0,�ɹ�;
//		 ����,������.
u8 stdbmp_decode(const u8 *filename) 
{
	FIL* f_bmp;
    u16 br;

    u16 count;		    	   
	u8  rgb ,color_byte;
	u16 x ,y,color;	  
	u16 countpix=0;//��¼���� 	 

	//x,y��ʵ������	
	u16  realx=0;
	u16 realy=0;
	u8  yok=1;  				   
	u8 res;


	u8 *databuf;    		//���ݶ�ȡ��ŵ�ַ
 	u16 readlen=BMP_DBUF_SIZE;//һ�δ�SD����ȡ���ֽ�������

	u8 *bmpbuf;			  	//���ݽ����ַ
	u8 biCompression=0;		//��¼ѹ����ʽ
	
	u16 rowlen;	  		 	//ˮƽ�����ֽ���  
	BITMAPINFO *pbmp;		//��ʱָ��
	
#if BMP_USE_MALLOC == 1	//ʹ��malloc	
	databuf=(u8*)pic_memalloc(readlen);		//����readlen�ֽڵ��ڴ�����
	if(databuf==NULL)return PIC_MEM_ERR;	//�ڴ�����ʧ��.
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_bmp==NULL)							//�ڴ�����ʧ��.
	{		 
		pic_memfree(databuf);
		return PIC_MEM_ERR;				
	} 	 
#else				 	//��ʹ��malloc
	databuf=bmpreadbuf;
	f_bmp=&f_bfile;
#endif
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//���ļ�	 						  
	if(res==0)//�򿪳ɹ�.
	{ 
		f_read(f_bmp,databuf,readlen,(UINT*)&br);	//����readlen���ֽ�  
		pbmp=(BITMAPINFO*)databuf;					//�õ�BMP��ͷ����Ϣ   
		count=pbmp->bmfHeader.bfOffBits;        	//����ƫ��,�õ����ݶεĿ�ʼ��ַ
		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//ѹ����ʽ
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//�õ�ͼƬ��� 
		ai_draw_init();//��ʼ�����ܻ�ͼ			
		//ˮƽ���ر�����4�ı���!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		//��ʼ����BMP   
		color=0;//��ɫ���	 													 
		x=0 ;
		y=picinfo.ImgHeight;
		rgb=0;      
		//���ڳߴ�С�ڵ����趨�ߴ��ͼƬ,���п��ٽ���
		realy=(y*picinfo.Div_Fac)>>13;
		bmpbuf=databuf;
		while(1)
		{				 
			while(count<readlen)  //��ȡһ��1024���� (SectorsPerClust ÿ��������)
		    {
				if(color_byte==3)   //24λ��ɫͼ
				{
					switch (rgb) 
					{
						case 0:				  
							color=bmpbuf[count]>>3; //B
							break ;	   
						case 1: 	 
							color+=((u16)bmpbuf[count]<<3)&0X07E0;//G
							break;	  
						case 2 : 
							color+=((u16)bmpbuf[count]<<8)&0XF800;//R	  
							break ;			
					}   
				}else if(color_byte==2)  //16λ��ɫͼ
				{
					switch(rgb)
					{
						case 0 : 
							if(biCompression==BI_RGB)//RGB:5,5,5
							{
								color=((u16)bmpbuf[count]&0X1F);	 	//R
								color+=(((u16)bmpbuf[count])&0XE0)<<1; //G
							}else		//RGB:5,6,5
							{
								color=bmpbuf[count];  			//G,B
							}  
							break ;   
						case 1 : 			  			 
							if(biCompression==BI_RGB)//RGB:5,5,5
							{
								color+=(u16)bmpbuf[count]<<9;  //R,G
							}else  		//RGB:5,6,5
							{
								color+=(u16)bmpbuf[count]<<8;	//R,G
							}  									 
							break ;	 
					}		     
				}else if(color_byte==4)//32λ��ɫͼ
				{
					switch (rgb)
					{
						case 0:				  
							color=bmpbuf[count]>>3; //B
							break ;	   
						case 1: 	 
							color+=((u16)bmpbuf[count]<<3)&0X07E0;//G
							break;	  
						case 2 : 
							color+=((u16)bmpbuf[count]<<8)&0XF800;//R	  
							break ;			
						case 3 :
							//alphabend=bmpbuf[count];//����ȡ  ALPHAͨ��
							break ;  		  	 
					}	
				}else if(color_byte==1)//8λɫ,��ʱ��֧��,��Ҫ�õ���ɫ��.
				{
				} 
				rgb++;	  
				count++ ;		  
				if(rgb==color_byte) //ˮƽ�����ȡ��1���������ݺ���ʾ
				{	
					if(x<picinfo.ImgWidth)	 					 			   
					{	
						realx=(x*picinfo.Div_Fac)>>13;//x��ʵ��ֵ
						if(is_element_ok(realx,realy,1)&&yok)//��������
						{						 				 	  	       
							pic_phy.draw_point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);//��ʾͼƬ	 
						}   									    
					}
					x++;//x������һ������ 
					color=0x00; 
					rgb=0;  		  
				}
				countpix++;//�����ۼ�
				if(countpix>=rowlen)//ˮƽ��������ֵ����.����
				{		 
					y--; 
					if(y==0)break;			 
					realy=(y*picinfo.Div_Fac)>>13;//ʵ��yֵ�ı�	 
					if(is_element_ok(realx,realy,0))yok=1;//�˴����ı�picinfo.staticx,y��ֵ	 
					else yok=0; 
					x=0; 
					countpix=0;
					color=0x00;
					rgb=0;
				}	 
			} 		
			res=f_read(f_bmp,databuf,readlen,(UINT *)&br);//����readlen���ֽ�
			if(br!=readlen)readlen=br;	//���һ������		  
			if(res||br==0)break;		//��ȡ����
			bmpbuf=databuf;
	 	 	count=0;
		}  
		f_close(f_bmp);//�ر��ļ�
	}  	
#if BMP_USE_MALLOC == 1	//ʹ��malloc	
	pic_memfree(databuf);	 
	pic_memfree(f_bmp);		 
#endif	
	return res;		//BMP��ʾ����.    					   
}		 
//С�ߴ��bmp����,����filename���BMP�ļ�		
//filename:����·�����ļ���
//x,y,width,height:��ʾ�����С(��������������ʾ)
//acolor:���ӵ�alphablend����ɫ(�������32λɫbmp��Ч!!!)
//mode:ģʽ(����bit5,�����ľ�ֻ��32λɫbmp��Ч!!!)
//     bit[7:6]:0,��ʹ��ͼƬ����͵�ɫalphablend;
//              1,��ͼƬ��acolor����alphablend,���Ҳ����ø��ӵ�͸����;
//              2,��ɫ,acolor,ͼƬ,һ�����alphablend;
//	   bit5:����
//     bit4~0:0~31,ʹ�ø���alphablend��͸���̶� 	      	  			  
//����ֵ:0,�ɹ�;
//    ����,������.
u8 minibmp_decode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u16 acolor,u8 mode)//�ߴ�С��240*320��bmpͼƬ����.
{
	FIL* f_bmp;
    u16 br;
	u8  color_byte;
	u16 tx,ty,color;	 
	//tx,ty��ʵ������	
	u8 res;
	u16 i,j;
	u8 *databuf;    		//���ݶ�ȡ��                                                                       �ŵ�ַ
 	u16 readlen=BMP_DBUF_SIZE;//һ�δ�SD����ȡ���ֽ�������,����С��LCD���*3!!!

	u8 *bmpbuf;			  	//���ݽ����ַ
	u8 biCompression=0;		//��¼ѹ����ʽ
	
	u16 rowcnt;				//һ�ζ�ȡ������
	u16 rowlen;	  		 	//ˮƽ�����ֽ���  
	u16 rowpix=0;			//ˮƽ����������	  
	u8 rowadd;				//ÿ������ֽ���

	u16 tmp_color;

	u8 alphabend=0xff;		//����͸��ɫΪ0����ȫ��͸��
	u8 alphamode=mode>>6;	//�õ�ģʽֵ,0/1/2
	BITMAPINFO *pbmp;   	//��ʱָ��		 
	//�õ�����ߴ�
	picinfo.S_Height=height;
	picinfo.S_Width=width;
		
#if BMP_USE_MALLOC == 1	//ʹ��malloc	
	databuf=(u8*)pic_memalloc(readlen);		//����readlen�ֽڵ��ڴ�����
	if(databuf==NULL)return PIC_MEM_ERR;		//�ڴ�����ʧ��.
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_bmp==NULL)								//�ڴ�����ʧ��.
	{		 
		pic_memfree(databuf);
		return PIC_MEM_ERR;				
	} 	 
#else
	databuf=bmpreadbuf;
	f_bmp=&f_bfile;
#endif
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//���ļ�	 						  
	if(res==0)//�򿪳ɹ�.
	{ 
		f_read(f_bmp,databuf,sizeof(BITMAPINFO),(UINT*)&br);//����BITMAPINFO��Ϣ 
		pbmp=(BITMAPINFO*)databuf;					//�õ�BMP��ͷ����Ϣ   
		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//ѹ����ʽ
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//�õ�ͼƬ��� 
		//ˮƽ���ر�����4�ı���!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		rowadd=rowlen-picinfo.ImgWidth*color_byte;	//ÿ������ֽ���
 		//��ʼ����BMP   
		color=0;//��ɫ���	 													 
		tx=0 ;
		ty=picinfo.ImgHeight-1;
		if(picinfo.ImgWidth<=picinfo.S_Width&&picinfo.ImgHeight<=picinfo.S_Height)
		{  				
			x+=(picinfo.S_Width-picinfo.ImgWidth)/2;	//ƫ�Ƶ�������
			y+=(picinfo.S_Height-picinfo.ImgHeight)/2;	//ƫ�Ƶ�������
			rowcnt=readlen/rowlen;						//һ�ζ�ȡ������
			readlen=rowcnt*rowlen;						//һ�ζ�ȡ���ֽ���
			rowpix=picinfo.ImgWidth;					//ˮƽ���������ǿ�� 
			f_lseek(f_bmp,pbmp->bmfHeader.bfOffBits);	//ƫ�Ƶ�������ʼλ�� 	  
			while(1)
			{	     
				res=f_read(f_bmp,databuf,readlen,(UINT *)&br);	//����readlen���ֽ�
				bmpbuf=databuf;									//�����׵�ַ  
				if(br!=readlen)rowcnt=br/rowlen;				//���ʣ�µ�����
				if(color_byte==3)  			//24λBMPͼƬ
				{
					for(j=0;j<rowcnt;j++)	//ÿ�ζ���������
					{
						for(i=0;i<rowpix;i++)//дһ������
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
 						 	pic_phy.draw_point(x+tx,y+ty,color);//��ʾͼƬ	
							tx++;
						}
						bmpbuf+=rowadd;//���������
						tx=0;
						ty--;
					}
				}else if(color_byte==2)//16λBMPͼƬ
				{
					for(j=0;j<rowcnt;j++)//ÿ�ζ���������
					{
						if(biCompression==BI_RGB)//RGB:5,5,5
						{
							for(i=0;i<rowpix;i++)
							{
								color=((u16)*bmpbuf&0X1F);			//R
								color+=(((u16)*bmpbuf++)&0XE0)<<1; 	//G
		 						color+=((u16)*bmpbuf++)<<9;  	    //R,G	 
							    pic_phy.draw_point(x+tx,y+ty,color);//��ʾͼƬ	
								tx++;
							}
						}else  //RGB 565
						{
							for(i=0;i<rowpix;i++)
							{											 
								color=*bmpbuf++;  			//G,B
		 						color+=((u16)*bmpbuf++)<<8;	//R,G	 
							  	pic_phy.draw_point(x+tx,y+ty,color);//��ʾͼƬ	
								tx++;
							}
						}
						bmpbuf+=rowadd;//���������
						tx=0;
						ty--;
					}	
				}else if(color_byte==4)		//32λBMPͼƬ
				{
					for(j=0;j<rowcnt;j++)	//ÿ�ζ���������
					{
						for(i=0;i<rowpix;i++)
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
							alphabend=*bmpbuf++;					//ALPHAͨ��
							if(alphamode!=1) //��Ҫ��ȡ��ɫ
							{
								tmp_color=pic_phy.read_point(x+tx,y+ty);//��ȡ��ɫ		   
							    if(alphamode==2)//��Ҫ���ӵ�alphablend
								{
									tmp_color=piclib_alpha_blend(tmp_color,acolor,mode&0X1F);	//��ָ����ɫ����blend		 
								}
								color=piclib_alpha_blend(tmp_color,color,alphabend/8); 			//�͵�ɫ����alphablend
							}else tmp_color=piclib_alpha_blend(acolor,color,alphabend/8);		//��ָ����ɫ����blend
 							pic_phy.draw_point(x+tx,y+ty,color);//��ʾͼƬ				   
							tx++;//x������һ������ 	  
						}
						bmpbuf+=rowadd;//���������
						tx=0;
						ty--;
					}

				}
				if(br!=readlen||res)break;	 
			}	 
		}	
		f_close(f_bmp);//�ر��ļ�      
	}else res=PIC_SIZE_ERR;//ͼƬ�ߴ����	
#if BMP_USE_MALLOC == 1	//ʹ��malloc	
	pic_memfree(databuf);	 
	pic_memfree(f_bmp);		 
#endif	
	return res;
}            




void u8_2_u16(u16 *pDesData, u8 *pSrcData,u32 size)
{
	u32 i;
	u8 h8Data;
	u8 l8Data;
	
	for(i = 0; i < size /2; i++)
	{
		h8Data = pSrcData[2*i];
		l8Data = pSrcData[2*i + 1];
		pDesData[i] = h8Data <<8 | l8Data;
//		DBG("0x%x,",pDesData[i]);	
//		if((i+1) % 8 == 0) DBG("\r\n");

	}
	
	
}



#define BMP_X_SIZE	320
#define BMP_Y_SIZE	240

#if 0
void display_bin2LCD(void)
{
	u8 *readBuff;//��SD�����ݻ���
	FIL* f_bmp;
	u8 res;
	u16 REDA_SIZE = BMP_X_SIZE * 2;
	u32 br;
	u32 j;
	readBuff = (u8 *)pic_memalloc(REDA_SIZE);
	if(readBuff == NULL) {DBG("#1 malloc memory fail\r\n"); return;}
	
	lcdSramBuffer = (u16 *)pic_memalloc(REDA_SIZE);
	if(lcdSramBuffer == NULL) {DBG("#11 malloc memory fail\r\n"); return;}
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_bmp==NULL)								//�ڴ�����ʧ��.
	{	
	DBG("#2 malloc memory fail\r\n");	
		pic_memfree(readBuff);
		pic_memfree(lcdSramBuffer);
		return ;				
	} 	
	
	res=f_open(f_bmp,(const TCHAR*)logo_icos_path_tbl[LANGUAGE],FA_READ|FA_OPEN_EXISTING);//
	
	if(res !=FR_OK )	
	{
		DBG("#3 open logo2.bin failed\r\n");	
		DBG("#4 free malloc\r\n");	
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);
		return ;	
	}
	j = 0;
while(1)
{
		j++;
	//	DBG("---------------offset %d\r\n",j);
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//����һ�����ص�����
		u8_2_u16(lcdSramBuffer,readBuff,br);
		dma_lcd_show(0,j-1,BMP_X_SIZE);		
		if(br!=REDA_SIZE||res){DBG("read over,%d\r\n",res);break;	 }
}
f_close(f_bmp);
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);		
	//	dma_lcd_show(j,0,sizeof(lcdSramBuffer));		

	
		
}
#endif

void display_big_pic_bin2LCD(u8 *fileName)
{
	u8 *readBuff;//��SD�����ݻ���
	FIL* f_bmp;
	u8 res;
	u16 REDA_SIZE = BMP_X_SIZE * 2;
	u32 br;
	u32 j;
	readBuff = (u8 *)pic_memalloc(REDA_SIZE);
	if(readBuff == NULL) {DBG("#1 malloc memory fail\r\n"); return;}
	
	lcdSramBuffer = (u16 *)pic_memalloc(REDA_SIZE);
	if(lcdSramBuffer == NULL) {DBG("#11 malloc memory fail\r\n"); return;}
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_bmp==NULL)								//�ڴ�����ʧ��.
	{	
	DBG("#2 malloc memory fail\r\n");	
		pic_memfree(readBuff);
		pic_memfree(lcdSramBuffer);
		return ;				
	} 	
	
	res=f_open(f_bmp,(const TCHAR*)fileName,FA_READ|FA_OPEN_EXISTING);//
	
	if(res !=FR_OK )	
	{
		DBG("#3 open logo2.bin failed\r\n");	
		DBG("#4 free malloc\r\n");	
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);
		return ;	
	}
	j = 0;
while(1)
{
		j++;
	//	DBG("---------------offset %d\r\n",j);
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//����һ�����ص�����
		u8_2_u16(lcdSramBuffer,readBuff,br);
		dma_lcd_show(0,j-1,BMP_X_SIZE);		
		if(br!=REDA_SIZE||res){DBG("read over,%d\r\n",res);break;	 }
}
f_close(f_bmp);
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);		
	//	dma_lcd_show(j,0,sizeof(lcdSramBuffer));		

	
		
}
#define BMP_XX_SIZE	78
#define BMP_YY_SIZE	104
/***************
��Image2LCD v2.9 �����bmp������ͼ��ת����bin�ļ�
���ã�1.�����������--������bin�ļ�
2.ɨ��ģʽ--ˮƽɨ��
3.����Ҷ�--16λ���ɫ
4.����Ⱥ͸߶�--��ͼƬ��ʵ��ȸ߶����
5.��λ��ǰ

������ѡ��Ĭ��
********/
//������ʾbin��ʽ��bmp�ļ�
void display_bmp_bin_to_LCD(u8 *fileName,u16 x, u16 y, u16 width,u16 heigth,u16 color)
{
	u8 *readBuff;//��SD�����ݻ���
	FIL* f_bmp;
	u8 res;
	u16 REDA_SIZE = width * 2;
	u32 br;
	u16 j = 0;
	
	u16 xOffSet = x;
	u16 yOffSet = y;
	

	readBuff = (u8 *)pic_memalloc(REDA_SIZE);
	if(readBuff == NULL) {DBG("#1 malloc memory fail\r\n"); return;}
	
	lcdSramBuffer = (u16 *)pic_memalloc(REDA_SIZE);
	if(lcdSramBuffer == NULL) {DBG("2 malloc memory fail\r\n"); return;}
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_bmp==NULL)								//�ڴ�����ʧ��.
	{	
	DBG("#3 malloc memory fail\r\n");	
		pic_memfree(readBuff);
		pic_memfree(lcdSramBuffer);
		return ;				
	} 	
	
	res=f_open(f_bmp,(const TCHAR*)fileName,FA_READ|FA_OPEN_EXISTING);//
	
	if(res !=FR_OK )	
	{
		DBG("#4 open %s failed\r\n",fileName);	
		DBG("#5 free malloc\r\n");	
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);
		return ;	
	}
//	gui_fill_rectangle_DMA(xOffSet,yOffSet,width,heigth,color);//����ͼƬ��ʾ����ɫ


while(1)
{
		
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//����һ�����ص�����
		u8_2_u16(lcdSramBuffer,readBuff,br);
		dma_lcd_show(xOffSet,yOffSet + j++,width);		
		if(br!=REDA_SIZE||res){break;}
}
f_close(f_bmp);
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);		
	
}













