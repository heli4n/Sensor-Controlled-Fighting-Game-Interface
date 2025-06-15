#include <Keyboard.h>

// Pin Definitions 
const int pressureLeftPin = A0;     // Left pressure sensor → LEFT ARROW
const int pressureRightPin = A1;    // Right pressure sensor → RIGHT ARROW
const int pirPin = 2;               // PIR motion sensor → '/'
const int irPin = 3;                // IR proximity sensor → 'l'

// Constants and State Flags 
const int threshold = 100;  // Threshold for pressure sensors
bool leftPressed = false;
bool rightPressed = false;
bool pirTriggered = false;
int lastIRState = LOW;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Required for Arduino Leonardo
  Keyboard.begin();

  pinMode(pirPin, INPUT);
  pinMode(irPin, INPUT);
}

void loop() {
  // 1. PIR Sensor (/ key) 
  int pirState = digitalRead(pirPin);
  if (pirState == HIGH && !pirTriggered) {
    Keyboard.write('/');
    Serial.println("PIR: '/' sent");
    pirTriggered = true;
  }
  if (pirState == LOW) {
    pirTriggered = false;
  }

  // 2. IR Sensor ('l') 
  int irState = digitalRead(irPin);
  if (irState == LOW && lastIRState == HIGH) {
    // Object just moved close
    Keyboard.write('l');
    Serial.println("IR: 'l' sent");
  }
  lastIRState = irState;

  // 3. Pressure Sensors (LEFT / RIGHT / UP) 
  int leftValue = analogRead(pressureLeftPin);
  int rightValue = analogRead(pressureRightPin);

  bool leftActive = leftValue >= threshold;
  bool rightActive = rightValue >= threshold;

  Serial.print("Left: ");
  Serial.print(leftValue);
  Serial.print(" | Right: ");
  Serial.println(rightValue);

  // If both feet are pressing → JUMP (UP arrow)
  if (leftActive && rightActive) {
    Keyboard.press(KEY_UP_ARROW);
    delay(50);
    Keyboard.release(KEY_UP_ARROW);
    Serial.println("Jump: ↑ key sent");
    leftPressed = rightPressed = true;
  }
  // If only left foot is pressing → LEFT ARROW
  else if (leftActive && !leftPressed) {
    Keyboard.press(KEY_LEFT_ARROW);
    delay(50);
    Keyboard.release(KEY_LEFT_ARROW);
    Serial.println("Move Left: ← key sent");
    leftPressed = true;
  }
  // If only right foot is pressing → RIGHT ARROW
  else if (rightActive && !rightPressed) {
    Keyboard.press(KEY_RIGHT_ARROW);
    delay(50);
    Keyboard.release(KEY_RIGHT_ARROW);
    Serial.println("Move Right: → key sent");
    rightPressed = true;
  }

  // Reset flags when feet are lifted
  if (!leftActive) leftPressed = false;
  if (!rightActive) rightPressed = false;

  delay(50);  // Small delay to reduce sensor bouncing
}
