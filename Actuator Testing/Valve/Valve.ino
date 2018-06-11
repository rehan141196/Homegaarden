const short int VALVE = 2;

void setup()
{
  Serial.begin(9600);
    pinMode(VALVE, OUTPUT);
    digitalWrite(VALVE, HIGH);
}

void loop()
{
  digitalWrite(VALVE, LOW);
  delay(2000);
  digitalWrite(VALVE, HIGH);
}
