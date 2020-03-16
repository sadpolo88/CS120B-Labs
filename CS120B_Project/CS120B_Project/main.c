/*
 * CS120B_Project.c
 *
 * Created: 2/26/2020 10:49:14 PM
 * Author : Tommy Liang
 */ 

#include <avr/io.h>
//#define F_CPU 8000000UL
//#include <util/delay.h>
#include "io.h"
#include "timer_isr.h"
#include "io.c"
#include "timer_isr.c"


// custom defines
#define true 1
#define TOTAL_TASKS 6
#define STRING_LENGTH 37
#define PERIOD_GCD 100
#define PERIOD_display PERIOD_GCD
#define PERIOD_keypad PERIOD_GCD
#define PERIOD_HEXDISPLAY 1000
#define PERIOD_SENSOR 100
#define PERIOD_COMBINE_SENSOR_HEX 100

#define PERIOD_speaker 100

// enumerations
enum _state_display { START_display, INIT, DISPLAY};
enum _state_keypad { START_keypad, READ };
enum States_speaker { START_speaker, NO_NOISE, NOISE };

// global structures
struct _task {
	unsigned short timeElapsed;
	unsigned short period;

	void ( *tick ) ( void );
};


// global variables
static struct _task gTaskSet [ TOTAL_TASKS ];
static char TimerFlag = 0;
static char gKeypadSample;
static char gHexDisplay = 0;
static char gMotionSensor = 0;

static char gKeypadSampleArray[4];
static char gPassword[] = {'1', '2', '3', '4'};
static unsigned char gCountDownFlag =0;
unsigned char gSpeaker=0;

// functions
void initializeTimer ( void );
void initializeTasks ( void );
void initializePorts ( void );
void executeTasks ( void );
void tick_display ( void );
void tick_keypad ( void );
void tick_hexdisplay(void);
void tick_motionsensor( void );
void tick_combine_sensor_hex( void );
void tick_speaker( void );
unsigned char equal ( char * array1, char * array2 );
unsigned char sampleKeypad ( void );



int main(void) {

	initializePorts();
	initializeTimer();
	initializeTasks();

	while (true)
	{
		while ( !TimerFlag ); // idle for the task to complete
		TimerFlag = 0x00;
		executeTasks();

	}

	return 1;
}

void tick_hexdisplay()
{
	//char array[]={~0x3F,~0x06,~0x5B,~0x4F,~0x66,~0x6D,~0x7D,~0x07,~0x7F,~0x67};
	char array[]={~0xE7,~0xFF,~0x87,~0xFD,~0xED,~0xE6,~0xCF,~0xDB,~0x86,~0xBF};
	static char index=0;
	gHexDisplay = array[index];
	//index = (--index < 0) ? 9 : index;
	index= ++index % 10;
	
	if(index==0)
	{
		gCountDownFlag=1;
	}
	return;
}

void tick_combine_sensor_hex( void ) {
	PORTB = ( gMotionSensor << 7 ) | gHexDisplay;
	
}

void tick_motionsensor()
{
	gMotionSensor =  (PIND & 0x01);
}

void executeTasks ( void ) {

	for ( int i = 0;  i < TOTAL_TASKS; i++ ) {
		gTaskSet[i].timeElapsed += PERIOD_GCD;
		if( gTaskSet[i].timeElapsed >= gTaskSet[i].period ) {
			gTaskSet[i].tick();
			gTaskSet[i].timeElapsed = 0;
		}
	}

	return;
}


void initializeTimer ( void ) {

	TimerSet( PERIOD_GCD );
	TimerOn();

	return;
}


void initializePorts ( void ) {

	/* Insert DDR and PORT initializations */
	DDRA = 0x0F; PORTA = 0xF0; // reserved for keypad
	DDRB = 0xFF; PORTB = 0x00; // reserved for timer display & motion sensor
	DDRC = 0xFF; PORTC = 0x00; // reserved for LCD display
	DDRD = 0xFE; PORTD = 0x01; // reserved for LCD display & speaker

	return;
}


void initializeTasks ( void ) {

	gTaskSet[0].timeElapsed = 0;
	gTaskSet[0].period = PERIOD_display;
	gTaskSet[0].tick = tick_display;


	gTaskSet[1].timeElapsed = 0;
	gTaskSet[1].period = PERIOD_keypad;
	gTaskSet[1].tick = tick_keypad;
	
	gTaskSet[2].timeElapsed = 0;
	gTaskSet[2].period = PERIOD_HEXDISPLAY;
	gTaskSet[2].tick=tick_hexdisplay;
	
	gTaskSet[3].timeElapsed=0;
	gTaskSet[3].period = PERIOD_SENSOR;
	gTaskSet[3].tick=tick_motionsensor;
	
	gTaskSet[4].timeElapsed=0;
	gTaskSet[4].period = PERIOD_COMBINE_SENSOR_HEX;
	gTaskSet[4].tick=tick_combine_sensor_hex;
	
	gTaskSet[5].timeElapsed=0;
	gTaskSet[5].period=PERIOD_speaker;
	gTaskSet[5].tick=tick_speaker;
	return;
}


void tick_display ( void ) {

	static enum _state_display currentState = START_display;
	static unsigned char indexLCD = 8;
	static unsigned char indexKeypad = 0;
	
	switch ( currentState ) {

		case START_display :
		currentState = INIT;
		break;

		case INIT :
		currentState = DISPLAY;
		break;

		case DISPLAY :
		currentState = DISPLAY;
		break;

		default:
		currentState = DISPLAY;
		break;

	}


	// actions
	switch ( currentState ) {

		case START_display:
		break;

		case INIT :
		LCD_init();
		LCD_ClearScreen();
		LCD_DisplayString(1,"Enter: ");
		break;

		case DISPLAY :
		if(gCountDownFlag==1)
		{
			indexLCD = 8;
			indexKeypad = 0;
			gCountDownFlag= 0;
		
			currentState=START_display;
		}
		
		else if ( gKeypadSample != 0 ) {

			
			LCD_Cursor( indexLCD++ );
			LCD_WriteData( gKeypadSample );
			gKeypadSampleArray[ indexKeypad ] = gKeypadSample;
			indexKeypad = ( indexKeypad + 1 ) % 4;
			PORTD=(gSpeaker<<5);
			
			if ( indexLCD == 12 ) {
				indexLCD = 0;
				LCD_ClearScreen();
				if ( equal( gKeypadSampleArray, gPassword )) {
					LCD_DisplayString(5, "SUCCESS!       Door Opened.");
					
				}
				else {
					LCD_DisplayString(5, "FAILED!        Try Again.");
				}
			}

		}

		break;

		default:
		break;

	}

	return;
}


void tick_keypad ( void ) {

	static enum _state_keypad currentKeypadState = START_keypad;
	//static unsigned char indexKeypad = 0;

	// transitions between states
	switch ( currentKeypadState ) {

		case START_keypad :
		currentKeypadState = READ;
		break;

		case READ :
		currentKeypadState = READ;
		break;

		default :
		currentKeypadState = READ;
		break;
	}


	// behavior of each state
	switch ( currentKeypadState) {

		case START_keypad :
		break;

		case READ :
		
		
		gKeypadSample = sampleKeypad();
		//gKeypadSampleArray[ indexKeypad ] = gKeypadSample;
		//indexKeypad = ( indexKeypad + 1 ) % 4;
		
		break;
		
		default :
		gKeypadSample = 0;
		break;
	}
}



unsigned char sampleKeypad ( void ) {

	const static unsigned char keypadDataMatrix[][4] = {{ 'D', 'C', 'B', 'A' }, { '#', '9', '6', '3' }, { '0', '8', '5', '2' }, { '*', '7', '4', '1' }};

	for ( unsigned char row = 0; row < 4; row++ ) {
		PORTA = ~( 0x01 << row ); // write to upper nibble of keypad port
		
		
		//LCD_Cursor( 3 );
		//LCD_WriteData( 'r' );


		for ( unsigned char col = 0; col < 4; col++ ) {

			//LCD_Cursor( 5 );
			//LCD_WriteData( 'c' );

			if ( ~PINA & (( 0x01 << col ) << 4 )) {
				return keypadDataMatrix[row][col];
			}
		}
	}

	return 0;
}


void TimerISR ( void ) {
	TimerFlag = 0x01;
	return;
}


ISR ( TIMER1_COMPA_vect ) {
	_avr_timer_cntcurr--;
	if ( _avr_timer_cntcurr == 0 ) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


unsigned char equal ( char * array1, char * array2 ) {
	
	for ( unsigned char index = 0; index < 4; index++ ) {
		if ( array1[ index ] != array2[ index ] ) {
			return 0;
		}
	}
	
	return 1;
}

void tick_speaker( void )
{
	static enum States_speaker state_speaker = START_speaker;
	
	switch(state_speaker) //transitions
	{
		case START_speaker:
		state_speaker= NO_NOISE;
		break;
		
		case NO_NOISE:
		if(gKeypadSample !=0)
		{
			state_speaker=NOISE;
		}
		else
		{
			state_speaker=NO_NOISE;
		}
		break;
		
		case NOISE:
		if(gKeypadSample==0)
		{
			state_speaker=NO_NOISE;
		}
		else
		{
			state_speaker=NOISE;
		}
		break;
		
		default:
		break;
	}
	
	switch(state_speaker) //actions
	{
		case START_speaker:
		break;
		
		case NO_NOISE:
		gSpeaker = 0x00;
		break;
		
		case NOISE:
		gSpeaker ^= 0x01;
		break;
		
		default:
		break;
	}
	
}