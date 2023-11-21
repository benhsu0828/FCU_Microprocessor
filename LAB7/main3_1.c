#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"
#include "LCD.h"
#include "PowerDown.h"

volatile uint8_t KEY_Flag;
volatile uint32_t index_5ms, cnt_5ms, cnt_1s, cnt_100ms,index_key_scan;
volatile int digit[4] = {0,0,0,0};
volatile int randSeed = 0;
volatile int startFlag = 0;
char line0[16] = "               ";
char line1[16] = "               ";
char line2[16] = "               ";
char line3[16] = "               ";
volatile int num[2]= {0,0};
volatile int countFlag = 0;
volatile int op = 0;
volatile int sleepFlag = 0;

void WDT_IRQHandler(void)
{ 
	CloseSevenSegment();
	clear_LCD();
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
		CloseSevenSegment();
		clear_LCD();
		num[1]= num[0]= 0;
		countFlag = 0;
		op = 0;
        sleepFlag = 1;
    GPIO_CLR_INT_FLAG(PB, BIT15);  // Clear GPIO interrupt flag
}


void Init_EXTINT(void)
{
    // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
    NVIC_EnableIRQ(EINT1_IRQn);

    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

void TMR1_IRQHandler(void)
{
	if (cnt_5ms % 20 == 0) 
	{
		cnt_100ms++;
		index_key_scan = cnt_100ms++ % 3;
		if (index_key_scan == 0)
		{
			PA0=1; PA1=1; PA2=1; PA3=1; PA4=1; PA5=0;
		}
		if (index_key_scan == 1)
		{
			PA0=1; PA1=1; PA2=1; PA3=1; PA4=0; PA5=1;
		}
		if (index_key_scan == 2)
		{
			PA0=1; PA1=1; PA2=1; PA3=0; PA4=1; PA5=1;
		}
		NVIC_EnableIRQ(GPAB_IRQn);
	}
	if(cnt_5ms % 200 == 0){
		cnt_1s++;
        if(cnt_1s % 5 ==0){
            sleep();
        }
	}
  cnt_5ms++;
	index_5ms = cnt_5ms % 4;
  TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void GPAB_IRQHandler(void)
{
		NVIC_DisableIRQ(GPAB_IRQn);
	
if (PA->ISRC & BIT0) {        // check if PA0 interrupt occurred
		PA0=1;
	  PA->ISRC |= BIT0;         // clear PA0 interrupt status
	
if (PA3==0) { KEY_Flag =3; PA3=1;cnt_1s=0;}
if (PA4==0) { KEY_Flag =6; PA4=1;cnt_1s=0;}
if (PA5==0) { KEY_Flag =9; PA5=1;cnt_1s=0;}
if(sleepFlag == 0){sleepFlag = 1;}// wake up
return;			
} 
if (PA->ISRC & BIT1) { // check if PA1 interrupt occurred
		PA1=1;
	  PA->ISRC |= BIT1;         // clear PA1 interrupt status  
if (PA3==0) { KEY_Flag =2; PA3=1;cnt_1s=0;}
if (PA4==0) { KEY_Flag =5; PA4=1;cnt_1s=0;}
if (PA5==0) { KEY_Flag =8; PA5=1;cnt_1s=0;} 
if(sleepFlag == 0){sleepFlag = 1;}// wake up
return;				
} 
if (PA->ISRC & BIT2) { // check if PB14 interrupt occurred
		PA2=1;
	  PA->ISRC |= BIT2;         // clear PA interrupt status  feeddog();Leave_PowerDown();
if (PA3==0) { KEY_Flag =1; PA3=1;cnt_1s=0;}
if (PA4==0) { KEY_Flag =4; PA4=1;cnt_1s=0;}
if (PA5==0) { KEY_Flag =7; PA5=1;cnt_1s=0;}
if(sleepFlag == 0){sleepFlag = 1;}// wake up
return;				
}                     // else it is unexpected interrupts
PA->ISRC = PA->ISRC;	      // clear all GPB pins
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

void Init_KEY(void)
{
		GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 |BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
		GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
		GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
		GPIO_EnableInt(PA, 2, GPIO_INT_LOW);		
		NVIC_EnableIRQ(GPAB_IRQn);   
	  NVIC_SetPriority(GPAB_IRQn,3);
		GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
		GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));			
}

void sleep(){
    clear_LCD();
    sprintf(line1, "SLEEP" );
	print_Line(1 ,line1);
    sleepFlag = 0;
}

int32_t main (void)
{
	
	SYS_Init();  
    Init_WDT();
    Init_GPIO();
	init_LCD();
	Init_EXTINT();
	Init_Timer1();
	Init_KEY();
	clear_LCD();
	OpenSevenSegment();
	OpenKeyPad();
	
  while(1) {
		feeddog();
		switch(KEY_Flag){
			case 1:// 1
				clear_LCD();
				KEY_Flag = 0;
				num[countFlag] = num[countFlag]*10 + 1;
				break;
			case 2:// 2
				clear_LCD();
				KEY_Flag = 0;
				num[countFlag] = num[countFlag]*10 + 2;
				break;
			case 3:// + op = 1
				clear_LCD();
                KEY_Flag = 0;
				sprintf(line1, "+" );
				op = 1;
				countFlag = 1;
				break;
			case 4:// 3
				clear_LCD();
				KEY_Flag = 0;
				num[countFlag] = num[countFlag]*10 + 3;
				break;
			case 5:// 4
				clear_LCD();
				KEY_Flag = 0;
				num[countFlag] = num[countFlag]*10 + 4;
				break;
			case 6:// - op = 2
				clear_LCD();
                KEY_Flag = 0;
				sprintf(line1, "-" );
				op = 2;
				countFlag = 1;
				break;
			case 7:// =
				clear_LCD();
                KEY_Flag = 0;
				if(op == 1){
					sprintf(line3, "%d" ,num[0] + num[1]);
				}else if(op == 2){
					sprintf(line3, "%d" ,num[0] - num[1]);
				}else if(op == 3){
					sprintf(line3, "%d" ,num[0] / num[1]);
				}else if(op == 4){
					sprintf(line3, "%d" ,num[0] * num[1]);
				}
				countFlag = 1;
				break;
			case 8:// / op = 3
				clear_LCD();
                KEY_Flag = 0;
				sprintf(line1, "/" );
				op = 3;
				countFlag = 1;
				break;
			case 9:// * op = 4
				clear_LCD();
                KEY_Flag = 0;
				sprintf(line1, "*" );
				op = 4;
				countFlag = 1;
				break;
			default:
				break;
		}
		//show LCD
		if(countFlag == 0&&sleepFlag == 1){
				sprintf(line0, "%d" ,num[0]);
				print_Line(0 ,line0);
			}else if(countFlag == 1&&sleepFlag == 1){
				sprintf(line2, "%d" ,num[1]);
				print_Line(0 ,line0);
				print_Line(1 ,line1);
				print_Line(2 ,line2);
				print_Line(3 ,line3);
			}
			
  }
}


