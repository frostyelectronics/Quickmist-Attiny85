//
//            +-\/-+
// LED5 PB5  1|    |8  Vcc
// LED3 PB3  2|    |7  PB2 BUTTON
// LED4 PB4  3|    |6  PB1 LED2
//      GND  4|    |5  PB0 LED1
//            +----+

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile int wdtTimeout = 0;            // Flag for WDT interrupt
volatile int buttonInterrupt = 0;       // Flag for button interrupt
volatile int debounceCounter = 0;       // Debouncing button
volatile int cyclesUntilDebounce = 0;   // Sleep cycles for debounce * 16ms

const int buttonPin = 2;          // PB2 as buttonPin
int arrLED[] = {0, 1, 3, 4};      // Green LEDs pin number
int numLED = 4;                   // Total Number of LEDs
int redLED = 5;                   // Red LED pin number

long arr60Timers[] = {50L, 100L, 150L, 200L};   // Initial time-values for Green LEDs
long arr15Timer = 250L;                         // Initial time-value for Red LED

long timer60Add = 230000L; // How much time to add when button is pressed. Green LEDs. 230000 = 58,00 min with 16ms ticks
long timer15Add = 57500L;  // How much time to add when button is pressed. Red LED. 57500 =  14,50 min with 16ms ticks


/******************************************************************/
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int sleeptime) {
  byte wdtia;
  int wdtvu;
  if (sleeptime > 9 ) sleeptime = 9;
  wdtia = sleeptime & 7;
  if (sleeptime > 7) wdtia |= (1 << 5);
  wdtia |= (1 << WDCE);
  wdtvu = wdtia;
  MCUSR &= ~(1 << WDRF);
  WDTCR |= (1 << WDCE) | (1 << WDE);
  WDTCR = wdtia;
  WDTCR |= _BV(WDIE);
}


void system_sleep() {
  sei();                               // enable interrupts
  cbi(ADCSRA, ADEN);                   // Analog to Digital converter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues here
  sbi(ADCSRA, ADEN);                   // Analog to Digital converter ON
}


ISR(WDT_vect) { // Interrupt Service Routine
  wdtTimeout = 1;
}


void setup()
{
  setup_watchdog(0); // 0 = 16ms

  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < numLED; i++) {
    pinMode(arrLED[i], OUTPUT);
    digitalWrite(arrLED[i], LOW);
  }
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);

  PCMSK  |= bit (PCINT2);  // Pin Change Mask Register, PB2 as interrupt
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts  General Interrupt Flag Register
  GIMSK  |= bit (PCIE);    // enable pin change interrupts      General Interrupt Mask Register
  sei();                   // enable interrupts
}

ISR (PCINT0_vect) { // Interrupt Service Routine
  buttonInterrupt = 1;
  cyclesUntilDebounce = 6;
}

void loop()
{
  if (buttonInterrupt == 1) {                         // Button wakeup. Sleep <cyclesUntilDebounce> times, then do stuff, then sleep again
    int reading = digitalRead(buttonPin);
    if (reading == HIGH) {                            // reset buttonInterrupt and go back to sleep if reading gets high
      buttonInterrupt = 0;
      system_sleep();
    }
    else if (reading == LOW) {
      if (cyclesUntilDebounce <= 0) {                 // Also check if x cycles has passed because debounce
        arr60Timers[3] = timer60Add;
        arr15Timer = timer15Add;
        sort(arr60Timers, 4);
        resolveLEDs();
        buttonInterrupt = 0;
        system_sleep();
      }
      else if (cyclesUntilDebounce > 0) {             // if not x cycles, reduce cyclesUntilDebounce and sleep
        cyclesUntilDebounce--;
        system_sleep();
      }

    }
  }
  else if (wdtTimeout == 1) {                         // Watchdog wakeup. Do stuff and go to sleep again
    reduceTimers();
    sort(arr60Timers, 4);
    resolveLEDs();
    wdtTimeout = 0;
    system_sleep();
  }
  else {                                              // system start
    wdtTimeout = 0;
    buttonInterrupt = 0;
    system_sleep();
  }
}

void reduceTimers() {
  for (int i = 0; i < 4; i++) {
    if (arr60Timers[i] > 0) {
      arr60Timers[i]--;
    }
    else if (arr60Timers[i] < 1) {
      arr60Timers[i] = 0;
    }
  }
  if (arr15Timer > 0) {
    arr15Timer--;
  }
  else if (arr15Timer < 1) {
    arr15Timer = 0;
  }
}

void resolveLEDs() {
  for (int i = 0; i < numLED; i++) {
    if (arr60Timers[i] > 0) {
      digitalWrite(arrLED[i], HIGH);
    }
    else if (arr60Timers[i] <= 0) {
      digitalWrite(arrLED[i], LOW);
    }
  }
  if (arr15Timer > 0) {
    digitalWrite(redLED, HIGH);
  }
  else if (arr15Timer <= 0) {
    digitalWrite(redLED, LOW);
  }

}

void sort(long a[], int size) {
  for (int i = 0; i < (size - 1); i++) {
    for (int o = 0; o < (size - (i + 1)); o++) {
      if (a[o] < a[o + 1]) {
        long t = a[o];
        a[o] = a[o + 1];
        a[o + 1] = t;
      }
    }
  }
}
