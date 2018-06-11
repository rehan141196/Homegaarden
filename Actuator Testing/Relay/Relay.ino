int relay = 13;

void setup()
{
  pinMode(relay, OUTPUT);
}


void loop()
{
  digitalWrite(relay, HIGH);   // turn the relay on 
  delay(5000);
  digitalWrite(relay, LOW);    // turn the relay off
  delay(5000);
}
