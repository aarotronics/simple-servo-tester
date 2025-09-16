/*
                           ATTINY85 SIMPLE SERVO TESTER
                  ===============================================

   Basic servo tester based on ATTiny85 uC up to 12V input voltage. Three function
   modes:

      1 --> MANUAL (Default when power up): change signal from 'MIN_PULSE' to
            'MAX_PULSE' with potentiometer position

      2 --> AUTO (One button clic from MANUAL): signal automatically changes from
            'MIN_PULSE' to 'MAX_PULSE' and back. Potentiometer changes the speed of
            increase and decrease in signal change

      3 --> NEUTRAL (Another button clic from AUTO): output signal will be
            'PWM_NEUTRAL' constantly. Another button clic in this mode will
            loop-return to the MANUAL Default state


   "THE BEERWARE LICENSE" (Revision 42):

   Aaron G. wrote this code in May 2019. As long as you retain this
   notice, you can do whatever you want with this stuff. If we meet someday,
   and you think this stuff is worth it, you can buy me a beer in return.

*/


#define INVERT_POT
#define INVERT_OUTPUT

#define BUTTON_PIN      0
#define POT_PIN         A1
#define OUTPUT_PIN      1
#define LED_MANUAL      3
#define LED_AUTO        4

#define MAX_PULSE       2012  //us (FrSky)
#define MIN_PULSE       988   //us (FrSky)
#define PWM_NEUTRAL     1500  //us (FrSky)
#define PWM_FREQ        50.0  //Hz



int pulseDuration;
int periodTime = (int)(1000.0 / PWM_FREQ);
int swingSteps;
long prevPulseStartTime;
bool manualMode = true;
bool autoMode = false;
bool neutralMode = false;
bool positiveSteps = false;



void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_MANUAL, OUTPUT);
  pinMode(LED_AUTO, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(LED_MANUAL, HIGH);
  digitalWrite(LED_AUTO, LOW);
#ifdef INVERT_OUTPUT
  digitalWrite(OUTPUT_PIN, HIGH);
#else
  digitalWrite(OUTPUT_PIN, LOW);
#endif
}

void loop() {

  //MANUAL MODE
  if (manualMode) {

    //Read potentiometer
#ifdef INVERT_POT
    pulseDuration = map(analogRead(POT_PIN), 0, 1023, MAX_PULSE, MIN_PULSE);
#else
    pulseDuration = map(analogRead(POT_PIN), 0, 1023, MIN_PULSE, MAX_PULSE);
#endif

    //Output signal to servo
    if ((millis() - prevPulseStartTime) >= periodTime) {
      prevPulseStartTime = millis();
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, LOW);
#else
      digitalWrite(OUTPUT_PIN, HIGH);
#endif
      delayMicroseconds(pulseDuration);
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, HIGH);
#else
      digitalWrite(OUTPUT_PIN, LOW);
#endif
    }
  }

  //AUTO MODE
  else if (autoMode) {

#ifdef INVERT_POT
    swingSteps = map(analogRead(POT_PIN), 0, 1023, 60, 5);
#else
    swingSteps = map(analogRead(POT_PIN), 0, 1023, 5, 60);
#endif

    if ((millis() - prevPulseStartTime) >= periodTime) {
      prevPulseStartTime = millis();
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, LOW);
#else
      digitalWrite(OUTPUT_PIN, HIGH);
#endif
      delayMicroseconds(pulseDuration);
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, HIGH);
#else
      digitalWrite(OUTPUT_PIN, LOW);
#endif
      if (positiveSteps) {
        pulseDuration = pulseDuration + swingSteps;
      } else {
        pulseDuration = pulseDuration - swingSteps;
      }
    }

    if (pulseDuration >= MAX_PULSE) {
      positiveSteps = false;
    } else if (pulseDuration <= MIN_PULSE) {
      positiveSteps = true;
    }
  }

  //NEUTRAL MODE
  else if (neutralMode) {
    if ((millis() - prevPulseStartTime) >= periodTime) {
      prevPulseStartTime = millis();
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, LOW);
#else
      digitalWrite(OUTPUT_PIN, HIGH);
#endif
      delayMicroseconds(PWM_NEUTRAL);
#ifdef INVERT_OUTPUT
      digitalWrite(OUTPUT_PIN, HIGH);
#else
      digitalWrite(OUTPUT_PIN, LOW);
#endif
    }
  }

  //READ BUTTON
  if (!digitalRead(BUTTON_PIN)) {
    delay(20); // Debounce switch
    while (!digitalRead(BUTTON_PIN));
    if (manualMode) {
      digitalWrite(LED_MANUAL, LOW);
      digitalWrite(LED_AUTO, HIGH);
      manualMode = false;
      autoMode = true;
      neutralMode = false;
    } else if (autoMode) {
      digitalWrite(LED_MANUAL, HIGH);
      digitalWrite(LED_AUTO, HIGH);
      manualMode = false;
      autoMode = false;
      neutralMode = true;
    } else if (neutralMode) {
      digitalWrite(LED_MANUAL, HIGH);
      digitalWrite(LED_AUTO, LOW);
      manualMode = true;
      autoMode = false;
      neutralMode = false;
    }
    delay(20);
  }
}
