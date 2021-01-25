#include "piclib.h"
#include "bmp.h"
#include "string.h"
#include "dma.h"
//图片解码 驱动代码-bmp解码部分	   

//升级说明 
//V1.1 20140722
//修改minibmp_decode函数,使图片在设定区域的正中央显示
//////////////////////////////////////////////////////////////////////////////////

//不使用内存分配
#if BMP_USE_MALLOC == 0	
FIL f_bfile;
u8 bmpreadbuf[BMP_DBUF_SIZE];
#endif 				    

//标准的bmp解码,解码filename这个BMP文件	
//速度比较慢.主要	
//filename:包含路径的文件名	       	  			  
//返回值:0,成功;
//		 其他,错误码.
u8 stdbmp_decode(const u8 *filename) 
{
	FIL* f_bmp;
    u16 br;

    u16 count;		    	   
	u8  rgb ,color_byte;
	u16 x ,y,color;	  
	u16 countpix=0;//记录像素 	 

	//x,y的实际坐标	
	u16  realx=0;
	u16 realy=0;
	u8  yok=1;  				   
	u8 res;


	u8 *databuf;    		//数据读取存放地址
 	u16 readlen=BMP_DBUF_SIZE;//一次从SD卡读取的字节数长度

	u8 *bmpbuf;			  	//数据解码地址
	u8 biCompression=0;		//记录压缩方式
	
	u16 rowlen;	  		 	//水平方向字节数  
	BITMAPINFO *pbmp;		//临时指针
	
#if BMP_USE_MALLOC == 1	//使用malloc	
	databuf=(u8*)pic_memalloc(readlen);		//开辟readlen字节的内存区域
	if(databuf==NULL)return PIC_MEM_ERR;	//内存申请失败.
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_bmp==NULL)							//内存申请失败.
	{		 
		pic_memfree(databuf);
		return PIC_MEM_ERR;				
	} 	 
#else				 	//不使用malloc
	databuf=bmpreadbuf;
	f_bmp=&f_bfile;
#endif
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//打开文件	 						  
	if(res==0)//打开成功.
	{ 
		f_read(f_bmp,databuf,readlen,(UINT*)&br);	//读出readlen个字节  
		pbmp=(BITMAPINFO*)databuf;					//得到BMP的头部信息   
		count=pbmp->bmfHeader.bfOffBits;        	//数据偏移,得到数据段的开始地址
		color_byte=pbmp->bmiHeader.biBitCount/8;	//彩色位 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//压缩方式
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//得到图片高度
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//得到图片宽度 
		ai_draw_init();//初始化智能画图			
		//水平像素必须是4的倍数!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		//开始解码BMP   
		color=0;//颜色清空	 													 
		x=0 ;
		y=picinfo.ImgHeight;
		rgb=0;      
		//对于尺寸小于等于设定尺寸的图片,进行快速解码
		realy=(y*picinfo.Div_Fac)>>13;
		bmpbuf=databuf;
		while(1)
		{				 
			while(count<readlen)  //读取一簇1024扇区 (SectorsPerClust 每簇扇区数)
		    {
				if(color_byte==3)   //24位颜色图
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
				}else if(color_byte==2)  //16位颜色图
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
				}else if(color_byte==4)//32位颜色图
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
							//alphabend=bmpbuf[count];//不读取  ALPHA通道
							break ;  		  	 
					}	
				}else if(color_byte==1)//8位色,暂时不支持,需要用到颜色表.
				{
				} 
				rgb++;	  
				count++ ;		  
				if(rgb==color_byte) //水平方向读取到1像素数数据后显示
				{	
					if(x<picinfo.ImgWidth)	 					 			   
					{	
						realx=(x*picinfo.Div_Fac)>>13;//x轴实际值
						if(is_element_ok(realx,realy,1)&&yok)//符合条件
						{						 				 	  	       
							pic_phy.draw_point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);//显示图片	 
						}   									    
					}
					x++;//x轴增加一个像素 
					color=0x00; 
					rgb=0;  		  
				}
				countpix++;//像素累加
				if(countpix>=rowlen)//水平方向像素值到了.换行
				{		 
					y--; 
					if(y==0)break;			 
					realy=(y*picinfo.Div_Fac)>>13;//实际y值改变	 
					if(is_element_ok(realx,realy,0))yok=1;//此处不改变picinfo.staticx,y的值	 
					else yok=0; 
					x=0; 
					countpix=0;
					color=0x00;
					rgb=0;
				}	 
			} 		
			res=f_read(f_bmp,databuf,readlen,(UINT *)&br);//读出readlen个字节
			if(br!=readlen)readlen=br;	//最后一批数据		  
			if(res||br==0)break;		//读取出错
			bmpbuf=databuf;
	 	 	count=0;
		}  
		f_close(f_bmp);//关闭文件
	}  	
#if BMP_USE_MALLOC == 1	//使用malloc	
	pic_memfree(databuf);	 
	pic_memfree(f_bmp);		 
#endif	
	return res;		//BMP显示结束.    					   
}		 
//小尺寸的bmp解码,解码filename这个BMP文件		
//filename:包含路径的文件名
//x,y,width,height:显示区域大小(在区域正中央显示)
//acolor:附加的alphablend的颜色(这个仅对32位色bmp有效!!!)
//mode:模式(除了bit5,其他的均只对32位色bmp有效!!!)
//     bit[7:6]:0,仅使用图片本身和底色alphablend;
//              1,仅图片和acolor进行alphablend,并且不适用附加的透明度;
//              2,底色,acolor,图片,一起进行alphablend;
//	   bit5:保留
//     bit4~0:0~31,使用附加alphablend的透明程度 	      	  			  
//返回值:0,成功;
//    其他,错误码.
u8 minibmp_decode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u16 acolor,u8 mode)//尺寸小于240*320的bmp图片解码.
{
	FIL* f_bmp;
    u16 br;
	u8  color_byte;
	u16 tx,ty,color;	 
	//tx,ty的实际坐标	
	u8 res;
	u16 i,j;
	u8 *databuf;    		//数据读取存                                                                       放地址
 	u16 readlen=BMP_DBUF_SIZE;//一次从SD卡读取的字节数长度,不能小于LCD宽度*3!!!

	u8 *bmpbuf;			  	//数据解码地址
	u8 biCompression=0;		//记录压缩方式
	
	u16 rowcnt;				//一次读取的行数
	u16 rowlen;	  		 	//水平方向字节数  
	u16 rowpix=0;			//水平方向像素数	  
	u8 rowadd;				//每行填充字节数

	u16 tmp_color;

	u8 alphabend=0xff;		//代表透明色为0，完全不透明
	u8 alphamode=mode>>6;	//得到模式值,0/1/2
	BITMAPINFO *pbmp;   	//临时指针		 
	//得到窗体尺寸
	picinfo.S_Height=height;
	picinfo.S_Width=width;
		
#if BMP_USE_MALLOC == 1	//使用malloc	
	databuf=(u8*)pic_memalloc(readlen);		//开辟readlen字节的内存区域
	if(databuf==NULL)return PIC_MEM_ERR;		//内存申请失败.
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_bmp==NULL)								//内存申请失败.
	{		 
		pic_memfree(databuf);
		return PIC_MEM_ERR;				
	} 	 
#else
	databuf=bmpreadbuf;
	f_bmp=&f_bfile;
#endif
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//打开文件	 						  
	if(res==0)//打开成功.
	{ 
		f_read(f_bmp,databuf,sizeof(BITMAPINFO),(UINT*)&br);//读出BITMAPINFO信息 
		pbmp=(BITMAPINFO*)databuf;					//得到BMP的头部信息   
		color_byte=pbmp->bmiHeader.biBitCount/8;	//彩色位 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//压缩方式
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//得到图片高度
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//得到图片宽度 
		//水平像素必须是4的倍数!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		rowadd=rowlen-picinfo.ImgWidth*color_byte;	//每行填充字节数
 		//开始解码BMP   
		color=0;//颜色清空	 													 
		tx=0 ;
		ty=picinfo.ImgHeight-1;
		if(picinfo.ImgWidth<=picinfo.S_Width&&picinfo.ImgHeight<=picinfo.S_Height)
		{  				
			x+=(picinfo.S_Width-picinfo.ImgWidth)/2;	//偏移到正中央
			y+=(picinfo.S_Height-picinfo.ImgHeight)/2;	//偏移到正中央
			rowcnt=readlen/rowlen;						//一次读取的行数
			readlen=rowcnt*rowlen;						//一次读取的字节数
			rowpix=picinfo.ImgWidth;					//水平像素数就是宽度 
			f_lseek(f_bmp,pbmp->bmfHeader.bfOffBits);	//偏移到数据起始位置 	  
			while(1)
			{	     
				res=f_read(f_bmp,databuf,readlen,(UINT *)&br);	//读出readlen个字节
				bmpbuf=databuf;									//数据首地址  
				if(br!=readlen)rowcnt=br/rowlen;				//最后剩下的行数
				if(color_byte==3)  			//24位BMP图片
				{
					for(j=0;j<rowcnt;j++)	//每次读到的行数
					{
						for(i=0;i<rowpix;i++)//写一行像素
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
 						 	pic_phy.draw_point(x+tx,y+ty,color);//显示图片	
							tx++;
						}
						bmpbuf+=rowadd;//跳过填充区
						tx=0;
						ty--;
					}
				}else if(color_byte==2)//16位BMP图片
				{
					for(j=0;j<rowcnt;j++)//每次读到的行数
					{
						if(biCompression==BI_RGB)//RGB:5,5,5
						{
							for(i=0;i<rowpix;i++)
							{
								color=((u16)*bmpbuf&0X1F);			//R
								color+=(((u16)*bmpbuf++)&0XE0)<<1; 	//G
		 						color+=((u16)*bmpbuf++)<<9;  	    //R,G	 
							    pic_phy.draw_point(x+tx,y+ty,color);//显示图片	
								tx++;
							}
						}else  //RGB 565
						{
							for(i=0;i<rowpix;i++)
							{											 
								color=*bmpbuf++;  			//G,B
		 						color+=((u16)*bmpbuf++)<<8;	//R,G	 
							  	pic_phy.draw_point(x+tx,y+ty,color);//显示图片	
								tx++;
							}
						}
						bmpbuf+=rowadd;//跳过填充区
						tx=0;
						ty--;
					}	
				}else if(color_byte==4)		//32位BMP图片
				{
					for(j=0;j<rowcnt;j++)	//每次读到的行数
					{
						for(i=0;i<rowpix;i++)
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
							alphabend=*bmpbuf++;					//ALPHA通道
							if(alphamode!=1) //需要读取底色
							{
								tmp_color=pic_phy.read_point(x+tx,y+ty);//读取颜色		   
							    if(alphamode==2)//需要附加的alphablend
								{
									tmp_color=piclib_alpha_blend(tmp_color,acolor,mode&0X1F);	//与指定颜色进行blend		 
								}
								color=piclib_alpha_blend(tmp_color,color,alphabend/8); 			//和底色进行alphablend
							}else tmp_color=piclib_alpha_blend(acolor,color,alphabend/8);		//与指定颜色进行blend
 							pic_phy.draw_point(x+tx,y+ty,color);//显示图片				   
							tx++;//x轴增加一个像素 	  
						}
						bmpbuf+=rowadd;//跳过填充区
						tx=0;
						ty--;
					}

				}
				if(br!=readlen||res)break;	 
			}	 
		}	
		f_close(f_bmp);//关闭文件      
	}else res=PIC_SIZE_ERR;//图片尺寸错误	
#if BMP_USE_MALLOC == 1	//使用malloc	
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
	u8 *readBuff;//读SD卡数据缓存
	FIL* f_bmp;
	u8 res;
	u16 REDA_SIZE = BMP_X_SIZE * 2;
	u32 br;
	u32 j;
	readBuff = (u8 *)pic_memalloc(REDA_SIZE);
	if(readBuff == NULL) {DBG("#1 malloc memory fail\r\n"); return;}
	
	lcdSramBuffer = (u16 *)pic_memalloc(REDA_SIZE);
	if(lcdSramBuffer == NULL) {DBG("#11 malloc memory fail\r\n"); return;}
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_bmp==NULL)								//内存申请失败.
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
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//读出一行像素点数据
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
	u8 *readBuff;//读SD卡数据缓存
	FIL* f_bmp;
	u8 res;
	u16 REDA_SIZE = BMP_X_SIZE * 2;
	u32 br;
	u32 j;
	readBuff = (u8 *)pic_memalloc(REDA_SIZE);
	if(readBuff == NULL) {DBG("#1 malloc memory fail\r\n"); return;}
	
	lcdSramBuffer = (u16 *)pic_memalloc(REDA_SIZE);
	if(lcdSramBuffer == NULL) {DBG("#11 malloc memory fail\r\n"); return;}
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_bmp==NULL)								//内存申请失败.
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
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//读出一行像素点数据
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
用Image2LCD v2.9 软件把bmp个数的图标转换成bin文件
设置：1.输出数据类型--二进制bin文件
2.扫描模式--水平扫描
3.输出灰度--16位真彩色
4.最大宽度和高度--与图片真实宽度高度相符
5.高位在前

其他的选项默认
********/
//横屏显示bin格式的bmp文件
void display_bmp_bin_to_LCD(u8 *fileName,u16 x, u16 y, u16 width,u16 heigth,u16 color)
{
	u8 *readBuff;//读SD卡数据缓存
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
	
	f_bmp=(FIL *)pic_memalloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_bmp==NULL)								//内存申请失败.
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
//	gui_fill_rectangle_DMA(xOffSet,yOffSet,width,heigth,color);//绘制图片显示背景色


while(1)
{
		
		res=f_read(f_bmp,readBuff,REDA_SIZE,(UINT *)&br);	//读出一行像素点数据
		u8_2_u16(lcdSramBuffer,readBuff,br);
		dma_lcd_show(xOffSet,yOffSet + j++,width);		
		if(br!=REDA_SIZE||res){break;}
}
f_close(f_bmp);
		pic_memfree(readBuff);
		pic_memfree(f_bmp);
		pic_memfree(lcdSramBuffer);		
	
}













