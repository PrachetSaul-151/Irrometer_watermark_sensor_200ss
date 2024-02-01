/*
* watermarksensor_200ss
* written by Prachet saul
*/
#include <Arduino.h>
#include <math.h>
#define num_of_read 1

const int Rx = 10000;
const long default_TempC = 24;
const long open_resistance = 35000;
const long short_resistance = 200;
const long short_CB = 240, open_CB = 255;
const int SupplyV = 5;
const float cFactor = 1.1;

int i, j = 0, WM1_CB = 0;
float SenV10K = 0, SenVWM1 = 0, SenVWM2 = 0, ARead_A1 = 0, ARead_A2 = 0, WM_Resistance = 0, WM1_Resistance = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(11, LOW);
  delay(100);
}

void loop()
{
  while (j == 0)
  {
    WM1_Resistance = readWMsensor();
    WM1_CB = myCBvalue(WM1_Resistance, default_TempC, cFactor);

    Serial.print("WM1 Resistance(Ohms)= ");
    Serial.print(WM1_Resistance);
    Serial.print("\n");
    Serial.print("WM1(cb/kPa)= ");
    Serial.print(abs(WM1_CB));
    Serial.print("\n");

    delay(200000);
  }
}

int myCBvalue(int res, float TC, float cF)
{
  int WM_CB;
  float resK = res / 1000.0;
  float tempD = 1.00 + 0.018 * (TC - 24.00);

  if (res > 550.00)
  {
    if (res > 8000.00)
    {
      WM_CB = (-2.246 - 5.239 * resK * (1 + .018 * (TC - 24.00)) - .06756 * resK * resK * (tempD * tempD)) * cF;
    }
    else if (res > 1000.00)
    {
      WM_CB = (-3.213 * resK - 4.093) / (1 - 0.009733 * resK - 0.01205 * (TC)) * cF;
    }
    else
    {
      WM_CB = (resK * 23.156 - 12.736) * tempD;
    }
  }
  else
  {
    if (res > 300.00)
    {
      WM_CB = 0.00;
    }
    if (res < 300.00 && res >= short_resistance)
    {
      WM_CB = short_CB;
      Serial.print("Sensor Short WM \n");
    }
  }
  if (res >= open_resistance || res == 0)
  {
    WM_CB = open_CB;
  }
  return WM_CB;
}

float readWMsensor()
{
  ARead_A1 = 0;
  ARead_A2 = 0;

  for (i = 0; i < num_of_read; i++)
  {
    digitalWrite(5, HIGH);
    delayMicroseconds(90);
    ARead_A1 += analogRead(15);
    digitalWrite(5, LOW);
    delay(100);

    digitalWrite(11, HIGH);
    delayMicroseconds(90);
    ARead_A2 += analogRead(15);
    digitalWrite(11, LOW);
  }

  SenVWM1 = ((ARead_A1 / 1024) * SupplyV) / (num_of_read);
  SenVWM2 = ((ARead_A2 / 1024) * SupplyV) / (num_of_read);

  double WM_ResistanceA = (Rx * (SupplyV - SenVWM1) / SenVWM1);
  double WM_ResistanceB = Rx * SenVWM2 / (SupplyV - SenVWM2);
  double WM_Resistance = ((WM_ResistanceA + WM_ResistanceB) / 2);
  return WM_Resistance;
}
