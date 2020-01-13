/*
 * CS120B_Lab2_Part3.c
 *
 * Created: 1/12/2020 8:53:33 PM
 * Author : Tommy 
 * Lab Section:24
 * Description:Extend the previous program(Lab2_Part2) to still write the available spaces number, 
 * but only to PC3..PC0, and to set PC7 to 1 if the lot is full. 
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpC = 0x00;
	unsigned char tmpD = 0x00;
	unsigned char cntavail = 0x00;
	
    /* Replace with your application code */
    while (1) 
    {
		tmpA= PINA & 0x01; //PINA bit 0
		tmpB= PINA & 0x02; //PINA bit 1
		tmpC= PINA & 0x04; //PINA bit 2
		tmpD= PINA & 0x08; //PINA bit 3
		
		tmpB= tmpB>>0x01; //dividing to make it one car space per spot
		tmpC= tmpC>>0x02;
		tmpD= tmpD>>0x03;
		cntavail = tmpA+tmpB+tmpC+tmpD;
		
		if(cntavail!=0x04)
		{
			cntavail = 0x04 - cntavail;
			PORTC=cntavail;
		}
		else
		{
			PORTC=0x80;
		}
    }
	return 0;
}

