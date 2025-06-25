#include <Servo.h>

Servo servo1, servo2, servo3;
int pos1 = 90, pos2 = 90, pos3 = 90; // Posición inicial (90°)

// Posiciones para cada letra (ejemplo para algunas letras, agrega las demás)
struct Posiciones {
  int s1;
  int s2;
  int s3;
};

// Define las posiciones para cada letra que necesites
// Aquí solo pongo algunas letras como ejemplo:
Posiciones posicionesAbc[26] = {
  {85, 65, 95},   // A (puedes cambiar)
  {85, 65, 87},   // B
  {85, 65, 82},   // C
  {85, 65, 77},   // D
  {85, 65, 72},   // E
  {100, 60, 95},   // F
  {100, 60, 90},   // G
  {100, 60, 84},   // H
  {100, 60, 80},   // I
  {100, 60, 75},   // J
  {100, 60, 71},   // K
  {120, 55, 95},   // L
  {120, 55, 90},   // M
  {120, 55, 82},   // N
  {120, 55, 75},   // O
  {120, 55, 70},   // P
  {145, 50, 95},   // Q
  {145, 50, 87},  // R (ejemplo, pon la letra que quieras)
  {145, 50, 82},   // S
  {145, 50, 76},   // T
  {145, 50, 70},   // U
  {160, 35, 90},   // V (por ejemplo, aquí puse la posición que mencionaste)
  {160, 35, 83},   // W
  {160, 35, 75},   // X
  {160, 35, 70},   // Y
  {160, 35, 65}    // Z
};

void moverServoPasoAPaso(int &posActual, int posObjetivo, Servo &servo) {
  if (posActual < posObjetivo) {
    for (int i = posActual + 1; i <= posObjetivo; i++) {
      servo.write(i);
      delay(15); // Ajusta el delay para velocidad de movimiento
    }
  } else if (posActual > posObjetivo) {
    for (int i = posActual - 1; i >= posObjetivo; i--) {
      servo.write(i);
      delay(15);
    }
  }
  posActual = posObjetivo;
}

void moverABase() {
  moverServoPasoAPaso(pos1, 90, servo1);
  moverServoPasoAPaso(pos2, 90, servo2);
  moverServoPasoAPaso(pos3, 90, servo3);
}

void setup() {
  Serial.begin(9600);
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);

  servo1.write(pos1);
  servo2.write(pos2);
  servo3.write(pos3);

  Serial.println("Sistema listo. Esperando letra...");
}

void loop() {
  if (Serial.available() > 0) {
    char letra = Serial.read();

    // Convierte a mayúscula si es minúscula
    if (letra >= 'a' && letra <= 'z') {
      letra = letra - 'a' + 'A';
    }

    if (letra >= 'A' && letra <= 'Z') {
      int index = letra - 'A';

      Serial.print("Recibida letra: ");
      Serial.println(letra);

      Posiciones destino = posicionesAbc[index];

      // Mover servos a la posición objetivo paso a paso
      moverServoPasoAPaso(pos1, destino.s1, servo1);
      moverServoPasoAPaso(pos2, destino.s2, servo2);
      moverServoPasoAPaso(pos3, destino.s3, servo3);

      Serial.println("Posición alcanzada.");
      delay(2000); // Espera 2 segundos

      // Regresa a la posición base (90°)
      moverABase();

      Serial.println("Regresando a posición base.");
      // Envía el entero 1 por serial (2 bytes)
      //int confirmacion = 1;
      //Serial.write((uint8_t*)&confirmacion, sizeof(confirmacion));
    } else {
      Serial.println("Letra no válida o no alfabética.");
    }
  }
  // Si no hay datos, se mantiene en posición base
}
