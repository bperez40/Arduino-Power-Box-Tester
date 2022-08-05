#include <SPI.h>
#include "PBTGraphics.hpp"

#define GLOBAL_TIME_LIMIT 8000 // Important define: Sets the time for a signal detection during auto test
#define PIM_TIME_LIMIT 10000
bool test_status = false;

void setup()
{
  Serial.begin(115200);
  initPins();
  initDisplay();
  drawMainMenu();
}

void loop()
{
  touchCheck();
  switch (getOption())
  {
  /* Case 1 is draw test menu and start test*/
  case 1:
    drawPreTestMenu(1);
    setActiveMenu(PRETEST);
    //  Function to setup fast ADC sampling
    ADCSetup();

    /* Reset test variables*/
    test_status = false; // Test is unsuccessful by default. Every test start, reset var
    /*
     * Start of PIM check
     */
    digitalWrite(POWERCTRL, HIGH);
    
    setSignalTimeout(PIM_TIME_LIMIT, POWERONOP);
    setSignalAlarm(waitUntilTriggered(PONSIG, getSignalTimeout(POWERONOP)), POWERONOP);

    /* IMPORTANT
     * You must have an abort exception if the relay on the PBT board is not powered
     * this will cause a 24VAC path to draw high current through where it shouldn't,
     * notably in the left basket and right basket tests.
     */

    if (getSignalAlarm(POWERONOP))
    {
      digitalWrite(POWERCTRL, LOW);
      drawAbortMenu();
      setActiveMenu(ABORTMENU);
    }
    else
    {
      digitalWrite(THCALLCTRL, HIGH); // Call for heat

      /*
       * Check for blower power. Otherwise, wait.
       */
      drawPreTestMenu(2);

      setSignalTimeout(PIM_TIME_LIMIT, BLOWERPOWEROP);
      setSignalAlarm(waitUntilTriggered(BLPWRSIG, getSignalTimeout(BLOWERPOWEROP)), BLOWERPOWEROP);

      setSignalTimeout(PIM_TIME_LIMIT, BLOWERCONTROLOP);
      setSignalAlarm(waitUntilTriggered(BLCTRLPWRSIG, getSignalTimeout(BLOWERCONTROLOP)), BLOWERCONTROLOP);

      setSignalTimeout(PIM_TIME_LIMIT, BLOWERPOWERNEUTRALOP);
      setSignalAlarm(waitUntilTriggered(BLPWRNEUSIG, getSignalTimeout(BLOWERPOWERNEUTRALOP)), BLOWERPOWERNEUTRALOP);

      drawPreTestMenu(4);
      setSignalTimeout(PIM_TIME_LIMIT, GASVALVEOP);
      setSignalAlarm(waitUntilTriggered(GASVALVESIG, getSignalTimeout(GASVALVEOP)), GASVALVEOP);

      /* This is where spark detection would go */

      drawPreTestMenu(3);
      setSignalTimeout(PIM_TIME_LIMIT, LOWDUTYCYCLEOP);
      /* For this first parameter, you can either use dutyCheck or waitUntilTriggered.
       * Wait until triggered will utilize the hardware PWM detection built into the board.
       * dutyCheck will use the firmware DSP approach
       * Generally recommend using waitUntilTriggered for now as it tend to be more reliable.
       * dutyCheck(LDLB, LDHB, getSignalTimeout(HIGHDUTYCYCLEOP))
       */
      setSignalAlarm(dutyCheck(LDLB, LDHB), LOWDUTYCYCLEOP); /* dutyCheck(LDLB, LDHB, getSignalTimeout(LOWDUTYCYCLEOP)) */

      drawPreTestMenu(5);
      setSignalTimeout(500, ALARMOP); // Might require adjusting to wait for alarm signal to go high
      setSignalAlarm(waitUntilTriggered(ALARMSIG, getSignalTimeout(ALARMOP), HIGH), ALARMOP);

      drawPreTestMenu(6);
      
      setSignalTimeout(PIM_TIME_LIMIT, HIGHDUTYCYCLEOP);
      /* For this first parameter, you can either use dutyCheck or waitUntilTriggered.
       * Wait until triggered will utilize the hardware PWM detection built into the board.
       * dutyCheck will use the firmware DSP approach
       * Generally recommend using waitUntilTriggered for now as it tend to be more reliable.
       * dutyCheck(HDLB, HDHB, getSignalTimeout(HIGHDUTYCYCLEOP))
       */
      setSignalAlarm(dutyCheck(HDLB, HDHB), HIGHDUTYCYCLEOP);
      /*
       *
       * End of PIM check
       *
       */

      /*
       * Filter electronics check
       * If configuration is set to 1 (non-filter), then we should skip the filter system tests.
       * Otherwise, we'll execute the filter system tests.
       */
      if (getConfiguration() == 1){
      }
      else{
      drawPreTestMenu(9);

      digitalWrite(PUMPCTRL, HIGH); // Enable relay to provide pump motor power

      setSignalTimeout(GLOBAL_TIME_LIMIT, PUMPPOWEROP);
      setSignalAlarm(waitUntilTriggered(PMPWRSIG, getSignalTimeout(PUMPPOWEROP)), PUMPPOWEROP);
      drawPreTestMenu(7);
      digitalWrite(SVALVECTRL, HIGH); // Should make that SVALVESIG signal active
      setSignalTimeout(GLOBAL_TIME_LIMIT, SOLENOIDVALVEOP);
      setSignalAlarm(waitUntilTriggered(SVALVESIG, getSignalTimeout(SOLENOIDVALVEOP), HIGH), SOLENOIDVALVEOP);

      /* If this part succeeds, test will progress */
      drawPreTestMenu(8);
      digitalWrite(SVALVECTRL, LOW);
      setSignalAlarm(waitUntilTriggered(SVALVESIG, getSignalTimeout(SOLENOIDVALVEOP)), SOLENOIDVALVEOP);
      /* Note that this solenoid check needs to happen after
       * the pump relay is activated, otherwise, it
       * will not be powered — even if the pump motor relay
       * is activated
       */
      }
      /*
       * Basket lift check
       */

      drawPreTestMenu(10);

      digitalWrite(BSKTCTRL, HIGH); // Activate basket control relay

      setSignalTimeout(GLOBAL_TIME_LIMIT, BASKETPOWEROP);
      setSignalAlarm(waitUntilTriggered(BSKTPWRSIG, getSignalTimeout(BASKETPOWEROP)), BASKETPOWEROP);

      drawPreTestMenu(11);

      setSignalTimeout(100, LEFTBASKETOP);
      setSignalAlarm(waitUntilTriggered(LBSKTSIG, getSignalTimeout(LEFTBASKETOP)), LEFTBASKETOP);

      drawPreTestMenu(12);

      setSignalTimeout(100, RIGHTBASKETOP);
      setSignalAlarm(waitUntilTriggered(RBSKTSIG, getSignalTimeout(RIGHTBASKETOP)), RIGHTBASKETOP);

      /*
       * End of basket lift check
       */
      switch(getConfiguration()){
      case 0:
        if (getSignalAlarm(HIGHDUTYCYCLEOP) || getSignalAlarm(LOWDUTYCYCLEOP)|| getSignalAlarm(ALARMOP) || getSignalAlarm(BASKETPOWEROP) || getSignalAlarm(BLOWERPOWERNEUTRALOP) || getSignalAlarm(BLOWERCONTROLOP) || getSignalAlarm(RIGHTBASKETOP) || getSignalAlarm(SOLENOIDVALVEOP) || getSignalAlarm(PUMPPOWEROP) || getSignalAlarm(GASVALVEOP) || getSignalAlarm(BLOWERPOWEROP) || getSignalAlarm(POWERONOP))
        {
          test_status = false;
        }
        else
        {
          test_status = true;
        }
        break;
      case 1:
        if (getSignalAlarm(HIGHDUTYCYCLEOP) || getSignalAlarm(LOWDUTYCYCLEOP)|| getSignalAlarm(ALARMOP) || getSignalAlarm(BASKETPOWEROP) || getSignalAlarm(BLOWERPOWERNEUTRALOP) || getSignalAlarm(BLOWERCONTROLOP) || getSignalAlarm(RIGHTBASKETOP) || getSignalAlarm(GASVALVEOP) || getSignalAlarm(BLOWERPOWEROP) || getSignalAlarm(POWERONOP))
        {
          test_status = false;
        }
        else
        {
          test_status = true;
        }
        break;
      default:
        if (getSignalAlarm(HIGHDUTYCYCLEOP) || getSignalAlarm(LOWDUTYCYCLEOP)|| getSignalAlarm(ALARMOP) || getSignalAlarm(BASKETPOWEROP) || getSignalAlarm(BLOWERPOWERNEUTRALOP) || getSignalAlarm(BLOWERCONTROLOP) || getSignalAlarm(RIGHTBASKETOP) || getSignalAlarm(SOLENOIDVALVEOP) || getSignalAlarm(PUMPPOWEROP) || getSignalAlarm(GASVALVEOP) || getSignalAlarm(BLOWERPOWEROP) || getSignalAlarm(POWERONOP))
        {
          test_status = false;
        }
        else
        {
          test_status = true;
        }
        break;
      }
      /* Disable GPIO */
      digitalWrite(POWERCTRL, LOW);
      digitalWrite(THCALLCTRL, LOW);
      digitalWrite(SVALVECTRL, LOW);
      digitalWrite(PUMPCTRL, LOW);
      digitalWrite(BSKTCTRL, LOW);

      /* Proceed to next menu */
      drawPostTestMenu(test_status);
      setActiveMenu(POSTTEST);
    }
    break;
  /* Case 2 will bring you back to the main menu and reset necessary variables */
  case 2:
    drawMainMenu();
    setActiveMenu(MAINMENU);
    break;
  /* Case 3 happens when the results button is pressed. It will set up the results screen. */
  case 3:
    drawResultsMenu();
    setActiveMenu(RESULTS);
    break;
  /* Case 4 happens when the PIM info button is pressed on the results screen */
  case 4:
    drawPIMInfoMenu();
    setActiveMenu(PIMINFO);
    break;
  case 5:
    drawPumpInfoMenu();
    setActiveMenu(PUMPINFO);
    break;
  case 6:
    drawBasketInfoMenu();
    setActiveMenu(BSKTINFO);
    break;
  case 8:
    drawConfigurationMenu();
    setActiveMenu(CONFIG);
    break;
  case 9:
    drawDebugMenu();
    setActiveMenu(DEBUG);
    break;
  case 10:
    drawConnectionsMenu();
    setActiveMenu(CONN);
    break;
  }
}