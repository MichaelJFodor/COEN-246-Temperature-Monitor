#include <wiringPi.h>
#include <stdio.h>

#define LedPin 2 // change LED pin as necessary

int main(void) {
        if(wiringPiSetup() == -1) { //when initialize wiringPi failed, print message to screen
                printf("setup wiringPi failed !\n");
                return -1;
        }

        pinMode(LedPin, OUTPUT);
        if(1) {
                digitalWrite(LedPin, LOW);   //led on
                printf("led off\n");
        }
        return 0;
}
