/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
#include "bsp.h"	
#include "ff.h"	


//FATFS底层(diskio) 驱动代码	   

////////////////////////////////////////////////////////////////////////////////// 

#define SD_CARD	 0  //SD卡,卷标为0
#define EX_FLASH 1	//外部flash,卷标为1

#define FLASH_SECTOR_SIZE 	512		

	  
//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.	剩余部分,给客户自己用	 			    
u32	    FLASH_SECTOR_COUNT = 1024*1024*12/FLASH_SECTOR_SIZE;	//W25Q1218,前12M字节给FATFS占用
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区


#define SECTOR_SIZE		512	/* SD卡扇区大小必须为512 */

//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 

	DSTATUS stat = STA_NOINIT;

	switch (pdrv)
	{
		case SD_CARD :
			stat = 0;
			break;
			
		case EX_FLASH :
			stat = 0;
			break;
			
		default:
			break;
	}
	return stat;
}  
//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;    
	switch(pdrv)
	{
		case SD_CARD://SD卡
			if (SD_Init() == SD_OK)
			{
				stat = RES_OK;
			}
			else
			{
				stat = STA_NODISK;
			}
		
  			break;
			
		case EX_FLASH://外部flash
			W25QXX_Init();

			stat = RES_OK;
 			break;
			
		default:
		break;
		
	}		 
	return stat;
} 
//读扇区
//pdrv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	SD_Error Status;
	DWORD tempSecotr;
	u8 res=RES_OK; 
	u8 cnt=0;//SD卡在读的时候被拔出，读取次数控制
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡

		tempSecotr = sector;
			if (count == 1)
			{

				Status = SD_ReadBlock(buff, tempSecotr , SECTOR_SIZE);
			}
			else
			{

				Status = SD_ReadMultiBlocks(buff, tempSecotr , SECTOR_SIZE, count);
			}
			
			/* SDIO工作在DMA模式，需要检查操作DMA传输是否完成 */
			Status = SD_WaitReadOperation();
			if (Status != SD_OK)
			{
				return RES_ERROR;
			}
			
			//while(SD_GetStatus() != SD_TRANSFER_OK); 如果不插卡，则此处会死机
			{
				uint32_t i;
				SDTransferState status;
				
				for (i = 0; i < 10; i++)
				{									
					status = SD_GetStatus();
					if (status == SD_TRANSFER_ERROR)
					{
						res = RES_ERROR;
						break;
					}
					else if (status == SD_TRANSFER_OK)
					{
						res = RES_OK;
						break;
					}
					//  SD_TRANSFER_BUSY 则继续等待 
				}
			}
			break;
			
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=RES_OK;
			break;
			
		default:
			res = RES_PARERR;
		break;
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
	return res;  
	
}
//写扇区
//pdrv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res=RES_OK;  
	SD_Error Status;
	DWORD tempSecotr;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	
		
	switch(pdrv)
	{
		case SD_CARD://SD卡
		tempSecotr = sector;	
			if (count == 1)
			{
				Status = SD_WriteBlock((uint8_t *)buff, tempSecotr , SECTOR_SIZE);
			}
			else
			{
				Status = SD_WriteMultiBlocks((uint8_t *)buff, tempSecotr ,SECTOR_SIZE, count);
			}

			/* SDIO工作在DMA模式，需要检查操作DMA传输是否完成 */
			Status = SD_WaitReadOperation();
			if (Status != SD_OK)
			{
				return RES_ERROR;
			}
			
			//while(SD_GetStatus() != SD_TRANSFER_OK); 如果不插卡，则此处会死机
			{
				uint32_t i;
				SDTransferState status;
				
				for (i = 0; i < 10; i++)
				{									
					status = SD_GetStatus();
					if (status == SD_TRANSFER_ERROR)
					{
						res = RES_ERROR;
						break;
					}
					else if (status == SD_TRANSFER_OK)
					{
						res = RES_OK;
						break;
					}
					//  SD_TRANSFER_BUSY 则继续等待 
				}
			}
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{										    
				W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=RES_OK;
			break;
			
		default:
			res=RES_PARERR; 
			break;
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
	return res;	
}
#endif
//其他表参数的获得
//pdrv:磁盘编号0~9
//ctrl:控制代码
//*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
DRESULT res = RES_PARERR;						  			     
	if(pdrv==SD_CARD)//SD卡
	{
		res = RES_ERROR;
		switch (cmd)
		{
			/* SD卡磁盘容量： SDCardInfo.CardCapacity */
			case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
				res = RES_OK;
				break;

			case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
				*(DWORD*)buff = SDCardInfo.CardCapacity / 512;
				res = RES_OK;
				break;

			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
				*(WORD*)buff = 512;
				res = RES_OK;
				break;

			//case CTRL_ERASE_SECTOR: /* Erase a block of sectors (used when _USE_ERASE == 1) */
			 case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;
			default:
				res = RES_PARERR;
				break;
		}
	}
	
	else if(pdrv==EX_FLASH)	//外部FLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE://每个块多大-扇区为单位
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT://总共有多少个扇区
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	else res=RES_ERROR;//其他的不支持
	
    return res;
}
#endif
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}

















