/*
 * display.c
 *
 * Created: 25-9-2012 11:50:20
 *  Author: Koen Beckers
 */ 

#define DISPLAY_C

#include <avr/pgmspace.h>
#include "main.h"
#include "display.h"

uint8_t row = 0;

void SPI_MasterInit(void){
	// Set MOSI and SCK output, all others input
	DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}




void SPI_MasterTransmit(uint8_t cData){
	//Start transmission
	//printf("SENDING: %i\r\n",cData);
	SPDR = cData;
	//Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}


void refreshDisplay(){
	if(row == 9){
		row = 0;
	}
	for(int display = (MAX_DISPLAY-1); display >= 0; display--){
		SPI_MasterTransmit(displays[display][row]);
	}
	ROWPORT = 0;
	LATCHROW();
	ENABLEROW(row);
	
	row++;
}

void doDisplay(uint16_t number){
	for(int display = (MAX_DISPLAY-1); display >= 0; display--){
		
		for(uint8_t t = 0; t <= 8; t++){
			displays[display][t] = pgm_read_byte(&font[number % 10][t]);
		}
		number = number / 10;
	}
}

void showClock(uint8_t left, uint8_t right){
	uint8_t l1 = left / 10;
	uint8_t l2 = left % 10;
	uint8_t r1 = right / 10;
	uint8_t r2 = right % 10;
	
	for(uint8_t t = 0; t<=8; t++){
		//First display will allways show first char:
		displays[0][t] = pgm_read_byte(&font[l1][t]);
		//Then, a space, so the next char we should move by 1..
		displays[1][t] = (pgm_read_byte(&font[l2][t])) << 1;
		//Then, the other should have the last row
		displays[2][t] = (pgm_read_byte(&font[l2][t])) >> 4;
		if(CHECKFLAG(FLAG_SYNCED) && CHECKFLAG(FLAG_ERROR)){
			displays[2][t] |= (pgm_read_byte(&font[12][t]));
		}else if(CHECKFLAG(FLAG_SYNCED) && !CHECKFLAG(FLAG_ERROR)){
			displays[2][t] |= (pgm_read_byte(&font[11][t]));
		}else if(!CHECKFLAG(FLAG_SYNCED) && CHECKFLAG(FLAG_ERROR)){
			displays[2][t] |= (pgm_read_byte(&font[13][t]));		
		}else{
			displays[2][t] |= (pgm_read_byte(&font[10][t]));
		}
		displays[2][t] |= (pgm_read_byte(&font[r1][t]) << 4);
		displays[3][t] = (pgm_read_byte(&font[r1][t]) >> 1);
		displays[4][t] = (pgm_read_byte(&font[r2][t]));
	}
}