//
// LCD_Bmp_Keypad: use 3x3 keypad to move bitmap on LCD
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Draw2D.h"


unsigned char bmp32x8[4*8] = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

unsigned char boundary16x48[2*48] = {
	0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF
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

void gameOver(int gameFlag){
	clear_LCD();
	if(gameFlag == 0){//O win
		print_Line(0,"O WIN");
	}else if(gameFlag == 1){//X win
		print_Line(0,"X WIN");
	
	}else if (gameFlag == 2){//tie game
		print_Line(0,"Game over");
		
	}

}

int main(void)
{
	int16_t x=0,y=0;
	int8_t keyin=0;
	int i = 0,j=0;
	int keypadFlag = 0;
	int cheak = 0;
	int OX = 1;//O=0 X=1 
	int	race[9] = {2,2,2,2,2,2,2,2,2};
	int gameFlag = -1;
	int gameCount = 0;

	SYS_Init();
	  
	init_LCD();
	clear_LCD();
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	
	OpenKeyPad();
	
  	x=0; y=0;	
	clear_LCD();
	
	for(i = 1;i<3;i++){
		draw_Line(42*i,0,42*i,64,FG_COLOR,BG_COLOR);
	}
	for(i=1;i<3;i++){
		draw_Line(0,20*i,128,20*i,FG_COLOR,BG_COLOR);
	}

	
	while(1) {
		fill_Rectangle(x+1,y+4,x+41,y+18,FG_COLOR,BG_COLOR);
		CLK_SysTickDelay(500000);                      // Delay for Vision
		fill_Rectangle(x+1,y+4,x+41,y+18,BG_COLOR,BG_COLOR);
		for(i=0;i<9;i++){
			if(race[i]==0){
				draw_Circle((i*42)+21, (i*20)+10,5,FG_COLOR,BG_COLOR);
			}else if(race[i]==1){
				draw_Line(x,y,x+38,y+16,FG_COLOR,BG_COLOR);
				draw_Line(x,y+16,x+38,y,FG_COLOR,BG_COLOR);
			}
		}
		
    keyin=ScanKey(); // Scan 3x3 keypad
		if(keypadFlag!=keyin){
			keypadFlag = keyin;
			if(keyin!=0){
				switch(keyin){
					case 2:
						y-=20;
						if(y<0) y=0;
						break;
					case 4:
						x-=42;
						if(x<0) x=0;
						break;
					case 5:
						cheak = 1;
						OX = (OX+1)%2;
						break;
					case 6:
						x+=42;
						if(x>128) x=84;
						break;
					case 8:
						y+=20;
						if(y>40) y=44;
						break;
				}
			}
		}
		if(cheak == 1){
			cheak = 0;
			//fill_Rectangle(x+1,y+4,x+41,y+18,BG_COLOR,BG_COLOR);
			if(race[x/42+(y/20)*3] != 2){//keep OX in last state
				OX = (OX+1)%2;
			}else{
				race[x/42+(y/20)*3] = OX%2;
				gameCount++;
				if(race[0]==race[1]&&race[1]==race[2]&&race[0]!=2）{
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[0]==race[3]&&race[3]==race[6]&&race[0]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[1]==race[4]&&race[4]==race[7]&&race[1]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[2]==race[5]&&race[5]==race[8]&&race[2]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[3]==race[4]&&race[4]==race[5]&&race[3]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[6]==race[7]&&race[7]==race[8]&&race[6]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[0]==race[4]&&race[4]==race[8]&&race[0]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}else if(race[2]==race[4]&&race[4]==race[6]&&race[2]!=2){
					gameFlag = OX%2;
					gameOver(gameFlag);
				}
				if(gameCount == 9&& gameFlag == -1){
					gameFlag = 2;
					gameOver(gameFlag);
				}
		}
		}
		if (gameFlag != -1){//to stuck the game result
			keyin = ScanKey();
			while(keyin == 0) keyin = ScanKey();
			for(i = 1;i<3;i++){
				draw_Line(42*i,0,42*i,64,FG_COLOR,BG_COLOR);
			}
			for(i=1;i<3;i++){
				draw_Line(0,20*i,128,20*i,FG_COLOR,BG_COLOR);
			}
			//init to start new game
			keypadFlag = 0;
			cheak = 0;
			OX = 1;//O=0 X=1 
			for(i = 0;i<8;i++){
				race[i]	= 2;
			}
			gameFlag = -1;
			gameCount = 0;
		}
		
	}		
}
