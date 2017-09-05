/* 
 * File:   main.c
 * Author: edouardyreux
 *
 * Created on September 4, 2017, 3:08 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <p33FJ32MC102.h>
#include <xc.h>


#define LED_ON  1
#define LED_OFF 0

#define INPUT  1
#define OUTPUT 0

#define LED_WHITE LATAbits.LATA0
#define LED_BLUE  LATAbits.LATA1
#define LED_RGB_R LATAbits.LATA2
#define LED_RGB_G LATAbits.LATA3
#define LED_RGB_B LATAbits.LATA4


#define TIMER_ON                    0x8000
#define TIMER_SOURCE_INTERNAL       0x0000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000
#define TIMER_PRESCALER_64          0x0020
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

/*
 * 
 */
void config_ra_output();
void config_timer();

int main(int argc, char** argv) {
    config_ra_output();
    LED_WHITE = LED_ON;
    config_timer();
    
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

    /* reset Timer 1 interrupt flag */
    IFS0bits.T1IF = 0 ;
}