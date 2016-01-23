/***********************************************
 * File:   main.c                              *
 * Author: Supercap2f                          *
 *                                             *
 * Created on January 22, 2016, 7:49 AM        *
 ***********************************************/
/***********************************************
 * includes                                    *
 ***********************************************/
#include <xc.h>

/***********************************************
 * device configuration                        *
 ***********************************************/
#pragma config CLKOUTEN=OFF // clock out function is on
#pragma config WDTE=OFF     // watch dog timer is off
#pragma config PWRTE=ON     // power up timer is on
#pragma config CP=OFF       // code protection is off
#pragma config BOREN=OFF    // brown out reset is off *MAY NEED TO BE CHANGED*
#pragma config FCMEN=OFF    // fail safe clock monitor is off
#pragma config MCLRE=ON     // MCLRE is reset *MAKE SURE IT HAS A RESISTOR!*
#pragma config CPD=OFF      // code protection is off
#pragma config IESO=OFF     // internal/external switchover is disabled *MAY NEED TO BE CHANGED*
#pragma config FOSC=INTOSC  // the oscillator is INTOSC
#pragma config PLLEN=ON     // turn on the 4x PLL

/***********************************************
 * definitions                                 *
 ***********************************************/
#define RED PORTAbits.RA0
#define GRN PORTAbits.RA4
#define BLU PORTAbits.RA5

/***********************************************
 * main function                               *
 ***********************************************/
void main(void) {
    /***********************************************
     * variable definitions                        *
     ***********************************************/
    
    
    /***********************************************
     * oscillator configuration                    *
     ***********************************************/
    OSCCONbits.SCS=0;       // set oscillator to 32MHz Tcy will be 8MHz
    OSCCONbits.IRCF=0b1110; //
    
    /***********************************************
     * pin setup                                   *
     ***********************************************
     *    | RA0 - RED | RA1 - SCL | RA2 - SDA |    * 
     *    | RA3 - RST | RA4 - GRN | RA5 - BLU |    *
     ***********************************************/
    TRISA=0b00001110;  // turn I/Os to I (1) or O (0)
    ANSELA=0b00000000; // make all pins digital
    
    
    while(1); // stop
}
