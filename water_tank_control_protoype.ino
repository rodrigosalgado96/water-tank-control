#include <LiquidCrystal.h>
#include <Arduino.h>

// LCD Config;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//State variables;
int sensorRead = 0;
int setPointMax = 85;
int setPointMaxWarning = 95;
int setPointMin = 30;
int setPointMinWarning = 10;
int sensorLevel;
int state;
int stateControl = 0;

//Debounce variables;
bool startButtonVar;
bool startButtonState;
bool startButtonLastState = false;
bool startButtonPressed;
unsigned long debounce;
int debounceTime = 200;

//LCD Message variables;
String warning;
String lastWarning;
int lastLevel;

#define buzzer 13
#define startButton 8
#define startLed 9
#define waterPump 10
#define alarmLed 7
#define sensor A0

void setup()
{
  pinMode(sensor, INPUT);
  pinMode(startButton, INPUT_PULLUP);

  pinMode(startLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(alarmLed, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(alarmLed, LOW);

  lcd.begin(16, 2); //Configura LCD
  lcd.print("Starting...");
  delay(3000);
  lcd.clear();
}

void loop() {
  sensorLevel = map(analogRead(sensor), 0, 1023, 0, 100);

  startButtonVar = buttonStartDebounce();

  // State Control;
  if (startButtonVar) stateControl++;
  if (stateControl > 1) stateControl = 0;
  if (stateControl) digitalWrite(startLed, HIGH);
  else digitalWrite(startLed, LOW);
  
  // Working State;
  if (!stateControl) state = 0;
  else if (sensorLevel <= setPointMin && sensorLevel > setPointMinWarning) state = 2;
  else if (sensorLevel <= setPointMinWarning) state = 3;
  else if (sensorLevel >= setPointMax && sensorLevel < setPointMaxWarning) state = 4;
  else if (sensorLevel >= setPointMaxWarning) state = 5;
  else state = 1;

  /* STATE //       DESCRIPTION      //             MEANING
   *   0   // Stop                   // stateControl = false;
   *   1   // Start                  // stateControl = true;
   *   2   // Lower Limit            // sensorLevel under the Lower Limit AND over the Warning limit;
   *   3   // Lower Limit Warning    // sensorLevel under the Warning limit;
   *   4   // Higher Limit           // sensorLevel over the Higher Limit AND under the Warning limit;
   *   5   // Higher Limit Warning   // sensorLevel over the Warning limit;     
   */
   
  switch (state) {
    case 0: // Stop;
      warning = "Status: Stopped!";
      digitalWrite(alarmLed, LOW);
      digitalWrite(buzzer, LOW);
      digitalWrite(waterPump, LOW);
      break;

    case 1: // Start;
      warning = "Status: Regular!";
      digitalWrite(alarmLed, LOW);
      digitalWrite(buzzer, LOW);
      break;

    case 2: // Lower Limit;
      warning = "Status: Low!";
      digitalWrite(alarmLed, LOW);
      digitalWrite(buzzer, LOW);
      digitalWrite(waterPump, HIGH);
      break;
    case 3: // Lower Limit Warning;
      warning = "TOO LOW!";
      digitalWrite(alarmLed, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(waterPump, HIGH);
      break;

    case 4: // Higher Limi;
      warning = "Status: High!";
      digitalWrite(alarmLed, LOW);
      digitalWrite(buzzer, LOW);
      digitalWrite(waterPump, LOW);
      break;

    case 5: // Higher Limit Warning
      warning = "TOO HIGH!";
      digitalWrite(alarmLed, HIGH);
      digitalWrite(buzzer, HIGH);
      break;
  }
  displayMessage();
}

// Debounce button control;
bool buttonStartDebounce(){
  startButtonState = !digitalRead(startButton);
  startButtonPressed = false;
  if( (millis()- debounce) > debounceTime){
    if(startButtonState && startButtonLastState){
      startButtonPressed = true;
      debounce = millis();
    }
  }
  if(startButtonState && startButtonLastState){
    debounce = millis();
  }
  startButtonLastState = startButtonState;

  return startButtonPressed;
}

void displayMessage() {
  if ((warning != lastWarning) || (sensorLevel != lastLevel)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(sensorLevel);
    lcd.print("%");
    lcd.setCursor(0, 2);
    lcd.print(warning);

    lastWarning = warning;
    lastLevel = sensorLevel;
  }
}
