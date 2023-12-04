//
// ADC_PWM : read VR1 (ADC7) and set PWM0 to dim blue-LED (GPA12)
//
// Connections of Nu-LB-NUC140 (learning board)
// PA7 /ADC7 is connected to VR1
// PA12/PWM0 is connected to RGB-LED (blue)

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "LCD.h"
#include "Note_Freq.h"

#define  P125ms 125000/2
#define  P250ms 250000/2
#define  P500ms 500000/2
#define  P1S   1000000/2

volatile uint8_t KEY_Flag;
volatile uint32_t index_key_scan;
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile uint32_t index1_5ms, cnt1_5ms, cnt1_1s, cnt1_100ms;
volatile uint8_t u8ADF;
volatile uint32_t u32ADCvalue;
volatile uint32_t duty;
char line[17] = "               ";

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT)
        u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}



void Init_ALL(void)
{
    SYS_Init(); 
    init_LCD();
    clear_LCD();
		//LCD back light
		PD14 = 0;
		//ADC
		ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
    //Buzzer
    //GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); 

}

void getVR(){
			ADC_START_CONV(ADC);
      while (u8ADF == 0);
      u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
			duty = u32ADCvalue*100/4096;
			//printf("ADC7 = %d, duty =%d\n", u32ADCvalue, duty);    
      ADC_DisableInt(ADC, ADC_ADF_INT);
}

int32_t main (void)
{
    
		
		uint8_t i;
		
  
		uint16_t music[72] = {
		E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
		C5 ,E5 ,A5 ,B5 ,B5 , 0 ,C5 ,A5 ,B5 ,C6 ,C6 , 0 ,
		E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
		C5 ,E5 ,A5 ,B5 ,B5 , 0 ,E5 ,C6 ,B5 ,A5 ,A5 , 0 ,
		B5 ,C6 ,D6 ,E6 ,E6 , 0 ,G5 ,F6 ,E6 ,D6 ,D6 , 0 ,
		F5 ,E6 ,D6 ,C6 ,C6 , 0 ,E5 ,D6 ,C6 ,B5 ,B5 , 0 };
		
		uint32_t pitch[72] = {
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
		P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,
		P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,
		P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms
		};

		Init_ALL();
		clear_LCD();
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);
		u8ADF = 0;		
		

	  
		PWM_ConfigOutputChannel(PWM1, PWM_CH0, C4, 90);
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
		  
	
    while(1) {
			getVR();
			sprintf(line,"DATA : %4d",u32ADCvalue);
			print_Line(1,line);
			if(u32ADCvalue<500){//no voice
				PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
			}else if(u32ADCvalue<=4000){
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, u32ADCvalue , 90);
				PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
			}else if(u32ADCvalue < 4096){
				for (i=0; i<72; i++) {
					getVR();
					sprintf(line,"DATA : %4d",u32ADCvalue);
					print_Line(1,line);
					if(u32ADCvalue < 4000) break;
					PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[i], 90); // 0=Buzzer ON
					if (music[i]!=0) PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
					else             PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
					CLK_SysTickDelay(P250ms);
				}
			}
  }
}
