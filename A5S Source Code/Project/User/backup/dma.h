

#ifndef __DMA__H
#define __DMA__H

#include "bsp.h"

#define DMA_PERIPHERAL_ADDR (&LCD->LCD_RAM)

#define DMA_SRAM_ADDR_TEST	sram_for_test
#define DMA_PERIPHERAL_SIZE	(60*1024)

#define	DMA_MEMO_NO_INC	0
#define	DMA_MEMO_INC	1

extern DMA_InitTypeDef    DMA_InitStructure;
extern vu8 dam_para_conf_flag;
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

extern  u16 sram_for_test[100];
extern vu16 *lcdSramBuffer;


void initDmaForExSram(uint16_t *srcAddr);
void start_dma_transfer(u16 *SrcAddr,u32 pic_size);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
void dma_lcd_draw_point( u16 x, u16 y,u16 color);
u16 dma_lcd_read_point(u16 x, u16 y);
void dma_lcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color);
void dma_lcd_show(u16 x, u16 y,u16 pic_size);
void pic_transfer_byDMA(u16 *SrcAddr,u32 pic_size,u8 incOrNo);
#endif
