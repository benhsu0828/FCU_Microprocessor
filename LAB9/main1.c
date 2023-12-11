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
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;
volatile uint32_t count_Line = 0; 
volatile uint32_t count_Char;
volatile char RX_buffer[RXBUFSIZE];
volatile int digit[4] = {0,0,0,0};
volatile int countInput = 0;
volatile int guessFlag = 0;
volatile int guess[4] = {0,0,0,0};
volatile int password = 0;
char line0[16] = "               ";
char line1[16] = "               ";
char line2[16] = "               ";
char line3[16] = "               ";


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
void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 200);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
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

void Init_UART(void)
{
  UART_Open(UART0, 115200);                     // set UART0 baud rate
  UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART02_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART02
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

void judge(){
	int temp_input_pwd[4];
	int temp_rand_pwd[4];
	int a=0;
	int b=0;
	int i = 0;
	int j = 0;
	temp_input_pwd[0] = password/1000;
	temp_input_pwd[1] = password/100 -(password/1000)*10;
	temp_input_pwd[2] = (password%100-password%10)/10;
	temp_input_pwd[3] = password%10;
	
	for (i = 0; i < 4; i++) {
			if(temp_input_pwd[i] == guess[i]){//judge a
				a++;
			}else{//judge b
				for(j = 0; j<4 ; j++){
					if(temp_input_pwd[i] == guess[j]&&i!=j){
						b++;
					}
				}
			}
		}
	sprintf(line0,"%d %dA%dB ",(guess[3]*1000 + guess[2]*100 + guess[1]*10 + guess[0]),a,b);
	print_Line(count_Line,line0);
	count_Line++;
}

int get_rand_pwd(int loop_count) {
	int rand_pwd=0, i;
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
	int temp4 = 0;
	srand(loop_count);
	temp1 = rand() % 9 + 1;
	temp2 = rand() % 9 + 1;
	temp3 = rand() % 9 + 1;
	temp4 = rand() % 9 + 1;
	while(temp2==temp1){
		temp2 = rand() % 9 + 1;
	}
	while(temp3==temp2||temp3==temp1){
		temp3 = rand() % 9 + 1;
	}
	while(temp4==temp3||temp4==temp2||temp4==temp1){
		temp4 = rand() % 9 + 1;
	}
	rand_pwd = temp1*1000 + temp2*100 + temp3*10 + temp4;
	return rand_pwd;
}


int main(void)
{
  uint8_t ASSCII;
	int i = 0;
	int tmp[4] = {0,0,0,0};
	SYS_Init();   // Intialize System/Peripheral clocks & multi-function I/Os
  Init_Timer1();
	Init_KEY();
	Init_UART();
  OpenSevenSegment();
	init_LCD();
	clear_LCD();
  Init_UART();
	//LCD back light
	PD14 = 0;
	//set password
	password = get_rand_pwd(10);
	digit[0] = password/1000;
	digit[1] = password/100 -(password/1000)*10;
	digit[2] = (password%100-password%10)/10;
	digit[3] = password%10;
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
		if(guessFlag == 1){
			guessFlag = 0;
			judge();
			KEY_Flag = 0;
			guess[3] = 0;
			guess[2] = 0;
			guess[1] = 0;
			guess[0] = 0;
			tmp[3] = tmp[2] = tmp[1] = tmp[0] = 0;
		}

  }
}
