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

#define FOSC    (3686400ULL)
#define FCY     (FOSC/2)
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config OSCIOFNC = ON          // Primary Oscillator Output Function (OSC2 pin has clock out function)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)
#pragma config PWMLOCK = OFF             


/*
 * 
 */
void config_ra_output();
void config_timer();
void config_i2c();
void config_pwm();
int command; 
int data_received; 
int command_is_received;

int main(int argc, char** argv) {
    config_pwm();
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


void config_pwm() {
//    //setup PWM in complementary
//    PWM1CON1bits.PMOD1 = 0;
//    PWM1CON1bits.PMOD2 = 0;
//    PWM1CON1bits.PMOD3 = 0;
//    
//    // set driver to pwm for all pins
//    PWM1CON1bits.PEN1H = 1;
//    PWM1CON1bits.PEN2H = 1;
//    PWM1CON1bits.PEN3H = 1;
//    PWM1CON1bits.PEN1L = 1;
//    PWM1CON1bits.PEN2L = 1;
//    PWM1CON1bits.PEN3L = 1;
//    
//    /* Immediate update of PWM enabled */
//    PWM1CON2bits.IUE = 1;
//    
//    /* PWM I/O pin controlled by PWM Generator */
//    P1OVDCONbits.POVD3H = 1;
//    P1OVDCONbits.POVD2H = 1;
//    P1OVDCONbits.POVD1H = 1;
//    P1OVDCONbits.POVD3L = 1;
//    P1OVDCONbits.POVD2L = 1;
//    P1OVDCONbits.POVD1L = 1;
//
//    //Clear faults
//    IFS3bits.PWM1IF     = 0;
//    IFS3bits.FLTA1IF    = 0;
//    IFS4bits.FLTB1IF    = 0;
//
////    P1TCONbits.PTOPS    = 0b0000; //1 CPU tick = 1 PWM tick
////    P1TCONbits.PTCKPS   = 0b00;
////    P1TCONbits.PTMOD    = 0b10;
////    P1TCONbits.PTSIDL   = 1;      //Halts when CPU idles
//
//    // no longer necessary since I disabled register write lock:
//        __builtin_write_PWMSFR(&P1FLTACON,0x0000,&PWM1KEY);
//        __builtin_write_PWMSFR(&P1FLTBCON,0x0000,&PWM1KEY);
//        __builtin_write_PWMSFR(&PWM1CON1 ,0x0077,&PWM1KEY);
//    
//    
//
////    PWM1CON1 = 0x0077;    //Enable all 3 channels
////    P1FLTACON = 0x0000;   //Disable faults
////    P1FLTBCON = 0x0000;
//
//    //Setup Wave freq/duty
//    //Fosc = 7.3728 MHz -> Fcy = 3.6864MHz
//    //Desire a PWM of 20250Hz (smaller scalar error than 20kHz)
//    //P1TPER = [Fcy/(Fpwm*Scalar)] - 1
//    //Therefore P1TPER = [3.6864M/(20250*1)] - 1 = 181;
////    P1TPER              = 181;
//    P1DC1               = 0x7fff;
//    P1DC2               = 0x7FFF;
//    P1DC3               = 0x7FFF;
//
////    P1OVDCON            = 0x3F00;   //Disable override; override disables PWM
////    PWM1CON2            = 0x0000;    
//    P1TCONbits.PTEN     = 1;        //Turn on
    
    P1TCONbits.PTOPS = 1; // PWM timer post-scale
    P1TCONbits.PTCKPS = 0; // PWM timer pre-scale
    P1TCONbits.PTMOD = 2; // PWM operates in Up-down Mode continuously
 
    P1TMR = 0; // PWM counter value, start at 0
 
    P1TPER = 19999; // PWM Timebase period
 
    PWM1CON1bits.PMOD3 = 0; // PWM in complimentary mode
    PWM1CON1bits.PMOD2 = 0; // PWM in complimentary mode
    PWM1CON1bits.PMOD1 = 0; // PWM in complimentary mode
    PWM1CON1bits.PEN3H = 1; // PWM High pin is enabled
    PWM1CON1bits.PEN2H = 1; // PWM High pin is enabled
    PWM1CON1bits.PEN1H = 1; // PWM High pin is enabled
    PWM1CON1bits.PEN3L = 1; // PWM Low pin enabled (direction control later?)
    PWM1CON1bits.PEN2L = 1; // PWM Low pin enabled (direction control later?)
    PWM1CON1bits.PEN1L = 1; // PWM Low pin enabled (direction control later?)
 
    //PWMCON2 = 0x0000; // PWM update info
 
    P1DTCON1bits.DTAPS = 0;  //DeadTime pre-scaler
    P1DTCON1bits.DTA = 59;   //DeadTime value for 4 us. 
 
    //FLTACON = 0x0000; // Fault A Control
 
    //OVDCON = 0x0000; // Override control info
 
    // Duty Cycle has a max value of 2xPeriod since output  
    // can change on rising or falling edge of Tcy
    P1DC1 = 30000; // PWM#1 Duty Cycle register (11-bit)
    P1DC2 = 19999; // PWM#2 Duty Cycle register (11-bit)
    P1DC3 = 19999; // PWM#3 Duty Cycle register (11-bit)
 
    P1TCONbits.PTEN = 1; // Enable PWM Timerbase!
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