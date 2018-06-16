#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleWiFiShield101.h>
#include "DHT.h"

#define BLYNK_PRINT Serial
#define TDSSENSORPIN A1 // Analog Input 1
#define VREF 5.0
#define SCOUNT  2
#define SensorPin A2  // pH meter on Analog Input 2
#define Offset -3.00  // deviation compensate for pH
#define LED 13
#define samplingInterval 30
#define printInterval 800
#define ArrayLenth  2 // times of collection
#define DHTTYPE DHT11

const short int RELAY = 3;
const short int VALVE1 = 4;
const short int VALVE2 = 6;
const short int VALVE3 = 8;
const short int VALVE4 = 9;
const int lightsensor = 0;
const int DHTPIN = 2;
int pHArray[ArrayLenth]; // Store the average value of the sensor feedback
int pHArrayIndex=0;
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

char auth[] = "49c87a5c009f4a9a950443f80434c73b";
char ssid[] = "Shounak's iPhone";
char pass[] = "Ramukaka";

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void myTimerEvent()
{
  Blynk.virtualWrite(V0, light());
  Blynk.virtualWrite(V1, temp());
  Blynk.virtualWrite(V2, tds());
  Blynk.virtualWrite(V3, pH());
}

BLYNK_WRITE(V4)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
    digitalWrite(VALVE3, HIGH);
  else
    digitalWrite(VALVE3, LOW);
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
    digitalWrite(VALVE4, HIGH);
  else
    digitalWrite(VALVE4, LOW);
}

BLYNK_WRITE(V6)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
    digitalWrite(VALVE1, HIGH);
  else
    digitalWrite(VALVE1, LOW);
}

BLYNK_WRITE(V7)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
    digitalWrite(VALVE2, HIGH);
  else
    digitalWrite(VALVE2, LOW);
}

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
    digitalWrite(RELAY, HIGH);
  else
    digitalWrite(RELAY, LOW);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000, myTimerEvent);
  dht.begin();
  pinMode(VALVE1, OUTPUT);
  digitalWrite(VALVE1, LOW);
  pinMode(VALVE2, OUTPUT);
  digitalWrite(VALVE2, LOW);
  pinMode(VALVE3, OUTPUT);
  digitalWrite(VALVE3, LOW);
  pinMode(VALVE4, OUTPUT);
  digitalWrite(VALVE4, LOW);
  pinMode(TDSSENSORPIN,INPUT);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

int light()
{ 
  int val = analogRead(lightsensor);
  return val;
}

int temp()
{
  float t = dht.readTemperature();

  if (isnan(t)) 
  {
    return -1;
  }
  return t;
}

int tds()
{
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TDSSENSORPIN);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
      return tdsValue;
   }
   return -1;
}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
    bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
    for (i = 0; i < iFilterLen - j - 1; i++) 
          {
      if (bTab[i] > bTab[i + 1]) 
            {
    bTemp = bTab[i];
          bTab[i] = bTab[i + 1];
    bTab[i + 1] = bTemp;
       }
    }
      }
      if ((iFilterLen & 1) > 0)
  bTemp = bTab[(iFilterLen - 1) / 2];
      else
  bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

float pH()
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
  Serial.print("Voltage:");
        Serial.print(voltage,2);
        Serial.print("    pH value: ");
  Serial.println(pHValue,2);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  delay(1000);
  return pHValue;
  }
  return -1;
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

