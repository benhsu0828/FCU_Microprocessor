//on-site2
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include <math.h>

#define BUZZER_UPDATE_TICK 1
#define NID_UPDARE_TICK 500
#define LED_UPDATE_TICK 250
#define SEVEN_SEG_UPDATE_TICK 5
#define TICK_PER_MS 1000
#define SPACE_16 "                "
#define SPACE_12 "            "
#define NID "D1053020"


const uint8_t led_mode[11][4] = {
	{0, 1, 1, 1},
	{1, 0, 1, 1},
	{1, 1, 0, 1},
	{1, 1, 1, 0},
	{1, 1, 1, 0},
	{1, 1, 1, 1},
	{1, 1, 1, 0},
	{1, 1, 0, 1},
	{1, 0, 1, 1},
	{0, 1, 1, 1},
	{1, 1, 1, 1}
};

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);//BUZZ
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
}

void win_led(uint8_t led_index) {
	PC12 = led_mode[led_index][0];
	PC13 = led_mode[led_index][1];
	PC14 = led_mode[led_index][2];
	PC15 = led_mode[led_index][3];
}

void update_seven_seg(int seven_seg_buffer[], uint16_t rand_pwd) {
	seven_seg_buffer[3] = rand_pwd / 1000;
	seven_seg_buffer[2] = (rand_pwd % 1000) / 100;
	seven_seg_buffer[1] = (rand_pwd % 100) / 10;
	seven_seg_buffer[0] = rand_pwd % 10;
}

void show_seven_seg(int seven_seg_buffer[], int seven_seg_index) {
	if (seven_seg_index == 4) seven_seg_index = 0;
	while (seven_seg_buffer[seven_seg_index] == 16) {
		seven_seg_index++;
		if (seven_seg_index == 4) seven_seg_index = 0;
	}
	if (seven_seg_buffer[seven_seg_index] != 0) {
		CloseSevenSegment();
		ShowSevenSegment(seven_seg_index, seven_seg_buffer[seven_seg_index]);
	}
}

int get_ans(int input_pwd, int rand_pwd, int line_index, int win_line_index, char lcd_buffer[4][17],int *limit_buzzer) {
	uint8_t a=0, b=0;
	int i;
	int j;
	int temp_input_pwd[4];
	int temp_rand_pwd[4];
	temp_input_pwd[0] = input_pwd/1000;
	temp_input_pwd[1] = input_pwd/100 -(input_pwd/1000)*10;
	temp_input_pwd[2] = (input_pwd%100-input_pwd%10)/10;
	temp_input_pwd[3] = input_pwd%10;
	temp_rand_pwd[0] = rand_pwd/1000;
	temp_rand_pwd[1] = rand_pwd/100 -(rand_pwd/1000)*10;
	temp_rand_pwd[2] = (rand_pwd%100-rand_pwd%10)/10;
	temp_rand_pwd[3] = rand_pwd%10;
	//printf("temp_input:%d %d %d %d\ntemp_rand:%d %d %d %d",temp_input_pwd[0],temp_input_pwd[1],temp_input_pwd[2],temp_input_pwd[3],temp_rand_pwd[0],temp_rand_pwd[1],temp_rand_pwd[2],temp_rand_pwd[3]);
	if (input_pwd == rand_pwd && input_pwd != 0) {
		sprintf(lcd_buffer[line_index], "%4d     %-4s   ", input_pwd, "WIN");
		win_line_index = line_index;
	} else if (input_pwd == 0) {																				//NULL
		sprintf(lcd_buffer[line_index], "         %-4s   ", "NULL");
		if (win_line_index != 4) {
			lcd_buffer[win_line_index][14] = ' ';
		}
		win_line_index = 4;
		*limit_buzzer = 5;
	} else if (input_pwd != rand_pwd) {
		for (i = 0; i < 4; i++) {
			if(temp_input_pwd[i] == temp_rand_pwd[i]){//judge a
				a++;
			}else{//judge b
				for(j = 0; j<4 ; j++){
					if(temp_input_pwd[i] == temp_rand_pwd[j]&&i!=j){
						b++;
					}
				}
			}
		}
		sprintf(lcd_buffer[line_index], "%-4d     %dA%dB   ", input_pwd, a, b);
		if (win_line_index != 4) {
			lcd_buffer[win_line_index][14] = ' ';
		}
		win_line_index = 4;
	}
	return win_line_index;
}

void win_show_nid(uint8_t win_line_index, uint8_t nid_index, char lcd_buffer[4][17]) {
	lcd_buffer[win_line_index][13] = NID[nid_index];
}

void show_lcd(int lcd_x, int lcd_y, char lcd_buffer[4][17], char lcd_now[4][17]) {
	if (lcd_buffer[lcd_y][lcd_x] != lcd_now[lcd_y][lcd_x]) {
		lcd_now[lcd_y][lcd_x] = lcd_buffer[lcd_y][lcd_x];
		printC(lcd_x * 8, lcd_y* 16, lcd_now[lcd_y][lcd_x]);
	}
}

int get_rand_pwd(int loop_count) {
	int rand_pwd=0, i;
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
	int temp4 = 0;
	srand(loop_count);
	temp1 = rand() % 6 + 1;
	temp2 = rand() % 6 + 1;
	temp3 = rand() % 6 + 1;
	temp4 = rand() % 6 + 1;
	while(temp2==temp1){
		temp2 = rand() % 6 + 1;
	}
	while(temp3==temp2||temp3==temp1){
		temp3 = rand() % 6 + 1;
	}
	while(temp4==temp3||temp4==temp2||temp4==temp1){
		temp4 = rand() % 6 + 1;
	}
	rand_pwd = temp1*1000 + temp2*100 + temp3*10 + temp4;
	return rand_pwd;
}

void clear_lcd_buffer(char lcd_buffer[4][17]) {
	int i;
	for (i = 0; i < 4; i++) {
		sprintf(lcd_buffer[i], SPACE_16);
	}
}

int main(void) {
	uint8_t keyin = 0,
			isPressed = 1,
			input_pwd_index = 0,
			line_index = 0,
			seven_seg_index = 0,
			win_line_index = 4,
			led_index = 0,
			nid_index = 0,
			update_lcd = 0;
	int input_pwd = 0, rand_pwd = 0;
	char lcd_buffer[4][17] = {SPACE_16, SPACE_16, SPACE_16, SPACE_16};
	char lcd_now[4][17] = {SPACE_16, SPACE_16, SPACE_16, SPACE_16};
	int seven_seg_buffer[4] = {16, 16, 16, 16};
	int lcd_x = 0, lcd_y = 0;
	int loop_count = 0;
	int limit_buzzer = 0;
	int i = 0;
	int last_input_pwd = 0;
		
	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();


	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 1) goto display;
			isPressed = 1;
		}
		switch (keyin) {
		case 0:
			isPressed = 0;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			if (input_pwd_index < 4 && line_index < 4) {
				input_pwd = input_pwd * 10 + keyin;
				input_pwd_index++;
				if(input_pwd_index==1)sprintf(lcd_buffer[line_index], "%d               ", input_pwd); //, SPACE_12
				else if(input_pwd_index==2)sprintf(lcd_buffer[line_index], "%d              ", input_pwd); //, SPACE_12
				else if(input_pwd_index==3)sprintf(lcd_buffer[line_index], "%d             ", input_pwd); //, SPACE_12
				else if(input_pwd_index==4)sprintf(lcd_buffer[line_index], "%d            ", input_pwd); //, SPACE_12
				update_lcd = 1;
				last_input_pwd = input_pwd;
				if(line_index>0&&win_line_index != 4){
					win_line_index = 4;
					sprintf(lcd_buffer[line_index-1], "%4d     %-4s   ", rand_pwd, "WIN");
					PC12 = 1;
					PC13 = 1;
					PC14 = 1;
					PC15 = 1;
				}
			}
			break;
		case 7:
			rand_pwd = get_rand_pwd(loop_count);
			update_seven_seg(seven_seg_buffer, rand_pwd);
			break;
		case 8:
			clear_lcd_buffer(lcd_buffer);
			rand_pwd = 0;
			CloseSevenSegment();
			if(line_index>0&&win_line_index != 4){
					win_line_index = 4;
					//sprintf(lcd_buffer[line_index-1], "%s",SPACE_16);
					PC12 = 1;
					PC13 = 1;
					PC14 = 1;
					PC15 = 1;
				}
			update_lcd = 1;
			input_pwd_index = 0;
			input_pwd = 0;
			line_index = 0;
			break;
		case 9:
			
			if (line_index < 4) {
				win_line_index = get_ans(input_pwd, rand_pwd, line_index, win_line_index, lcd_buffer,&limit_buzzer);
				if (win_line_index == 4) {
					PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
					led_index = 0;
					/*if(line_index>0){
						sprintf(lcd_buffer[line_index-1], "%-4d            ",last_input_pwd);
					}*/
				}
				line_index++;
				input_pwd_index = 0;
				input_pwd = 0;
				update_lcd = 1;
			}
			break;
		default:
			break;
		}

		display:
		if (win_line_index != 4 && loop_count % LED_UPDATE_TICK == 0) {//win
			win_led(led_index);
			led_index++;
			if (led_index == 11) led_index = 0;
			if (loop_count % NID_UPDARE_TICK == 0) {
				win_show_nid(win_line_index, nid_index, lcd_buffer);
				update_lcd = 1;
				nid_index++;
				if (nid_index == 8) nid_index = 0;
			}
		}

		if (rand_pwd != 0 && loop_count % SEVEN_SEG_UPDATE_TICK == 0) {
			show_seven_seg(seven_seg_buffer, seven_seg_index);
			seven_seg_index++;
			if (seven_seg_index == 4) seven_seg_index = 0;
		}

		if (update_lcd == 1) {
			show_lcd(lcd_x, lcd_y, lcd_buffer, lcd_now);
			if (lcd_y < 4) {
				if (lcd_x < 16) {
					lcd_x++;
				} else {
					lcd_x = 0;
					lcd_y++;
				}
			}
			if (lcd_y == 4) {
				lcd_x = 0;
				lcd_y = 0;
				update_lcd = 0;
			}
		}
		if (limit_buzzer > 0 && loop_count % BUZZER_UPDATE_TICK == 0){
			PB11 = 0;
			CLK_SysTickDelay(TICK_PER_MS);
		}
		if (limit_buzzer > 0 && loop_count % BUZZER_UPDATE_TICK * 2 == 0) {
			PB11 = 1;
			limit_buzzer--;
		}

		if ((loop_count + 1) > INT32_MAX) loop_count = 0;
		loop_count++;
		CLK_SysTickDelay(TICK_PER_MS);
	}
}