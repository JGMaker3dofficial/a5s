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


//FATFS�ײ�(diskio) ��������	   

////////////////////////////////////////////////////////////////////////////////// 

#define SD_CARD	 0  //SD��,���Ϊ0
#define EX_FLASH 1	//�ⲿflash,���Ϊ1

#define FLASH_SECTOR_SIZE 	512		

	  
//����W25Q128
//ǰ12M�ֽڸ�fatfs��,12M�ֽں�,���ڴ���ֿ�,�ֿ�ռ��3.09M.	ʣ�ಿ��,���ͻ��Լ���	 			    
u32	    FLASH_SECTOR_COUNT = 1024*1024*12/FLASH_SECTOR_SIZE;	//W25Q1218,ǰ12M�ֽڸ�FATFSռ��
#define FLASH_BLOCK_SIZE   	8     	//ÿ��BLOCK��8������


#define SECTOR_SIZE		512	/* SD��������С����Ϊ512 */

//��ô���״̬
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
//��ʼ������
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;    
	switch(pdrv)
	{
		case SD_CARD://SD��
			if (SD_Init() == SD_OK)
			{
				stat = RES_OK;
			}
			else
			{
				stat = STA_NODISK;
			}
		
  			break;
			
		case EX_FLASH://�ⲿflash
			W25QXX_Init();

			stat = RES_OK;
 			break;
			
		default:
		break;
		
	}		 
	return stat;
} 
//������
//pdrv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
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
	u8 cnt=0;//SD���ڶ���ʱ�򱻰γ�����ȡ��������
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��

		tempSecotr = sector;
			if (count == 1)
			{

				Status = SD_ReadBlock(buff, tempSecotr , SECTOR_SIZE);
			}
			else
			{

				Status = SD_ReadMultiBlocks(buff, tempSecotr , SECTOR_SIZE, count);
			}
			
			/* SDIO������DMAģʽ����Ҫ������DMA�����Ƿ���� */
			Status = SD_WaitReadOperation();
			if (Status != SD_OK)
			{
				return RES_ERROR;
			}
			
			//while(SD_GetStatus() != SD_TRANSFER_OK); ������忨����˴�������
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
					//  SD_TRANSFER_BUSY ������ȴ� 
				}
			}
			break;
			
		case EX_FLASH://�ⲿflash
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
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
	return res;  
	
}
//д����
//pdrv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
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
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	
		
	switch(pdrv)
	{
		case SD_CARD://SD��
		tempSecotr = sector;	
			if (count == 1)
			{
				Status = SD_WriteBlock((uint8_t *)buff, tempSecotr , SECTOR_SIZE);
			}
			else
			{
				Status = SD_WriteMultiBlocks((uint8_t *)buff, tempSecotr ,SECTOR_SIZE, count);
			}

			/* SDIO������DMAģʽ����Ҫ������DMA�����Ƿ���� */
			Status = SD_WaitReadOperation();
			if (Status != SD_OK)
			{
				return RES_ERROR;
			}
			
			//while(SD_GetStatus() != SD_TRANSFER_OK); ������忨����˴�������
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
					//  SD_TRANSFER_BUSY ������ȴ� 
				}
			}
			break;
		case EX_FLASH://�ⲿflash
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
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
	return res;	
}
#endif
//����������Ļ��
//pdrv:���̱��0~9
//ctrl:���ƴ���
//*buff:����/���ջ�����ָ��
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
DRESULT res = RES_PARERR;						  			     
	if(pdrv==SD_CARD)//SD��
	{
		res = RES_ERROR;
		switch (cmd)
		{
			/* SD������������ SDCardInfo.CardCapacity */
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
	
	else if(pdrv==EX_FLASH)	//�ⲿFLASH  
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
		    case GET_BLOCK_SIZE://ÿ������-����Ϊ��λ
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT://�ܹ��ж��ٸ�����
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	else res=RES_ERROR;//�����Ĳ�֧��
	
    return res;
}
#endif
//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}

















