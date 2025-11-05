Sodium vapor bulb driver

#include <AsyncDelay.h>
AsyncDelay delay_3s;
#include "RunningAverage.h"

RunningAverage myRA(3);

static float senseresistor = 2.1;//sense resistor ohms
static float maxcurrent = 1075.0;//maximum/target current in Milliamps
static float referencevoltage = 5.0;//internal reference voltage for your arduino chip
static int analogreadresolution = 1024;//resolution of your analog read

static float topresistor = 10000; //top resistor on divider
static float bottomresistor = 1000;//bottom resistor
static float maxvoltage = 32.0;
static int hyst = 50;


static int voltagePin = A0;
static int currentPin = A1;
static int pwmPin = 10;
static int enablePin = 5;
static int ignitePin = 6;
static int idlePin = 12;

static int vmax = (maxvoltage*(bottomresistor/(topresistor+bottomresistor)))*(analogreadresolution/referencevoltage); 
//(sense resistor ohms * desired max current)/(internal voltage reference / analog read resolution)


static int pwmMax = 180;
static int pwmmin = 80;
static int pwmIgniteStart = 120;
static int pwmIgniteMax = 180;
int pwmOut = 9;
int disableMin = 0;
int disableMax = 0;

float voltage = 0;
int lastCurrent = 0;
int current = 0;

//low voltage cutoff
static int batVoltagePin = A4;
static float BRTop = 100000;
static float BRBottom = 22000;
int lowVoltageCount = 0;

static float LVCutoff = 10.5; //low voltage cutoff, set for lead acid batteries.


void setup() {
  // put your setup code here, to run once:
//pinMode(red,OUTPUT);
//pinMode(green,OUTPUT);
Serial.begin(9600);
digitalWrite(pwmPin, LOW);
pinMode(pwmPin,OUTPUT);
pinMode(enablePin, INPUT_PULLUP);
pinMode(ignitePin, INPUT_PULLUP);
delay_3s.start(3000, AsyncDelay::MILLIS);
myRA.clear(); // explicitly start clean
pinMode(idlePin, OUTPUT);
digitalWrite(idlePin, HIGH);
// Pins D9 and D10 - 31.4 kHz
TCCR1A = 0b00000001; // 8bit
TCCR1B = 0b00000001; // x1 phase correct
// Pins D9 and D10 - 62.5 kHz
//TCCR1A = 0b00000001; // 8bit
//TCCR1B = 0b00001001; // x1 fast pwm
// Pins D9 and D10 - 15.6 kHz 10bit
//TCCR1A = 0b00000011; // 10bit
//TCCR1B = 0b00001001; // x1 fast pwm
}

void loop() {

readPower();  //reads the voltage and current inputs

if(digitalRead(enablePin) == LOW){
  if(digitalRead(ignitePin) == LOW){//ignition
    int ignitePwm = pwmIgniteStart;
    while(digitalRead(ignitePin) == LOW){
      analogWrite(pwmPin, ignitePwm);
      if(ignitePwm < pwmIgniteMax) {
        ignitePwm++;
        delay(10);
      }
      Serial.print("ignite pwm: ");
      Serial.println(ignitePwm);
    }
    pwmOut = pwmMax;
    myRA.clear();
  }
  //end of ignition
  else{//normal running
    driveLamp();
  }
}
else {//if lamp is not enabled
  digitalWrite(pwmPin, LOW);
  digitalWrite(13, HIGH);
}

if(current < 100) {//visual feedback for current load
  digitalWrite(idlePin, HIGH);
}
else {
  digitalWrite(idlePin, LOW);
}


if (delay_3s.isExpired()) {

    if(lowVoltageCheck() == 1) {
      lowVoltageCount = lowVoltageCount++;
    }
    else{
      lowVoltageCount = 0;
    }

    if(lowVoltageCount > 3){
      digitalWrite(pwmPin, LOW);
      
      while(true){
        digitalWrite(idlePin, HIGH);
        delay(1000);
        digitalWrite(idlePin, LOW);
        delay(1000);
      }
    }
    
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" ,Current: ");
    Serial.print(current);
    Serial.print(" ,disableMin: ");
    Serial.print(disableMin);    
    Serial.print(" ,disableMax: ");
    Serial.print(disableMax);    
    Serial.print(" ,pwm: ");
    Serial.println(pwmOut);

    
    delay_3s.repeat(); // Count from when the delay expired, not now
  }


//delay(1);
}

void readPower() {//reads the voltage and current
  voltage = (analogRead(voltagePin)*referencevoltage/analogreadresolution)*(topresistor+bottomresistor)/bottomresistor;
  lastCurrent = map(analogRead(currentPin),0,1024,0,(referencevoltage/senseresistor*1000));
  myRA.addValue(lastCurrent);
  current = myRA.getAverage();
}

void driveLamp() {//does the actual driving of the lamp
      if(lastCurrent < (maxcurrent-hyst) && pwmOut < pwmMax) {
      pwmOut++;
      pwmOut++;
    }
    if(current > (maxcurrent+hyst) && pwmOut > 0){
      if(disableMin < 2000 && pwmOut > pwmmin) {
        pwmOut--;
      }
      else if(disableMin > 2000){
        pwmOut--;
      }
    }
    if(current > (maxcurrent+hyst) && pwmOut <= pwmmin) {
      disableMin++;
    }
    else if(current < (maxcurrent+hyst) && disableMin > 0) {
      disableMin--;
    }
    if(voltage < maxvoltage) {
      analogWrite(pwmPin, pwmOut);
      if(pwmOut >= pwmMax) {
        disableMax++;
      }
      else if(disableMax > 0) {
        disableMax--;
      }
    }
    if(voltage > maxvoltage) {
      digitalWrite(pwmPin, LOW);
      disableMax = 0;
    }
    if (disableMax > 2000) {
      digitalWrite(pwmPin, LOW);
      delay(10000);
    }
    if (pwmOut >= pwmMax) {
      digitalWrite(13,LOW);
    }
    else {
      digitalWrite(13,HIGH);
    }
}

bool lowVoltageCheck() {//monitor battery voltage and check for low voltage. if voltage is low, enter shutdown loop.
  float batVoltage = (analogRead(batVoltagePin)*referencevoltage/analogreadresolution)*(BRTop+BRBottom)/BRBottom;
  if(batVoltage < LVCutoff && batVoltage > 1) {
    return 1;
  }
  else{
    return 0;
  }
}
