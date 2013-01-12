/*
 * main.h
 *
 * Created: 11-9-2012 11:02:13
 *  Author: Koen Beckers
 */ 

#include <avr/io.h>

#ifdef MAIN_C
#define PROTO
#else
#define PROTO extern
#endif


#define MAX_DISPLAY	5

#define DCFPIN		PC3
#define DCFREADPIN	PINC
#define DCFDDR		DDRC

/* DEBUG */
#define DEBUGLED	PB2
#define DEBUG_AAN()	(PORTB |= _BV(DEBUGLED))
#define DEBUG_UIT()	(PORTB &= ~_BV(DEBUGLED))

/* ROW */
#define ROWPORT		PORTD
#define ROW1		PD0

#define LATCHPORT	PORTB
#define LATCHPIN	PB0
#define LATCHAAN()	LATCHPORT |= _BV(LATCHPIN);
#define LATCHUIT()	LATCHPORT &= ~_BV(LATCHPIN);
#define LATCHROW()	LATCHAAN();LATCHUIT()

#define DD_MOSI		PB3		
#define DD_SCK		PB5	
#define DDR_SPI		DDRB	


#define ENABLEROW(row)	(ROWPORT = (1 << row))


// FLAGS					  76543210
#define FLAG_READTEMP			0b00000001
#define FLAG_REFRESHDISPLAY		0b00000010
#define FLAG_ERROR				0b00000100
#define FLAG_HANDLEBIT			0b00001000
#define FLAG_SYNCED				0b00010000
#define FLAG_HANDLECLOCK		0b00100000
#define FLAG_STARTUP			0b01000000

#define CHECKFLAG(flag) (flags & flag)
#define SETFLAG(flag)	(flags |= flag)
#define RESETFLAG(flag)	(flags &= ~flag)

typedef struct
{
	uint8_t hours;
	uint8_t minutes;
} time;

//Variables
PROTO uint8_t flags;

PROTO uint8_t pulseCounter;
PROTO uint8_t offCounter;
PROTO uint8_t onCounter;
PROTO uint16_t mSecCounter;
PROTO uint8_t secondCounter;


PROTO time currentTime;
PROTO time dcfTime;




//extern void handleDCF(uint8_t bit, uint8_t second);
extern void handleBit();

