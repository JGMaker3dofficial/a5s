

#include "dma.h"

DMA_InitTypeDef    DMA_InitStructure;
// u16 sram_for_test[100]={
//1,2,3,4,5,6,7,8,9,
//10,11,12,13,14,15,16,17,18,19,
//20,21,22,23,24,25,26,27,28,29,
//30,31,32,33,34,35,36,37,38,39,
//40,41,42,43,44,45,46,47,48,49,
//50,51,52,53,54,55,56,57,58,59,
//60,61,62,63,64,65,66,67,68,69,
//70,71,72,73,74,75,76,77,78,79,
//80,81,82,83,84,85,86,87,88,89,
//90,91,92,93,94,95,96,97,98,99,
//100,
//};
vu16 *lcdSramBuffer;
vu8 dam_para_conf_flag = DMA_MEMO_INC;


void initDmaForExSram(uint16_t *srcAddr)
{
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	  /* DMA1 channel3 configuration */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DMA_PERIPHERAL_ADDR;  //inner SRAM-->Ext SRAM(peripheral)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)srcAddr;//
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = DMA_PERIPHERAL_SIZE;//传输长度，最大65536
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
if (dam_para_conf_flag==DMA_MEMO_INC)	
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
else
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

  /* Enable DMA1 channel3 */
 // DMA_Cmd(DMA1_Channel3, ENABLE);
//	DEBUG_PRINTF("#1 dma transfer begin\r\n");
}



//每个pixel占用2个字节，RGB=565，R-5bit，G--6bit，B--5bit
//lcdSramBuffer数组的长度=液晶屏长度*液晶屏宽度=320*240，sizeof(lcdSramBuffer)=320*240*2
void dma_lcd_draw_point( u16 x, u16 y,u16 color)
{
	lcdSramBuffer[x*y + x] = color;
}


u16 dma_lcd_read_point(u16 x, u16 y)
{
	return lcdSramBuffer[x*y + x];
}

void dma_lcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{   
	u16 i,j; 
 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			dma_lcd_draw_point(x+j,y+i,*color++);
		}	
	}	
} 

void start_dma_transfer(u16 *SrcAddr,u32 pic_size)
{	

	u16 transferedDataCount = 0;
	//u16 pic_size = width*height;
	if( pic_size > lcddev.height*lcddev.width) 
	{DEBUG_PRINTF("parameter error @file:%s,line:%d \r\n",__FILE__,__LINE__);	return;}

//	DEBUG_PRINTF("################# \r\n");
	
	initDmaForExSram(SrcAddr);
	while( pic_size )
	{
		if( pic_size >= DMA_PERIPHERAL_SIZE)
		{
			 DMA_InitStructure.DMA_BufferSize = DMA_PERIPHERAL_SIZE;
			 DMA_Init(DMA1_Channel3, &DMA_InitStructure);
			 pic_size -= DMA_PERIPHERAL_SIZE;
		}
		else
		{
			 DMA_InitStructure.DMA_BufferSize = pic_size;
			 DMA_Init(DMA1_Channel3, &DMA_InitStructure);
			 pic_size = 0;
		}
		
	DMA_Cmd(DMA1_Channel3, ENABLE);
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC3))//等待传输完成
	{
		bsp_Idle();
		//lcd_update();
	}
	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_Cmd(DMA1_Channel3, DISABLE);
	
	transferedDataCount += DMA_InitStructure.DMA_BufferSize;
	
if (dam_para_conf_flag==DMA_MEMO_INC)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(lcdSramBuffer + transferedDataCount);
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
//	DEBUG_PRINTF("transferedDataCount:%d \r\n",transferedDataCount);
	}
	
//	DEBUG_PRINTF("dma transfer done \r\n");
	
}


void dma_lcd_show(u16 x, u16 y,u16 pic_size)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	
//	start_dma_transfer(pic_size);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
  }

  return PASSED;  
}
void pic_transfer_byDMA(u16 *SrcAddr,u32 pic_size,u8 incOrNo)
{
	dam_para_conf_flag = incOrNo;
	start_dma_transfer(SrcAddr,pic_size);
//	DEBUG_PRINTF("free lcdSramBuffer\r\n"); 

}
