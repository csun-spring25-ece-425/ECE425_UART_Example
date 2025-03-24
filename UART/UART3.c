/**
 * @file UART3.c
 *
 * @brief Source code for the UART3 driver.
 *
 * This file contains the function definitions for the UART3 driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 * Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the system clock (50 MHz) is used.
 *
 * @author Aaron Nanas
 */

#include "UART3.h"

void UART3_Init(void)
{
	// Enable the clock to UART3 by setting the 
	// R3 bit (Bit 3) in the RCGCUART register
	SYSCTL->RCGCUART |= 0x08;
	
	// Enable the clock to Port C by setting the
	// R2 bit (Bit 2) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x04;
	
	// Configure the PC7 (U3TX) and PC6 (U3RX) pins to use the alternate function
	// by setting Bits 7 to 6 in the AFSEL register
	GPIOC->AFSEL |= 0xC0;
	
	// Clear the PMC7 (Bits 31 to 28) and PMC6 (Bits 27 to 24) fields in the PCTL register before configuration
	GPIOC->PCTL &= ~0xFF000000;
	
	// Configure the PC7 pin to operate as a U3TX pin by writing 0x1 to the
	// PMC7 field (Bits 31 to 28) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOC->PCTL |= 0x10000000;
	
	// Configure the PC6 pin to operate as a U3RX pin by writing 0x1 to the
	// PMC6 field (Bits 27 to 24) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOC->PCTL |= 0x01000000;
	
	// Enable the digital functionality for the PC7 and PC6 pins
	// by setting Bits 7 to 6 in the DEN register
	GPIOC->DEN |= 0xC0;
	
	// Disable the UART3 module before configuration by clearing
	// the UARTEN bit (Bit 0) in the CTL register
	UART3->CTL &= ~0x0001;
	
	// Specify the UART clock source to use the system clock by
	// writing a value of 0x0 to the CS field (Bits 3 to 0) in the CC register
	UART3->CC &= ~0xF;
	
	// Configure the UART3 module to use the system clock (50 MHz)
	// divided by 16 by clearing the HSE bit (Bit 5) in the CTL register
	UART3->CTL &= ~0x0020;
	
	// Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
	// and the DIVFRAC field (Bits 5 to 0) in the IBRD and FBRD registers, respectively.
	// The integer part of the calculated constant will be written to the IBRD register,
	// while the fractional part will be written to the FBRD register.
	// BRD = (System Clock Frequency) / (16 * Baud Rate)
	// BRDI = (50,000,000) / (16 * 9600) = 325.5208333 (IBRD = 325)
	// BRDF = ((0.5208333 * 64) + 0.5) = 33.8333 (FBRD = 33)
	UART3->IBRD = 325;
	UART3->FBRD = 33;
	
	// Configure the data word length of the UART packet to be 8 bits by 
	// writing a value of 0x3 to the WLEN field (Bits 6 to 5) in the LCRH register
	UART3->LCRH |= 0x60;
	
	// Enable the transmit and receive FIFOs by setting the FEN bit (Bit 4) in the LCRH register
	UART3->LCRH |= 0x10;
	
	// Select one stop bit to be transmitted at the end of a UART frame by
	// clearing the STP2 bit (Bit 3) in the LCRH register
	UART3->LCRH &= ~0x08;
	
	// Disable the parity bit by clearing the PEN bit (Bit 1) in the LCRH register
	UART3->LCRH &= ~0x02;
	
	// Enable the UART3 module after configuration by setting
	// the UARTEN bit (Bit 0) in the CTL register
	UART3->CTL |= 0x01;
}

char UART3_Input_Character(void)
{
	while((UART3->FR & UART3_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART3->DR & 0xFF);
}

void UART3_Output_Character(char data)
{
	while((UART3->FR & UART3_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	
	UART3->DR = data;
}
