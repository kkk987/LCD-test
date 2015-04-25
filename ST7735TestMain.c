// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ST7735.h"
#include "PLL.h"
#include "inc/tm4c123gh6pm.h"
#include "Switch.h"

#define N_key 0    //no key
#define S_key_SW1 1    //single key
#define D_key_SW1 2    //double key
#define L_key_SW1 3    //long key
#define S_key_SW2 4    //single key
#define D_key_SW2 5    //double key
#define L_key_SW2 6    //long key
#define S_key_SW12 7    //single key
#define D_key_SW12 8    //double key
#define L_key_SW12 9    //long key

#define key_state_0 0
#define key_state_1 1
#define key_state_2 2
#define key_state_3 3
#define key_state_4 4
#define key_state_5 5
#define key_state_6 6
#define key_state_7 7
#define key_state_8 8
#define key_state_9 9

void digit_mode(int press);
void analog_mode(void);
void operation_mode(int press, int op_selc);
void journey_mode();
void DelayWait10ms(uint32_t n);
void reverse(char s[]);
void itoa(int n, char s[]);
unsigned char key_driver(uint32_t status);
unsigned char key_read(uint32_t status);

int display = 1;
int inc = 0;
int dec = 0;
int speed = 0;
int acc = 0;
int g = 0;
int mile = 0;
int dia = 600;
int mass = 800;
int ini = 1;
int mode = 0;
int mode_1 = 1;
int mode_2 = 1;
int mode_3 = 1;
int mode_4 = 1;
char c1[] = "Digit mode";
  char c2[] = "Analog mode";
  char c3[] = "Operation mode";
  char c4[] = "Journey log mode";
  char t1[] = "Speed: ";
  char t2[] = "Acceleration: ";
  char t3[] = "Lateral g force: ";
  char t4[] = "Odometer: ";
  char t5[] = "Wheel diameter: ";
  char t6[] = "Vehicle mass: ";
  char u1[] = "km/h";
  char u2[] = "m/s2";
  char u3[] = "g";
  char u4[] = "km";
  char u5[] = "mm";
  char u6[] = "kg";
  char *modeptr1, *modeptr2, *modeptr3, *modeptr4;
  char *textptr1, *textptr2, *textptr3, *textptr4, *textptr5, *textptr6;
  char *uintptr1, *uintptr2, *uintptr3, *uintptr4, *uintptr5, *uintptr6;
  char *dataptr1, *dataptr2, *dataptr3, *dataptr4, *dataptr5, *dataptr6;
  char data1[5] = {0};
  char data2[5] = {0};
  char data3[5] = {0};
  char data4[5] = {0};
  char data5[5] = {0};
  char data6[5] = {0};
// test image
// [blue] [green]
// [red ] [white]
const uint16_t Test[] = {
  0x001F, 0xFFFF, 0xF800, 0x07E0
};
// test image2
// [red]   [yellow]  [green]   [blue]    [black]
// [red]   [yellow]  [green]   [blue]    [white]
// [red]   [yellow]  [green]   [blue]    [black]
// [red]   [yellow]  [green]   [blue]    [white]
// [red]   [yellow]  [green]   [blue]    [black]
// [black] [75%grey] [50%grey] [25%grey] [white]
const uint16_t Test2[] = {
  0x0000, 0x4208, 0x8410, 0xC618, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000
};




int main(void){
  uint32_t status;

  int press = 0;
  int bk = 0;
  int op_selc = 0;

  unsigned char key;
  modeptr1 = c1;
  modeptr2 = c2;
  modeptr3 = c3;
  modeptr4 = c4;
  textptr1 = t1;
  textptr2 = t2;
  textptr3 = t3;
  textptr4 = t4;
  textptr5 = t5;
  textptr6 = t6;
  uintptr1 = u1;
  uintptr2 = u2;
  uintptr3 = u3;
  uintptr4 = u4;
  uintptr5 = u5;
  uintptr6 = u6;
  itoa(speed, data1);
  itoa(acc, data2);
  itoa(g, data3);
  itoa(mile, data4);
  itoa(dia, data5);
  itoa(mass, data6);
  dataptr1 = data1;
  dataptr2 = data2;
  dataptr3 = data3;
  dataptr4 = data4;
  dataptr5 = data5;
  dataptr6 = data6;
  //int speed_temp, acc_temp, g_temp, mile_temp, dia_temp, mass_temp;


  PLL_Init();
  Board_Init();
  ST7735_InitR(INITR_REDTAB);
  while(1){
	    status = Board_Input();

	    key = key_read(status);
	    switch(key) {
	    case S_key_SW1:
	    				if(press == 0) {
	    					mode++;//select mode from 0 to 4
	    					//ST7735_FillScreen(0xAAAA);
	    					if(mode > 4) {
	    						mode = 0;
	    					}
	    		        }else {
	    		        	op_selc++;
	    		        	if(op_selc > 2) {
	    		        		op_selc = 1;
	    		        	}
	    		        }
	    				break;
	    case D_key_SW1:
	    				if(press == 0) {
	    					mode--;
	    					if(mode < 0) {
	    						mode = 4;
	    					}
	    		    	}
	    				break;
	    case L_key_SW1:
	    				break;
	    case S_key_SW2:
	    				if(press == 0){
	    					press = 1;
	    		    	}else if(press == 1) {
	    		    		inc = 1;
	    		    	}
	    	    		break;
	    case D_key_SW2:
	    				dec = 1;
	    	    		break;
	    case L_key_SW2:
	    	ST7735_FillScreen(0xAAAA);
	    		    				bk = 1;
	    	    		break;
	    case S_key_SW12:
	    	ST7735_FillScreen(0xAAAA);
	    				bk = 1;
	    	    		break;
	    case D_key_SW12:
	    	    		break;
	    case L_key_SW12:
	    	ST7735_FillScreen(0xAAAA);
	    		    				bk = 1;
	    	    		break;
	    default: break;
	    }
	    if(ini && mode == 0) {
	    	ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
	    	ST7735_DrawString(0, 2, modeptr2, 0xFFFF);
	    	ST7735_DrawString(0, 4, modeptr3, 0xFFFF);
	    	ST7735_DrawString(0, 6, modeptr4, 0xFFFF);
	    	ini--;
	    	mode_1 = 1;
	    	mode_2 = 1;
	    	mode_3 = 1;
	    	mode_4 = 1;
	    }
	    if(mode == 1){
	    	digit_mode(press);
	    }else if(mode == 2){
	    	analog_mode();
	    }else if(mode == 3){
	    	operation_mode(press, op_selc);
	    }else if(mode == 4){
	    	journey_mode();
	    }
	    if(bk) {
	    	display = 1;
	    	ini = 1;
	    	mode = 0;
	    	press = 0;
	    	bk = 0;
	    	Output_Clear();
	    }


	  	/*switch(mode) {
	  	case 1:
				if(press) {
					if(i) {
						Output_Clear();
						i--;
					}
					if(j) {
						ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
						ST7735_DrawString(0, 2, textptr1, 0xFFFF);
						ST7735_DrawString(0, 3, dataptr1, 0xFFFF);
						ST7735_DrawString(8, 3, uintptr1, 0xFFFF);
						ST7735_DrawString(0, 4, textptr2, 0xFFFF);
						ST7735_DrawString(0, 5, dataptr2, 0xFFFF);
						ST7735_DrawString(8, 5, uintptr2, 0xFFFF);
						ST7735_DrawString(0, 6, textptr3, 0xFFFF);
						ST7735_DrawString(0, 7, dataptr3, 0xFFFF);
						ST7735_DrawString(8, 7, uintptr3, 0xFFFF);
						ST7735_DrawString(0, 8, textptr4, 0xFFFF);
						ST7735_DrawString(0, 9, dataptr4, 0xFFFF);
						ST7735_DrawString(8, 9, uintptr4, 0xFFFF);
						j--;
					}

					speed_temp = speed;
					acc_temp = acc;
					g_temp = g;
					mile_temp = mile;

					speed++;
					acc++;
					g++;
					mile++;

					if(speed_temp != speed) {
						itoa(speed, data1);
						ST7735_DrawString(0, 3, dataptr1, 0xFFFF);
					}
					if(acc_temp != acc) {
						itoa(acc, data2);
						ST7735_DrawString(0, 5, dataptr2, 0xFFFF);
					}
					if(g_temp != g) {
						itoa(g, data3);
						ST7735_DrawString(0, 7, dataptr3, 0xFFFF);
					}
					if(mile_temp != mile) {
						itoa(mile, data4);
						ST7735_DrawString(0, 9, dataptr4, 0xFFFF);
					}
					if(bk) {
						i = 1;
						j = 1;
						mode = 0;
						press = 0;
						mode_status = 0;
						bk = 0;
						Output_Clear();
						break;
					}
				}else {
					ST7735_DrawString(0, 0, modeptr1, 0x001F);
					ST7735_DrawString(0, 6, modeptr4, 0xFFFF);
					if(clr) {
						  Output_Clear();//prevent corrupted case
						  i = 1;
						  j = 1;
						  mode = 0;
						  press = 0;
						  mode_status = 0;
						  bk = 0;
						  clr = 0;
					}
				}
				break;
	  	case 2:
	  			ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
				ST7735_DrawString(0, 2, modeptr2, 0x001F);
				break;
	  	case 3:
	  			if(press) {
	  				if(i) {
	  					Output_Clear();
	  					i--;
	  				}
	  				if(j) {
	  					ST7735_DrawString(0, 4, modeptr3, 0xFFFF);
	  					ST7735_DrawString(0, 2, textptr5, 0xFFFF);
	  					ST7735_DrawString(0, 3, dataptr5, 0xFFFF);
	  					ST7735_DrawString(8, 3, uintptr5, 0xFFFF);
	  					ST7735_DrawString(0, 4, textptr6, 0xFFFF);
	  					ST7735_DrawString(0, 5, dataptr6, 0xFFFF);
	  					ST7735_DrawString(8, 5, uintptr6, 0xFFFF);
	  					j--;
	  				}

	  				if(op_selc == 1) {
	  					ST7735_DrawString(0, 3, dataptr5, 0x001F);
	  					ST7735_DrawString(0, 5, dataptr6, 0xFFFF);
	  					if(inc) {
	  						if((inc*20 + dia) <= 800) {
	  							dia+=20;
	  							itoa(dia, data5);
	  							inc = 0;
	  						}
	  					}
	  					if(dec) {
	  						if((dia - dec*20) >= 600) {
	  							dia-=20;
	  							itoa(dia, data5);
	  							dec = 0;
	  						}
	  					}
	  				}else if(op_selc == 2) {
	  					ST7735_DrawString(0, 3, dataptr5, 0xFFFF);
	  					ST7735_DrawString(0, 5, dataptr6, 0x001F);
	  					if(inc) {
	  						if((inc*100 + mass) <= 2000) {
	  							mass+=100;
	  						  	itoa(mass, data6);
	  						  	inc = 0;
	  						}
	  					}
	  					if(dec) {
	  						if((mass - dec*100) >= 800) {
	  							mass-=100;
	  							itoa(mass, data6);
	  							dec = 0;
	  						}
	  					}
	  				}
	  				if(bk) {
	  					i = 1;
	  					j = 1;
	  					mode = 0;
	  					press = 0;
	  					mode_status = 0;
	  					bk = 0;
	  					Output_Clear();
	  					break;
	  				}
	  			}else {
	  				ST7735_DrawString(0, 2, modeptr2, 0xFFFF);
	  				ST7735_DrawString(0, 4, modeptr3, 0x001F);
	  				if(clr) {
	  					Output_Clear();//prevent corrupted case
	  					i = 1;
	  					j = 1;
	  					mode = 0;
	  					press = 0;
	  					mode_status = 0;
	  					bk = 0;
	  				    clr = 0;
	  				}
	  			}

				break;
	  	case 4:
	  			ST7735_DrawString(0, 4, modeptr3, 0xFFFF);
				ST7735_DrawString(0, 6, modeptr4, 0x001F);
				break;
	  	default:

				ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
				ST7735_DrawString(0, 2, modeptr2, 0xFFFF);
				ST7735_DrawString(0, 4, modeptr3, 0xFFFF);
				ST7735_DrawString(0, 6, modeptr4, 0xFFFF);
				break;
	  	}*/
	  	DelayWait10ms(1);
  }

}

void digit_mode(int press) {
	int speed_temp, acc_temp, g_temp, mile_temp;
	if(press) {
		if(display) {
			Output_Clear();
			ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
			ST7735_DrawString(0, 2, textptr1, 0xFFFF);
			ST7735_DrawString(0, 3, dataptr1, 0xFFFF);
			ST7735_DrawString(8, 3, uintptr1, 0xFFFF);
			ST7735_DrawString(0, 4, textptr2, 0xFFFF);
			ST7735_DrawString(0, 5, dataptr2, 0xFFFF);
			ST7735_DrawString(8, 5, uintptr2, 0xFFFF);
			ST7735_DrawString(0, 6, textptr3, 0xFFFF);
			ST7735_DrawString(0, 7, dataptr3, 0xFFFF);
			ST7735_DrawString(8, 7, uintptr3, 0xFFFF);
			ST7735_DrawString(0, 8, textptr4, 0xFFFF);
			ST7735_DrawString(0, 9, dataptr4, 0xFFFF);
			ST7735_DrawString(8, 9, uintptr4, 0xFFFF);
			display--;
		}
		speed_temp = speed;
		acc_temp = acc;
		g_temp = g;
		mile_temp = mile;
		speed++;
		acc++;
		g++;
		mile++;

		if(speed_temp != speed) {
			itoa(speed, data1);
			ST7735_DrawString(0, 3, dataptr1, 0xFFFF);
		}
		if(acc_temp != acc) {
			itoa(acc, data2);
			ST7735_DrawString(0, 5, dataptr2, 0xFFFF);
		}
		if(g_temp != g) {
			itoa(g, data3);
			ST7735_DrawString(0, 7, dataptr3, 0xFFFF);
		}
		if(mile_temp != mile) {
			itoa(mile, data4);
			ST7735_DrawString(0, 9, dataptr4, 0xFFFF);
		}
		}else{
			if(mode_1) {
				ST7735_DrawString(0, 0, modeptr1, 0x001F);
				ST7735_DrawString(0, 6, modeptr4, 0xFFFF);
				mode_1 = 0;
				ini = 1;
			}
		}
}

void analog_mode(void) {
	if(mode_2){
		ST7735_DrawString(0, 0, modeptr1, 0xFFFF);
		ST7735_DrawString(0, 2, modeptr2, 0x001F);
		mode_2 = 0;
		ini = 1;
	}
}

void operation_mode(int press, int op_selc) {
	if(press){
		if(display) {
			Output_Clear();
			ST7735_DrawString(0, 0, modeptr3, 0xFFFF);
			ST7735_DrawString(0, 2, textptr5, 0xFFFF);
			ST7735_DrawString(0, 3, dataptr5, 0xFFFF);
			ST7735_DrawString(8, 3, uintptr5, 0xFFFF);
			ST7735_DrawString(0, 4, textptr6, 0xFFFF);
			ST7735_DrawString(0, 5, dataptr6, 0xFFFF);
			ST7735_DrawString(8, 5, uintptr6, 0xFFFF);
			display--;
		}
		if(op_selc == 1) {
			  ST7735_DrawString(0, 3, dataptr5, 0x001F);
			  ST7735_DrawString(0, 5, dataptr6, 0xFFFF);
			  if(inc) {
			  		if((inc*20 + dia) <= 800) {
			  			dia+=20;
			  			itoa(dia, data5);
			  			inc = 0;
			  		}
			  }
			  if(dec) {
			  		if((dia - dec*20) >= 600) {
			  			dia-=20;
			  			itoa(dia, data5);
			  			dec = 0;
			  		}
			  }
		}else if(op_selc == 2) {
			  ST7735_DrawString(0, 3, dataptr5, 0xFFFF);
			  ST7735_DrawString(0, 5, dataptr6, 0x001F);
			  if(inc) {
				  if((inc*100 + mass) <= 2000) {
					  mass+=100;
					  itoa(mass, data6);
					  inc = 0;
				  }
			  }
			  if(dec) {
			  		if((mass - dec*100) >= 800) {
			  			mass-=100;
			  			itoa(mass, data6);
			  			dec = 0;
			  		}
			  }
		}

	}else{
		if(mode_3){
			ST7735_DrawString(0, 2, modeptr2, 0xFFFF);
			ST7735_DrawString(0, 4, modeptr3, 0x001F);
			mode_3 = 0;
			ini = 1;
		}
	}
}

void journey_mode(void){
	if(mode_4){
		ST7735_DrawString(0, 4, modeptr3, 0xFFFF);
		ST7735_DrawString(0, 6, modeptr4, 0x001F);
		mode_4 = 0;
		ini = 1;
	}
}

#define PF2   (*((volatile uint32_t *)0x40025010))

// Make PF2 an output, enable digital I/O, ensure alt. functions off
void SSR_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2
  GPIO_PORTF_PCTL_R &= ~0x00000F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x04;      // 4) disable analog function on PF2
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x04;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x04;         // 7) enable digital port
}


void Delay1ms(uint32_t n);


// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}



unsigned char key_driver(uint32_t status) {
	static unsigned char key_state = key_state_0, key_time = 0;
	unsigned char key_return = N_key;

	//key_press = Board_Input(); //read pin value 0x01 SW1, 0x10 SW2, 0x00 SW1&SW2, 0x11 NO KEY

	switch(key_state) {
	case key_state_0:
					if(status == 0x01) {
						key_state = key_state_1;
					}else if(status == 0x10) {
						key_state = key_state_4;
					}else if(status == 0x00) {
						key_state = key_state_7;
					}
					break;
	case key_state_1:
					if(status == 0x01) {
						key_time = 0;
						key_state = key_state_2;
					}else {
						key_state = key_state_0;
					}
					break;
	case key_state_2:
					if(status == 0x11) {
						key_return = S_key_SW1;
						key_state = key_state_0;
					}else if(++key_time >= 50) {
						key_return = L_key_SW1;
						key_state = key_state_3;
					}
					break;
	case key_state_3:
					key_state = key_state_0;
					key_return = N_key;
					break;
	case key_state_4:
					if(status == 0x10) {
						key_time = 0;
						key_state = key_state_5;
					}else {
						key_state = key_state_0;
					}
					break;
	case key_state_5:
					if(status == 0x11) {
						key_return = S_key_SW2;
						key_state = key_state_0;
					}else if(++key_time >= 50) {
						key_return = L_key_SW2;
						key_state = key_state_6;
					}
					break;
	case key_state_6:
					key_state = key_state_0;
					key_return = N_key;
					break;
	case key_state_7:
					if(status == 0x00) {
						key_time = 0;
						key_state = key_state_8;
					}else {
						key_state = key_state_0;
					}
					break;
	case key_state_8:
					if(status == 0x11) {
						key_return = S_key_SW12;
						key_state = key_state_0;
					}else if(++key_time >= 50) {
						key_return = L_key_SW12;
						key_state = key_state_9;
					}
					break;
	case key_state_9:
					key_state = key_state_0;
					key_return = N_key;
					break;

	}
	return key_return;
}

unsigned char key_read(uint32_t status) {
	static unsigned char key_m = key_state_0, key_time_1 = 0;
	unsigned char key_return = N_key, key_temp;

	key_temp = key_driver(status);

	switch(key_m) {
	case key_state_0:
					if(key_temp == S_key_SW1) {
						key_time_1 = 0;
						key_m = key_state_1;
					}else if(key_temp == S_key_SW2) {
						key_time_1 = 0;
						key_m = key_state_2;
					}else if(key_temp == S_key_SW12) {
						key_time_1 = 0;
						key_m = key_state_3;
					}else {
						key_return = key_temp;
					}
					break;
	case key_state_1:
					if(key_temp == S_key_SW1) {
						key_return = D_key_SW1;
						key_m = key_state_0;
					}else {
						if(++key_time_1 >= 25) {
							key_return = S_key_SW1;
							key_m = key_state_0;
						}
					}
					break;
	case key_state_2:
					if(key_temp == S_key_SW2) {
						key_return = D_key_SW2;
						key_m = key_state_0;
					}else {
						if(++key_time_1 >= 25) {
							key_return = S_key_SW2;
							key_m = key_state_0;
						}
					}
					break;
	case key_state_3:
					if(key_temp == S_key_SW12) {
						key_return = D_key_SW12;
						key_m = key_state_0;
					}else {
						if(++key_time_1 >= 25) {
							key_return = S_key_SW12;
							key_m = key_state_0;
						}
					}
						break;
	}
	return key_return;
}
