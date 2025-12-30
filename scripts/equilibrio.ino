
#include <Wire.h>

const int MPU_ADDR = 0x68;

int16_t ax, ay, az;
float roll, pitch;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  setupMPU9250();
}

void loop() {
  leerAcelerometro();

  roll  = calcularRoll();
  pitch = calcularPitch();

  Serial.printf("Roll: %6.2f | Pitch: %6.2f\n", roll, pitch);

  delay(50);
}

void setupMPU9250() {
  // Despertar el sensor
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0);
  Wire.endTransmission(true);
}

void leerAcelerometro() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

float calcularRoll() {
  float r = atan2(ay, az) * 180.0 / PI;
  if (r < 0) r += 360; 
  return r;
}

float calcularPitch() {
  float p = atan2(-ax, az) * 180.0 / PI; 
  if (p < 0) p += 360; 
  return p;
}
