#include "SPIPWM_ver01.h"

#include <xc.h>
#include <stdbool.h>    // bool???????????

unsigned char PWM = 00000000;
 
// ????????
#pragma config BORV = HI      
bool InitDevice();                         // ????????? 
bool ChangeRightMotorSpeed(int per);     // ????????????
int sousinData = 0;
int jusinData = 0;

void interrupt OnInterSpi()
{
    SlaveStart();
    jusinData = SlaveRec();
    PWM = jusinData;
    sousinData = PWM;
    SlaveSen(sousinData);
}

 



void main(void)
{
    SlaveInit();
    
    // ???????
    if(!InitDevice()){
        // ??????
    }
    
    
    while(1)
    {
        ChangeRightMotorSpeed(PWM);
    };
    
}

bool InitDevice()
{
    // ???????
    OSCCON = 0b01110010;    // ?????8MHz???
    
    // ????????????
    TRISA = 0b00000000;
    TRISB = 0b00000000;
    
    // ????????????????
    ANSELA = 0b00000000;
    ANSELB = 0b00000000;
    
    // CCP?????????
    CCP3CON = 0b00001100;   // CCP3?PWM?????????CCPR3L???2????0????
    CCP4CON = 0b00001100;   // CCP4?PWM?????????CCPR4L???2????0????
    
    // CCP??????????
    CCPTMRS = 0b00000000;   // CCP3?CCP4?????????Timer2
    
    // ??????
    T2CON = 0b00000100;     // TIMER2??????????????????
    PR2 = 255;              // PWM Period?1.28 * 10^4(= (255(PR2) + 1) * 1 / 8000000(CLOCK) * 1(Prescale))
    
    return true;
}
 
 
// ????????????
bool ChangeRightMotorSpeed(int pwm)
{
            CCPR3L = pwm;
    return true;
}

