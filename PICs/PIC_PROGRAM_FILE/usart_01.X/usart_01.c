/*
 * File:   main.c
 * Author: amutake
 *
 * Created on November 22, 2015, 1:43 PM
 * 
 * ?????????????? (char ?) ???? Analog In ???????????
 * ??????????
 * 
 * RB1: RX
 * RB2: TX
 * RB3: Analog In
 */


#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000 // 8MHz


// ???????????????
#pragma config FOSC = INTOSC    // ???????????(INTOSC)
#pragma config WDTE = OFF       // ???????????????(OFF)
#pragma config PWRTE = ON       // ??ON??64ms??????????????(ON)
#pragma config MCLRE = OFF      // ??????????????????????(RA5)??????(OFF)
#pragma config CP = OFF         // ?????????????????(OFF)
#pragma config CPD = OFF        // ??????????????(OFF)
#pragma config BOREN = ON       // ????????????ON(ON)
#pragma config CLKOUTEN = OFF   // CLKOUT????RA6????????(OFF)
#pragma config IESO = OFF       // ????????????????????(OFF)
#pragma config FCMEN = OFF      // ???????????(FCMEN_OFF)

// ???????????????
#pragma config WRT = OFF        // Flash??????????(OFF)
#pragma config PLLEN = OFF      // ???????32MHz????????(OFF)
#pragma config STVREN = ON      // ?????????????????????????????(ON)
#pragma config BORV = HI        // ????????????(2.5V)??(HI)
#pragma config LVP = OFF        // ?????????????????(OFF)

volatile char RXBUFF;
volatile short RXSET;

void interrupt isr() {
    if (RCIF == 1) {
        RXBUFF = RCREG;
        RXSET = 1;
        RCIF = 0;
    }
}

char wait_recv() {
    while (RXSET == 0);
    char c = RXBUFF;
    RXSET = 0;
    return c;
}

void send_char(char c) {
    while (TXIF == 0) {
        // wait...
    }
    TXREG = c;
}

void send_line(char str[]) {
    for (short i = 0; str[i] != '\0'; i++) {
        send_char(str[i]);
    }
    send_char('\n');
}

unsigned int adconv() {
    unsigned int temp;

    GO_nDONE = 1; // ????????????
    while (GO_nDONE); // ?????????
    temp = ADRESH; // ADRESH ? ADRESL ??????
    temp = (temp << 8) | ADRESL; // ????????????

    return temp;
}

void loop() {
    wait_recv();
    unsigned int v = adconv();
    char str[10];
    sprintf(str, "%d", v);
    send_line(str);
}

void main(void) {
    // general settings
    OSCCON = 0b01110010;
    ANSELA = 0b00000000; // RA0-RA7, ???????????
    ANSELB = 0b00001000; // RB0-RB7, ??????????? (RB3)
    TRISA = 0b00000000; // ??(RA)??????????
    TRISB = 0b00001010; // ??(RB)??????????
    PORTA = 0b00000000; // RA????????(??LOW???)
    PORTB = 0b00000000; // RB????????(??LOW???)
    // A/D converter settings
    ADCON1 = 0b10010000;    // ????????A/D???????FOSC/8?VDD??????????
    ADCON0 = 0b00100101;    // ??????????(AN9?????)
    __delay_us(5);          // ???????????????????????

    // serial communication settings
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;
    TXCKSEL = 0; // pin8 as TX
    RXDTSEL = 0; // pin7 as RX
    SPBRG = 51; // 9600
    RCIF = 0;
    RCIE = 1; // receive interruption enabled
    PEIE = 1 ; // peripheral interruption enabled
    GIE = 1; // global interruption enabled
    RXSET = 0; // ?????????????

    while (1) {
        loop();        
    }
    
    return;
}
