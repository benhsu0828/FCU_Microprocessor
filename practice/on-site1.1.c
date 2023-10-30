//
// LCD_keypad : 3x3 keypad input and display on LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "Seven_Segment.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"

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

int main(void)
{
	/*
	char plus[16] = "                ";
	char minus[16] = "                ";
	char dot[16] = "                ";
	char div[16] = "                ";
	*/
	char line0[16] = "               ";
	char line1[16] = "               ";
	char line2[16] = "               ";
	char line3[16] = "               ";
	int input = 0;
	int check = 0;
	int segArr[2] ={0,0};
	int lcdFlag = 0;
	int lcdState = 0;//0:+-* 1:-*/ 2:*/%
	int i =0;
	int temp = 0;

	SYS_Init();
	
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	OpenSevenSegment();
	OpenKeyPad();
	//Init LCD
	init_LCD();
	clear_LCD();
	srand(count);
	
	
	while(1)
	{
		
		
		input = ScanKey();
		
		if(input!=check){
			if(input!=0){
				switch (input)
				{
				case 1:
					/* code */
					break;
				case 2:
					lcdState-—;
					break;
				case 3:
					/* code */
					break;
				case 4:
					
					temp =segArr[0]/10;
					segArr[0] = (segArr[0]%10)*10+segArr[1]/10;
					segArr[1] = (segArr[1]%10)*10+temp;
					break;
				case 5:
						count ++;
						segArr[0] = rand()%9+1;
						segArr[0] = (segArr[0])*10+rand()%9+1;
						segArr[1] = rand()%9+1;
						segArr[1] = (segArr[1])*10+rand()%9+1;
						sprintf(line0, "D1053020");
					break;
				case 6:
					temp =segArr[0]/10;
					segArr[0] = (segArr[0]%10)*10+segArr[1]/10;
					segArr[1] = (segArr[1]%10)*10+temp;
					break;
				case 7:
					break;
				case 8:
					lcdState++
					break;
				case 9:
					lcdFlag = 1;
					break;
				
				default:
					break;
				}
				
				
				}
			check = input;
		}
			Display_7seg(segArr);
			if(lcdFlag == 1){
			clear_LCD();
			print_Line(0, line0);
			print_Line(1, line1);
			print_Line(2, line2);
			print_Line(3, line3);
			lcdFlag = 0;
		}	
	}
	
}

/*
		sprintf(Text, "%d&&%d = %d", 1,0,0);
		sprintf(Text+10, "Hello");
		printf("%s",Text);
		print_Line(1, Text);
		printS(0, 32, "Hello");
		printS(80, 32, "World");
		printC(0, 48, 'H');
		printC(3*8, 48, 'i');
		CLK_SysTickDelay(5000); 	   // delay 
*/
