#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"
#include "LCD.h"

volatile uint32_t index_5ms,cnt_5ms,cnt_100ms,cnt_1s;
volatile int digit[4];
int randSeed = 0;
int stopFlag = 0;

void WDT_IRQHandler(void)
{ 
  WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
}

void feeddog(void){
  SYS_UnlockReg();
	WDT_RESET_COUNTER();
  SYS_LockReg();
}

void Init_WDT(void)
{
  // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
  SYS_UnlockReg();
  WDT_Open(WDT_TIMEOUT_2POW16, 0, TRUE, TRUE);
  WDT_EnableInt();          // Enable WDT timeout interrupt
  NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
  SYS_LockReg();
}

void Init_GPIO(void)
{
	GPIO_SetMode(PC, BIT12, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
	PA12=1;
	PA13=1;
	PA14=1;
  PC12=1;
}

void EINT1_IRQHandler(void)
{
		while(1){
			CloseSevenSegment();
			clear_LCD();
			PC12=0;
			CLK_SysTickDelay(100000);
			PC12=1;
			CLK_SysTickDelay(100000);
		};
    GPIO_CLR_INT_FLAG(PB, BIT15);  // Clear GPIO interrupt flag
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

void TMR1_IRQHandler(void)
{
	//????
	if(cnt_5ms % 20 == 0)cnt_100ms++;
	if(cnt_5ms % 200 == 0){
		cnt_1s++;
		if(stopFlag == 0){
			digit[3] = rand()%9+1;
			digit[2] = rand()%9+1;
			digit[1] = rand()%9+1;
			digit[0] = rand()%9+1;
		}
	}
	CloseSevenSegment();
	ShowSevenSegment(index_5ms,digit[index_5ms]);
  cnt_5ms++;
	index_5ms = cnt_5ms % 4;
  TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

int32_t main (void)
{
  int keyin = 0;
	int checkin = 0;
	int num1 = 0;
	int num2 = 0;
	char line0[16] = "               ";
	char line1[16] = "               ";
	char line2[16] = "               ";
	char line3[16] = "               ";
	
	SYS_Init();  
  Init_WDT();
  Init_GPIO();
	init_LCD();
	Init_EXTINT();
	Init_Timer1();
	clear_LCD();
	OpenSevenSegment();
	OpenKeyPad();
	
  while(1) {
		srand(randSeed++);
		keyin = ScanKey();
		feeddog();
		if(keyin != checkin){
			switch(keyin){
				case 5:
					clear_LCD();
					if(stopFlag == 0){ // stop
						num1 = digit[3]*10 + digit[2];
						num2 = digit[1]*10 + digit[0];
						sprintf(line0, "%2d + %2d = %d" ,num1, num2, num1+num2);
						sprintf(line1, "%2d - %2d = %d" ,num1, num2, num1-num2);
						sprintf(line2, "%2d * %2d = %d" ,num1, num2, num1*num2);
						sprintf(line3, "%2d / %2d = %d" ,num1, num2, num1/num2);
						print_Line(0 ,line0);
						print_Line(1 ,line1);
						print_Line(2 ,line2);
						print_Line(3 ,line3);
					}else{ //continue
						num1 = 0;
						num2 = 0;
					}
					stopFlag = ~stopFlag;
					break;
				default:
					checkin = keyin;
					break;
			}
		}
		
  }
}


