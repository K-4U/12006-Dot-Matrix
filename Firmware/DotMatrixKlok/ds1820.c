#ifndef ONE_WIRE_C
#define ONE_WIRE_C

#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "onewire.h"

#define DQ_PIN	PC0
#define DQ_READPIN PINC
#define DQ_PORT	PORTC
#define DQ_DDR	DDRC

/*
#define OW_PIN  PC0
#define OW_IN   PINC
#define OW_OUT  PORTC
#define OW_DDR  DDRC
#define OW_CONF_DELAYOFFSET 0

#define OW_GET_IN()   ( OW_IN & (1<<OW_PIN))
#define OW_OUT_LOW()  ( OW_OUT &= (~(1 << OW_PIN)) )
#define OW_OUT_HIGH() ( OW_OUT |= (1 << OW_PIN) )
#define OW_DIR_IN()   ( OW_DDR &= (~(1 << OW_PIN )) )
#define OW_DIR_OUT()  ( OW_DDR |= (1 << OW_PIN) )
*/

/*
 * onewire_reset()
 * Description: Initiates the one wire bus.
 */
// OK if just using a single permanently connected device
/*uint8_t onewire_reset() {
	uint8_t err;
	
	OW_OUT_LOW();
	OW_DIR_OUT();
	_delay_us(480);
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// set Pin as input - wait for clients to pull low
		OW_DIR_IN(); // input
		OW_OUT_HIGH();
			
		_delay_us(64);       // was 66
		err = OW_GET_IN();   // no presence detect
		// if err!=0: nobody pulled to low, still high
	}
	_delay_us(480 - 64);       // was 480-66
	if( OW_GET_IN() == 0 ) {
		err = 1;             // short circuit, expected low but got high
	}
	
	return err;
	/*
	DQ_DDR |= (1 << DQ_PIN); //OUTPUT
    DQ_PORT &= ~(1 << DQ_PIN);	// pull the bus low for reset
    _delay_us(50);
    DQ_DDR &= ~(1 << DQ_PIN);			// float the bus high (Input)
	DQ_PORT |= (1 << DQ_PIN);
    _delay_us(50);                // wait-out remaining initialisation window
    DQ_DDR &= ~(1 << DQ_PIN);
	DQ_PORT |= (1 << DQ_PIN);
}*/


/*
 * onewire_write(int8 data)
 * Arguments: a byte of data.
 * Description: writes a byte of data to the device.
 */
/*
void onewire_write(unsigned char data)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		unsigned char count;
		unsigned char data_bit;

		for(count = 0; count < 8; ++count)
		{
			data_bit = data & 0x01;
			OW_DIR_OUT();
			//DQ_DDR |= (1 << DQ_PIN);
			OW_OUT_LOW();
			//DQ_PORT &= ~(1 << DQ_PIN);
			_delay_us(2);                // pull 1-wire low to initiate write time-slot.
			DQ_PORT |= (data_bit << DQ_PIN);
			data=data>>1;
			_delay_us(60);               // wait until end of write slot.
			OW_DIR_IN();
			OW_OUT_HIGH();
			//DQ_DDR &= ~(1 << DQ_PIN);				  // set 1-wire high again,
			_delay_us(2);                // for more than 1us minimum.
		}
	}	
}*/

/*
 * onewire_read()
 * Description: reads and returns a byte of data from the device.
 */
/*
unsigned char onewire_read()
{
	unsigned char count, data, bit_in;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		
    
		data = 0;

		for(count = 0; count < 8; ++count)
		{
			OW_DIR_OUT();
			//DQ_DDR |= (1 << DQ_PIN);
			OW_OUT_HIGH();
			//DQ_PORT |= (1 << DQ_PIN);
			_delay_us(2);
			OW_DIR_IN();
			//DQ_DDR &= ~(1 << DQ_PIN);				  // now let 1-wire float high,
			OW_OUT_LOW();
			//DQ_PORT |= (1 << DQ_PIN);
			_delay_us(8);                // let device state stabilise,
        
			bit_in = (DQ_READPIN & (1 << DQ_PIN));
			data = data >> 1;        	  //-- Shift previous bits across
			if(bit_in == 1)               //-- If data was high then add High bit to data
				data +=0x80;
		
			_delay_us(50);              // wait until end of read slot.
		}
	}	
    return data;
} 
*/

/*
 * Reads temp from the DS18B20
 */
double ds1820_read(void)
{
	uint8_t busy=0, temp1, temp2;
    int16_t temp3;
    double result;

    ow_reset();
	ow_byte_wr(0xCC); 
	ow_byte_wr(0x44);
	
	while(busy == 0){
		busy = ow_byte_rd();
	}
	
	ow_reset();
	ow_byte_wr(0xCC);
	ow_byte_wr(0xBE);
	
	temp1 = ow_byte_rd();
	temp2 = ow_byte_rd();
	
	
    /*onewire_write(0xCC);            //Skip ROM, address all devices
    onewire_write(0x44);            //Start temperature conversion

    while(busy == 0)                //Wait while busy (bus is low)
        busy = onewire_read();

    onewire_reset();
    onewire_write(0xCC);            //Skip ROM, address all devices
    onewire_write(0xBE);            //Read scratchpad
    temp1 = onewire_read();
    temp2 = onewire_read();*/
    
    temp3 = temp2;
    temp3 = temp3 << 8;
    temp3 = temp3 | temp1;
 
    //result = (float) temp3 / 2.0;   //Calculation for DS18S20 with 0.5 deg C resolution
    result = (double) temp3 / 16.0;    //Calculation for DS18B20
 
    //_delay_ms(200);
    return(result);
}	

#endif /*ONE_WIRE_C*/