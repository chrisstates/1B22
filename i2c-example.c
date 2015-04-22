#include <htc.h>
#include "i2c.h"
/*
 * Hi-Tech C Demo program for 12F1822
 *
 * Writes values to a slave servo chip and reads values from it
 * via I2C
 *
 * Copyright (C)2011 HobbyTronics.co.uk
 * Freely distributable.
*/

// Config MCLR Disabled, Code Protect on, Watchdog disabled, Brownout off, Internal Osc
__CONFIG(MCLRE_OFF & CP_ON & WDTE_OFF & BOREN_OFF & FOSC_INTOSC);

#define _XTAL_FREQ 16000000			// Used in __delay_ms() functions
#define I2C_SLAVE 40				// Slave device I2C address

// Send a command to servo ic
void i2c_servo_command(unsigned char address, unsigned char command)
{
	i2c_Start();      					// send Start
	i2c_Address(I2C_SLAVE, I2C_WRITE);	// Send  slave address with write operation
	i2c_Write(address);					// Servo Speed 
	i2c_Write(command);					// 2 seconds on servo 0
 	i2c_Stop();			  				// send Stop
}

// Read a char from servo ic
unsigned char i2c_servo_read(unsigned char address)
{
	unsigned char read_byte;
	// Read one byte (i.e. servo pos of one servo)
	i2c_Start();      					// send Start
	i2c_Address(I2C_SLAVE, I2C_WRITE);	// Send slave address with write operation
	i2c_Write(address);					// Set register for servo 0
	i2c_Restart();						// Restart
	i2c_Address(I2C_SLAVE, I2C_READ);	// Send slave address with read operation	
	read_byte = i2c_Read(0);			// Read one byte
										// If more than one byte to be read, (0) should
										// be on last byte only
										// e.g.3 bytes= i2c_Read(1); i2c_Read(1); i2c_Read(0);
   	i2c_Stop();							// send Stop
	return read_byte;					// return byte. If reading more than one byte
										// 				store in an array
}

void main(void)
{
	unsigned char i;
	unsigned char result;
	unsigned char servo0_pos=0;

	OSCCON = 0b01111010;	// set internal osc to 16MHz
	TRISA = 0b000000;		//
	PORTA = 0b000000;

	i2c_Init();				// Start I2C as Master 100KHz

	i2c_servo_command(63, 20);		// Set Servo Speed to 2s for servo 0
	i2c_servo_command(60, 1);		// Turn servo outputs ON

	while(1){

		i2c_servo_command(0, 255);				// Set servo 0 to position 255

		// Wait until Servo 0 has reached half way
		while(servo0_pos < 127)
		{
			__delay_ms(50);
			servo0_pos = i2c_servo_read(0);		// Read pos of servo 0
		}

		i2c_servo_command(1, 255);				// Set servo1 to position 255

		// Wait until Servo 0 has reached end
		while(servo0_pos < 255)
		{
			__delay_ms(50);
			servo0_pos = i2c_servo_read(0);		// Read pos of servo 0
		}

		i2c_servo_command(0, 0);				// Set servo 0 to position 0

		// Wait until Servo 0 has reached half way
		while(servo0_pos > 127)
		{
			__delay_ms(50);
			servo0_pos = i2c_servo_read(0);		// Read pos of servo 0
		}

		i2c_servo_command(1, 0);				// Set servo1 to position 0

		// Wait until Servo 0 has reached start
		while(servo0_pos > 0)
		{
			__delay_ms(50);
			servo0_pos = i2c_servo_read(0);		// Read pos of servo 0
		}

	}
}
