//
// TMR_LED : change LED on/off by Timer1 interrupt
//
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "LCD.h"
#include "Note_Freq.h"

#define RXBUFSIZE 3
#define TXBUFSIZE 3
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

volatile uint8_t KEY_Flag;
volatile uint32_t index_key_scan;
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;
volatile uint32_t count_Line = 0; 
volatile uint32_t count_Char;
volatile uint8_t u8ADF;
volatile uint32_t u32ADCvalue;
volatile uint32_t duty;
volatile int ball_x = 64;
volatile int ball_y = 32;
volatile int ball_vx = 1;
volatile int ball_vy = 1;
volatile int ball_dirx = 1;
volatile int ball_diry = 1;
volatile int pad_x = 0;
volatile int pad_y = 56;
volatile int ypad_x = 112;
volatile int ypad_y = 0;
volatile char RX_buffer[RXBUFSIZE];//you pad_x
volatile char TX_buffer[TXBUFSIZE];//my pad_x[3],ball_x[3],ball_y[2],digit[4]
volatile int comRbytes = 0;
volatile int UARTRead[RXBUFSIZE];
volatile int digit[4] = {0,0,0,0};
volatile int resultFlag = 0;
volatile int forCount = 0;
volatile int touchFlag = 0;
char line0[16] = "               ";
char line1[16] = "               ";
char line2[16] = "               ";
char line3[16] = "               ";
unsigned char pad[32] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    };
unsigned char ball[8] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    };


void EINT1_IRQHandler(void)//PB15
{
    GPIO_CLR_INT_FLAG(PB, BIT15);  // Clear GPIO interrupt flag
}

void TMR0_IRQHandler(void){
    if (cnt0_5ms % 20 == 0) 
	{
    digit[2]++;
		if(digit[2] == 10){
				digit[2] = 0;
				digit[3]++;
     }
		if(digit[3] == 6){
				digit[3] = 0;
     }
		cnt0_100ms++;
    }
    if(cnt0_5ms % 200 == 0){
		cnt0_1s++;
	}
    cnt0_5ms++;
    TIMER_ClearIntFlag(TIMER0); // Clear Timer0 time-out interrupt flag
}

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


void UART02_IRQHandler(void)
{
	uint32_t u32IntSts= UART0->ISR; // UART interrupt status bit
	uint8_t c;
    if(u32IntSts & UART_IS_RX_READY(UART0)){
		UART_Read(UART0,RX_buffer,1);
        for(forCount = 0;forCount<RXBUFSIZE;forCount++){
            UARTRead[forCount] =RX_buffer[forCount] - 48;
        }
        ypad_x = UARTRead[0]*100+UARTRead[1]*10+UARTRead[2];
    }
}
/*
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
				RX_buffer[comRbytes] = c;
				comRbytes++;
			} else {				
				ypad_x = (RX_buffer[0]-'0')*100+(RX_buffer[1]-'0')*10+RX_buffer[2]-'0';	
				comRbytes=0;
				break;
			}
		}		
	}
	printf("ypad_x: %d\n",ypad_x);
}
*/
void ADC_IRQHandler(void)
{
    uint32_t u32Flag;
    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
    if(u32Flag & ADC_ADF_INT)u8ADF = 1;    
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void setUARTWrite(){
	sprintf(TX_buffer, "%03d\n",(u32ADCvalue/43));//(u32ADCvalue*96/4096)
	printf("TX_buffer: %s\n",TX_buffer);
}
void getVR(){
    ADC_START_CONV(ADC);
    //remove sleep just start ADC interrupt
    while (u8ADF == 0);
		u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
		if(u32ADCvalue*96/4096 != pad_x){
			pad_x = u32ADCvalue*96/4096;//turn 0~4096 to 0~100
			//CLK_SysTickDelay(100000);
			//UART write
			setUARTWrite();
			UART_Write(UART0, TX_buffer, 1);
		}
    //printf("ADC7 = %d, duty =%d\n", u32ADCvalue, duty);    
    ADC_DisableInt(ADC, ADC_ADF_INT);
}

void Init_ALL(void)
{
    SYS_Init(); 
    init_LCD();
    clear_LCD();
    OpenSevenSegment();
		OpenKeyPad();
    u8ADF = 0;
    //Buzzer
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); 
    //LED
	GPIO_SetMode(PC, (BIT12|BIT13|BIT14|BIT15), GPIO_MODE_OUTPUT);
    PC12=PC13=PC14=PC15=1;// turn off LED
    // // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
    // SYS_UnlockReg();
    // WDT_Open(WDT_TIMEOUT_2POW16, 0, FALSE, FALSE);
    // WDT_EnableInt();          // Enable WDT timeout interrupt
    // NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
    // SYS_LockReg();
    //GPIO RGBLED
    GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
    PA12=1;// PA12 = Blue,  0 : on, 1 : off
    PA13=1;// PA13 = Green, 0 : on, 1 : off
    PA14=1;// PA14 = Red,   0 : on, 1 : off
    //EXT
    // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
    NVIC_EnableIRQ(EINT1_IRQn);
    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
    //TIMER0
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
    //TIMER1
    TIMER_Open(TIMER1, TMR1_OPERATING_MODE,200); // PERIODIC 200HZ = 5ms
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
    //keypad interrupt
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 |BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 2, GPIO_INT_LOW);		
    NVIC_EnableIRQ(GPAB_IRQn);   
    NVIC_SetPriority(GPAB_IRQn,3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));	
    //ADC
		ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
    // PWMA_Ch0 = 50Hz, 50% duty cycle
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);
    //UART
    UART_Open(UART0, 115200);                     // set UART0 baud rate
    UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
    NVIC_EnableIRQ(UART02_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART02
}

void judgeup(){
    if(ball_x > ypad_x && ball_x < ypad_x + 32){
				touchFlag = 1;
        digit[1]++;
        if(digit[1] == 10){
            digit[1] = 0;
        }
				PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
        PWM_ConfigOutputChannel(PWM1, PWM_CH0, 523, 20);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    }
}
void judgedown(){
   if(ball_x > pad_x && ball_x < pad_x + 32){
				touchFlag = 1;
        digit[0]++;
        if(digit[0] == 10){
            digit[0] = 0;
        }
				PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
        PWM_ConfigOutputChannel(PWM1, PWM_CH0, 1046, 20);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    }
}


int main(void)
{
	int i = 0;
	int dir;
	Init_ALL();
		srand(3020);
		dir = rand() % 4;
		if(dir == 0){
			ball_dirx = 1;
			ball_diry = 1;
		}else if(dir == 1){
			ball_dirx = -1;
			ball_diry = 1;
		}else if(dir == 2){
			ball_dirx = 1;
			ball_diry = -1;
		}else if(dir == 3){
			ball_dirx = -1;
			ball_diry = -1;
		}
		
	 while(1) {
        if(resultFlag == 0){
            if (cnt0_5ms % 20 == 0) 
            {
								if(touchFlag == 0){
									if(cnt0_100ms % 2 == 0)PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
								}else if(touchFlag == 1){
									PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
									touchFlag = 0;
								}
								getVR();
                //pinpon move
                ball_x += ball_vx * ball_dirx;
                ball_y += ball_vy * ball_diry;
                if (ball_x > 120) {
                    ball_x = 120;
                    ball_dirx = -1;
                    PWM_ConfigOutputChannel(PWM1, PWM_CH0, 784, 20);
										PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
                }
                if (ball_x < 0) {
                    ball_x = 0;
                    ball_dirx = 1;
                    PWM_ConfigOutputChannel(PWM1, PWM_CH0, 784, 20);
										PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
                }
                if (ball_y > 56) {
									judgedown();
                    ball_y = 56;
                    ball_diry = -1;
										if(touchFlag == 0){
											PWM_ConfigOutputChannel(PWM1, PWM_CH0, 784, 20);
											PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
										}
                }
                if (ball_y < 0) {
										judgeup();
                    ball_y = 0;
                    ball_diry = 1;
                    if(touchFlag == 0){
											PWM_ConfigOutputChannel(PWM1, PWM_CH0, 784, 20);
											PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
										}
										
                }

                //draw LCD
                clear_LCD();
                //draw ball
								draw_Bmp8x8(ball_x, ball_y,FG_COLOR,BG_COLOR , ball);
								//draw pad
								draw_Bmp32x8(pad_x,pad_y ,FG_COLOR,BG_COLOR , pad);
								draw_Bmp32x8(96 - ypad_x,ypad_y ,FG_COLOR,BG_COLOR , pad);
                
            }
            
            switch(KEY_Flag){
                case 1:// 1
                    KEY_Flag = 0;
                    break;
                case 2:// 2 speed up
                    ball_vx++;
                    ball_vy++;
                    KEY_Flag = 0;
                    break;
                case 3:
                    KEY_Flag = 0;
                    break;
                case 4:// 
                    KEY_Flag = 0;
										ball_vx=4;
										ball_vy=4;
                    break;
                case 5:// 
                    KEY_Flag = 0;
										ball_vx=0;
										ball_vy=0;
                    break;
                case 6:// 
                    KEY_Flag = 0;
                    break;
                case 7:// =
                    clear_LCD();
                    KEY_Flag = 0;
                    break;
                case 8:// speed down
                    ball_vx/=2;
                    ball_vy/=2;
                    KEY_Flag = 0;
                    break;
                case 9:// 
                    KEY_Flag = 0;
                    break;
                default:
                    break;
            }
        }    
  }
}
