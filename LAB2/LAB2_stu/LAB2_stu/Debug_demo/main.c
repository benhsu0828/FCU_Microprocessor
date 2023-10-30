#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
	
	int time = 0;
	SYS_Init();
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	
	while(1){
		PC12 = PC13 = PC14 = PC15 = 1;
		
		if(time == 0){
			PC15 = 0;
			printf("PC15 Light!\n");
		}
		else if(time == 1){
			PC14 = 0;
			printf("PC14 Light!\n");
		}
		else if(time == 2){
			PC13 = 0;
			printf("PC13 Light!\n");
		}
		else if(time == 3){
			PC12 = 0;
			printf("PC12 Light!\n");
		}
		
		CLK_SysTickDelay(330000);	 // Delay
		time = time + 1;
		if(time == 4) time = 0;
	}
}
