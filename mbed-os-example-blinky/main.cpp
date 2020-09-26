/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     2000ms


void addOne_val(int i) {
  i++; // Has no effect outside this scope because this is a copy of the original
}

// Best -> I ne peut pas etre null !!
void addOne_ref(int& i) {
  i++; // Actually changes the original variable
}

void addOne(int* i) {
  (*i)++; // Actually changes the original variable, but i can be null ! 
}

int main()
{
    int i = 0;
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);

    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);

        
  
        addOne_val(i);
        printf("1 value of i is %d\n", i);


        addOne_ref(i);
        printf("2 value of i is %d\n", i);

        addOne(&i);
        printf("3 value of i is %d\n", i);
    }
}
