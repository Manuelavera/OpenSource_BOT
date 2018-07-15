

// *******Variables y libreria //


// Sensor IR
#include <Arduino.h>
#include <NECIRrcv.h>
#define IRPIN 7   // pin al que conectamos el receptor de IR
NECIRrcv ir(IRPIN) ;
unsigned long estado;

// Ultrasonido //
#include <Ultrasonic.h>
Ultrasonic ultrasonic(10, 11); // 10 = trigger;  11 = echo
int distSeguridad = 30;  // modo automático, distancia mínima de los obstáculos
int distancia;
int i = 90;
int sentido_giro = 1; // modo automático, para controlar la cabeza gira hacia la derecha o hacia la izquierda
boolean izquierda = false; // modo automático, controla si el obstáculo está a la izquierda
boolean derecha = false;  // modo automático, controla si el obstáculo está a la derecha

// Servos //
int pinMotorIzqdo = 8;  // motor rueda motriz izquierda
int pinMotorDcho = 9;  // motor rueda motriz derecha
int pinServoUltrasonido = 5;  // servo motor de la cabeza
#include <Servo.h>
Servo servoDcho;
Servo servoIzqdo;
Servo servoUltrasonido;


// Buzzer y led
int buzz_pin = 3;   // pin que controla el zumbador
int led = 4;        //pin que controla el led de avido de foto
int raspyOUT = 2;   //pin que recibe señal de la raspberry una vez realizada la foto
int raspyIN = 12;   //pin que envía una señal a la respberry para que haya la fot0

void setup() {
  Serial.begin(9600); // inicia el puerto serial

  // Infrarrojos //
  ir.begin() ;      // inicia el puerto serial para comunicacion con el IR

  // Servos //    valores iniciales (cabeza centrada y robot parado)
  servoDcho.attach(pinMotorDcho);
  servoDcho.write (1500);
  servoIzqdo.attach(pinMotorIzqdo);
  servoIzqdo.write (1500);
  servoUltrasonido.attach(pinServoUltrasonido);
  servoUltrasonido.write (80);

  //Buzzer, led y raspy
  pinMode(buzz_pin, OUTPUT);
  analogWrite(buzz_pin, 255);
  pinMode (led, OUTPUT);
  digitalWrite (led, LOW);
  pinMode (raspyOUT, OUTPUT);
  digitalWrite (raspyOUT, LOW);
  pinMode (raspyIN, INPUT);
  delay (100);


  // Inicia el vehiculo detenido
  parar();

  delay (200);
}

void loop() {
  leerIR();

  // según el comando recibido por el lector de IR se realizará una acción de las siguientes:

  if (estado == 3108437760) { // Desplazar el robot hacia delante
    avanzar();
    i = 80;  // cabeza centrada
    servoUltrasonido.write(i);
  }
  if (estado == 3141861120) { // Desplazar el robot hacia la derecha
    girarDerecha();
  }
  if (estado == 3208707840) { //  Parar el robot
    parar();
  }
  if (estado == 3158572800) { // Desplazar el robot hacia la izquierda
    girarIzquierda();
  }

  if (estado == 3927310080) { // Desplazar el robot hacia atrás
    retroceder();
  }
  if (estado == 3175284480) { // Entrar en modo automático
    automatico();
  }
  if (estado == 3041591040) { // Hacer una foto
    parar();
    buzz_led();
    estado = 0;
    digitalWrite (raspyOUT, LOW);

    /// Controlar si se ha hecho una foto
    if ((digitalRead (raspyIN)) == HIGH) {
      foto_hecha();
      delay (100);
    }
  }



}

//-------- Leer sensor IR
void leerIR() {
  while (ir.available()) {
    estado = ir.read() ;
    delay (100);
  }
}

//-------- Función de funcionamiento automático
void automatico() {
  if (sentido_giro == 1) {
    moverUltrasonidoDerecha();
    movimiento();
  }
  else {
    moverUltrasonidoIzquierda();
    movimiento();
  }
}


// *********  Chequea en qué posiciones hay obstáculos y mueve el vehículo hacia el lado contrario
void movimiento() {
  if (izquierda == false && derecha == false) {
    avanzar();
  }
  else if (izquierda == true) {
    parar();
    delay(300);
    girarIzquierda();
    delay(1000);
    parar();
  }
  else {
    parar();
    delay(300);
    girarDerecha();
    delay(1000);
    parar();
  }

  izquierda = false;
  derecha = false;
}


// --------------  Función que mueve el servo hacia la izquierda para medir distancias
void moverUltrasonidoIzquierda() {
  i = i - 3;
  servoUltrasonido.write(i);

  if ( i < 56) {
    sentido_giro = 1;
  }
  lecturaDistancia();
  delay(10);
}

// --------------  Función que mueve el servo hacia la derecha para medir distancias
void moverUltrasonidoDerecha() {
  i = i + 3;
  servoUltrasonido.write(i);

  if ( i > 120) {
    sentido_giro = 2;
  }
  lecturaDistancia();
  delay(10);
}

// --------------  Función que lee la distancia
void lecturaDistancia() // Lectura de distancia y comparación con distancia de seguridad
{
  distancia = 0;
  while (distancia == 0) {
    distancia = ultrasonic.distanceRead();
  }
  delay(50);

  // Comparación con distancia de seguridad
  if (distancia < distSeguridad) {
    if (i < 90) {
      derecha = true;
    }
    else {
      izquierda = true;
    }
  }
}

//-------------- Función que enciende el buzz y el led
void buzz_led() {

  // Aviso de que se va a realizar una foto //
  for (int i = 1; i < 4; i++) {
    analogWrite(buzz_pin, 255);
    digitalWrite (led, LOW);
    delay(800);
    analogWrite(buzz_pin, 200);
    digitalWrite (led, HIGH);
    delay(300);
  }

  // Aviso de que se está realizando la foto//
  digitalWrite (raspyOUT, HIGH);
  analogWrite(buzz_pin, 255);
  digitalWrite (led, LOW);
  delay(500);
  analogWrite(buzz_pin, 200);
  digitalWrite (led, HIGH);
  delay(2000);
  analogWrite(buzz_pin, 255);
  digitalWrite (led, LOW);
}


//--------- Función me avisa de que ya se ha hecho la foto

void foto_hecha() {
  delay (2000);
  for (int i = 1; i < 8; i++) {
    analogWrite(buzz_pin, 255);
    digitalWrite (led, LOW);
    delay(300);
    analogWrite(buzz_pin, 200);
    digitalWrite (led, HIGH);
    delay(100);
  }
  analogWrite(buzz_pin, 255);
  digitalWrite (led, LOW);

}

// *******  Parar  *********
void parar() {
  servoDcho.write (1500);
  servoIzqdo.write (1500);
}

// ****** Avanzar ********
void avanzar() {
  servoDcho.write (1300);
  servoIzqdo.write (1725);
}

// ****** Retroceder ********
void retroceder() {
  servoDcho.write (1575);
  servoIzqdo.write (1430);
}

// *****Girar a la derecha ****
void girarDerecha() {
  servoDcho.write (1600);
  servoIzqdo.write (1675);
}

// *****Girar a la izquierda ****
void girarIzquierda() {
  servoDcho.write (1312);
  servoIzqdo.write (1400);
}




