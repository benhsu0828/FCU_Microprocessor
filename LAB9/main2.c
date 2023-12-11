//
// I2C_MPU6050_Tilt : using I2C to read Accelerometer to calculate tilt angles
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
// 
// MPU6050 Connections
// SCL : to I2C1-SCL/PA11 (LQFP100 pin9 )
// SDA : to I2C1-SDA/PA10 (LQFP100 pin10)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "MPU6050.h"
#include "Seven_Segment.h"
#include "Draw2D.h"

#define ACCEL_SENSITIVITY 16384  // 32768/2 = 16384 LSB per g
#define GYRO_SENSITIVITY  16.384 // 32768/2000 = 16.384 LSB per degree
#define RAD_TO_DEG 180 / 3.1415926 // degree = radians * 180 / PI
#define ALPHA 0.98
volatile float dt = 0.01;
#define PI 3.14159265359
volatile uint32_t index0_5ms, cnt0_5ms, cnt0_1s, cnt0_100ms;
volatile int ballx = 64;
volatile	int bally = 32;


int32_t main()
{
		int16_t AccX, AccY, AccZ;
    float theta, psi, phi;
		
		PD14 = 0;
	
    SYS_Init();	
    I2C_Open(I2C1, I2C1_CLOCK_FREQUENCY);	
    Init_MPU6050();
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
		init_LCD();
    clear_LCD();

    while(1) {
			
      AccX = Read_MPU6050_AccX();
      AccY = Read_MPU6050_AccY();
      AccZ = Read_MPU6050_AccZ();		
      //printf("Accelerometer: %6d, %6d, %6d\n", AccX, AccY, AccZ);
			
      // calculate tilt (degree = radians *180 / PI)
      theta = atan(AccX / sqrt(pow(AccY,2)+pow(AccZ,2))) *180 /PI;		
      psi   = atan(AccY / sqrt(pow(AccX,2)+pow(AccZ,2))) *180 /PI;
      phi   = atan(sqrt(pow(AccX,2)+pow(AccY,2))/AccZ)   *180 /PI;
      //printf("theta=%d, psi=%d, phi=%d\n", (int) theta, (int) psi, (int) phi);
			if(theta<0){
				ballx+=3;
				if(ballx >= 120){
					ballx = 120;
				}
			}else{
				ballx-=3;
				if(ballx <= 8){
					ballx = 8;
				}
			}
			if(psi>0){
				bally+=3;
				if(bally >= 56){
					bally = 56;
				}
			}else{
				bally-=3;
				if(bally <= 8){
					bally = 8;
				}
			}
			clear_LCD();
			draw_Circle(ballx, bally, 8, 0XFF, 0x00);
			CLK_SysTickDelay(100000);                      // Delay for Vision
			//draw_Circle(ballx, bally, 8, 0X00, 0xFF);
			
    }

	
}
