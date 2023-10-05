//
// GPIO_Keypad : 3x3 keypad input and control LEDs (or Relays)
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
 
// PA0,1,2,3,4,5 connected to 3x3 Keypad
// PC12,13,14,15 connected to LEDs (or Relays)
 
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "Seven_Segment.h"
 
void Init_GPIO(void)
{
	  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	  PC12=1; PC13=1; PC14=1; PC15=1;
}
 
void Display_7seg(uint16_t value)
{
  uint8_t digit;
	digit = value / 1000;
	CloseSevenSegment();
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
 
	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);
 
	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);
 
	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}
 
int main(void)
{
	uint32_t i = 0;
	uint32_t num = 0;
	int flag = 0;
	int tmp = 0;
	int key_temp = 0;
	SYS_Init();
	OpenKeyPad();
	Init_GPIO();
	OpenSevenSegment();
 
 	while(1) 
  {
 
		i=ScanKey();
		if (key_temp != i) {
				if(i != 0){
					if(i == 7){
						tmp = num/1000;
						num = num -(num/1000)*1000;
						num *= 10;
						num += tmp;
					}
					else if(i == 8){
						num = 0;
						flag = 0;
					}
					else if(i == 9){
						tmp = num%10;
						num /= 10;
						num = num + tmp*1000;
					}else{
						if(num >= 1000){
						num = num -(num/1000)*1000;
					}
						if(num%10 == 0){
							num += i;
						}
						else{
							num *= 10;
							num += i;
							flag++;
						}
					}
			}
				key_temp = i;
		}
 
		Display_7seg(num);
	}
 
}
 
