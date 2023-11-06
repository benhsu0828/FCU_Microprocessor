//
// LCD_Bmp_Keypad: use 3x3 keypad to move bitmap on LCD
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"

/*
unsigned char bmp16x16[32] = {
	0x00,0x00,0x40,0xE0,0xE0,0xE0,0xE0,0x70,0x7C,0xFE,0xF8,0x80,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x03,0x7F,0x3E,0x1E,0x07,0x07,0x07,0x03,0x03,0x02,0x00
};
*/
unsigned char bmp16x8[16] = {
	0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
	0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF
};

unsigned char bmp8x16[16] = {
	0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0xFF
};

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(10000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(10000);	 // Delay 
	}
}

int main(void)
{
	int8_t x=0,y=0;
	int8_t keyin=0;
	int8_t movX=3, dirX=0;
	int8_t movY=3, dirY=0;
	int rectengleFlag = 0;
	int keypadFlag = 0;

	SYS_Init();
	  
	init_LCD();
	clear_LCD();
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	
	OpenKeyPad();
	
  x=64; y=32;	
	print_Line(0,"LCD Bmp Keypad");
	print_Line(2,"to move");
	print_Line(3,"press 2,4,5,6,8");
	draw_Bmp16x8(x,y,FG_COLOR,BG_COLOR,bmp16x8);
	
	while(keyin==0) keyin=ScanKey(); // wait till key is pressed

	clear_LCD();
	
	while(1) {
		if(rectengleFlag == 0){ // horizontal
			draw_Bmp16x8(x,y,FG_COLOR,BG_COLOR,bmp16x8); // Draw Object
			CLK_SysTickDelay(100000);                      // Delay for Vision
			draw_Bmp16x8(x,y,BG_COLOR,BG_COLOR,bmp16x8); // Erase Object
		}else if(rectengleFlag == 1){
			draw_Bmp8x16(x,y,FG_COLOR,BG_COLOR,bmp8x16); // Draw Object
			CLK_SysTickDelay(100000);                      // Delay for Vision
			draw_Bmp8x16(x,y,BG_COLOR,BG_COLOR,bmp8x16); // Erase Object
		}
    keyin=ScanKey(); // Scan 3x3 keypad
		if(keyin!=0) keypadFlag = 1;
		switch(keyin){   // input 1~9 to chang (x,y) direction
			case 2:
				if(keypadFlag == 1){
					dirX=0;  
					dirY=-1;
				}
				break;
			case 4:
				if(keypadFlag == 1){
					dirX=-1;
					dirY=0;
				}
				break;
			case 5: 
				if(keypadFlag == 1){
					rectengleFlag = (rectengleFlag+1)%2;
				}
				break;
			case 6: 
				if(keypadFlag == 1){
					dirX=+1; 
					dirY=0;
				}
				break;
			case 8: 
				if(keypadFlag == 1){
					dirX=0;  
					dirY=+1; 
				}
				break;		
			default: 
				if(keypadFlag == 1)keypadFlag = 0;
				break;
		}
		
		x=x + dirX * movX; // increment/decrement X
		y=y + dirY * movY; // increment/decrement Y
		
		if(rectengleFlag == 0){
			if(x<0||x>LCD_Xmax-16||y<3||y>LCD_Ymax-8){
				if(x<0) x = 0;
				if(x>LCD_Xmax-16) x = LCD_Xmax-16;
				if(y<3) y = 0;
				if(y>LCD_Ymax-8) y = LCD_Ymax-8;
				dirX*=-1;
				dirY*=-1;
				Buzz(1); // Buzz 2 times
			}
		}else if(rectengleFlag == 1){
			if(x<0||x>LCD_Xmax-8||y<2||y>LCD_Ymax-14){
				if(x<0) x = 0;
				if(x>LCD_Xmax-8) x = LCD_Xmax-8;
				if(y<3) y = 0;
				if(y>LCD_Ymax-14) y = LCD_Ymax-16;
				dirX*=-1;
				dirY*=-1;
				Buzz(1); // Buzz 2 times
			}
		}
	}
}
