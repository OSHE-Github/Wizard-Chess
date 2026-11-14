#define SHIFT_REG_SHLD 22
#define SHIFT_REG_CLOCK 23
#define SHIFT_REG_INPUT 25

// uint8_t getState(int64_t* serial_data, uint8_t index) {
//   return (serial_data >> index) & 0x1;
// }

void setup() {
  Serial.begin(9600);
  pinMode(SHIFT_REG_CLOCK, OUTPUT);
  pinMode(SHIFT_REG_SHLD, OUTPUT);
  pinMode(SHIFT_REG_INPUT, INPUT);
  Serial.println("SETUP COMPLETE");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delay(10);
  digitalWrite(SHIFT_REG_SHLD, LOW);
  delay(10);
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delay(10);

  uint64_t value = shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST);
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 8;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 16;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 24;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 32;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 40;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 48;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 56;
  Serial.println(value, BIN);

  delay(100);
}
