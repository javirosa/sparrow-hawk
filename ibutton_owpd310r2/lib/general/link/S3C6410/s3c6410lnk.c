//---------------------------------------------------------------------------
// Copyright (C) 2001 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
//  TODO.C - Link Layer functions required by general 1-Wire drive
//           implimentation.  Fill in the platform specific code.
//
//  Version: 3.00
//
//  History: 1.00 -> 1.01  Added function msDelay.
//           1.02 -> 1.03  Added function msGettick.
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.10 -> 3.00  Added owReadBitPower and owWriteBytePower
//

#include "ownet.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/w1-gpio.h>

#include <linux/delay.h>

#include <asm/gpio.h>

//typedef unsigned int PIN;

// exportable link-level functions
SMALLINT owTouchReset(int);
SMALLINT owTouchBit(int,SMALLINT);
SMALLINT owTouchByte(int,SMALLINT);
SMALLINT owWriteByte(int,SMALLINT);
SMALLINT owReadByte(int);
SMALLINT owSpeed(int,SMALLINT);
SMALLINT owLevel(int,SMALLINT);
SMALLINT owProgramPulse(int);
void msDelay(int);
long msGettick(void);


static SMALLINT SystemTick = 0;

//index: 1-Wire Port No.
//value: GPIO PIN No.
static unsigned int IOTable[MAX_PORTNUM];

//--------------------------------------------------------------------------
// 1-Wire Master Timing (APN126)
// Recommended (by us)

#define STANDARD_TIMING_A   6
#define STANDARD_TIMING_B   64
#define STANDARD_TIMING_C   60
#define STANDARD_TIMING_D   10
#define STANDARD_TIMING_E   9
#define STANDARD_TIMING_F   55
#define STANDARD_TIMING_G   0
#define STANDARD_TIMING_H   480
#define STANDARD_TIMING_I   70
#define STANDARD_TIMING_J   410

#define OVERDRIVE_TIMING_A   1.0
#define OVERDRIVE_TIMING_B   7.5
#define OVERDRIVE_TIMING_C   7.5
#define OVERDRIVE_TIMING_D   2.5
#define OVERDRIVE_TIMING_E   1.0
#define OVERDRIVE_TIMING_F   7
#define OVERDRIVE_TIMING_G   2.5
#define OVERDRIVE_TIMING_H   70
#define OVERDRIVE_TIMING_I   8.5
#define OVERDRIVE_TIMING_J   40

// Ticks for standard drive ( in 1/4 us )
#define STANDARD_TICKS_A       	((int)(STANDARD_TIMING_A * 4))
#define STANDARD_TICKS_B       	((int)(STANDARD_TIMING_B * 4))
#define STANDARD_TICKS_C       	((int)(STANDARD_TIMING_C * 4))
#define STANDARD_TICKS_D       	((int)(STANDARD_TIMING_D * 4))
#define STANDARD_TICKS_E       	((int)(STANDARD_TIMING_E * 4))
#define STANDARD_TICKS_F       	((int)(STANDARD_TIMING_F * 4))
#define STANDARD_TICKS_G       	((int)(STANDARD_TIMING_G * 4))
#define STANDARD_TICKS_H       	((int)(STANDARD_TIMING_H * 4))
#define STANDARD_TICKS_I       	((int)(STANDARD_TIMING_I * 4))
#define STANDARD_TICKS_J       	((int)(STANDARD_TIMING_J * 4))

// Ticks for over drive ( in 1/4 us )
#define OVERDRIVE_TICKS_A   	((int)(OVERDRIVE_TIMING_A * 4))
#define OVERDRIVE_TICKS_B       ((int)(OVERDRIVE_TIMING_B * 4))
#define OVERDRIVE_TICKS_C       ((int)(OVERDRIVE_TIMING_C * 4))
#define OVERDRIVE_TICKS_D       ((int)(OVERDRIVE_TIMING_D * 4))
#define OVERDRIVE_TICKS_E       ((int)(OVERDRIVE_TIMING_E * 4))
#define OVERDRIVE_TICKS_F       ((int)(OVERDRIVE_TIMING_F * 4))
#define OVERDRIVE_TICKS_G       ((int)(OVERDRIVE_TIMING_G * 4))
#define OVERDRIVE_TICKS_H       ((int)(OVERDRIVE_TIMING_H * 4))
#define OVERDRIVE_TICKS_I       ((int)(OVERDRIVE_TIMING_I * 4))
#define OVERDRIVE_TICKS_J       ((int)(OVERDRIVE_TIMING_J * 4))

// Global variable for timing
static SMALLINT  A = STANDARD_TICKS_A;
static SMALLINT  B = STANDARD_TICKS_B;
static SMALLINT  C = STANDARD_TICKS_C;
static SMALLINT  D = STANDARD_TICKS_D;
static SMALLINT  E = STANDARD_TICKS_E;
static SMALLINT  F = STANDARD_TICKS_F;
static SMALLINT  G = STANDARD_TICKS_G;
static SMALLINT  H = STANDARD_TICKS_H;
static SMALLINT  I = STANDARD_TICKS_I;
static SMALLINT  J = STANDARD_TICKS_J;


//--------------------------------------------------------------------------
// Some useful macro definitions
//
#define bus_init(pin)		{ gpio_direction_input(pin); }
#define bus_low(pin)		{ gpio_direction_output(pin, 0); }
#define bus_release(pin)  	{ gpio_direction_input(pin); }
#define bus_sample(pin)   	{ gpio_get_value(pin) ? 1 : 0; }

//--------------------------------------------------------------------------
// Pause for exactly 'tick' number of ticks = 0.25us
// Bad news:  Refer to arch/arm/lib/delay.S, use Assembly code
// Good news: No need to implement this method directly, because all the usage will be times of 4 ticks
//
static void tickDelay(int ticks)
{
	//4 ticks = 1 us
	int usec = ticks / 4;
	udelay(usec);
}

//--------------------------------------------------------------------------
// Write one bit to the very GPIO PIN
// Write High: Release Bus
// Write Low:  Drive Low
//
static void w1_gpio_write_bit(unsigned int pin, SMALLINT bit)
{
	if (bit)
		gpio_direction_input(pin);
	else
		gpio_direction_output(pin, 0);
}

//--------------------------------------------------------------------------
// Write one bit from the very GPIO PIN
//
static SMALLINT w1_gpio_read_bit(unsigned int pin)
{
	return gpio_get_value(pin) ? 1 : 0;
}


/// Initialize the IO pin for iButtons
/// Must executed before other owXXX functions and called only once!
void OWInit()
{
	IOTable[0] = 194; //GPQ0
	IOTable[1] = 194; //GPQ0
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void OWWriteBit(int portnum, SMALLINT bit)
{
	if (bit)
	{
		// Write '1' bit
		//outp(PORTADDRESS,0x00); // Drives DQ low
		bus_low( IOTable(portnum) );
		tickDelay(A);

		//outp(PORTADDRESS,0x01); // Releases the bus
		bus_release( IOTable(portnum) );
		tickDelay(B); // Complete the time slot and 10us recovery
	}
	else
	{
		// Write '0' bit
		//outp(PORTADDRESS,0x00); // Drives DQ low
		bus_low( IOTable(portnum) );
		tickDelay(C);

		//outp(PORTADDRESS,0x01); // Releases the bus
		bus_release( IOTable(portnum) );
		tickDelay(D);
	}
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
//
SMALLINT OWReadBit(int portnum)
{
	SMALLINT result;

	//outp(PORTADDRESS,0x00); // Drives DQ low
	bus_low( IOTable(portnum) );
	tickDelay(A);

	//outp(PORTADDRESS,0x01); // Releases the bus
	bus_release( IOTable(portnum) );
	tickDelay(E);

	//result = inp(PORTADDRESS) & 0x01; // Sample the bit value from the slave
	result = bus_sample( IOTable(portnum) );
	tickDelay(F); // Complete the time slot and 10us recovery

	return result;
}


//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void OWWriteByte(int data)
{
	int loop;
	// Loop to write each bit in the byte, LS-bit first
	for (loop = 0; loop < 8; loop++)
	{
		OWWriteBit(data & 0x01);
		// shift the data byte for the next bit
		data >>= 1;
	}
}
//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
int OWReadByte(void)
{
	int loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// if result is one, then set MS bit
		if (OWReadBit())
				result |= 0x80;
	}
	return result;
}


//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
SMALLINT owTouchReset(int portnum)
{
	SMALLINT bit;

	tickDelay(G);

	//outp(PORTADDRESS,0x00); // Drives DQ low
	bus_low( IOTable[portnum] );
	tickDelay(H);

	//outp(PORTADDRESS,0x01); // Releases the bus
	bus_release( IOTable[portnum] );
	tickDelay(I);

	//result = inp(PORTADDRESS) ^ 0x01; // Sample for presence pulse from slave
	bit = bus_sample( IOTable[portnum] );
	tickDelay(J); // Complete the reset sequence recovery

	///Bit = 0 means device present, Bit =1 meand no devie
	return ( bit == 0 ); // Return sample presence pulse result
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
SMALLINT owTouchBit(int portnum, SMALLINT sendbit)
{
	SMALLINT bit = sendbit;

	if ( bit == 1 )
	{
		bit = OWReadBit( portnum );
	}
	else
	{   ///Send 0, don't care about the sample result;
		OWWriteBit( portnum, bit );
	}

	return bit;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bytes read from sendbyte
//
SMALLINT owTouchByte(int portnum, SMALLINT sendbyte)
{
	int loop;
	int result = 0;
	SMALLINT data = sendByte;
	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// If sending a '1' then read a bit else write a '0'
		if (data & 0x01)
		{
			if (OWReadBit(portnum))
				result |= 0x80;
		}
		else
			OWWriteBit(portnum, 0);
		// shift the data byte for the next bit
		data >>= 1;
	}
	return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteByte(int portnum, SMALLINT sendbyte)
{
   return (owTouchByte(portnum,sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
SMALLINT owReadByte(int portnum)
{
   return owTouchByte(portnum,0xFF);
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
SMALLINT owSpeed(int portnum, SMALLINT new_speed)
{
	// Adjust tick values depending on speed, see AN126
	if (new_speed == 0x00)
	{
		// Standard Speed, timing by tick
		A = 6 * 4;
		B = 64 * 4;
		C = 60 * 4;
		D = 10 * 4;
		E = 9 * 4;
		F = 55 * 4;
		G = 0;
		H = 480 * 4;
		I = 70 * 4;
		J = 410 * 4;
	}
	else
	{
		// Overdrive Speed, timing by tick
		A = 1.5 * 4;
		B = 7.5 * 4;
		C = 7.5 * 4;
		D = 2.5 * 4;
		E = 0.75 * 4;
		F = 7 * 4;
		G = 2.5 * 4;
		H = 70 * 4;
		I = 8.5 * 4;
		J = 40 * 4;
	}
	return new_speed;
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for NewLevel are
// as follows:
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//                MODE_BREAK      0x08
//
// Returns:  current 1-Wire Net level
//
SMALLINT owLevel(int portnum, SMALLINT new_level)
{
	SMALLINT result = 0;

	switch ( new_level )
	{
		case MODE_STRONG5:
		case MODE_NORMAL:
			{
				result = new_level;
			}
			break;

		default:
			break;
	}

   return result;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
SMALLINT owProgramPulse(int portnum)
{
   // add platform specific code here
   return 0;
}

//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' ms
//
void msDelay(int len)
{
	mdelay(len);
}

//--------------------------------------------------------------------------
// Get the current millisecond tick count.  Does not have to represent
// an actual time, it just needs to be an incrementing timer.
//
long msGettick(void)
{
	// not fully supported yet
	// returns timer 1, which is in 8-bit auto-reload (presumably)
	// may not be good enough.
	return ++SystemTick;
}


