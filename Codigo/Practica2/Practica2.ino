#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

//Variable de flujo
int estado = 0;

//crear un objeto LiquidCrystal (rs,enable,d4,d5,d6,d7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Ultrasónico el sensor HC-SR04
long duracion; //tiempo en que la onda viaja al objeto y regresa
long distancia; //distancia del objeto
int led = 13;
int trig = 9; // recibe un pulso para comenzar el ciclo de medición
int echo = 8; //devuelve un pulso continuo que dura el tiempo en que la onda tarda en ir y regresar del objeto

//Mensaje de bienvenida
String mensaje1_fila0 = "Smart House   ";
String mensaje1_fila1 = "    Ace1      ";
String mensaje2_fila0 = " ACE1-B-G2-S1 ";
String mensaje2_fila1 = "               ";
unsigned long anteriorT_mensaje = 0;
const long intervalo = 1500;
bool flag_mensaje = true;

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
int ledState = LOW;
const long interval = 30;
const long intervalo_Alarma = 13500;

//Servo motor
Servo myservo;
int pos = 0;

void setup() {
  Serial.begin(9600);

  //pantalla
  lcd.begin(16, 2); //16 columnas y 2 filas
  lcd.setCursor(1, 0); //1ra columna y fila 0
  lcd.print(mensaje1_fila0);
  lcd.setCursor(1, 1);
  lcd.print(mensaje1_fila1);

  //Servo
  myservo.attach(10); //pin 10 del arduino

  //ultrasonico
  pinMode(trig, OUTPUT); //emisor
  pinMode(echo, INPUT); //receptor
  pinMode(led, OUTPUT);

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
  duracion = pulseIn(echo, HIGH); //tiempo en que va y regresa la onda ultrasónica
  //calculo de la distancia
  distancia = duracion / 58.4; //distancia en cm
  if (distancia <= 150) {
    //Serial.println("A MENOS de 150 cm");
    digitalWrite(led, HIGH);
    return true;
  } else {
    //Serial.println("A MAS de 150 cm");
    digitalWrite(led, LOW);
    return false;
  }
}

void PASSWORD() {
  Disparo_Ultrasonico();
  if (!Distancia_teclado()) {
    estado = 0;
  }
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print("  PASSWORD:    ");
  Verificar_Password();
  delay(50);
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
        Serial.println("password is correct");
        // DO YOUR WORK HERE
        //MOVEMOS MOTERES Y BIENVENIDO A CASA
        lcd.setCursor(1, 0);
        lcd.print("  BIENVENIDO A      ");
        lcd.setCursor(1, 1);
        lcd.print("  CASA ^-^      ");
        Activar_Motor();
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
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);
    delay(20);
  }
  delay(11000);

  for (pos = 90; pos >= 0; pos --) {
    myservo.write(pos);
    delay(20);
  }
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
      break;
  }
}
