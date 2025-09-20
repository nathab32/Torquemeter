#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <NAU7802.h>
#include <Button.h>
#include <MovingAverage.h>
#include "LowPass2.0.h"

// user-defined variables
bool debug = false;
float calValue = 54.11255;
float armLengthCM = 2.3;

// variables
Adafruit_SSD1306 display(128, 64, &Wire, -1);
NAU7802 adc;

Button button1(2, 50, 2000);// button 1 switches units
Button button2(4, 50, 2000);// button 2 zeroes the scale and calibrates on hold
Button button3(7, 50, 2000);// button 3 turns on a timer and enables logging on hold

enum TorqueUnit
{
  OZ_IN,
  GF_CM,
  IN_LB,
  MN_M,
  UNIT_COUNT
};
TorqueUnit unit = OZ_IN;
float torqueFactor = armLengthCM;
int decimals = 1;
String suffix = "ozin";

bool timerOn = false;
unsigned long startTime;

bool calMode = false;

const int avgLength = 10;
LowPass<2> filter(1.0, 9.3, true);
MovingAverage<uint_fast32_t> avg(avgLength);
// MovingAverage<uint_fast32_t> avg1(5);
float tareReading;

// functions

void onePressCallback(){
  unit = static_cast<TorqueUnit>((unit+1) % UNIT_COUNT);
  switch (unit)
  {
  case OZ_IN:
    torqueFactor = 0.01388738 * armLengthCM;
    decimals = 2;
    suffix = F("ozin");
    break;
  case GF_CM:
    torqueFactor = armLengthCM;
    decimals = 2;
    suffix = F("gfcm");
    break;
  case IN_LB:
    torqueFactor = 0.0008679617 * armLengthCM;
    decimals = 5;
    suffix = F("inlb");
    break;
  case MN_M:
    torqueFactor = 0.0980665 * armLengthCM;
    decimals = 3;
    suffix = F("mNm");
    break;
  }
}

void twoPressCallback(){
  while (!adc.getDataReady()) delay(10);
  long adcVal = adc.read();
  float lpFilter = filter.filt(adcVal);
  tareReading = avg.update(lpFilter) * 4.5 / pow(2, 24);
}

void threePressCallback(){
  timerOn = !timerOn;
  startTime = millis();
}

void oneHoldCallback(){

}

void twoHoldCallback(){
  calMode = !calMode;
}

void threeHoldCallback(){

}


void displaySetup() {
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 initialized successfully."));
  } else {
    Serial.println(F("SSD1306 initialization failed."));
  }
  display.clearDisplay();
  display.fillScreen(SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(31, 24);
  display.print(F("Torquemeter"));
  display.setCursor(24, 32);
  display.print(F("by Nathan Chiu"));
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

bool calibration() {
  display.clearDisplay();
  display.println(F("Place calibration platform on load cell"));
  display.display();
  return true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Serial started"));
  displaySetup();

  //ADC setup
  {
    if(adc.begin() == true) {
      Serial.println(F("NAU7802 initialized successfully."));
    } else {
      Serial.println(F("NAU7802 initialization failed."));
    }
    adc.standby(false);
    adc.setAnalogSupply(NAU7802_SUPPLY_4V5);
    adc.setChannel(NAU7802_CHANNEL_1);
    adc.setConversionRate(NAU7802_CONVERSIONRATE_80);
    adc.setLdoMode(NAU7802_LDOMODE_1);
    adc.pgaBypass(true);
    adc.calibrate(NAU7802_CALIBRATE_INTERNAL_OFFSET);
  }

  //button setup
  {
    button1.pressFunction(onePressCallback);
    button2.pressFunction(twoPressCallback);
    button3.pressFunction(threePressCallback);
    button1.holdFunction(oneHoldCallback);
    button2.holdFunction(twoHoldCallback);
    button3.holdFunction(threeHoldCallback);
  }

  //tare / moving avg setup
  {
    for (int i = 0; i < avgLength; i++) {
      while (!adc.getDataReady()) delay(10);
      avg.update(adc.read());
      // Serial.println(val);
      delay(50);

    }
    // Serial.println(sum);
    tareReading = avg.getAverage() * 4.5 / pow(2, 24);
    // Serial.print("Tare value:");
    // Serial.println(tareReading);
  }
}


void loop() {
    // put your main code here, to run repeatedly:
  if (calMode) calibration();

  while (!adc.getDataReady()) delay(10);
  long adcVal = adc.read();
  float lpFilter = filter.filt(adcVal);

  //filtering testing
{  
  // float voltage = adcVal * 4.5 / pow(2, 24) - tareReading;
  // float voltageFiltered = lpFilter * 4.5 / pow(2, 24) - tareReading;
  // float voltageAveraged = avg.update(adcVal) * 4.5 / pow(2, 24) - tareReading;
  // float voltageCombined = avg1.update(lpFilter) * 4.5 / pow(2, 24) - tareReading;

  // float mass = (voltage * .995) * 100/2.7;
  // float massFiltered = (voltageFiltered * .995) * 100 / 2.7;
  // float massAveraged = (voltageAveraged * .995) * 100 / 2.7;
  // float massCombined = (voltageCombined * .995) * 100 / 2.7;

  // Serial.print(F(">Voltage:"));
  // Serial.print(voltage,4);
  // Serial.print(F(",voltageFiltered:"));
  // Serial.print(voltageFiltered, 4);
  // Serial.print(F(",voltageAveraged:"));
  // Serial.print(voltageAveraged, 4);
  // Serial.print(F(",voltageCombined:"));
  // Serial.print(voltageCombined, 4);

  // Serial.print(F(",zero:0"));

  // Serial.print(F(",massFiltered:"));
  // Serial.print(massFiltered, 4);
  // Serial.print(F(",massAveraged:"));
  // Serial.print(massAveraged, 4);
  // Serial.print(F(",massCombined:"));
  // Serial.print(massCombined, 4);
  // Serial.print(F(",mass:"));
  // Serial.println(mass,4);
  }
  
  float voltage = avg.update(lpFilter) * 4.5 / pow(2, 24) - tareReading;
  float mass = voltage * calValue;
  bool overload = mass > 105;

  button1.handle();
  button2.handle();
  button3.handle();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  float torque = mass * torqueFactor;
  if (debug)
  {  
    display.println(F("Voltage: "));
    display.println(voltage+tareReading, 3);

    display.println(F("Mass: "));
    display.println(mass, 2);

    display.print(F("Torque: "));
    if (overload) display.println("OVERLOAD\n");
    else {
      display.println(torque, decimals);
      display.println(" " + suffix);
    }

    display.print(F("Timer: "));
    if (timerOn) display.print((millis() - startTime) / 1000.0, 2);
    else display.print(F("0.00"));
    display.println(F("s"));
  }
  else
  {
    display.setTextSize(2);
    display.println(F("Torque: "));
    if (overload) display.println("OVERLOAD\n");
    else {
      display.println(torque, decimals);
      display.println(suffix);
    }

    if (timerOn) display.print((millis() - startTime) / 1000.0, 2);
    else display.print(F("0.00"));
    display.println(F("s"));
  }
  
  
  display.display();
  
}