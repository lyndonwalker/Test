 /*********************************************
 Author: 		Lyndon  Walker
 Description: 	Accepts a double click on switch input and 
               generates a single click on output 
  
 Chip type          : ATtiny13A
 Clock frequency    : 1.0 MHz default
 I/O				: Switch input 
					:
 Initial revision: 8/25/2012
 
 
*********************************************/
/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/sleep.h>


/* Prototypes */
void process(void);
void test(void);

#define TRUE		1
#define FALSE		0
#define SW_BIT		PB3
#define OPTO_BIT	PB2

//#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))


// Globals
volatile unsigned char _pause=0;

ISR(PCINT0_vect)
{
	// No processing in ISR
}

int main( void )
{
	// Indicator LED and inputs
	DDRB= (1<<PB2);

	// Make input pullups high
	PORTB= 0xFF;

	// Set prescaler on timer 1 and disconnect it from the outputs
	// Prescaler is set to divide by 1024, so each bit is a period of
	// 1uS * 1024 = 1024uS
	TCCR0B= 5;
	TCCR0A= 0;
	
	// Disable timer interrupts
	TIMSK0= 0;
		
	// Enable PC interrupt
	GIMSK= (1<<PCIE);
	PCMSK= (1<<PCINT3);
	//while(1) test();

	sei();

 	// Set sleep mode to power down
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Main loop
	for(;;) 	    
	{
		// Put processor to sleep. Will wake up when switch pressed
		sleep_enable();
		sleep_cpu();

		// No more interrupts until we're done
		cli();
		process();
		sei();
	}
}

//
// Processes double click input: LOW (no more than 0.1 sec) - HIGH(0.1 - 0.4 second) - LOW
// and produces single pulse on output
//
// Called when input is LOW
//
void process(void)
{
	const int MAXDOWN= 1000;
	const int MAXUP=250;
	const int THRESHOLD=80;
	const int DEBOUNCE=20;

	int i=0, j=0;
	int countHigh=0;

	// Wait for it to stop bouncing
	_delay_ms(DEBOUNCE);

	// Check switch depressed
	for (i=0; i < MAXDOWN; i++)
	{
		if (!(PINB & (1 << SW_BIT)) )
		{
			_delay_ms(1);
		}
		else
		{
			break;
		}
	}

	// Check HIGH
	for (j=0; j < MAXUP; j++)
	{
		if (PINB & (1 << SW_BIT)) 
		{
			countHigh++;
			_delay_ms(1);
		}
		else
		{
			break;
		}
	}

	// If didn't time out, we had a double click
	if (i != MAXDOWN && j != MAXUP)
	{
		// Check if click pattern is OK
		if (countHigh > THRESHOLD)
		{
			// Toggle output
			PORTB &= ~(1 << OPTO_BIT);
			_delay_ms(200);
			PORTB |= (1 <<OPTO_BIT);
			_delay_ms(500);
		}
	}
}

void test(void)
{
	PORTB &= ~(1 << OPTO_BIT);
	_delay_ms(100);
	PORTB |= (1 <<OPTO_BIT);
	_delay_ms(1250);
}
