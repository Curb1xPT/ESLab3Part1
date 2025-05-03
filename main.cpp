#include "mbed.h"
#include "arm_book_lib.h"


DigitalIn enterButton(BUTTON1);
DigitalIn gasDetector(D2);
DigitalIn overTempDetector(D3);
DigitalIn aButton(D4);
DigitalIn bButton(D5);
DigitalIn cButton(D6);
DigitalIn dButton(D7);

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

bool alarmState = OFF;
bool gasState = OFF;
bool tempState = OFF;
int numberOfIncorrectCodes = 0;


void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

void uartTask();
void availableCommands();


int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
        uartTask();
    }
}


void inputsInit()
{
    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
}

void outputsInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
}

void alarmActivationUpdate()
{
    if ( gasDetector || overTempDetector ) {
        alarmState = ON;
    }
    alarmLed = alarmState;

    if (gasDetector == ON) {
        gasState = ON;
    }

    if (overTempDetector == ON) {
        tempState = ON;
    }
}

void alarmDeactivationUpdate()
{
    if ( numberOfIncorrectCodes < 5 ) {
        if ( aButton && bButton && cButton && dButton && !enterButton ) {
            incorrectCodeLed = OFF;
        }
        if ( enterButton && !incorrectCodeLed && alarmState ) {
            if ( aButton && !bButton && !cButton && dButton ) {
                alarmState = OFF;
                gasState = OFF;
                tempState = OFF;
                numberOfIncorrectCodes = 0;
            } else {
                incorrectCodeLed = ON;
                numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            }
        }
    } else {
        systemBlockedLed = ON;
    }
}

void uartTask()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
        if ( receivedChar == '1') {
            if ( alarmState ) {
                uartUsb.write("\nThe alarm is activated.\r\n", 28);
            } else {
                uartUsb.write("\nThe alarm is not activated.\r\n", 32);
            }
        } 
        
        if (receivedChar == '2') {
            if (gasState) {
                uartUsb.write("\nThe gas detector is activated.\r\n", 33);
            } else {
                uartUsb.write("\nThe gas detector is not activated.\r\n", 37);
            }
        }

        if (receivedChar == '3') {
            if (tempState) {
                uartUsb.write("\nThe temperature detector is activated.\r\n", 41);
            } else {
                uartUsb.write("\nThe temperature detector is not activated.\r\n", 45);
            }
        }

        availableCommands();
        
    }
}

void availableCommands()
{
    uartUsb.write("\nAvailable commands:\r\n", 23);
    uartUsb.write("Press '1' to get the alarm state.\r\n", 36);
    uartUsb.write("Press '2' to get the gas detector state.\r\n", 44);
    uartUsb.write("Press '3' to get the temperature detector state.\r\n", 53);
}

