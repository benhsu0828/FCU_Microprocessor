//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
//
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

#define Do 2000
#define Re 1700
#define Mi 1500
#define Fa 1432
#define So 1280
#define La 1136
#define Si 1000

// display an integer on four 7-segment LEDs
void Display_7seg(int value[])
{
  uint8_t digit;
	digit = value[0];
	CloseSevenSegment();
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
			
	digit = value[1];
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);

	digit = value[2];
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);

	digit = value[3];
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}

void Buzz(int number)	//1/2400  0.002 
{
	int i;
	if(number == 1){
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Do*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Do*0.8);	 // Delay 
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Re*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Re*0.8);	 // Delay 
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Mi*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Mi*0.8);	 // Delay 
		}
	}else if(number == 2){
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Mi*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Mi*0.8);	 // Delay 
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Re*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Re*0.8);	 // Delay 
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Do*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Do*0.8);	 // Delay 
		}
	}else if(number == 3){
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Do*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Do*0.8);	 // Delay		
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Do*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Do*0.8);	 // Delay		
		}
		CLK_SysTickDelay(50000);
		for (i=0; i<500; i++) {
			PB11=0; // PB11 = 0 to turn on Buzzer
			CLK_SysTickDelay(Do*0.2);	 // Delay
			PB11=1; // PB11 = 1 to turn off Buzzer	
			CLK_SysTickDelay(Do*0.8);	 // Delay		
		}
	}
}

void push(int arr[],int tmp){
	arr[1] = arr[2];
	arr[2] = arr[3];
	arr[3] = tmp;
}

void backspace(int arr[]){
	arr[3] = arr[2];
	arr[2] = arr[1];
	arr[1] = 0;
}

void play(int value[])
{
  uint8_t digit;
	while(value[3] !=0){
		if(value[1]!=0){
			value[0] = value[1];
			value[1] = 0;
		}else if(value[2]!=0){
			value[0] = value[2];
			value[2] = 0;
		}else if(value[3]!=0){
			value[0] = value[3];
			value[3] = 0;
		}
		digit = value[0];
		CloseSevenSegment();
		ShowSevenSegment(3,digit);
		Buzz(digit);
	}
	value[0] = 0;
}

int main(void)
{
		int input[4] = {0, 0, 0, 0};
		int k = 0;
		int key_temp = 0;
		int i=0;
	
    SYS_Init();
    OpenSevenSegment(); // for 7-segment
	  OpenKeyPad();       // for keypad
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); // for Buzzer
	
 	  while(1) {
		  k=ScanKey();
			if(k!=key_temp){
				if(k == 4){
					for(i = 0;i<4;i++){
						input[i] = 0;
					}
				}else if(k == 5){ //play
					play(input);
				}else if(k == 6){
					backspace(input);
				}else if(k!=0&&k!=7&&k!=8&&k!=9){
					push(input, k);
					Display_7seg(input);
				}
				
				key_temp = k;
			}
			Display_7seg(input);
	  }
			
}
