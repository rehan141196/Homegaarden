int lightsensor = 0; // Put pin number here

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int light = LightSensor();
  Serial.print("Light Value: ");
  Serial.println(light);
}

int LightSensor()
{ 
  int val = analogRead(lightsensor);     
  delay(1000); 
  return val;
}
