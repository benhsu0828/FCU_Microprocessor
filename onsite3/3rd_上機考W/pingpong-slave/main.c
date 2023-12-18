//
// ADC_VR1 : using ADC7 to read Variable Resistor 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// VR1 : Variable Resistor on learning board
//
// ADC7/PA7 : NUC140VE3CN LQFP100 pin78

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Seven_Segment.h"


char val[17]="               ";
volatile uint8_t u8ADF;

char     Text[16];
uint8_t  comRbuf[256];
volatile uint8_t comRbytes = 0;
volatile uint8_t RX_Flag =0;

int8_t r=0, v=0, x=0 ,y=0;

unsigned char ball[8] = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

unsigned char racket[32] = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};


void UART02_IRQHandler(void)
{
	uint8_t c, i;
	uint32_t u32IntSts = UART0->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART0)) // check ISR on & RX is ready
  {
		while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART0); // read UART RX data
			if (c!='\n') {        // check line-end 
				PC12=0;
				comRbuf[comRbytes] = c;
				comRbytes++;
			} else {
								
				r = (comRbuf[0]-'0')*100+(comRbuf[1]-'0')*10+comRbuf[32]-'0';
				v = (comRbuf[3]-'0');
				x = (comRbuf[4]-'0')*100+(comRbuf[5]-'0')*10+comRbuf[6]-'0';
				y = (comRbuf[7]-'0')*10+comRbuf[8]-'0';
				
				comRbytes=0;
				RX_Flag=1;	                 
				break;
			}
		}		
	}
}

void Init_UART(void)
{
	UART_Open(UART0,115200);
	UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART02_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART02
}


int32_t main (void)
{
    uint32_t u32ADCvalue;
	
    SYS_Init();
		Init_UART();
		init_LCD();
		clear_LCD();
	
    u8ADF = 0;
	
		//x=56,y=24;
		
    while(1) {
					
			
			sprintf(Text, "%03d\n",(u32ADCvalue/43));
			UART_Write(UART0, Text, 4);
			
			draw_Bmp32x8(r,6,FG_COLOR,BG_COLOR,racket); // opp
			draw_Bmp32x8((u32ADCvalue/43),50,FG_COLOR,BG_COLOR,racket);	// my
			draw_Bmp8x8((128-x),(64-y),FG_COLOR,BG_COLOR,ball);
			CLK_SysTickDelay(100000);
			clear_LCD();
			/*
			draw_Bmp32x8(r,50,BG_COLOR,BG_COLOR,racket); //opp
			draw_Bmp32x8((u32ADCvalue/43),6,BG_COLOR,BG_COLOR,racket);	//my
			draw_Bmp8x8(x,y,BG_COLOR,BG_COLOR,ball);
			*/
			
    }
}

