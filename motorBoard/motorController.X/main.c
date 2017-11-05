/* 
 * File:   main.c
 * Author: edouardyreux
 *
 * Created on September 4, 2017, 3:08 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <p33FJ32GP102.h>
#include <xc.h>


#define LED_ON  1
#define LED_OFF 0

#define INPUT  1
#define OUTPUT 0

#define LED_WHITE LATAbits.LATA0
#define LED_BLUE  LATAbits.LATA1
#define LED_RGB_B LATAbits.LATA2
#define LED_RGB_G LATAbits.LATA3
#define LED_RGB_R LATAbits.LATA4


#define TIMER_ON                    0x8000
#define TIMER_SOURCE_INTERNAL       0x0000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000
#define TIMER_PRESCALER_64          0x0020
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config OSCIOFNC = ON          // Primary Oscillator Output Function (OSC2 pin has clock out function)

/*
 * 
 */
void config_ra_output();
void config_timer();
void config_i2c();
int command; 
int data_received; 
int command_is_received;

int main(int argc, char** argv) {
    config_ra_output();
    LED_WHITE = LED_ON;
    LED_BLUE = LED_OFF;
    LED_RGB_B = LED_OFF;
    LED_RGB_G = LED_OFF;
    LED_RGB_R = LED_OFF;
    data_received = 0;
    command_is_received = 0;
    command = 0;
    
    config_timer();
    config_i2c();
    
    while(1);
    return (EXIT_SUCCESS);
} 

void config_ra_output() {
    TRISAbits.TRISA0 = OUTPUT;
    TRISAbits.TRISA1 = OUTPUT;
    TRISAbits.TRISA2 = OUTPUT;
    TRISAbits.TRISA3 = OUTPUT;
    TRISAbits.TRISA4 = OUTPUT;
}

void config_i2c(void)
{
	I2C1CON = 0x9040;	//Enable I2C1 module, enable clock stretching	

	I2C1ADD = 0x50;			// 7-bit I2C slave address must be initialized here. 
	IFS1=0;
	_SI2C1IE = 1;
}

void config_timer() {

    IPC0bits.T1IP = TIMER_INTERRUPT_PRIORITY_4;
    IFS0bits.T1IF = 0;

    TMR1 = 0;

    PR1 = 0xF424;
    T1CON = TIMER_ON |
            TIMER_SOURCE_INTERNAL |
            GATED_TIME_DISABLED |
            TIMER_16BIT_MODE |
            TIMER_PRESCALER_64;
    IEC0bits.T1IE = 1;


}

void __attribute__ ( ( __interrupt__ , auto_psv ) ) _T1Interrupt ( void )
{
    /* Toggle LED at 1 Hz rate */
   LED_WHITE ^= LED_ON ;
//    LED_BLUE ^= LED_ON;
//    LED_RGB_B ^= LED_ON;
//    LED_RGB_G ^= LED_ON;
//    LED_RGB_R ^= LED_ON;

    /* reset Timer 1 interrupt flag */
    IFS0bits.T1IF = 0 ;
}

unsigned char data_to_write = 0; // used to send to master

void __attribute__((interrupt,no_auto_psv)) _SI2C1Interrupt(void)
{
    
    unsigned char u_address = 0;
    char reception = 0;
	if((I2C1STATbits.R_W == 0)&&(I2C1STATbits.D_A == 0))	//Address matched
    {
		// R/W bit = 0 --> indicates data transfer is input to slave
		// D/A bit = 0 --> indicates last byte was address  
        
		// reset any state variables needed by a message sequence	
		// perform a dummy read of the address
        u_address = I2C1RCV; // this only contains the address
        command_is_received = 0;
        I2C1CONbits.SCLREL = 1;	//Release SCL1 line        
    }
	else if((I2C1STATbits.R_W == 0)&&(I2C1STATbits.D_A == 1))	// master is sending us data	
    {
        // R/W bit = 0 --> indicates data transfer is input to slave
		// D/A bit = 1 --> indicates last byte was data
        // writing data to our module, just store it in data_received
        reception = I2C1RCV; // contains data :)
        if (command_is_received == 0) {
            command = (int)reception;
            command_is_received = 1;
        } else {
            data_received = (int)reception;
            if (command == 1) {
                if (data_received >= 0) {
                    LED_BLUE = LED_ON;
                } else {
                    LED_BLUE = LED_OFF;
                }
                if (data_received <= 0) {
                    LED_RGB_G = LED_ON;
                } else {
                    LED_RGB_G = LED_OFF;
                }
            } else {
                if (data_received >= 0) {
                    LED_RGB_R = LED_ON;
                } else {
                    LED_RGB_R = LED_OFF;
                }
                if (data_received <= 0) {
                    LED_RGB_B = LED_ON;
                } else {
                    LED_RGB_B = LED_OFF;
                }
            }
        }
        I2C1CONbits.SCLREL = 1;	//Release SCL1 line
    }
	else if((I2C1STATbits.R_W == 1)&&(I2C1STATbits.D_A == 0)) // master is trying to read data from us
		// R/W bit = 1 --> indicates data transfer is output from slave
		// D/A bit = 0 --> indicates last byte was address
	{
		// read of the slave device, read the address 
		
		u_address = I2C1RCV; // this only contains our address
		I2C1TRN = data_to_write;	//Read data from RAM & send data to I2C master device
        I2C1CONbits.SCLREL = 1;	//Release SCL1 line
        while(I2C1STATbits.TBF != 0);//Wait till all 
        data_to_write ++;

	}
    
	else if ((I2C1STATbits.R_W == 1) && (I2C1STATbits.D_A == 1)&& (I2C1STATbits.ACKSTAT == 0))
    {
		// R/W bit = 1 --> indicates data transfer is output from slave
		// D/A bit = 1 --> indicates last byte was data
        
        // output the data until the MASTER terminates the
		// transfer with a NACK, continuing reads return 0
        I2C1TRN = data_to_write;	//Read data from RAM & send data to I2C master device
        I2C1CONbits.SCLREL = 1;	//Release SCL1 line
        data_to_write++;
        while(I2C1STATbits.TBF != 0);//Wait till all 
    }
    
	_SI2C1IF = 0;	//clear I2C1 Slave interrupt flag
}	