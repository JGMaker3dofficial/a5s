#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//spi通讯是否使用DMA方式通讯	  
#define SPI_DMA_TRANSMIT  


#define SPI_TRANSMIT_SIZE	32
#define RX_LEN 			(uint8_t)20
#define TX_LEN          (uint8_t)20


/* SPI DUMMY_BYTE */
#define DUMMY_BYTE		0xFF









#define SPI_TX_DMA_CHENEL	DMA1_Channel5
#define SPI_RX_DMA_CHENEL	DMA1_Channel4
#define SPI_RX_DMA_CLK	RCC_AHBPeriph_DMA1
#define SPI_DATA_ADDR	(&(SPI2->DR))
#define SPIx	SPI2
#define SPI_RX_DMA_CHENEL_TC_FLAG  DMA1_FLAG_TC4
#define SPI_TX_DMA_CHENEL_TC_FLAG  DMA1_FLAG_TC5
#define SPI_TX_DMA_CLEAR_FLAG 	 DMA_ClearFlag(DMA1_FLAG_GL5|DMA1_FLAG_TC5|DMA1_FLAG_HT5|DMA1_FLAG_TE5);  
#define SPI_RX_DMA_CLEAR_FLAG 	 DMA_ClearFlag(DMA1_FLAG_GL4|DMA1_FLAG_TC4|DMA1_FLAG_HT4|DMA1_FLAG_TE4);  

#define SPI_TX_DMA_TE_FLAG  DMA1_FLAG_TE5
#define SPI_RX_DMA_TE_FLAG  DMA1_FLAG_TE4


#if 0
#define SPI_TX_DMA_CHENEL	DMA2_Channel2
#define SPI_RX_DMA_CHENEL	DMA2_Channel1
#define SPI_RX_DMA_CLK	RCC_AHBPeriph_DMA2
#define SPI_DATA_ADDR	(&(SPI2->DR))
#define SPIx	SPI2
#define SPI_RX_DMA_CHENEL_TC_FLAG  DMA2_FLAG_TC1
#define SPI_TX_DMA_CHENEL_TC_FLAG  DMA2_FLAG_TC2


#define SPI_TX_DMA_CLEAR_FLAG 	 DMA_ClearFlag(DMA2_FLAG_GL2|DMA2_FLAG_TC2|DMA2_FLAG_HT2|DMA2_FLAG_TE2);  
#define SPI_RX_DMA_CLEAR_FLAG 	 DMA_ClearFlag(DMA2_FLAG_GL1|DMA2_FLAG_TC1|DMA2_FLAG_HT1|DMA2_FLAG_TE1);  

#define SPI_TX_DMA_TE_FLAG  DMA2_FLAG_TE2
#define SPI_RX_DMA_TE_FLAG  DMA2_FLAG_TE1
#endif


#define SPIx_PORT					GPIOB
#define SPIx_CLK_IO					GPIO_Pin_13
#define SPIx_MISO_IO				GPIO_Pin_14
#define SPIx_MOSI_IO				GPIO_Pin_15
#define SPIx_PORT_APB_CLK			RCC_APB2Periph_GPIOB
#define SPIx_APB_CLK			    RCC_APB1Periph_SPI2

#define SPIx_PORT_APB_CMD		RCC_APB2PeriphClockCmd(	SPIx_PORT_APB_CLK, ENABLE )
#define SPIx_APB_CMD		 	RCC_APB1PeriphClockCmd(SPIx_APB_CLK,  ENABLE )








extern volatile uint8_t sem_W25X_DMA_Busy;  
extern volatile uint8_t sem_W25X_DMA_RxRdy;  
extern uint8_t SPI_RX_BUFFER[RX_LEN];
extern uint8_t SPI_TX_BUFFER[TX_LEN];
	  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
void spi_trans(uint8_t *rx_buf, uint8_t *tx_buf,uint16_t length);
     
#ifdef SPI_DMA_TRANSMIT
void spi_trans_read(uint8_t *rx_buf,uint8_t *tx_data,uint16_t length);
void spi_trans_write(uint8_t *rx_data, uint8_t *tx_buffer, uint16_t length);




#endif
u8 SPI2_ReadWriteByte(u8 TxData);
//u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节





		 
#endif

