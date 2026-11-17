
#define SHIFT_REG_SHLD 32
#define SHIFT_REG_CLOCK 35
#define SHIFT_REG_INPUT 34

// uint8_t getState(int64_t* serial_data, uint8_t index) {
//   return (serial_data >> index) & 0x1;
// }

void setup() {
  Serial.begin(9600);
  pinMode(SHIFT_REG_CLOCK, OUTPUT);
  pinMode(SHIFT_REG_SHLD, OUTPUT);
  pinMode(SHIFT_REG_INPUT, INPUT);
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  digitalWrite(SHIFT_REG_CLOCK, LOW);
  Serial.println("SETUP COMPLETE");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(10000);
  digitalWrite(SHIFT_REG_SHLD, LOW);
  delayMicroseconds(10000);
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(10000);

  uint32_t value = shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST);
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 8;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 16;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 24;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 32;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 40;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 48;
  // value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 56;
  // for (int i = 0; i < 8; i++) {
  //   Serial.print((uint8_t)(value >> (8*i)), BIN);
  // }
  // Serial.println();
  Serial.println(value, BIN);

  delay(100);
}
