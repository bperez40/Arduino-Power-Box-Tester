#include "PBTCheck.hpp"
/*
 * Instantiates PWM class for utilizing PWM abilities.
 */
PWM pwminst(ADCPIN, MAXSAMPLES, SAMPLEPERIOD); // ADCPIN defined in iodefs.hpp, others in PBTCheck.hpp
void ADCSetup()
{
/*
 * !DO NOT TOUCH! Correctly sets up ADC for fast sampling.
 */
#define FASTADC 1
#ifndef cbi // defines for setting and clearing register bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
/*
 * !DO NOT TOUCH! Correctly sets up ADC for fast sampling.
 */
#if FASTADC
    sbi(ADCSRA, ADPS2); // set prescale to 16
    cbi(ADCSRA, ADPS1); //
    cbi(ADCSRA, ADPS0); //
#endif
    /*
     * End of ADC Setup
     */
}
/*
 * Recommend "inverted" logic for event b/c hardware configuration is open drain.
 * If you so desire to switch the event trigger to HIGH you can.
 */
void waitUntilTriggered(int pin, int event = LOW)
{
    bool event_triggered = false;
    while (!event_triggered)
    {
        if (digitalRead(pin) == event)
        {
            event_triggered = true;
        }
    }
}

void dutyCheck(double low_threshold, double high_threshold)
{
    bool pwm_met = false;
    int *x = new int[MAXSAMPLES];
    double duty = 0;
    int duty_count = 0;
    while (!pwm_met)
    {
        pwminst.pwmMeasure(x);           // Take a thousand samples at frequency defined in the PWM library and store them to array x
        duty = pwminst.calcDutyCycle(x); // Caculate duty cycle from acquired samples
        if (duty <= high_threshold && duty >= low_threshold)
        {
            duty_count += 1;
        }
        if (duty_count >= CYCLES)
        {
            pwm_met == true;
        }
    }
    delete (x); // Delete allocated memory to prevent memory leaks
}

void halt(){
    while(true){
    }
}