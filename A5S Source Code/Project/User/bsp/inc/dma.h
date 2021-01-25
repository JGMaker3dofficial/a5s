

#ifndef __DMA__H
#define __DMA__H

#include "bsp.h"

#define DMA_PERIPHERAL_ADDR (&LCD->LCD_RAM)

#define DMA_SRAM_ADDR_TEST	sram_for_test
#define DMA_PERIPHERAL_SIZE	(60*1024)

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


extern u16 *lcdSramBuffer;


void initDmaForExSram(void);
void start_dma_transfer(u32 pic_size);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
void dma_lcd_draw_point( u16 x, u16 y,u16 color);
u16 dma_lcd_read_point(u16 x, u16 y);
void dma_lcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color);
void dma_lcd_show(u16 x, u16 y,u32 pic_size);
void initDmaForExSram_FIX(u16 *fillColor);
void start_dma_transfer_fix(u32 pic_size,u16 *fillColor);
void fill_color_used_dma(u16 x, u16 y,u16 *fillColor,u32 pic_size);
#endif
