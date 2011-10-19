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

/*========================================================================
    *Contact information: 
	    HUANG YI: huangyi@bsmart-solutions.com

    *Module Desciption:	
       Platform dependent part of IButton 1-wire protocol on iFastrax02,03
    
	*Last modified @15:48:28 2006-05-22
 =========================================================================
*/
#ifndef __VSDSP__
#error "Error: Must be build on VSDSP plarform"
#endif

#include "ownet.h"

///Cross SDK definition
#ifdef __ISUITE_02__
#define DSP_LOCK()			{ Disable(); Forbid(); }
#define DSP_UNLOCK()		{ Permit();  Enable(); }
#define GPIO_DIR_OUT(x)     ( USEY(GPIO_DIR) |=  ( 1<<(x) ) )
#define GPIO_DIR_IN(x)      ( USEY(GPIO_DIR) &= ~( 1<<(x) ) )
#define bus_init(x)		{ GPIO_DIR_IN (x) ; }
#define bus_low(x)		{ GPIO_DIR_OUT ( x );	GPIO_OFF ( x ) ; }
#define bus_release(x)  { GPIO_DIR_IN ( x ); }
#define bus_sample(x)   ( (  (USEY(GPIO_DATA)) & ( 1 << x ) ) != 0 )
#endif

#ifdef __ISUITE_03__  ///iSuite03
#define DSP_LOCK()			{ ISYS_Disable(); ISYS_Forbid(); }
#define DSP_UNLOCK()		{ ISYS_Permit();  ISYS_Enable(); }

/*Set one, can work (search, temp), but fail to do ecash
#define bus_init(x)		{  GPIO_Reserve ( x, GPIO_PULLUP_INPUT ); }
///both GPIO_NORMAL_OUTPUT and GPIO_OPENDRAIN_OUTPUT can work here.
#define bus_low(x)		{  GPIO_Reconfig ( x,GPIO_OPENDRAIN_OUTPUT );  	\
                           GPIO_SetSingle( x, 0);   }
#define bus_release(x)  {  GPIO_Reconfig ( x,GPIO_PULLUP_INPUT); }
#define bus_sample(x)   (  GPIO_GetSingle(x) )
*/

///Assume the GPIO for IButton is A pin
///set as input, pulled up
#define bus_init(x)		{  USEY(GPIO_A_DIR) &= ~( 1<<(x) ); }   
///set as output low
#define bus_low(x)      {  USEY(GPIO_A_DIR) |=  ( 1<<(x) ); USEY(GPIO_A_DOUT) &=  ~( 1<<(x) );  } 
#define bus_sample(x)   ( (  (USEY(GPIO_A_DIN)) & ( 1 << x ) ) != 0 ) 
///set as input
#define bus_release(x)  {  USEY(GPIO_A_DIR) &= ~( 1<<(x) ); }   

#endif ///__ISUITE_03__

#ifdef __ISUITE_02__

uchar memBlock[512];
uchar * malloc( int len )
{
	return memBlock;
}

void free ( uchar* ptr )
{
	return;
}
#endif


/// Timing for standard drive ( in 1/4 us )
#define WIRE_TIME_A       6*4    		 
#define WIRE_TIME_B       64*4   	    
#define WIRE_TIME_C       60*4         
#define WIRE_TIME_D       10*4         
#define WIRE_TIME_E       9*4         
#define WIRE_TIME_F       55*4        
#define WIRE_TIME_G       0          
#define WIRE_TIME_H       480*4        
#define WIRE_TIME_I       70*4         
#define WIRE_TIME_J       410*4        

/// Timing for over drive ( in 1/4 us )
#define WIRE_TIME_A2       6       /* 1.5*4  */
#define WIRE_TIME_B2       30      /* 7.5*4  */
#define WIRE_TIME_C2       30      /* 7.5*4  */
#define WIRE_TIME_D2       10      /* 2.5*4  */
#define WIRE_TIME_E2       3       /* 0.75*4 */
#define WIRE_TIME_F2       7*4      
#define WIRE_TIME_G2       0       
#define WIRE_TIME_H2       10      /* 2.5*4  */
#define WIRE_TIME_I2       34      /* 8.5*4  */
#define WIRE_TIME_J2       40*4  

/***************************Global variable for time interval********************/
static u16 __y  wire_time_A = WIRE_TIME_A;
static u16 __y  wire_time_B = WIRE_TIME_B;
static u16 __y  wire_time_C = WIRE_TIME_C;
static u16 __y  wire_time_D = WIRE_TIME_D;
static u16 __y  wire_time_E = WIRE_TIME_E;
static u16 __y  wire_time_F = WIRE_TIME_F;
static u16 __y  wire_time_G = WIRE_TIME_G;
static u16 __y  wire_time_H = WIRE_TIME_H;
static u16 __y  wire_time_I = WIRE_TIME_I;
static u16 __y  wire_time_J = WIRE_TIME_J;

static uchar  IOTable[MAX_PORTNUM]; 

/// Init the IO pin for ibuttons
/// Must executed before other owXXX functions and called only once!
void FAR_CODE OW_Init( uchar* ioTable, WORD count )
{
	WORD __y num = ( count > MAX_PORTNUM ) ? MAX_PORTNUM: count;
	
	if ( ioTable )
	{
		WORD i ;
		for ( i =0 ; i<num; i++ )
		{
			IOTable[i] = ioTable[i];
		}
	}
}

/******************************************************************************
 *delay for a specified time . The time is in 1/4 Usec unit
 loop 1000~200us
 ******************************************************************************/
void NEAR bus_delay ( uchar io, u32 quartUsecs )
{
	register u32 j=0;
	register u32 interval;
	interval = ( quartUsecs >> 2 ) ;
	interval = ( interval ) + ( interval<<2 );
	while ( j++ < interval );

	return;

}

/************************************************************************
 *Send a 1-Wire write bit. Provide 10us recovery time.
 ************************************************************************/
void NEAR OWWriteBit ( int portnum, uchar bit )
{
	uchar io = IOTable[portnum];

	DSP_LOCK();

	if ( bit == 1 )
	{
		bus_low   ( io );
	    bus_delay ( io, wire_time_A );
	    bus_release ( io );
	    bus_delay ( io, wire_time_B );
	}
	else
	{ 
  	    bus_low ( io );
        bus_delay ( io, wire_time_C );
        bus_release ( io );
        bus_delay ( io, wire_time_D );
	}

	DSP_UNLOCK();

	return;
}


/*************************************************************************
 *Read 1 bit of data from the 1-Wire bus.Provide 10us recovery time.
 *Return 1 : bit read is 1
         0 : bit read is 0
 *************************************************************************/
uchar NEAR OWReadBit ( int portnum )
{
	uchar retBit;
	uchar io = IOTable[portnum];

	DSP_LOCK();
	
	bus_low ( io );
    bus_delay ( io, wire_time_A );
    bus_release ( io );
    bus_delay ( io, wire_time_E );
    retBit = bus_sample ( io );
    bus_delay ( io, wire_time_F );
	bus_release ( io );

	DSP_UNLOCK();
  
    return retBit;
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
SMALLINT NEAR owTouchReset (int portnum )
{
   // add platform specific code here
  uchar bit;
  uchar io = IOTable[portnum];

  DSP_LOCK();

  bus_delay ( io, wire_time_G );
  bus_low ( io );
  bus_delay ( io ,wire_time_H ); 
  bus_release ( io );
  bus_delay ( io, wire_time_I  );  
  
  bit = bus_sample ( io );
  bus_delay ( io, wire_time_J );

  DSP_UNLOCK();
 
  ///Bit = 0 means device present, Bit =1 meand no devie
  return ( bit == 0 ) ;
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
SMALLINT NEAR owTouchBit ( int portnum, SMALLINT sendbit )
{
   // add platform specific code here
	SMALLINT bit = sendbit;

	DSP_LOCK();

	if ( bit == 1 )
	{
		bit = OWReadBit ( portnum );
	}
	else
	{   ///Send 0, don't care about the sample result;
		OWWriteBit ( portnum, bit );
	}

	DSP_UNLOCK();
	 
   return bit ;
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
uchar /*SMALLINT*/ FAR_CODE owTouchByte(int portnum, SMALLINT sendbyte)
{
   uchar i;
   uchar result = 0;

   for (i = 0; i < 8; i++)
   {
       result |= (owTouchBit(portnum,sendbyte & 1) << i);
       sendbyte >>= 1;
   }

   return result;
}

#ifdef INCLUDE_EX

SMALLINT owTouchByte2(int portnum, SMALLINT sendbyte)
{
	 // add platform specific code here
  SMALLINT  result = 0;
  int loop;

  for ( loop = 0; loop < 8; loop++ )
  {
		/* shift the result to get it ready for the next bit*/
		result >>= 1;

		/* If sending a ‘1’ then read a bit else write a ‘0’*/
		if ( sendbyte & 0x01 )
		{
		   if ( OWReadBit( portnum ) == 1 )
		       result |= 0x80;
		}
		else
		{
		    OWWriteBit ( portnum, 0 );
		}

		/* shift the data byte for the next bit*/
		sendbyte >>= 1;
  }
  
   return result;
}

SMALLINT owTouchBit2 ( int portnum, SMALLINT sendbit )
{
   // add platform specific code here
	SMALLINT bit = sendbit;
	uchar io = IOTable[portnum];

	if ( bit == 1 )
	{ 
		bus_low   (  io );
	    bus_delay ( io, wire_time_A );
	    bus_release ( io );
	    bus_delay ( io, wire_time_B );

		bit = bus_sample( io ) ;
 
	}
	else
	{   ///Send 0, don't care about the sample result;
		 
  	    bus_low ( io );
        bus_delay ( io, wire_time_C );
        bus_release ( io );
        bus_delay ( io, wire_time_D );

		bit = bus_sample( io ) ;
 
	}
	 
   return bit ;
}

#endif

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
SMALLINT FAR_CODE  owWriteByte(int portnum, SMALLINT sendbyte)
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
SMALLINT FAR_CODE owReadByte(int portnum)
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
SMALLINT FAR_CODE owSpeed(int portnum, SMALLINT new_speed )
{
   // add platform specific code here

	/// Adjust tick values depending on speed
	if ( new_speed == MODE_NORMAL )
	{
		// Standard Speed
		wire_time_A = WIRE_TIME_A ;
		wire_time_B = WIRE_TIME_B;
		wire_time_C = WIRE_TIME_C;
		wire_time_D = WIRE_TIME_D;
		wire_time_E = WIRE_TIME_E;
		wire_time_F = WIRE_TIME_F;
		wire_time_G = WIRE_TIME_G;
		wire_time_H = WIRE_TIME_H;
		wire_time_I = WIRE_TIME_I;
		wire_time_J = WIRE_TIME_J;
	}
	else if ( new_speed == MODE_OVERDRIVE )
	{
		// Overdrive Speed
		wire_time_A = WIRE_TIME_A2 ;
		wire_time_B = WIRE_TIME_B2;
		wire_time_C = WIRE_TIME_C2;
		wire_time_D = WIRE_TIME_D2;
		wire_time_E = WIRE_TIME_E2;
		wire_time_F = WIRE_TIME_F2;
		wire_time_G = WIRE_TIME_G2;
		wire_time_H = WIRE_TIME_H2;
		wire_time_I = WIRE_TIME_I2;
		wire_time_J = WIRE_TIME_J2;
	}

   return new_speed;   ///HY???
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
SMALLINT FAR_CODE owLevel(int portnum, SMALLINT new_level)
{
	SMALLINT result = 0;
   // add platform specific code here
#ifdef __VSDSP__

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

#endif

   return result;
}


//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' ms
//
void FAR_CODE msDelay(int len)
{
   // add platform specific code here
#ifdef __ISUITE_02__
		Delay( len );
		return;
#endif
    
#ifdef __ISUITE_03__
		ISYS_Delay( len );
		return;
#endif
}




//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of delivering power. 
//
SMALLINT FAR_CODE hasPowerDelivery ( int portnum )
{
   // add adapter specific code here
   return TRUE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of over drive. 
//
SMALLINT FAR_CODE hasOverDrive(int portnum)
{
   // add adapter specific code here
   return TRUE;
}
//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse 
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available  
SMALLINT FAR_CODE hasProgramPulse(int portnum)
{
   // add adapter specific code here
   return TRUE;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).  
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same 
//
SMALLINT FAR_CODE owWriteBytePower(int portnum, SMALLINT sendbyte)
{
   // replace if platform has better implementation (faster response)
   if (!hasPowerDelivery(portnum))
      return FALSE;
   
   if(owTouchByte(portnum,sendbyte) != sendbyte)
      return FALSE;

   if(owLevel(portnum,MODE_STRONG5) != MODE_STRONG5)
      return FALSE;

   return TRUE;
}

#ifndef __VSDSP__


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
SMALLINT FAR_CODE owProgramPulse(int portnum)
{
   // add platform specific code here
   return 0;
}

///20060406: Added by HY
SMALLINT FAR_CODE owHasProgramPulse(int portnum)
{
   // add platform specific code here
   return 0;
}





//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and verify that the
// response matches the 'applyPowerResponse' bit and apply power delivery
// to the 1-Wire net.  Note that some implementations may apply the power
// first and then turn it off if the response is incorrect.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery 
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
SMALLINT FAR_CODE owReadBitPower(int portnum, SMALLINT applyPowerResponse)
{
   // replace if platform has better implementation (faster response)
   if (!hasPowerDelivery(portnum))
      return FALSE;

   if(owTouchBit(portnum,0x01) != applyPowerResponse)
      return FALSE;

   if(owLevel(portnum,MODE_STRONG5) != MODE_STRONG5)
      return FALSE;

   return TRUE;
}

//--------------------------------------------------------------------------
// Get the current millisecond tick count.  Does not have to represent
// an actual time, it just needs to be an incrementing timer.
//
long msGettick(void)
{
   // add platform specific code here
   return 0;
}
#endif
