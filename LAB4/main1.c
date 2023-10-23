//
// LCD_keypad : 3x3 keypad input and display on LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
#include "string.h"
#include <stdio.h>
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

int main(void)
{
	char plus[16] = "                ";
	char minus[16] = "                ";
	char dot[16] = "                ";
	char div[16] = "                ";
	int input = 0;
	int segArr[4] ={0,0,0,0};
	int segFlag = 2;
	int check = 0;
	uint8_t keyin;
	
	SYS_Init();
	
	OpenSevenSegment();
	OpenKeyPad();
	//Init LCD
	init_LCD();
	clear_LCD();
	
	//print_Line(0, "LCD_Keypad");        // print title
	
	while(1)
	{
		Display_7seg(segArr);
		input = ScanKey();
		if(input!=check){
			if(input!=0){
				if(segFlag == 0||segFlag == 2){
					segArr[0] = input;
					segFlag = 1;
				}else if(segFlag == 1){
					segArr[3] = input;
					segFlag = 0;
				}
			}
			check = input;
		}
		if(segFlag == 0){
			clear_LCD();
			sprintf(plus, "%d + %d = %d", segArr[0],segArr[3],segArr[0]+segArr[3]);
			sprintf(minus, "%d - %d = %d", segArr[0],segArr[3],segArr[0]-segArr[3]);
			sprintf(dot, "%d X %d = %d", segArr[0],segArr[3],segArr[0]*segArr[3]);
			sprintf(div, "%d / %d = %d", segArr[0],segArr[3],segArr[0]/segArr[3]);
			print_Line(0, plus);
			print_Line(1, minus);
			print_Line(2, dot);
			print_Line(3, div);
			segFlag = 2;
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
