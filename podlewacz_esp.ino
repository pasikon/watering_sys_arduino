
// ------------- hardware setup ------------------
#define powerOnPin 12                // ESP8266 CH_PD
#define softRX 3                     // soft RX on Arduino
#define softTX 2                     // soft TX on Arduino

#define configDelay 20000            // max time in ms to wait for ESP8266 comes up
#define serialDelay 200              // delay between two serial prints to ESP8266

// ------------- mode setup ------------------
#define lowPowerMode 0               // lowPower = 1, standard = 0s

// ------------- time setup ------------------
#define wifiSendPeriod 300000       // send data every x ms to thingspeak
#define meanPeriod 300000            // calculate the average of the sensor values of the last x ms
#define avAnzDef 60                  // number of values within the meanPeriod for averaging

// ------------- additional libraries ------------------
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define ttp229 (0xAF>>1)

// soft-serial
#include <SoftwareSerial.h>
SoftwareSerial wifiPort(softRX, softTX);   // RX, TX

//---------------------------------

uint16_t data_out = 0;
uint16_t data1, data2;
int count = 0;
boolean count2 = false;
int button[16];
int oldata[16];

const int stepPin = 2;
const int dirPin = 3;

const int pumpPin = 8;

const int soilInputPin1 = A1;
const int soilInputPin2 = A2;
const int soilInputPin3 = A3;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address Ox3F (Check yours)

boolean backlight = false;

int li = 0;


// ------------- variables WiFi and serial------------------
// time
long time = 0;
float timeSent = 0;

// ADC read intervall and average period
long meanUpdateIntervall;
long timeAverage = 0;

// ESP01
int wifiStat = 0;
int wifiSentStat = 0;
long wifiSendTime = 0;
int confStat = -1;

// Serial Input
String inputString = "";         // a string to hold incoming data
String inputStringH = "";
boolean stringComplete = false;  // whether the string is complete

// Soft-Serial
String inputStringWifi = "";         // a string to hold incoming data
String inputStringHWifi = "";
boolean stringCompleteWifi = false;  // whether the string is complete
int sendV = 0;
int sendStat = 0;


//---------- Setup ------------
void setup() {

  Wire.begin();
  lcd.begin(16, 2); // defines it is a 16 character two line display
  lcd.noBacklight();
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  pinMode(soilInputPin1, INPUT);
  pinMode(soilInputPin2, INPUT);
  pinMode(soilInputPin3, INPUT);

  lcd.clear();

  Serial.begin(115200);

  // Set reset high
  pinMode(powerOnPin, OUTPUT);
  digitalWrite(powerOnPin, LOW);

  // set the data rate for the SoftwareSerial port
  wifiPort.begin(9600);
  // delay(1000);

  // Set ADC update time intervall
  meanUpdateIntervall = meanPeriod / avAnzDef;

  // Set times
  time = millis();
  wifiSendTime = time - wifiSendPeriod;

  Serial.println("Start");
}

void loop() {

  getTouch();

  if (button[0] != 7 && button[0] != 15) { //tryb default
    lcd.clear();
    lcd.setCursor(0, 0); // start postion of Humidity text on LCD
    lcd.print(button[0], 1); // 0 creates whole number, 1 two decimal
    lcd.print(" wcisniety");
    lcd.setCursor(0, 1); // start postion of temperature text on LCD
    lcd.print(6, 1);
    lcd.print(" TRYB WIFI");

    //TODO: oryginalny projekt zawiera logike czujnika oswietlenia

    // New Time
    time = millis();

    // ADC Read
    if (time - timeAverage >= meanUpdateIntervall) {
      timeAverage = time;
    }

    // Check Wifi UP Time
    if (time - wifiSendTime > wifiSendPeriod) {
      wifiStat = 1;
      wifiSendTime = time;
      Serial.println("-> Start ESP");
    }

    // Control ESP8266
    if (wifiStat == 1) {
      wifiStat = 0;
      confStat = 0;
      wifiSentStat = 0;
      digitalWrite(powerOnPin, HIGH);
      Serial.println("-> ESP On");
    } else if (wifiStat == -1) {
      wifiStat = 0;
      confStat = -1;
      wifiSentStat = -1;
      digitalWrite(powerOnPin, LOW);
      Serial.println("-> ESP Off");
    }

    // Check conf intervall
    if (confStat == 0 && time - wifiSendTime > configDelay) {
      wifiStat == 0;
      wifiSendTime = time;
      wifiSentStat = 0;

      restartESP();
      Serial.println("-> ESP restart");
    }

    // Send Data to ESP8266
    if (wifiSentStat == 0 && confStat == 1) {

      // Soil sensor 1
      wifiPort.print("field1=");
      //    wifiPort.println(temp,2); TODO!!!!!!!!!!!
      wifiPort.println("");
      delay(serialDelay);

      // Send data
      wifiPort.println("sendData()");
      wifiSentStat = 1;

      Serial.println("Sent Data");
    }

    // Process Soft Serial Input
    softSerialEvent();

    if (stringCompleteWifi) {

      Serial.println(inputStringWifi);

      if (inputStringWifi == "WS1") {
        confStat = 1;
      } else if (inputStringWifi == "D1" && lowPowerMode == 1) {
        wifiStat = -1;
        // wifiSentStat = 1;
      }

      inputStringWifi = "";
      stringCompleteWifi = false;
    }
  }

  if (button[0] == 7) { //tryb ruch silnika

    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
    for (int x = 0; x < 20000; x++) {
      digitalWrite(stepPin, HIGH);
      delay(1500);
      digitalWrite(stepPin, LOW);
      delay(1500);
    }
    delay(1000); // One second delay

    //    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
    //    // Makes 200 pulses for making one full cycle rotation
    //    for (int x = 0; x < 200; x++) {
    //      digitalWrite(stepPin, HIGH);
    //      delayMicroseconds(500);
    //      digitalWrite(stepPin, LOW);
    //      delayMicroseconds(500);
    //    }
    //    delay(1000); // One second delay

    //    digitalWrite(dirPin, LOW); //Changes the rotations direction
    //    // Makes 400 pulses for making two full cycle rotation
    //    for (int x = 0; x < 400; x++) {
    //      digitalWrite(stepPin, HIGH);
    //      delayMicroseconds(500);
    //      digitalWrite(stepPin, LOW);
    //      delayMicroseconds(500);
    //    }
    //    delay(1000);
  }

  if (button[0] == 15) { //tryb display soil
    lcd.clear();
    int soil1 = analogRead(soilInputPin1);
    int soil2 = analogRead(soilInputPin2);
    int soil3 = analogRead(soilInputPin3);
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Soil sensors...");

    lcd.setCursor(0, 1);
    lcd.print("1:");
    lcd.print(soil1, 1);
    lcd.print(" 2:");
    lcd.print(soil2, 1);
    lcd.print(" 3:");
    lcd.print(soil3, 1);
  }

  if (button[0] == 10) { //tryb wlacz pompke
    lcd.clear();
    lcd.setCursor(0, 0);

    digitalWrite(pumpPin, HIGH);
    lcd.print("Pump turned ON");
  }

  if (button[0] == 9) { //tryb wylacz pompke
    lcd.clear();
    lcd.setCursor(0, 0);

    digitalWrite(pumpPin, LOW);
    lcd.print("Pump turned OFF");
  }

  if (button[0] == 1) { //zgas zapal swiatlo
    button[0] = 0;
    if (!backlight) {
      backlight = true;
      lcd.backlight();
      delay(100);
    } else {
      backlight = false;
      lcd.noBacklight();
      delay(100);
    }
  }


  delay(100);

}

// Soft Serial Input
void softSerialEvent() {
  
  wifiPort.listen();
  if (wifiPort.available()) {
    Serial.println("ss event");
    // get the new byte:
    char inChar = (char)wifiPort.read();

    if (inChar == '\n') {
      stringCompleteWifi = true;
    } else if (inChar != '\r') {
      inputStringWifi += inChar;
    }
  }
}

// Restart ESP
void restartESP() {
  digitalWrite(powerOnPin, LOW);
  delay(1000);
  digitalWrite(powerOnPin, HIGH);
}

void getTouch() {
  Wire.requestFrom(ttp229, 2, true);
  delay(1);
  while (Wire.available())
  {
    data1 = Wire.read();
    data2 = Wire.read();
    data_out = (data1 << 8) | data2;
    for (int i = 1; i < 17; i++)
    {
      uint16_t contrast = 0x8000;
      if (data_out & contrast)
      {
        //Serial.println(i);
        button[count] = i;
        count++;
        delay(1);
      }
      data_out <<= 1;
    }
    if (count2)
    {
      switch (count)
      {
        case 1:
          if (oldata[0] == button[0])
          {
            Serial.println(button[0]);
            delay(20);
          }
          break;
        case 2:
          if (oldata[0] == button[0] && oldata[1] == button[1])
          {
            for (int i = 0; i < 2; i++)
            {
              Serial.print(button[i]);
              Serial.print("  ");
              delay(10);
            }
          }
          break;
      }
    }
    delay(50);
    for (int i = 0; i < 16; i++)
    {
      oldata[i] = button[i];
    }
    count = 0;
    count2 = !count2;
  }
}

