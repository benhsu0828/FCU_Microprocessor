#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "string.h"
#include <stdlib.h>
#include "NUC100Series.h"
#include "Seven_Segment.h"
#include "LCD.h"
#include "Scankey.h"

volatile uint32_t index_5ms,cnt_5ms,cnt_100ms,cnt_1s;
volatile int digit[4];
int redGreenLightFlag = 0;

/*
unsigned char greenPeople[3][32*8] = {
	0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
	0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF
};

unsigned char redPeople[32*8] = {
	0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
	0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF
};
*/

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(50000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(50000);	 // Delay 
	}
}

void TMR1_IRQHandler(void)
{	
	cnt_5ms++;
	index_5ms = cnt_5ms % 4;
	
	CloseSevenSegment();
	ShowSevenSegment(index_5ms,digit[index_5ms]);
	//計算時間
	if(cnt_5ms % 20 == 0)cnt_100ms++;
	if(cnt_5ms % 200 == 0){
		cnt_1s++;
		if(redGreenLightFlag == 0){
			//控制照片
			clear_LCD();
			//draw_Bmp32x64(int16_t x, int16_t y, FG_COLOR, BG_COLOR, greenPeople[cnt_1s%3]);
			//修改七段顯示器數字
			digit[0]--;
		}else if(redGreenLightFlag == 1){
			//控制照片
			clear_LCD();
			//draw_Bmp32x64(int16_t x, int16_t y, FG_COLOR, BG_COLOR, redPeople);
			//修改七段顯示器數字
			digit[3]--;
		}
	}
	
  TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE,200); // 200HZ = 5ms
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
} 

void EINT1_IRQHandler(void)
{
	GPIO_CLR_INT_FLAG(PB, BIT15);	// Clear GPIO interrupt flag
	PA12=1; PA13=1; PA14=0;
   digit[3] = 5;
	digit[0] = 0;
	//Buzz(2);
}

void Init_EXTINT(void)
{
    // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
    NVIC_EnableIRQ(EINT1_IRQn);

    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

void Init_GPIO(void)
{
	GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
	PA12=1;
	PA13=1;
	PA14=1;
}

int main(void)
{
  	SYS_Init();   // Intialize System/Peripheral clocks & multi-function I/Os
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	OpenSevenSegment();
	//Init LCD
	init_LCD();
	clear_LCD();
	Init_EXTINT();
  	Init_Timer1();
	Init_GPIO();
	redGreenLightFlag = 0;//0 green 1 red
	digit[3] = digit[2] = digit[1] = 0;
	digit[0] = 9;
	PA12=1; PA13=0; PA14=1;
	
	
  while(1){
	//控制紅綠燈
	if(redGreenLightFlag == 0&&digit[0]==0){
		//綠轉紅
		PA12=1; PA13=1; PA14=0;
		redGreenLightFlag = 1;
		digit[3] = 5;
		}else if(redGreenLightFlag == 1&&digit[3] == 0){
		//紅轉綠
		PA12=1; PA13=0; PA14=1;
		redGreenLightFlag = 0;
		digit[0] = 9;
		}
	}
}