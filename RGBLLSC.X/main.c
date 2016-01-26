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

#define I2CADDRESS 0x20

/***********************************************
 * global variables                            *
 ***********************************************/
int static x=0;
unsigned char static color[3] ={0,0,0}; 

int random;
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
    SSP1ADD=I2CADDRESS;       // slave I2C address 
    
    /***********************************************
     * interrupt configuration                     *
     ***********************************************/
    PIE1bits.SSP1IE=1; // enables the MSSP interrupt 
    //PIR1=0x00;         // clear interrupt flags
    //PIR2=0x00;         //
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1; // enable peripheral interrupts
    
    SSP1CON1bits.SSPEN=1; // enable the MSSP module 
    
    /***********************************************
     * LED loop: this loop sends PWM signals to    *
     * the LEDs based upon the R,G and B variables *
     ***********************************************/
    RED=1; BLU=1; GRN=1;             // start the LED loop with LEDs on 
    while(1) {                       // loop forever 
        if(Rx<color[0])                     // if the time for the RED LED to be on/off has not ended 
            Rx++;                    //     increment the time 
        else {                       // else if it has 
            RED=~RED; color[0]=254-color[0]; Rx=0; //     clear the time and flip the LED state 
        }                            //
        
        if(Gx<color[1])                     // if the time for the GREEN LED to be on/off has not ended 
            Gx++;                    //     increment the time
        else {                       // else if it has
            GRN=~GRN; color[1]=254-color[1]; Gx=0; //     clear the time and flip the LED state
        }                            // 
        
        if(Bx<color[2])                     // if the time for the BLUE LED to be on/off has not ended
            Bx++;                    //     increment the time
        else {                       // else if it has
            BLU=~BLU; color[2]=254-color[2]; Bx=0; //     clear the time and flip the LED state
        }                            //
    }                                 
    
    while(1); // stop
}

/***********************************************
 * interrupt vector                            *
 ***********************************************/
void interrupt isr(void) {
    if(PIR1bits.SSP1IF==1) {
        PIR1bits.SSP1IF=0; 
        //while(SSPSTATbits.P==0){ // wait for a stop bit 
            if(SSPSTATbits.D_nA==0){ // if the last byte received was a address 
                random=SSP1BUF; // dummy read
                CKP=1; // release SCL line
                
                SSP1CON1bits.SSPOV=0;
                SSP1CON1bits.WCOL=0;
                
                return;
                
                //while(PIR1bits.SSP1IF==0);    // wait for next byte
                //PIR1bits.SSP1IF=0;            // clear the flag
            }
            if(SSPSTATbits.D_nA==1) { // if the last byte received was data
                color[x]=SSP1BUF;
                CKP=1;  // release the SCL line 
                x++;
                if(x==3) 
                    x=0;
                
                SSP1CON1bits.SSPOV=0;
                SSP1CON1bits.WCOL=0;
                
                return;
                
                //while(PIR1bits.SSP1IF==0); // wait for next byte 
               // PIR1bits.SSP1IF=0;         // clear the flag
            } 
        //} 
    }
    
    return;  
}