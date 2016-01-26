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
#define _XTAL_FREQ 32000000 // must be defined for __delay to work
#define I2CADDRESS 0x20     // the devices address

#define RED PORTAbits.RA0   // the RED LED
#define GRN PORTAbits.RA4   // the GREEN LED
#define BLU PORTAbits.RA5   // the BLUE LED
 
/***********************************************
 * global variables - NOTE: all variables are  *
 * declared global because you can't pass      *
 * variables to a ISR                          *
 ***********************************************/
int static ColorSel=0;                  // holds the current selected color
unsigned char static color[3] ={0,0,0}; // holds the PWM value for each color
int Rx=0; int Gx=0; int Bx=0;           // holds the PWM count
int random;                             // dummy variable 

/***********************************************
 * main function                               *
 ***********************************************/
void main(void) {
    
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
    INTCONbits.GIE=1;  // global interrupt enable 
    INTCONbits.PEIE=1; // enable peripheral interrupts
    
    SSP1CON1bits.SSPEN=1; // enable the MSSP module 
    
    /***********************************************
     * LED loop: this loop sends PWM signals to    *
     * the LEDs based upon the R,G and B variables *
     ***********************************************/
    RED=1; BLU=1; GRN=1;        // start the LED loop with LEDs on 
    while(1) {                  // loop forever 
        if(Rx<color[0])         // if the time for the RED LED to be on/off has not ended 
            Rx++;               //     increment the time 
        else {                  // else if it has 
            RED=~RED;           //
            color[0]=~color[0]; //
            Rx=0;               // clear the time and flip the LED state 
        }                       //
        
        if(Gx<color[1])         // if the time for the GREEN LED to be on/off has not ended 
            Gx++;               //     increment the time
        else {                  // else if it has
            GRN=~GRN;           // clear the time and flip the LED state
            color[1]=~color[1]; //
            Gx=0;               //     
        }                       // 
        
        if(Bx<color[2])         // if the time for the BLUE LED to be on/off has not ended
            Bx++;               //     increment the time
        else {                  // else if it has
            BLU=~BLU;           // clear the time and flip the LED state
            color[2]=~color[2]; //
            Bx=0;               //     
        }                       //
    }                                   
    while(1);                   // stop
}

/***********************************************
 * interrupt vector                            *
 ***********************************************/
void interrupt isr(void) {
    if(PIR1bits.SSP1IF==1) {      // if the MSSP caused the interrupt
        PIR1bits.SSP1IF=0;        // clear the MSSP flag
        if(SSPSTATbits.D_nA==0){  // if the last byte received was a address 
            random=SSP1BUF;       // dummy read
            CKP=1;                // release SCL line
                                  //
            SSP1CON1bits.SSPOV=0; // clear any errors 
            SSP1CON1bits.WCOL=0;  //
            return;               // return back to LED loop
        }
        if(SSPSTATbits.D_nA==1){  // if the last byte received was data
            color[ColorSel]=SSP1BUF; // load the data into the right color 
            switch(ColorSel){     // if the selected color is
                case 0:           //    RED clear the RED LED   
                    RED=0;        //
                    break;        //
                case 1:           //    GREEN clear the GREEN LED
                    GRN=0;        //
                    break;        //
                case 2:           //    BLUE clear the BLUE LED
                    BLU=0;        //
                    break;        //
            }                     //
            ColorSel++;           // increment the selected color 
            if(ColorSel==3)       // if the selected color has rolled over
                ColorSel=0;       //    reset it 
            Rx=0; Gx=0; Bx=0;     // clear each color PWM count 
            CKP=1;                // release the SCL line
                                  //
            SSP1CON1bits.SSPOV=0; // clear any errors 
            SSP1CON1bits.WCOL=0;  //
            return;               // return back to the LED loop 
        }       
    }   
    return;  
}