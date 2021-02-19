#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <stdio.h>

//Variable de flujo
int estado = 3;

//crear un objeto LiquidCrystal (rs,enable,d4,d5,d6,d7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Fotoresistencias
int habitacion = 0;
const int pinLDR1 = A0;
int valorLDR1;
const int pinLDR2 = A1;
int valorLDR2;
const int pinLDR3 = A2;
int valorLDR3;
const int pinLDR4 = A3;
int valorLDR4;

//Ultrasónico el sensor HC-SR04
long duracion; //tiempo en que la onda viaja al objeto y regresa
long distancia; //distancia del objeto
int led = 13;
int trig = 9; // recibe un pulso para comenzar el ciclo de medición
int echo = 8; //devuelve un pulso continuo que dura el tiempo en que la onda tarda en ir y regresar del objeto

//Ultrasónico el sensor HC-SR04
long duracion2; //tiempo en que la onda viaja al objeto y regresa
long distancia2; //distancia del objeto
int led2 = 36;
int trig2 = 38; // recibe un pulso para comenzar el ciclo de medición
int echo2 = 37; //devuelve un pulso continuo que dura el tiempo en que la onda tarda en ir y regresar del objeto

//Mensaje de bienvenida
String mensaje1_fila0 = "  Smart House   ";
String mensaje1_fila1 = "      Ace1      ";
String mensaje2_fila0 = "   ACE1-B-G2-S1  ";
String mensaje2_fila1 = "               ";
unsigned long anteriorT_mensaje = 0;
const long intervalo = 1000;
bool flag_mensaje = true;
bool llego_salida = true;

//Variables teclado
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {25, 26, 27, 28}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {22, 23, 24}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password = "20212"; // change your password here
String input_password;
int intentos;
unsigned long previousMillis = 0;
unsigned long AlarmaAnterior = 0;
unsigned long T_anterior = 0;
int ledState = LOW;
const long interval = 30;
const long intervalo_Alarma = 13500;

//Servo motor
Servo myservo;
int pos = 0;

//Servo motor
Servo myservo2;
int pos2 = 0;

void setup() {
  Serial.begin(9600);
  //Iniciar los pines que encienden las luces de cada habitacion
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);

  //pantalla
  lcd.begin(16, 2); //16 columnas y 2 filas
  lcd.setCursor(1, 0); //1ra columna y fila 0
  lcd.print(mensaje1_fila0);
  lcd.setCursor(1, 1);
  lcd.print(mensaje1_fila1);

  //Servo
  myservo.attach(10); //pin 10 del arduino


  //Servo
  myservo2.attach(39); //pin 39 del arduino

  //ultrasonico
  pinMode(trig, OUTPUT); //emisor
  pinMode(echo, INPUT); //receptor
  pinMode(led, OUTPUT);

  //ultrasonico2
  pinMode(trig2, OUTPUT); //emisor
  pinMode(echo2, INPUT); //receptor
  pinMode(led2, OUTPUT);

  //Password
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  input_password.reserve(32);

}

void MENSAJE() {
  digitalWrite(29, LOW);
  digitalWrite(30, LOW);
  digitalWrite(31, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  input_password = "";
  intentos = 0;

  Disparo_Ultrasonico();
  if (Distancia_teclado()) {
    estado = 1;
  }
  unsigned long actualT_mensaje = millis();
  if (actualT_mensaje - anteriorT_mensaje >= intervalo) {
    anteriorT_mensaje = actualT_mensaje;
    if (flag_mensaje == true) {
      flag_mensaje = false;
    } else {
      flag_mensaje = true;
    }
    if (flag_mensaje) {
      lcd.setCursor(1, 0); //1ra columna y fila 0
      lcd.print(mensaje1_fila0);
      lcd.setCursor(1, 1);
      lcd.print(mensaje1_fila1);
    } else  {
      lcd.setCursor(1, 0); //1ra columna y fila 0
      lcd.print(mensaje2_fila0);
      lcd.setCursor(1, 1); //1ra columna y fila 0
      lcd.print(mensaje2_fila1);
    }
  }
  delay(50);
}

void Disparo_Ultrasonico() {
  //disparo para activar el ultrasónico
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  //termina el disparo de activación
}

bool Distancia_teclado() {
  Disparo_Ultrasonico();
  duracion = pulseIn(echo, HIGH); //tiempo en que va y regresa la onda ultrasónica
  //calculo de la distancia
  distancia = duracion / 58.4; //distancia en cm
  if (distancia <= 150) {
    //Serial.println("A MENOS de 150 cm");
    return true;
  } else {
    //Serial.println("A MAS de 150 cm");
    return false;
  }
}

void PASSWORD() {
  if (!Distancia_teclado()) {
    estado = 0;
  }
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print("  PASSWORD:    ");
  Verificar_Password();
  delay(20);
}

void Verificar_Password() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == '#') {
      input_password = ""; // clear input password
    } else if (key == '*') {
      if (password == input_password) {
        estado = 3;
        //Serial.println("password is correct");
        //MOVEMOS MOTERES Y BIENVENIDO A CASA
        lcd.setCursor(1, 0);
        lcd.print("  BIENVENIDO A      ");
        lcd.setCursor(1, 1);
        lcd.print("  CASA ^-^      ");
        digitalWrite(led, HIGH);
        Activar_Motor();
        lcd.clear();
        digitalWrite(29, LOW);
        digitalWrite(30, LOW);
        digitalWrite(31, LOW);

      } else {
        intentos++;
        if (intentos == 3) {
          digitalWrite(31, HIGH);
          lcd.setCursor(1, 0);
          lcd.print("  ACCESO NO       ");
          lcd.setCursor(1, 1);
          lcd.print("  AUTORIZADO      ");
          delay(400);
          estado = 2;
        } else if (intentos == 2) {
          digitalWrite(30, HIGH);
          lcd.setCursor(1, 0);
          lcd.print(" ERROR EN       ");
          lcd.setCursor(1, 1);
          lcd.print(" LA CONTRASENIA      ");
          delay(600);
        } else if (intentos == 1) {
          digitalWrite(29, HIGH);
          lcd.setCursor(1, 0);
          lcd.print(" ERROR EN       ");
          lcd.setCursor(1, 1);
          lcd.print(" LA CONTRASENIA      ");
          delay(600);
        }
        Serial.println("password is incorrect, try again");
      }

      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}

void Activar_Motor() {
  for (pos = 90; pos >= 0; pos --) {
    myservo.write(pos);
    delay(1);
  }
  delay(1500);
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);
    delay(1);
  }
  digitalWrite(led, LOW);
}

void LEDS() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(11, ledState);
    digitalWrite(12, !ledState);
  }
}

void ALARMA() {
  unsigned long AlarmaActual = millis();
  if (AlarmaActual - AlarmaAnterior >= intervalo_Alarma) {
    AlarmaAnterior = AlarmaActual;
    estado = 0;
  }
  LEDS();
}

void LEDS_HABITACIONES() {
  valorLDR1 = analogRead(pinLDR1);
  valorLDR2 = analogRead(pinLDR2);
  valorLDR3 = analogRead(pinLDR3);
  valorLDR4 = analogRead(pinLDR4);
  if (valorLDR1 > 960 ) {
    digitalWrite(40, 1);
  } else {
    digitalWrite(40, 0);
  }
  if (valorLDR2 > 960 ) {
    digitalWrite(41, 1);
  } else {
    digitalWrite(41, 0);
  }
  if (valorLDR3 > 960 ) {
    digitalWrite(42, 1);
  } else {
    digitalWrite(42, 0);
  }
  if (valorLDR4 > 960 ) {
    digitalWrite(43, 1);
  } else {
    digitalWrite(43, 0);
  }
}

void ILUMINACION() {
  LEDS_HABITACIONES();
  unsigned long T_Actual = millis();
  if (T_Actual - T_anterior >= 1000) {
    T_anterior = T_Actual;
    if (habitacion == 4) {
      habitacion = 1;
    } else {
      habitacion++;
      lcd.clear();
    }
  }
  switch (habitacion) {
    case 1:
      valorLDR1 = analogRead(pinLDR1);
      if (valorLDR1 > 960 ) {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Encendida");

      } else {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Apagada");
      }
      break;
    case 2:
      valorLDR2 = analogRead(pinLDR2);
      if (valorLDR2 > 960 ) {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Encendida");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Apagada");
      }
      break;
    case 3:
      valorLDR3 = analogRead(pinLDR3);
      if (valorLDR3 > 960 ) {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Encendida");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Apagada");
      }
      break;
    case 4:
      valorLDR4 = analogRead(pinLDR4);
      if (valorLDR4 > 960 ) {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Encendida");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Habitacion: ");
        lcd.print(habitacion);
        lcd.setCursor(0, 1);
        lcd.print("Luz Apagada");
        break;
      }
  }
  delay(10);
  SALIDA();
}

void Disparo_Ultrasonico_Puerta() {
  //disparo para activar el ultrasónico
  digitalWrite(trig2, LOW);
  delayMicroseconds(4);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  //termina el disparo de activación
}

bool Distancia_Puerta() {
  Disparo_Ultrasonico_Puerta();
  duracion2 = pulseIn(echo2, HIGH); //tiempo en que va y regresa la onda ultrasónica
  //calculo de la distancia
  distancia2 = duracion2 / 58.4; //distancia en cm
  if (distancia2 <= 150) {
    //Serial.println("CERCA");
    return true;
  } else {
    //Serial.println("LEJOS");
    return false;
  }
}

void SALIDA() {
  char buf[20];
  if (Distancia_Puerta() && llego_salida) {
    for (pos2 = 0; pos2 <= 25; pos2++) {
      myservo2.write(pos2);
      sprintf(buf, "for ida: %d", pos2); //format two ints into character array
      //Serial.println(buf);
      delay(20);
    }
    digitalWrite(led2, HIGH);
    llego_salida = false;
  }
  if (!llego_salida && !Distancia_Puerta()) {
    for (pos2 = 65; pos2 <= 90; pos2++) {
      myservo2.write(pos2);
      sprintf(buf, "for ida: %d", pos2); //format two ints into character array
      //Serial.println(buf);
      delay(20);
    }
    digitalWrite(led2, LOW);
    llego_salida = true;
    estado = 0;
    lcd.clear();
    delay(10);
  }

  sprintf(buf, "actual: %d", pos2); //format two ints into character array
  //Serial.println(buf);
}

void loop() {
  switch (estado) {
    case 0:
      MENSAJE();
      break;
    case 1:
      PASSWORD();
      break;
    case 2:
      ALARMA();
      break;
    case 3:
      ILUMINACION();
      break;
  }
}
