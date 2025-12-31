#include <Wire.h>
#include <math.h>

const int MPU_ADDR = 0x68;

int16_t ax, ay, az;
float roll, pitch;
float errorRoll = 0, errorPitch = 0; // Variables para almacenar el error

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  setupMPU9250();

  // --- Proceso de Calibración ---
  Serial.println("Calibrando... No muevas el sensor.");
  calibrarSensor(200); // Tomamos 200 muestras para promediar
  Serial.println("Calibración finalizada.");
  Serial.printf("Errores calculados -> Roll: %.2f | Pitch: %.2f\n", errorRoll, errorPitch);
  delay(1000);
}

void loop() {
  leerAcelerometro();

  // Calculamos valores y restamos el error de calibración
  roll  = (atan2(ay, az) * 180.0 / PI) - errorRoll;
  pitch = (atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * 180.0 / PI) - errorPitch;

  Serial.printf("Roll: %6.2f | Pitch: %6.2f\n", roll, pitch);

  delay(50);
}

void setupMPU9250() {
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

void calibrarSensor(int muestras) {
  float sumaRoll = 0;
  float sumaPitch = 0;

  for (int i = 0; i < muestras; i++) {
    leerAcelerometro();
    
    // Calculamos los ángulos actuales sin corregir
    float r = atan2(ay, az) * 180.0 / PI;
    float p = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * 180.0 / PI;

    sumaRoll += r;
    sumaPitch += p;
    delay(5); // Pequeña pausa entre muestras
  }

  // El error es el promedio de las lecturas cuando debería estar en 0
  errorRoll = sumaRoll / muestras;
  errorPitch = sumaPitch / muestras;
}
