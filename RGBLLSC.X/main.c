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
#define _XTAL_FREQ 32000000

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
    int R=0;  int G=0;  int B=0; 
    int Rx=0; int Gx=0; int Bx=0;
    
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
    
    /***********************************************
     * I2C configuration                           *
     ***********************************************/
    SSPSTATbits.CKE=1;        // input thresholds are compliant with SMBus
    SSP1CON1bits.SSPM=0b0110; // I2C 7 bit address mode with no STOP/START interrupts *MAY NEED TO BE CHANGED*
    SSP1CON2bits.GCEN=1;      // enable general call address interrupt 
    SSP1CON2bits.SEN=1;       // clock will be stretched after each cycle 
    SSP1CON3bits.PCIE=0;      // stop condition interrupt disabled
    SSP1CON3bits.BOEN=0;      // SSP1BUF is only updated when SSP1OV is clear
    SSP1ADD=0xA2;             // slave I2C address 
    
    /***********************************************
     * interrupt configuration                     *
     ***********************************************/
    PIE1bits.SSP1IE=1; // enables the MSSP interrupt 
    PIR1=0x00;         // clear interrupt flags
    PIR2=0x00;         //
    INTCONbits.PEIE=1; // enable peripheral interrupts
    
    SSP1CON1bits.SSPEN=1; // enable the MSSP module 
    
    
    
    
    
    
    R=254;
    G=55;
    B=0;
    
    /***********************************************
     * LED loop: this loop sends PWM signals to    *
     * the LEDs based upon the R,G and B variables *
     ***********************************************/
    RED=1; BLU=1; GRN=1;             // start the LED loop with LEDs on 
    while(1) {                       // loop forever 
        if(Rx<R)                     // if the time for the RED LED to be on/off has not ended 
            Rx++;                    //     increment the time 
        else {                       // else if it has 
            RED=~RED; R=254-R; Rx=0; //     clear the time and flip the LED state 
        }                            //
        
        if(Gx<G)                     // if the time for the GREEN LED to be on/off has not ended 
            Gx++;                    //     increment the time
        else {                       // else if it has
            GRN=~GRN; G=254-G; Gx=0; //     clear the time and flip the LED state
        }                            // 
        
        if(Bx<B)                     // if the time for the BLUE LED to be on/off has not ended
            Bx++;                    //     increment the time
        else {                       // else if it has
            BLU=~BLU; B=254-B; Bx=0; //     clear the time and flip the LED state
        }                            //
    }                                 
    
    while(1); // stop
}
