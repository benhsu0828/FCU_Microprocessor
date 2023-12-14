//
// TMR_LED : change LED on/off by Timer1 interrupt
//
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "LCD.h"

#define RXBUFSIZE 1

volatile uint8_t KEY_Flag;
volatile uint32_t index_key_scan;
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;
volatile uint32_t count_Line = 0; 
volatile uint32_t count_Char;
volatile char RX_buffer[RXBUFSIZE];
volatile int digit[4] = {0,0,0,0};
char line0[16] = "               ";
char line1[16] = "               ";
char line2[16] = "               ";
char line3[16] = "               ";


void EINT1_IRQHandler(void)//PB15
{
    GPIO_CLR_INT_FLAG(PB, BIT15);  // Clear GPIO interrupt flag
}

void TMR0_IRQHandler(void){
    if (cnt0_5ms % 20 == 0) 
	{
		cnt0_100ms++;
    }
    if(cnt1_5ms % 200 == 0){
		cnt1_1s++;
	}
    cnt0_5ms++;
    TIMER_ClearIntFlag(TIMER0); // Clear Timer0 time-out interrupt flag
}

void TMR1_IRQHandler(void)
{// for keypad and seven segment
	if (cnt1_5ms % 20 == 0) 
	{
		cnt1_100ms++;
		index_key_scan = cnt1_100ms++ % 3;
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
	if(cnt1_5ms % 200 == 0){
		cnt1_1s++;
	}
    cnt1_5ms++;
    CloseSevenSegment();
    ShowSevenSegment(index1_5ms,digit[index1_5ms]);
    index1_5ms = cnt1_5ms % 4;
    TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void GPAB_IRQHandler(void)
{
    NVIC_DisableIRQ(GPAB_IRQn);

    if (PA->ISRC & BIT0) {        // check if PA0 interrupt occurred
        PA0=1;
        PA->ISRC |= BIT0;         // clear PA0 interrupt status
    if (PA3==0) { KEY_Flag =3; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =6; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =9; PA5=1;cnt1_1s=0;}
    return;			
    } 
    if (PA->ISRC & BIT1) { // check if PA1 interrupt occurred
            PA1=1;
        PA->ISRC |= BIT1;         // clear PA1 interrupt status  
    if (PA3==0) { KEY_Flag =2; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =5; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =8; PA5=1;cnt1_1s=0;} 
    return;				
    } 
    if (PA->ISRC & BIT2) { // check if PB14 interrupt occurred
            PA2=1;
        PA->ISRC |= BIT2;         // clear PA interrupt status  feeddog();Leave_PowerDown();
    if (PA3==0) { KEY_Flag =1; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =4; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =7; PA5=1;cnt1_1s=0;}
    return;				
    }                     // else it is unexpected interrupts
    PA->ISRC = PA->ISRC;	      // clear all GPB pins
}


void GPAB_IRQHandler(void)
{
    NVIC_DisableIRQ(GPAB_IRQn);

    if (PA->ISRC & BIT0) {        // check if PA0 interrupt occurred
            PA0=1;
        PA->ISRC |= BIT0;         // clear PA0 interrupt status
        
    if (PA3==0) { KEY_Flag =3; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =6; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =9; PA5=1;cnt1_1s=0;}
    return;			
    } 
    if (PA->ISRC & BIT1) { // check if PA1 interrupt occurred
            PA1=1;
        PA->ISRC |= BIT1;         // clear PA1 interrupt status  
    if (PA3==0) { KEY_Flag =2; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =5; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =8; PA5=1;cnt1_1s=0;} 
    return;				
    } 
    if (PA->ISRC & BIT2) { // check if PB14 interrupt occurred
            PA2=1;
        PA->ISRC |= BIT2;         // clear PA interrupt status  feeddog();Leave_PowerDown();
    if (PA3==0) { KEY_Flag =1; PA3=1;cnt1_1s=0;}
    if (PA4==0) { KEY_Flag =4; PA4=1;cnt1_1s=0;}
    if (PA5==0) { KEY_Flag =7; PA5=1;cnt1_1s=0;}
    return;				
    }                     // else it is unexpected interrupts
    PA->ISRC = PA->ISRC;	      // clear all GPB pins
}


void UART02_IRQHandler(void)
{
	  uint32_t u32IntSts= UART0->ISR; // UART interrupt status bit
		uint8_t c;
		
    if(u32IntSts & UART_IS_RX_READY(UART1)){
			while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
				c = UART_READ(UART0);
				if (c!= '0') {
					guess[countInput] = c - 48;
					//printf("%d\n",guess[countInput]);
					countInput++;
					if(countInput == 4){
						countInput = 0;
						guessFlag = 1;
					}
				}
			}		
    }
}

void Init_ALL(void)
{
    SYS_Init(); 
    init_LCD();
    clear_LCD();
    OpenSevenSegment();
	OpenKeyPad();
    //Buzzer
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); 
    //LED
	GPIO_SetMode(PC, (BIT12|BIT13|BIT14|BIT15), GPIO_MODE_OUTPUT);
    PC12=PC13=PC14=PC15=1;// turn off LED
    // // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
    // SYS_UnlockReg();
    // WDT_Open(WDT_TIMEOUT_2POW16, 0, FALSE, FALSE);
    // WDT_EnableInt();          // Enable WDT timeout interrupt
    // NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
    // SYS_LockReg();
    //GPIO RGBLED
    GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
    PA12=1;// PA12 = Blue,  0 : on, 1 : off
    PA13=1;// PA13 = Green, 0 : on, 1 : off
    PA14=1;// PA14 = Red,   0 : on, 1 : off
    //EXT
    // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
    NVIC_EnableIRQ(EINT1_IRQn);
    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
    //TIMER0
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
    //TIMER1
    TIMER_Open(TIMER1, TMR1_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
    //keypad interrupt
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 |BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 2, GPIO_INT_LOW);		
    NVIC_EnableIRQ(GPAB_IRQn);   
    NVIC_SetPriority(GPAB_IRQn,3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));	
    //UART
    UART_Open(UART0, 115200);                     // set UART0 baud rate
    UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
    NVIC_EnableIRQ(UART02_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART02
}


int main(void)
{
  uint8_t ASSCII;
	int i = 0;
	int tmp[4] = {0,0,0,0};
	Init_ALL();
	//LCD back light
	PD14 = 0;

	//guess
	 while(1) {
		 if(KEY_Flag!=0){
			//printf("key: %d\n",KEY_Flag);
			tmp[3] = tmp[2];
			tmp[2] = tmp[1];
			tmp[1] = tmp[0];
			tmp[0] = KEY_Flag;
			KEY_Flag = 0;
			if(tmp[3]!=0){
				for(i = 0;i<4;i++){
					ASSCII = tmp[i]+48;
					UART_Write(UART0, &ASSCII, 1);
				}
			}
		}
        switch(KEY_Flag){
			case 1:// 1
				KEY_Flag = 0;
				break;
			case 2:// 2
				clear_LCD();
				KEY_Flag = 0;
				break;
			case 3:// + op = 1
				clear_LCD();
                KEY_Flag = 0;
				break;
			case 4:// 3
				clear_LCD();
				KEY_Flag = 0;
				break;
			case 5:// 4
				clear_LCD();
				KEY_Flag = 0;
				break;
			case 6:// - op = 2
				clear_LCD();
                KEY_Flag = 0;
				break;
			case 7:// =
				clear_LCD();
                KEY_Flag = 0;
				break;
			case 8:// / op = 3
				clear_LCD();
                KEY_Flag = 0;
				break;
			case 9:// * op = 4
				clear_LCD();
                KEY_Flag = 0;
				break;
			default:
				break;
		}
		

  }
}
