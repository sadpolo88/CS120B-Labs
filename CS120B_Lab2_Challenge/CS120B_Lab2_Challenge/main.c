/*
 * CS120B_Lab2_Challenge.c
 *
 * Created: 1/12/2020 10:03:18 PM
 * Author : Tommy
 * Lab section:24
 * Description:An amusement park kid ride cart has three seats, with 8-bit weight 
 * sensors connected to ports A, B, and C (measuring from 0-255 kilograms). 
 * Set PD0 to 1 if the cart's total passenger weight exceeds the maximum of 140 kg. 
 * Also, the cart must be balanced: Set port PD1 to 1 if the difference between A and C exceeds 80 kg.
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; 
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned char seatA = 0x00;
	unsigned char seatB = 0x00;
	unsigned char seatC = 0x00;
	unsigned char total = 0x00;
	unsigned char tmpBalanced = 0x00;
	
	
    /* Replace with your application code */
    while (1) 
    {
		seatA=PINA;
		seatB=PINB;
		seatC=PINC;
		
		total=seatA+seatB+seatC;
		tmpBalanced=seatA-seatC;
		if(total>=0x8C)
		{
			PORTD=0x01;
		}
		if(tmpBalanced>=0x50)
		{
			PORTD=0x02;
		}
    }
	return 0;
}

