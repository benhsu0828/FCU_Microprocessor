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
	int segArr[4] ={0,0,0,0};
	int segint;
	int lcdFlag = 0;
	int check = 0;
	int count = 0;
	int randomNum = 0;
	int Lcdint = 0;
	int lineFlag = 0;
	int i =0;
	uint8_t keyin;
	
	SYS_Init();
	
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	OpenSevenSegment();
	OpenKeyPad();
	//Init LCD
	init_LCD();
	clear_LCD();
	
	
	while(1)
	{
		count ++;
		srand(count);
		input = ScanKey();
		
		if(input!=check){
			if(input!=0){
				if(input == 7){//creat a random num
					randomNum = rand() %1000;
					segArr[0] = (randomNum/1000)%6+1;
					segArr[1] = ((randomNum - (randomNum/1000)*1000)/100)%6+1;
					segArr[2] = ((randomNum - ((randomNum - (randomNum/1000)*1000)/100)*100)/10)%6+1;
					segArr[3] = (randomNum%10)%6+1;
					segint = segArr[0]*1000 + segArr[1]*100 + segArr[2]*10 + segArr[3];
				}else if(input == 8){//clean
					clear_LCD();
					sprintf(line0, " ");
					sprintf(line1, " ");
					sprintf(line2, " ");
					sprintf(line3, " ");
					Lcdint = 0;
					lineFlag = 0;
				}else if(input == 9){//open
					if(Lcdint == segint){//PASS
								switch(lineFlag){
									case 0:
										sprintf(line0, "%4d     PASS", Lcdint);
										break;
									case 1:
										sprintf(line1, "%4d     PASS", Lcdint);
										break;
									case 2:
										sprintf(line2, "%4d     PASS", Lcdint);
										break;
									case 3:
										sprintf(line3, "%4d     PASS", Lcdint);
										break;
								}
								for( i = 0; i<2;i++){
									PC12 =0; // turn on LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC12 =1; // turn off LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC13 =0; // turn on LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC13 =1; // turn off LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC14 =0; // turn on LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC14 =1; // turn off LED
									CLK_SysTickDelay(100000);	 // Delay
									PC15 =0; // turn on LED
									CLK_SysTickDelay(100000);	 // Delay 
									PC15 =1; // turn off LED
									CLK_SysTickDelay(100000);	 // Delay									

								}
						}else if(Lcdint == 0){
							switch(lineFlag){
									case 0:
										sprintf(line0, "          NULL");
										break;
									case 1:
										sprintf(line1, "          NULL");
										break;
									case 2:
										sprintf(line2, "          NULL");
										break;
									case 3:
										sprintf(line3, "          NULL");
										break;
								}
						}else{//error
							switch(lineFlag){ 
									case 0:
										sprintf(line0, "%4d     ERROR", Lcdint);
										break;
									case 1:
										sprintf(line1, "%4d     ERROR", Lcdint);
										break;
									case 2:
										sprintf(line2, "%4d     ERROR", Lcdint);
										break;
									case 3:
										sprintf(line3, "%4d     ERROR", Lcdint);
										break;
								}
							Buzz(2);
						}
						if(lineFlag<4){
							lineFlag++;
						}
						lcdFlag = 1;//allow show LCD
						Lcdint = 0;
				}else{//input1~6
					if(Lcdint < 1000){//input num
						Lcdint *= 10;
						Lcdint += input;
						//set LCD
						switch(lineFlag){
								case 0:
									sprintf(line0, "%d", Lcdint);
									break;
								case 1:
									sprintf(line1, "%d", Lcdint);
									break;
								case 2:
									sprintf(line2, "%d", Lcdint);
									break;
								case 3:
									sprintf(line3, "%d", Lcdint);
									break;
							}
					}
					lcdFlag = 1;//allow show LCD
				}
				}
			check = input;
		}
		if(lcdFlag == 1){
			clear_LCD();
			print_Line(0, line0);
			print_Line(1, line1);
			print_Line(2, line2);
			print_Line(3, line3);
			lcdFlag = 0;
		}			
		
			Display_7seg(segArr);
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
