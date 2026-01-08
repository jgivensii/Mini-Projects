/*
Samples photodiode output
Averages values and reports values, rms’, error estimates.
Provides same values integrated over a short interval, which
will have lower noise.
Not calibrated.
DVB 20200217.1
*/
#include <avr/pgmspace.h>
#include "TimerOne.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define FS(x) (__FlashStringHelper*)(x)

const char DEVICE_ID[] PROGMEM = {"DEVICE, Arduino UNO R3 + BTW34 Photodiode + LM386"};
const char SERVICE_ID[] PROGMEM = {"SERVICE, Radiometer Mark 1"};
const char VERSION_ID[] PROGMEM = {"VERSION, 20200217.1"};

const char* DELIMITER = ", ";
const char* DEVICE = "ARDUINO UNO R3";

volatile int RUNNING = 0;
volatile long PERIOD = 1000000;
volatile int fastN = 1;

#define FASTADC 1

#ifndef cbi
#define cbi(sfr, bit)(_SFR_BYTE(sfr) &= -_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int value[100];
int i=0;

void setupFASTADC()
{

 #if FASTADC
 sbi(ADCSRA,ADPS2);
 cbi(ADCSRA, ADPS1);
 cbi(ADCSRA, ADPS0);
 #endif
}
