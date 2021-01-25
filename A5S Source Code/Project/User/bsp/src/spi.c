#include "spi.h"
#include "bsp.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��



uint8_t SPI_RX_BUFFER[RX_LEN]={0x55};
uint8_t SPI_TX_BUFFER[TX_LEN]={0xaa}; 

volatile uint8_t sem_W25X_DMA_Busy = TRUE;  
volatile uint8_t sem_W25X_DMA_RxRdy= FALSE;  

void SPI2_Init(void)
{
/************************************SPI GPIO����******************************************/
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
DMA_InitTypeDef  spiDMA_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure; 
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
//	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
	 SPIx_PORT_APB_CMD;
	 SPIx_APB_CMD;
 
	GPIO_InitStructure.GPIO_Pin = SPIx_CLK_IO | SPIx_MISO_IO | SPIx_MOSI_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIx_PORT, &GPIO_InitStructure);//��ʼ��GPIOB
 	GPIO_SetBits(SPIx_PORT,SPIx_CLK_IO | SPIx_MISO_IO | SPIx_MOSI_IO);  //PB13/14/15����


/************************************SPI ����*****************************************/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPIx, ENABLE); //ʹ��SPI����
	
	

#ifdef SPI_DMA_TRANSMIT
/************************************SPI DMA����******************************************/	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(SPI_RX_DMA_CLK , ENABLE);			
  /* SPI_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
  DMA_DeInit(SPI_RX_DMA_CHENEL);
  spiDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DATA_ADDR;	
  spiDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_RX_BUFFER;
  spiDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���赽�ڴ�
  spiDMA_InitStructure.DMA_BufferSize = SPI_TRANSMIT_SIZE;
  spiDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  spiDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  spiDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  spiDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  spiDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//����ͨ�����ȼ��Ƚϸ�
  spiDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(SPI_RX_DMA_CHENEL, &spiDMA_InitStructure);

  /* Enable SPI Rx request */
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
  /* Enable DMA1 Channel4 */
  DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);			//ʹ��DMAͨ�����ڿ��ƴ��亯�������

 /*********************************TX config******************************************/ 
	DMA_DeInit(SPI_TX_DMA_CHENEL);
   spiDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DATA_ADDR;
  spiDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_TX_BUFFER;
  spiDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//�ڴ浽����
  spiDMA_InitStructure.DMA_BufferSize = SPI_TRANSMIT_SIZE;
  spiDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//�ڴ��ַ������
 // spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  spiDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  spiDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  spiDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  spiDMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//����ͨ�����ȼ��ϵ�
  spiDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(SPI_TX_DMA_CHENEL, &spiDMA_InitStructure);

  /* Enable SPI Tx request */
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
  /* Enable DMA1 Channel5 */
  DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		//ʹ��DMAͨ�����ڿ��ƴ��亯�������
  
  
  
   //���DMA��Ӧͨ����־
 
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
//    DMA_ITConfig(SPI_RX_DMA_CHENEL,DMA_IT_TC,ENABLE); 
	
/*	
	//��DMAæ�ź� 
    sem_W25X_DMA_Busy = FALSE;  
      
    //�ж�����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
*/	
	
	
	
	
#endif

}   

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPIx,ENABLE); 

} 

#ifdef SPI_DMA_TRANSMIT

/**
  *	@breif  The spi dma trans function.
  * @param  rx_buf -- the point of rx buffer
  * @param  tx_buf -- the point of tx buffer
  * @elngth length -- the size of data.
  * @retval None
  */  
void spi_trans(uint8_t *rx_buf, uint8_t *tx_buf,uint16_t length)     
{
EX_AGAIN:
	/*�رն�Ӧ��DMAͨ�� */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	/* ���ô����ֽ���*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* ���ͽ������ݴ����ַ���� */
	SPI_RX_DMA_CHENEL->CCR |= (1 << 7);
	SPI_TX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* ���ý��պͷ��͵��ڴ��ַ */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_buf;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_buf;
	

	/*��ȡһ��SPI_DR,ʹ��������� */
	SPIx->DR;
	
	/* �ȴ�������Ϊ�� */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//�ȴ�BUSYλΪ�ͣ�ȷ�����һ�����ݴ������
	
	/* �� DMAͨ��*/

	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);
	
	
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//���ջ��߷��ʹ��󣬴��´���
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//�����޴�����������ж��Ƿ������
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}

	
	/* �ر�DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);	
	
	/* ���DMA��ر�־*/
	DMA_ClearFlag(SPI_TX_DMA_CHENEL_TC_FLAG);
	DMA_ClearFlag(SPI_RX_DMA_CHENEL_TC_FLAG);
	
	
	
	
}
/**
  *	@breif  The spi dma trans function.
  * @param  rx_buf -- the point of rx buffer
  * @param  tx_data -- the spi tx data.
  * @elngth length -- the size of data.
  * @retval None
  */  
void spi_trans_read(uint8_t *rx_buf,uint8_t *tx_data,uint16_t length)
{
EX_AGAIN:
	/*�رն�Ӧ��DMAͨ�� */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	
	/* ���ô����ֽ���*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* �������ݴ����ַ����,���͵�ַ������ */
	SPI_RX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* ���ý��պͷ��͵��ڴ��ַ */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_buf;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_data;

	/*��ȡһ��SPI_DR,ʹ��������� */
	SPIx->DR;
	
	/*�ȴ�������Ϊ�� */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//�ȴ�BUSYλΪ�ͣ�ȷ�����һ�����ݴ������
	
	/* �� DMAͨ��*/
	
	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);	
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//���ջ��߷��ʹ��󣬴��´���
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//�����޴�����������ж��Ƿ������
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}
	//ȷ�������޴�����������ж��Ƿ������
//	while( DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == RESET);
//	while( DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == RESET);
	
	
	/* �ر�DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);	
	
	/* ���DMA��ر�־*/
	DMA_ClearFlag(SPI_TX_DMA_CHENEL_TC_FLAG);
	DMA_ClearFlag(SPI_RX_DMA_CHENEL_TC_FLAG);
}

/**
  *	@breif  The spi dma trans function.
  * @param  tx_buf -- the point of tx buffer
  * @param  rx_data -- the spi rx data.
  * @elngth length -- the size of data.
  * @retval None
  */  
void spi_trans_write(uint8_t *rx_data, uint8_t *tx_buffer, uint16_t length)
{
EX_AGAIN:
	/*�رն�Ӧ��DMAͨ�� */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	
	/* ���ô����ֽ���*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* �������ݴ����ַ����,���յ�ַ������ */
	SPI_TX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* ���ý��պͷ��͵��ڴ��ַ */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_data;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_buffer;

	/*��ȡһ��SPI_DR,ʹ��������� */
	SPIx->DR;
	
	/*�ȴ�������Ϊ�� */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//�ȴ�BUSYλΪ�ͣ�ȷ�����һ�����ݴ������
	
	
	/* �� DMAͨ��*/
	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//���ջ��߷��ʹ��󣬴��´���
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//�����޴�����������ж��Ƿ������
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}
	
	
	
	/* �ر�DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);		
	/* ���DMA��ر�־*/
	DMA_ClearFlag(SPI_TX_DMA_CHENEL_TC_FLAG);
	DMA_ClearFlag(SPI_RX_DMA_CHENEL_TC_FLAG);
	
	
}





#else  /* ��ͨ��ʽ����*/
/**
  *	@breif  The spi read & write a word data function.
  * @param  None
  * @retval None
  */  

/**
  *	@breif  The spi simple trans function.
  * @param  rx_buf -- the point of rx buffer
  * @param  tx_buf -- the point of tx buffer
  * @elngth length -- the size of data.
  * @retval None
  */  
void spi_trans(uint8_t *rxbuf,uint8_t *txbuf,uint16_t length)
{
	while(length--)
	{
		*rxbuf++ = SPI2_ReadWriteByte(*txbuf++);
	}
}
#endif /* USE_DMA_TRANS */


//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
}




























