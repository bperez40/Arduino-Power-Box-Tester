#pragma once
#include "Arduino.h"

/*
* Pin definitions. Should be left as is because these are constrained by the hardware.
* Generally needs to run before the main loop (so in setup) because the Arduino will only enable
* these pins after their set up.
*/
#define ADCPIN 69
#define SVALVECTRL 8
#define THCALLCTRL 9
#define DHWCALLCTRL 10
#define PUMPCTRL 11
#define BTNILLCTRL 12
#define BSKTCTRL 13
#define BTNSIG  54
#define RBSKTSIG 55
#define LBSKTSIG 56
#define ALARMSIG 57
#define BLPWRSIG 58
#define SVALVESIG 59
#define PMPWRSIG 60
#define PONSIG  61
#define GASVALVESIG 62
#define BLCTRLPWRSIG 63
#define RST 22
#define INT 23
#define WAIT 24
#define MISO 50
#define MOSI 51
#define SCK 52
#define CS 53
#define SPACTIVE 6
#define YP 64
#define YN 65
#define XN 66
#define XP 67
#define LITE 68

void initPins();