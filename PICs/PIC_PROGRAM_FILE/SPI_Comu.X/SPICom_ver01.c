

#include "SPICom_ver01.h"
#include <xc.h>



void SlaveInit(void){
    OSCCON = 0b00110100;    // ??????4Mhz(4xPLL???32MHz???)
    
    ANSELA = 0b00000000;    // RA????????
    TRISA = 0b00000001;     // RA0??????
    PORTA = 0b00000000;     // RA???
 
    // SPI??????????
    ANSELB = 0b00000000;
    TRISB = 0b00010010;     // RB1(7/SDI1)?RB4(10/SCK1)???
    PORTB = 0b00000000;     // RB???
    
    SDO1SEL = 0;             // RB2?SDO?????
    SSP1CON1 = 0b00100100;    // ?????SS??
    SSP1STAT = 0b01000000;    // ????, ???????LOW
    
    SSP1IF = 0;  // SPI?????????????
    SSP1IE = 1;  // SPI?????????
    PEIE = 1;   // ????????????
    GIE = 1;   // ???????????
}

void SlaveStart(void){
    if (SSP1IF == HIGH){  // SPI???????
        SSP1IF = LOW;     // SPI????????
    }
    
}

int SlaveRec()
{
    unsigned char read_data;
    read_data = SSP1BUF;      
    return_data = read_data;
    return return_data;
}

void SlaveSen(int sendData)
{
    SSP1BUF = sendData;
}