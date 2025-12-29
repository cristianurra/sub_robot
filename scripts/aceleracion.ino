const int NUM_MOTORES = 8;

const int escPin[NUM_MOTORES] = {
  18, 19, 21, 22, 23, 25, 26, 27
};

const int pwmChannel[NUM_MOTORES] = {
  0, 1, 2, 3, 4, 5, 6, 7
};

const int pwmFreq = 50;
const int pwmResolution = 16;

const uint32_t PWM_REVERSE = 2949;
const uint32_t PWM_STOP    = 4915;
const uint32_t PWM_FORWARD = 6881;

const int VELOCIDAD_PASO = 1;
const int TIEMPO_PASO_MS = 20;   

int velocidadActual[NUM_MOTORES];
int velocidadObjetivo[NUM_MOTORES];

unsigned long ultimoPaso[NUM_MOTORES];

void setMotorVelocidad(int motor, int vel);
void actualizarMotores();

void setup() {
  Serial.begin(115200);
  Serial.println("Armando ESC submarinos...");

  for (int i = 0; i < NUM_MOTORES; i++) {
    ledcSetup(pwmChannel[i], pwmFreq, pwmResolution);
    ledcAttachPin(escPin[i], pwmChannel[i]);
    ledcWrite(pwmChannel[i], PWM_STOP);

    velocidadActual[i] = 50; 
    velocidadObjetivo[i] = 50;
    ultimoPaso[i] = millis();
  }

  delay(3000);
  Serial.println("ESC listos");
  Serial.println("Comando: motor velocidad");
  Serial.println("Ejemplo: 1 75");
}

void loop() {
  if (Serial.available()) {
    int motor = Serial.parseInt();
    int velocidad = Serial.parseInt();

    if (motor >= 1 && motor <= NUM_MOTORES &&
        velocidad >= 0 && velocidad <= 100) {

      velocidadObjetivo[motor - 1] = velocidad;

      Serial.print("Motor ");
      Serial.print(motor);
      Serial.print(" -> objetivo ");
      Serial.println(velocidad);
    } else {
      Serial.println("Error. Usa: motor(1-8) velocidad(0-100)");
    }

    while (Serial.available()) Serial.read();
  }

  actualizarMotores();  // <<< rampa suave
}

void actualizarMotores() {
  unsigned long ahora = millis();

  for (int i = 0; i < NUM_MOTORES; i++) {
    if (ahora - ultimoPaso[i] >= TIEMPO_PASO_MS) {
      ultimoPaso[i] = ahora;

      if (velocidadActual[i] < velocidadObjetivo[i]) {
        velocidadActual[i] += VELOCIDAD_PASO;
        if (velocidadActual[i] > velocidadObjetivo[i])
          velocidadActual[i] = velocidadObjetivo[i];

        escribirPWM(i, velocidadActual[i]);
      }
      else if (velocidadActual[i] > velocidadObjetivo[i]) {
        velocidadActual[i] -= VELOCIDAD_PASO;
        if (velocidadActual[i] < velocidadObjetivo[i])
          velocidadActual[i] = velocidadObjetivo[i];

        escribirPWM(i, velocidadActual[i]);
      }
    }
  }
}

void escribirPWM(int index, int vel) {
  uint32_t duty;

  if (vel < 50) {
    duty = map(vel, 0, 50, PWM_REVERSE, PWM_STOP);
  } else {
    duty = map(vel, 50, 100, PWM_STOP, PWM_FORWARD);
  }

  ledcWrite(pwmChannel[index], duty);
}

void setMotorVelocidad(int motor, int vel) {
  if (motor < 1 || motor > NUM_MOTORES) return;
  if (vel < 0 || vel > 100) return;

  velocidadObjetivo[motor - 1] = vel;
}
