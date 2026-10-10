#define SENSOR_PIN 36
void setup() {
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t value = analogReadMilliVolts(SENSOR_PIN);
  Serial.println(value);
  delay(100);
}
