/*
    Ultrasonic sensor Pins:
        VCC : +5
        Trig: A3
        Echo: A2
        GND : GND
 */

const int Trig = A3;
const int Echo = A2;

// Definir los pines de los motores (estos pueden variar según tu configuración)
const int PWM2A = 11; // Motor PWM1
const int PWM2B = 3;  // Motor PWM2
const int PWM0A = 6;  // Motor PWM3
const int PWM0B = 5;  // Motor PWM4
const int DIR_CLK = 4; // Clock para dirección de los motores
const int DATA = 8;    // Datos para dirección de los motores
const int DIR_LATCH = 12; // Latch de dirección de los motores

// Definir las constantes de movimiento
const int Move_Forward = 39;
const int Move_Backward = 216;
const int Stop = 0;

// Variables globales
int Speed1 = 255; // Velocidad del motor
int Speed2 = 255; // Velocidad del motor
int Speed3 = 255; // Velocidad del motor
int Speed4 = 255; // Velocidad del motor

float checkdistance() 
{
  // Enviar un pulso bajo para garantizar un inicio limpio
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);

  // Enviar el pulso de activación
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  // Medir el tiempo que tarda el pulso en regresar
  float distance = pulseIn(Echo, HIGH) / 58.00; // Convertir tiempo a distancia en cm
  delay(10);  // Pequeño retraso para estabilidad
  return distance;
}

void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
  analogWrite(PWM2A, Speed1);
  analogWrite(PWM2B, Speed2);
  analogWrite(PWM0A, Speed3);
  analogWrite(PWM0B, Speed4);

  digitalWrite(DIR_LATCH, LOW);         
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Direccionar movimiento
  digitalWrite(DIR_LATCH, HIGH);        
}

void setup() {
  // Configuración del puerto serial para monitoreo
  Serial.begin(9600);
  
  // Configuración de los pines
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
  // Configurar los pines de los motores
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM0B, OUTPUT);
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
}

void loop() {
  // Comprobar la distancia con el sensor ultrasónico
  float distance = checkdistance();
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Lógica de control de movimiento
  if (distance < 15) {
    // Si un objeto está demasiado cerca, retroceder
    Serial.println("Objeto cercano, retrocediendo...");
    Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
    delay(1000);
    Motor(Stop, 0, 0, 0, 0);
    delay(1000);
  } else if (distance > 20) {
    // Si la distancia es mayor a 20 cm, avanzar
    Serial.println("Avanzando...");
    Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
  } else {
    // Si la distancia está entre 15 y 20 cm, detenerse
    Serial.println("Distancia segura, deteniendo...");
    Motor(Stop, 0, 0, 0, 0);
  }
  
  // Añadir un pequeño retraso antes de la próxima medición
  delay(500);
}


