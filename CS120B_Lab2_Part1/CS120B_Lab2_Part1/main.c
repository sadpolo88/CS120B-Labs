/*
 * CS120B_Lab2_Part1.c
 *
 * Created: 1/12/2020 5:07:18 PM
 * Author : Tommy
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A

    /* Replace with your application code */
    while (1) 
    {
		tmpA=PINA & 0x01; //garage door sensor
		tmpB=PINA & 0x02; //light sensor 
		if ((tmpA==0x01 && tmpB==0x00))
		{
			PORTB=tmpA;
		}
		else
		{
			PORTB=0x00;
		}
    }
	return 0;
}

