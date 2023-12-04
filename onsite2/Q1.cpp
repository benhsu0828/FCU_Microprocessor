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
volatile uint32_t index_key_scan;
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;
volatile int digit[4] = {0,0,0,0};
volatile int randSeed = 0;
volatile int sleepFlag = 0;
volatile int stateFlag = 0;
volatile int LCDFlag = 0;
volatile int buzzTime = 0;
volatile int totalPrice = 0;
char line0[17] = "                ";
char line1[17] = "                ";
char line2[17] = "                ";
char line3[17] = "                ";

void WDT_IRQHandler(void) //watch god
{ 
    WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
}

void feeddog(void){
    SYS_UnlockReg();
    WDT_RESET_COUNTER();
    SYS_LockReg();
}


void EINT1_IRQHandler(void)
{
		stateFlag = 2;
		clear_LCD();
		sprintf(line0,"               ");
		sprintf(line3,"               ");
		switch(LCDFlag){
				case 0: //WATER$15
					if(totalPrice >= 15){
						sprintf(line1,"Thank you!");
						sprintf(line2,"$%d Refound",totalPrice-15);
					}else{
						sprintf(line1,"Not enough $%d",15-totalPrice);
						sprintf(line2,"$%d Refound",totalPrice);
					}
					break;
				case 1://>MILK$50
					if(totalPrice >= 50){
						sprintf(line1,"Thank you!");
						sprintf(line2,"$%d Refound",totalPrice-50);
					}else{
						sprintf(line1,"Not enough $%d",50-totalPrice);
						sprintf(line2,"$%d Refound",totalPrice);
					}
					break;
				case 2://COLA$30
					if(totalPrice >= 30){
						sprintf(line1,"Thank you!");
						sprintf(line2,"$%d Refound",totalPrice-30);
					}else{
						sprintf(line1,"Not enough $%d",30-totalPrice);
						sprintf(line2,"$%d Refound",totalPrice);
					}
					break;
				case 3 ://JUICE$25
					if(totalPrice >= 25){
						sprintf(line1,"Thank you!");
						sprintf(line2,"$%d Refound",totalPrice-25);
					}else{
						sprintf(line1,"Not enough $%d",25-totalPrice);
						sprintf(line2,"$%d Refound",totalPrice);
					}
					break;
				case 4://TEA$10
					if(totalPrice >= 10){
						sprintf(line1,"Thank you!");
						sprintf(line2,"$%d Refound",totalPrice-10);
					}else{
						sprintf(line1,"Not enough $%d",10-totalPrice);
						sprintf(line2,"$%d Refound",totalPrice);
					}
					break;
				case 5://EXIT
					sprintf(line1,"Thank you!");
					sprintf(line2,"$%d Refound",totalPrice);
					break;
				default:
					break;
			}
		totalPrice = 0;
    GPIO_CLR_INT_FLAG(PB, BIT15);  // Clear GPIO interrupt flag
}

void TMR0_IRQHandler(void){
    if (cnt0_5ms % 20 == 0) 
	{
		if(buzzTime!=0){
			PB11 = 0;
			buzzTime--;
		}else{
			PB11 = 1;//turn off Buzzer
		}
		if(cnt0_100ms % 5 ==0 && stateFlag == 1){ //no light>
			switch(LCDFlag){
				case 0:
					sprintf(line0,"WATER        $15");
					break;
				case 1:
					sprintf(line1,"MILK         $50");
					break;
				case 2:
					sprintf(line2,"COLA         $30");
					break;
				case 3 :
					sprintf(line3,"JUICE        $25");
					break;
				case 4:
					sprintf(line3,"TEA          $10");
					break;
				case 5:
					sprintf(line3,"EXIT            ");
					break;
				default:
					break;
			}
		}else if(cnt0_100ms %5 ==2&& stateFlag == 1){ //light>
			switch(LCDFlag){
					case 0:
						sprintf(line0,">WATER       $15");
						sprintf(line1,"MILK         $50");
						sprintf(line2,"COLA         $30");
						sprintf(line3,"JUICE        $25");
						break;
					case 1:
						sprintf(line0,"WATER        $15");
						sprintf(line1,">MILK        $50");
						sprintf(line2,"COLA         $30");
						sprintf(line3,"JUICE        $25");
						break;
					case 2:
						sprintf(line0,"WATER        $15");
						sprintf(line1,"MILK         $50");
						sprintf(line2,">COLA        $30");
						sprintf(line3,"JUICE        $25");
						break;
					case 3 :
						sprintf(line0,"WATER        $15");
						sprintf(line1,"MILK         $50");
						sprintf(line2,"COLA         $30");
						sprintf(line3,">JUICE       $25");
						break;
					case 4:
						sprintf(line0,"MILK         $50");
						sprintf(line1,"COLA         $30");
						sprintf(line2,"JUICE        $25");
						sprintf(line3,">TEA         $10");
						break;
					case 5:
						sprintf(line0,"COLA         $30");
						sprintf(line1,"JUICE        $25");
						sprintf(line2,"TEA          $10");
						sprintf(line3,">EXIT           ");
						break;
					default:
						break;
				}
		}
		cnt0_100ms++;
    }
    if(cnt0_5ms % 200 == 0){
		cnt0_1s++;
	}
    cnt0_5ms++;
    TIMER_ClearIntFlag(TIMER0); // Clear Timer0 time-out interrupt flag
}

void TMR1_IRQHandler(void)
{// for keypad and seven segment
	if (cnt1_5ms % 20 == 0) 
	{
		cnt1_100ms++;
		index_key_scan = cnt1_100ms % 3;
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
		if(sleepFlag == 5 &&stateFlag == 2){
			stateFlag = 3;
		}
		if(stateFlag == 2)sleepFlag++;
		cnt1_1s++;
	}
    cnt1_5ms++;
	if(stateFlag==1){
		CloseSevenSegment();
    ShowSevenSegment(index1_5ms,digit[index1_5ms]);
	}
		
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

void sleep(){
    clear_LCD();
    sprintf(line1, "SLEEP" );
	print_Line(1 ,line1);
    sleepFlag = 0;
}
void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(100000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(100000);	 // Delay 
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
    // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
    SYS_UnlockReg();
    WDT_Open(WDT_TIMEOUT_2POW16, 0, FALSE, FALSE);
    WDT_EnableInt();          // Enable WDT timeout interrupt
    NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
    SYS_LockReg();
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
}

int32_t main (void)
{
    Init_ALL();
  while(1) {
		feeddog();
		if(stateFlag == 3){			
			clear_LCD();
			stateFlag = 0;
			LCDFlag = 0;
			totalPrice = 0;
		}
		if(stateFlag == 0){
			totalPrice = 0;
			CloseSevenSegment();
			sprintf(line1,"PLZ PUT IN COINS");
			print_Line(1,line1);
		}else if(stateFlag == 1){
			//calculate digit
			digit[3] = totalPrice/1000;
			digit[2] = totalPrice/100 - (totalPrice/1000)*10;
			digit[1] = totalPrice/10 - (totalPrice/100)*10;
			digit[0] = totalPrice%10;
			//show LCD
      print_Line(0, line0);
			print_Line(1, line1);
			print_Line(2, line2);
			print_Line(3, line3);
		}else if(stateFlag == 2){
			CloseSevenSegment();
			ShowSevenSegment(0,0);
			print_Line(0, line0);
			print_Line(1, line1);
			print_Line(2, line2);
			print_Line(3, line3);
		}
		
		switch(KEY_Flag){
			case 1:// add5
				clear_LCD();
				sleepFlag = 0;
				KEY_Flag = 0;
				stateFlag = 1;
				buzzTime = 1;
				totalPrice += 5;
				break;
			case 2: //LCD up
				if(stateFlag == 1){
					if(LCDFlag == 0){LCDFlag = 0;}
					else{LCDFlag--;}
				}
				KEY_Flag = 0;
				break;
			case 3: 
				KEY_Flag = 0;
				break;
			case 4:// 
				KEY_Flag = 0;
				break;
			case 5:// add10
				stateFlag = 1;
				sleepFlag = 0;
				KEY_Flag = 0;
				buzzTime = 5;
				totalPrice += 10;
				break;
			case 6:// 
				KEY_Flag = 0;
				break;
			case 7:// 
				KEY_Flag = 0;
				break;
			case 8:// LCD down
				if(stateFlag == 1){
					if(LCDFlag == 5){LCDFlag = 5;}
					else{LCDFlag++;}
				}
				KEY_Flag = 0;
				break;
			case 9:// add50
				clear_LCD();
				sleepFlag = 0;
				stateFlag = 1;
				buzzTime = 10;
				KEY_Flag = 0;
				totalPrice += 50;
				break;
			default:
				break;
		}
		
		
        
  }
		
		
       
}
