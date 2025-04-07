#include <SoftwareSerial.h>

// Configuración de comunicación Bluetooth
SoftwareSerial mySerial(A0, A1);  // RX, TX
String BT_value = "";
String BT_value_temp = "";

// Configuración de pines y variables globales
const int Trig = A3;
const int Echo = A2;
const int PWM2A = 11;      // Motor 1
const int PWM2B = 3;       // Motor 2  
const int PWM0A = 6;       // Motor 3
const int PWM0B = 5;       // Motor 4
const int DIR_CLK = 4;     // Reloj de dirección
const int DIR_EN = 7;      // Habilitación del L293D
const int DATA = 8;        // Datos de dirección
const int DIR_LATCH = 12;  // Reloj de latch

// Definición de movimientos
const int Move_Forward = 39;
const int Move_Backward = 216;
const int Left_Move = 116;
const int Right_Move = 139;
const int Left_Rotate = 106;
const int Right_Rotate = 149;
const int Stop = 0;

// Velocidades por defecto
int Speed1 = 255;
int Speed2 = 255;
int Speed3 = 255;
int Speed4 = 255;

// Variables para evitar obstáculos
volatile int Front_Distance;
volatile boolean Flag = true;

// Función para controlar los motores
void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
    analogWrite(PWM2A, Speed1);
    analogWrite(PWM2B, Speed2);
    analogWrite(PWM0A, Speed3);
    analogWrite(PWM0B, Speed4);

    digitalWrite(DIR_LATCH, LOW);
    shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Dirección de movimiento
    digitalWrite(DIR_LATCH, HIGH);
}

// Función para medir la distancia con el sensor ultrasónico
float checkdistance() {
    digitalWrite(Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    float distance = pulseIn(Echo, HIGH) / 58.00;
    delay(10);
    return distance;
}

// Función de evitación de obstáculos
void Ultrasonic_Avoidance() {
    Front_Distance = checkdistance();
    if (Front_Distance > 0 && Front_Distance <= 45) {
        if (Front_Distance <= 20) {
            Flag = !Flag;
            Motor(Stop, 0, 0, 0, 0);
            delay(250);
            Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
            delay(200);
            Motor(Stop, 0, 0, 0, 0);
            delay(250);

            if (Flag) {
                Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
            } else {
                Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
            }
            delay(100);
            Motor(Stop, 0, 0, 0, 0);
            delay(250);
        } else {
            Motor(Stop, 0, 0, 0, 0);
            delay(250);
            if (Flag) {
                Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
            } else {
                Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
            }
            delay(100);
            Motor(Stop, 0, 0, 0, 0);
            delay(250);
        }
    } else {
        Motor(Move_Forward, 100, 100, 100, 100);
    }
}

// Función de seguimiento de objeto
void Ultrasonic_Follow() {
    Front_Distance = checkdistance();
    if (Front_Distance >= 0 && Front_Distance <= 10) {
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        delay(20);
    } else if (Front_Distance > 10 && Front_Distance <= 15) {
        Motor(Stop, 0, 0, 0, 0);
        delay(20);
    } else {
        Motor(Move_Forward, 170, 170, 170, 170);
        delay(20);
    }
}

// Configuración inicial
void setup() {
    mySerial.begin(9600);   // Configuración de puerto serial para Bluetooth
    Serial.begin(9600);     // Configuración de puerto serial para monitoreo

    // Configuración de pines
    pinMode(DIR_CLK, OUTPUT);
    pinMode(DATA, OUTPUT);
    pinMode(DIR_EN, OUTPUT);
    pinMode(DIR_LATCH, OUTPUT);
    pinMode(PWM0B, OUTPUT);
    pinMode(PWM0A, OUTPUT);
    pinMode(PWM2A, OUTPUT);
    pinMode(PWM2B, OUTPUT);
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
}

// Bucle principal
void loop() {
    // Verificación de datos Bluetooth
    if (mySerial.available() > 0) {
        BT_value_temp += (char)(mySerial.read());
        if (BT_value_temp.endsWith("#")) {
            BT_value = BT_value_temp;
            BT_value_temp = "";
        }
    }

    if (BT_value.length() > 0) {
        Serial.println("Comando recibido: " + BT_value);
        if (BT_value.length() <= 4) {
            if (BT_value.charAt(0) == '%' && BT_value.charAt(BT_value.length() - 1) == '#') {
                switch (BT_value.charAt(1)) {
                    case 'A': Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4); break;//adelante 
                    case 'B': Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4); break;
                    case 'C': Motor(Left_Move, Speed1, Speed2, Speed3, Speed4); break;
                    case 'D': Motor(Right_Move, Speed1, Speed2, Speed3, Speed4); break; 
                    case 'E': Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4); break;
                    case 'F': Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4); break;//derecha 
                    case 'T': Ultrasonic_Avoidance(); break;
                    case 'W': Ultrasonic_Follow(); break;
                    case 'S': Motor(Stop, 0, 0, 0, 0); break;
                }
            }
            BT_value = "";  // Restablecer después de procesar el comando
        }
    }
}

