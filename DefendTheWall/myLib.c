#include "myLib.h"



unsigned short *videoBuffer = (unsigned short *)0x6000000;



void setPixel(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row, col, 240)] = color;
}

void drawRect(int row, int col, int height, int width, volatile unsigned short color)
{
	for(int r=0; r<height; r++)
	{
		REG_DMA3SAD = (u32)&color;
		REG_DMA3DAD = (u32)&videoBuffer[OFFSET(row+r, col, 240)];
		REG_DMA3CNT = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
		
	}
}

void drawTarget(int row, int col, int size, volatile unsigned short color)
{
	drawRect(row - (size / 2), col, size, 1, color); //draws horizontal crosshair
	drawRect(row, col - (size / 2), 1, size, color); //draws vertical crosshair
}

void drawImage(int row, int col, int width, int height, const u16* image) {
    for (int r = 0; r < height; r++) {
        DMA[3].src = image + r * width;
        DMA[3].dst = videoBuffer + OFFSET(row + r, col, 240);
        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void repairBackground(int row, int col, int height, int width, const u16* image) 
{
	for (int r = 0; r < height; r++) 
	{
	        DMA[3].src = image + ((row + r) * 240) + col;
	        DMA[3].dst = videoBuffer + ((row + r) * 240) + col;
	        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
        }
}

void delay(int n)
{
	volatile int size = 0;
	for(int i=0; i<n*4000; i++)
	{
		size = size + 1;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 161);
}

void fillScreen(volatile u16 color)
{
	REG_DMA3SAD = (u32)&color;
	REG_DMA3DAD = (u32)videoBuffer;
	REG_DMA3CNT = (160*240) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;

}
