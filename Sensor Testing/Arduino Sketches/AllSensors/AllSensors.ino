#include "DHT.h"
#define DHTPIN 2     // Digital Input 2
#define DHTTYPE DHT11
int lightsensor = 0; // Analog Input 0

DHT dht(DHTPIN, DHTTYPE);


void setup()
{
  Serial.begin(9600);
  dht.begin();

}

void loop()
{
  light();
  temperature();
}

int light()
{ 
  int val = analogRead(lightsensor);     
  delay(1000);
  Serial.print("Light Value: ");
  Serial.println(val);
  return val;
}

int temperature()
{
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) && isnan(t) && isnan(f)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  return t;
}
