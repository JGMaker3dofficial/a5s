

#include "dma.h"



u16 *lcdSramBuffer;
//bmpͼƬ������ʾʱ������һ��Һ����ɨ�跽ʽΪ���¡���>�ϣ���->�ң���ʾ��bmp��ָ�ԭ����ɨ�跽ʽ
void initDmaForExSram(void)
{
DMA_InitTypeDef    DMA_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	  /* DMA1 channel3 configuration */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DMA_PERIPHERAL_ADDR;  //inner SRAM-->Ext SRAM(peripheral)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)lcdSramBuffer;//
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = DMA_PERIPHERAL_SIZE;//���䳤�ȣ����65536
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);


//	DBG("#1 dma transfer begin\r\n");
}

//����Һ����������ɫ������ʼ��
void initDmaForExSram_FIX(u16 *fillColor)
{
DMA_InitTypeDef    DMA_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	  /* DMA1 channel1 configuration */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DMA_PERIPHERAL_ADDR;  //inner SRAM-->Ext SRAM(peripheral)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)fillColor;//
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = DMA_PERIPHERAL_SIZE;//���䳤�ȣ����65536,64KB
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);


}

//pic_size--���ص�����
void fill_color_used_dma(u16 x, u16 y,u16 *fillColor,u32 pic_size)
{

	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
	initDmaForExSram_FIX(fillColor);
	start_dma_transfer_fix(pic_size,fillColor);
	

}

void start_dma_transfer_fix(u32 pic_size,u16 *fillColor)
{	

	u16 saveDataCount = 0;
	u32 timeOver = 0;
	//u16 pic_size = width*height;
	if( pic_size > lcddev.height*lcddev.width) 
	{DBG("parameter error @file:%s,line:%d \r\n",__FILE__,__LINE__);	return;}
	
DMA_TRM_FIX:
	while( pic_size )
	{
			saveDataCount = pic_size;//���ݵ�ǰ�����ֽ���
			if( pic_size >= DMA_PERIPHERAL_SIZE)
			{
				
		DMA_SetCurrDataCounter(DMA1_Channel1, (uint16_t)DMA_PERIPHERAL_SIZE);
				 pic_size -= DMA_PERIPHERAL_SIZE;
			}
			else
			{
				 DMA_SetCurrDataCounter(DMA1_Channel1, (uint16_t)pic_size);
				 pic_size = 0;
			}
			
		DMA_Cmd(DMA1_Channel1, ENABLE);
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC1))//�ȴ��������
//		{
//			bsp_Idle();
//			//lcd_update();
//		}
		while(1)
		{
			timeOver = millis();
			if(DMA_GetFlagStatus(DMA1_FLAG_TC1) == SET) {break;}
			if(DMA_GetFlagStatus(DMA1_FLAG_TE1) == SET || (millis() - timeOver >= 10))//���������ִ���,����´���
			{
				
				DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
				pic_size = saveDataCount;
				DMA_ClearFlag(DMA1_FLAG_TE1);
				goto DMA_TRM_FIX;
			}
		
			bsp_Idle();
			
		}
		
		DMA_ClearFlag(DMA1_FLAG_TC1);
		DMA_Cmd(DMA1_Channel1, DISABLE);
		
	//	DBG("transferedDataCount:%d \r\n",transferedDataCount);
	}
	
//	DBG("dma transfer done \r\n");
	
}



void start_dma_transfer(u32 pic_size)
{	

	u16 transferedDataCount = 0;
	u16 saveDataCount = 0;
	u16 saveErroDataCount = 0;
	u32 timeOver = 0;
	if( pic_size > lcddev.height*lcddev.width) 
	{DBG("parameter error @file:%s,line:%d \r\n",__FILE__,__LINE__);	return;}

DMA_TRM:
	while( pic_size )
	{
			saveErroDataCount = pic_size;
			if( pic_size >= DMA_PERIPHERAL_SIZE)
			{
				saveDataCount = DMA_PERIPHERAL_SIZE;//���ݵ�ǰ�����ֽ�
				DMA_SetCurrDataCounter(DMA1_Channel3, (uint16_t)DMA_PERIPHERAL_SIZE);
				 pic_size -= DMA_PERIPHERAL_SIZE;
			}
			else
			{
				saveDataCount = pic_size;//���ݵ�ǰ�����ֽ�
				DMA_SetCurrDataCounter(DMA1_Channel3, (uint16_t)pic_size);
				 pic_size = 0;
			}
			
		DMA_Cmd(DMA1_Channel3, ENABLE);
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC3))//�ȴ��������
//		{
//			bsp_Idle();
//			//lcd_update();
//		}

		while(1)
		{
			timeOver = millis();
			if(DMA_GetFlagStatus(DMA1_FLAG_TC3) == SET) break;
			if(DMA_GetFlagStatus(DMA1_FLAG_TE3) == SET || (millis() - timeOver >= 10))//���������ִ���,����´���
			{
				
				DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
				pic_size = saveErroDataCount;
				DMA_ClearFlag(DMA1_FLAG_TE3);
				goto DMA_TRM;
			}			
			bsp_Idle();
			
		}



		DMA_ClearFlag(DMA1_FLAG_TC3);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		transferedDataCount += saveDataCount;
		DMA1_Channel3->CMAR = (uint32_t)(lcdSramBuffer + transferedDataCount);//�����ڴ��ַ
	//	DBG("transferedDataCount:%d \r\n",transferedDataCount);
	}
	
//	DBG("dma transfer done \r\n");
	
}
//�����64KB���ݣ�pic_size--�����ص�����
void dma_lcd_show(u16 x, u16 y,u32 pic_size)
{

	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
	initDmaForExSram();
	start_dma_transfer(pic_size);
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
