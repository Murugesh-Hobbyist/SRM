#include <Arduino.h>

// -------- Motor Pins --------
#define ENA 4
#define IN1 5
#define IN2 6

#define ENB 7
#define IN3 8
#define IN4 9

// -------- IR Sensors --------
#define IR1 10
#define IR2 11

// -------- Variables --------
int speedPercent = 50;   // default 50%
int speedPWM = 127;      // mapped value (0–255)

String input = "";

// -------- Setup --------
void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  Serial.println("Enter speed (0 to 100):");
}

// -------- Motor Functions --------

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedPWM);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speedPWM);  
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speedPWM);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedPWM);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void stopMotor() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// -------- Loop --------
void loop() {

  // -------- Read Serial Speed --------
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      int value = input.toInt();

      if (value < 0) value = 0;
      if (value > 100) value = 100;

      speedPercent = value;
      speedPWM = map(speedPercent, 0, 100, 0, 255);

      Serial.print("Speed set to: ");
      Serial.print(speedPercent);
      Serial.println("%");

      input = "";
    } else {
      input += c;
    }
  }

  // -------- Read Sensors --------
  int s1 = digitalRead(IR1);
  int s2 = digitalRead(IR2);

  Serial.print("IR1: ");
  Serial.print(s1);
  Serial.print("  IR2: ");
  Serial.println(s2);

  // -------- Line Logic --------
  // LOW = line detected

  if (s1 == LOW && s2 == LOW) {
    forward();
  }
  else if (s1 == LOW && s2 == HIGH) {
    left();
  }
  else if (s1 == HIGH && s2 == LOW) {
    right();
  }
  else {
    stopMotor();
  }

  delay(10);
}