#include "spi.h"
#include "bsp.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01						  
//SPI口初始化
//这里针是对SPI2的初始化



uint8_t SPI_RX_BUFFER[RX_LEN]={0x55};
uint8_t SPI_TX_BUFFER[TX_LEN]={0xaa}; 

volatile uint8_t sem_W25X_DMA_Busy = TRUE;  
volatile uint8_t sem_W25X_DMA_RxRdy= FALSE;  

void SPI2_Init(void)
{
/************************************SPI GPIO设置******************************************/
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
DMA_InitTypeDef  spiDMA_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure; 
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
//	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能 	
	 SPIx_PORT_APB_CMD;
	 SPIx_APB_CMD;
 
	GPIO_InitStructure.GPIO_Pin = SPIx_CLK_IO | SPIx_MISO_IO | SPIx_MOSI_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPIx_PORT, &GPIO_InitStructure);//初始化GPIOB
 	GPIO_SetBits(SPIx_PORT,SPIx_CLK_IO | SPIx_MISO_IO | SPIx_MOSI_IO);  //PB13/14/15上拉


/************************************SPI 配置*****************************************/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPIx, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE); //使能SPI外设
	
	

#ifdef SPI_DMA_TRANSMIT
/************************************SPI DMA设置******************************************/	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(SPI_RX_DMA_CLK , ENABLE);			
  /* SPI_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
  DMA_DeInit(SPI_RX_DMA_CHENEL);
  spiDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DATA_ADDR;	
  spiDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_RX_BUFFER;
  spiDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设到内存
  spiDMA_InitStructure.DMA_BufferSize = SPI_TRANSMIT_SIZE;
  spiDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  spiDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  spiDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  spiDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  spiDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//接收通道优先级比较高
  spiDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(SPI_RX_DMA_CHENEL, &spiDMA_InitStructure);

  /* Enable SPI Rx request */
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
  /* Enable DMA1 Channel4 */
  DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);			//使能DMA通道留在控制传输函数中完成

 /*********************************TX config******************************************/ 
	DMA_DeInit(SPI_TX_DMA_CHENEL);
   spiDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DATA_ADDR;
  spiDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_TX_BUFFER;
  spiDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//内存到外设
  spiDMA_InitStructure.DMA_BufferSize = SPI_TRANSMIT_SIZE;
  spiDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//内存地址不增加
 // spiDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  spiDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  spiDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  spiDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  spiDMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//发送通道优先级较低
  spiDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(SPI_TX_DMA_CHENEL, &spiDMA_InitStructure);

  /* Enable SPI Tx request */
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
  /* Enable DMA1 Channel5 */
  DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		//使能DMA通道留在控制传输函数中完成
  
  
  
   //清除DMA对应通道标志
 
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
//    DMA_ITConfig(SPI_RX_DMA_CHENEL,DMA_IT_TC,ENABLE); 
	
/*	
	//清DMA忙信号 
    sem_W25X_DMA_Busy = FALSE;  
      
    //中断配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
*/	
	
	
	
	
#endif

}   

//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
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
	/*关闭对应的DMA通道 */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	/* 设置传输字节数*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* 发送接收数据储存地址自增 */
	SPI_RX_DMA_CHENEL->CCR |= (1 << 7);
	SPI_TX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* 设置接收和发送的内存地址 */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_buf;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_buf;
	

	/*读取一次SPI_DR,使其内容清空 */
	SPIx->DR;
	
	/* 等待发送区为空 */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//等待BUSY位为低，确保最后一个数据传输完成
	
	/* 打开 DMA通道*/

	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);
	
	
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//接收或者发送错误，从新传输
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//传输无错误情况下再判断是否传输结束
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}

	
	/* 关闭DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);	
	
	/* 清除DMA相关标志*/
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
	/*关闭对应的DMA通道 */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	
	/* 设置传输字节数*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* 接收数据储存地址自增,发送地址不自增 */
	SPI_RX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* 设置接收和发送的内存地址 */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_buf;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_data;

	/*读取一次SPI_DR,使其内容清空 */
	SPIx->DR;
	
	/*等待发送区为空 */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//等待BUSY位为低，确保最后一个数据传输完成
	
	/* 打开 DMA通道*/
	
	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);	
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//接收或者发送错误，从新传输
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//传输无错误情况下再判断是否传输结束
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}
	//确保传输无错误情况下再判断是否传输结束
//	while( DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == RESET);
//	while( DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == RESET);
	
	
	/* 关闭DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);	
	
	/* 清除DMA相关标志*/
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
	/*关闭对应的DMA通道 */
	 DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);
	 DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);		
SPI_TX_DMA_CLEAR_FLAG;
SPI_RX_DMA_CLEAR_FLAG;	
	
	/* 设置传输字节数*/
	DMA_SetCurrDataCounter(SPI_RX_DMA_CHENEL, (uint16_t)length);
	DMA_SetCurrDataCounter(SPI_TX_DMA_CHENEL, (uint16_t)length);
	
	/* 发送数据储存地址自增,接收地址不自增 */
	SPI_TX_DMA_CHENEL->CCR |= (1 << 7);
	
	/* 设置接收和发送的内存地址 */
	SPI_RX_DMA_CHENEL->CMAR = (uint32_t)rx_data;
	SPI_TX_DMA_CHENEL->CMAR = (uint32_t)tx_buffer;

	/*读取一次SPI_DR,使其内容清空 */
	SPIx->DR;
	
	/*等待发送区为空 */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);//等待BUSY位为低，确保最后一个数据传输完成
	
	
	/* 打开 DMA通道*/
	DMA_Cmd(SPI_RX_DMA_CHENEL, ENABLE);
	DMA_Cmd(SPI_TX_DMA_CHENEL, ENABLE);
	
	while(1)
	{
		if(DMA_GetFlagStatus(SPI_TX_DMA_TE_FLAG) == SET || DMA_GetFlagStatus(SPI_RX_DMA_TE_FLAG) == SET)	//接收或者发送错误，从新传输
		{
			DBG("->file:%s,line:%d dma transimt error\r\n",__FILE__,__LINE__);
			goto EX_AGAIN;
		}
		//传输无错误情况下再判断是否传输结束
		if(DMA_GetFlagStatus(SPI_RX_DMA_CHENEL_TC_FLAG) == SET && DMA_GetFlagStatus(SPI_TX_DMA_CHENEL_TC_FLAG) == SET) break;
		bsp_Idle();
	}
	
	
	
	/* 关闭DMA */
	DMA_Cmd(SPI_TX_DMA_CHENEL, DISABLE);
	DMA_Cmd(SPI_RX_DMA_CHENEL, DISABLE);		
	/* 清除DMA相关标志*/
	DMA_ClearFlag(SPI_TX_DMA_CHENEL_TC_FLAG);
	DMA_ClearFlag(SPI_RX_DMA_CHENEL_TC_FLAG);
	
	
}





#else  /* 普通方式传输*/
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


//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据					    
}




























