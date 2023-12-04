//
// ADC_PWM : read VR1 (ADC7) and set PWM0 to dim blue-LED (GPA12)
//
// Connections of Nu-LB-NUC140 (learning board)
// PA7 /ADC7 is connected to VR1
// PA12/PWM0 is connected to RGB-LED (blue)

#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "LCD.h"
#include "Note_Freq.h"

#define  P125ms 125000
#define  P250ms 250000
#define  P500ms 500000
#define  P1S   1000000

volatile uint8_t KEY_Flag;
volatile uint32_t index_key_scan;
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;


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

void Init_ALL(void)
{
    SYS_Init(); 
    init_LCD();
    clear_LCD();
		OpenKeyPad();
    //Buzzer
    //GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); 
		//TIMER1
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE,200); // PERIODIC 200HZ = 5ms
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);

    //keypad interrupt
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 |BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 2, GPIO_INT_LOW);		
    NVIC_EnableIRQ(GPAB_IRQn);   
    NVIC_SetPriority(GPAB_IRQn,3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));	
}


int32_t main (void)
{
    Init_ALL();
		clear_LCD();
		
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);	
		  
	
    while(1) {
			
		switch(KEY_Flag){
			case 1:// 1
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, C4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = C4");
				break;
			case 2:// 2
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, D4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = D4");
				break;
			case 3:// 3
        KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, E4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = E4");
				break;
			case 4:// 3
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, F4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = F4");
				break;
			case 5:// 4
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, G4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = G4");
				break;
			case 6:// 
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, A4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = A4");
				break;
			case 7:// 
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, B4, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = B4");
				break;
			case 8:// 
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, C5, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = C5");
				break;
			case 9:// 
				KEY_Flag = 0;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, D5, 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				print_Line(1, "MUSIC = D5");
				break;
			default:
				PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
				//print_Line(1, "           ");
				break;
		}
		CLK_SysTickDelay(P500ms);
  }
}
