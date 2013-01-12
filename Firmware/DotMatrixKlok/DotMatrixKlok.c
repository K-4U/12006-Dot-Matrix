/*
 * DotMatrixKlok.c
 *
 * Created: 11-9-2012 11:00:43
 *  Author: Koen Beckers
 */
#define MAIN_C
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "main.h"
#include "ds1820.h"
#include "display.h"

uint16_t temperature = 0;
uint16_t tCounter = 0;

int main(void){
	//PORT mapping
	//On PORTD, we have the matrix the lines.
	//On PORTC, we have inputs, 2 buttons and the DCF and Time sensors
	//On PortB we have control over the shift registers and the DEBUG led!
	//		 76543210
	DDRC = 0b00000110;
	DDRD = 0b11111111;
	DDRB = 0b11111111;
	PORTC = 0b00111001;
	PORTD = 0;
	PORTB = 0;
	
	//Initialize Interrupt for timer0
	TCNT0 = 0xFC;
	TCCR0A = (1 << WGM01);
	TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00);
	//TIMSK0 |= _BV(TOIE0);
	
	//Initialize Interrupt for timer1
	TCCR1A |= (0 << COM1A1) | (0 << COM1A0);
	TCCR1B |= (1 << WGM12) | (1 << CS11); // Set CTC and prescaler :8
	OCR1A = 0x61A7;
	TIMSK1 |= (1 << OCIE1A);
	sei();

	DEBUG_UIT();
	SPI_MasterInit();
	
	//SETFLAG(FLAG_STARTUP);
	
	uint16_t refreshCounter = 0;
	uint16_t ttCounter = 0;
	uint8_t tempCounter = 0;
	doDisplay(0);
    while(1) {
		if(CHECKFLAG(FLAG_REFRESHDISPLAY)){
			RESETFLAG(FLAG_REFRESHDISPLAY);
			refreshDisplay();
		}
		
		if(CHECKFLAG(FLAG_HANDLEBIT)){
			RESETFLAG(FLAG_HANDLEBIT);
			//DEBUG_AAN();
			//handleBit();
		}
		
		if(CHECKFLAG(FLAG_STARTUP)){
			//Do something nice..
			if(ttCounter == 4000){
				for(uint8_t display = 0;display <= MAX_DISPLAY; display++){
					for(uint8_t row = 0;row<=8;row++){
						if(display == (tempCounter / 5)){
							displays[display][row] = (3 << (tempCounter % 5));
						}else{
							displays[display][row] = 0;
						}
					}					
				}				
				tempCounter++;
				ttCounter=0;
				if(tempCounter == MAX_DISPLAY * 5){
					tempCounter=0;
				}
			}
			ttCounter++;
		}else{
		//if(secondCounter % 10 >= 8){
			/*if(CHECKFLAG(FLAG_READTEMP)){
				temperature = (uint16_t) ds1820_read();
				RESETFLAG(FLAG_READTEMP);
				//doDisplay(temperature);
			}else{*/
				showClock(12,34);
			//}			
		//}else{
			//showClock(currentTime.hours,currentTime.minutes);
		//}
		}
		
		if(refreshCounter == 20){
			SETFLAG(FLAG_REFRESHDISPLAY);
			refreshCounter = 0;
			//doDisplay(refreshCounter);
		}
		refreshCounter++;
		//doDisplay(refreshCounter);
    }
}

ISR(TIMER0_OVF_vect){
	//This gets called every 1.25 µS!
	refreshDisplay();
	DEBUG_AAN();
}	

ISR(TIMER1_COMPA_vect){  //Gets triggered every 10 msec!
	if(DCFREADPIN & (1 << DCFPIN)){
		onCounter++;
		DEBUG_AAN();
	}else{
		offCounter++;
		//DEBUG_UIT();
	}
	if(onCounter + offCounter >= 99){
		SETFLAG(FLAG_HANDLEBIT);
	}
	tCounter++;
	if(tCounter == 100){
		SETFLAG(FLAG_READTEMP);
		tCounter = 0;
	}
}