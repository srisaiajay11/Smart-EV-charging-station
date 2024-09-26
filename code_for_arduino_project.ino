#include <Servo.h> 
#include <MFRC522.h>
#include <SPI.h>

#define RFID_RST_PIN 9
#define RFID_SS_PIN 10
#define RELAY_PIN 8

int IRSensor = 2; // IR sensor connected to Arduino pin D2
const int LP = 7; // GPIO pin for left motor positive
const int LN = 6; // GPIO pin for left motor negative
const int RP = 5; // GPIO pin for right motor positive
const int RN = 4; // GPIO pin for right motor negative
bool actionTaken = false;
Servo servo; // Create object for Servo motor
int position = 0;  // Variable to store the position

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN); // Create MFRC522 instance

// Define the allowed tag's UID (replace these values with your tag's UID)
byte allowedTagUID[] = {0xD3, 0x4A, 0x57, 0xFB,};

void setup() {
  Serial.begin(9600); // Initialize Serial at 9600 Baud Rate
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Initialize relay in OFF state
  SPI.begin();
  rfid.PCD_Init(); // Initiate MFRC522
  Serial.println("Serial Working"); // Test to check if serial is working or not
  pinMode(IRSensor, INPUT); // IR Sensor pin as INPUT
  pinMode(LP, OUTPUT);
  pinMode(LN, OUTPUT);
  pinMode(RP, OUTPUT);
  pinMode(RN, OUTPUT);
  digitalWrite(LP, LOW);
  digitalWrite(LN, LOW);
  digitalWrite(RP, LOW);
  digitalWrite(RN, LOW);
  servo.attach(3); // Set PWM pin 3 for Servo motor
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (compareUID(rfid.uid.uidByte, allowedTagUID, rfid.uid.size)) {
      digitalWrite(RELAY_PIN, HIGH); // Turn on relay when authorized tag is detected
      Serial.println("Vehicle detected. Charging activated.");
    } else {
      digitalWrite(RELAY_PIN, LOW); // Turn off relay when no tag is detected
      Serial.println("Unauthorized tag detected. Charging deactivated.");
    }
  }

  int sensorStatus = digitalRead(IRSensor); // Read IR sensor status
  if (sensorStatus == HIGH && !actionTaken) {
    // If obstacle detected by IR sensor
    digitalWrite(LP, HIGH);
    digitalWrite(LN, LOW);
    digitalWrite(RP, HIGH);
    digitalWrite(RN, LOW);
    Serial.println("Removing"); // Print message to serial monitor
    delay(15000); // Move forward for 15 seconds

    // Rotating Servo motor in clockwise from 0 degree to 180 degree 
    for (position = 0; position <= 180; position++) { 
      servo.write(position); // Set position of Servo motor 
      delay(15); // Short delay to control the speed 
    } 

    // Rotating Servo motor in anti clockwise from 180 degree to 0 degree 
    for (position = 180; position >= 0; position--) { 
      servo.write(position); // Set position of Servo motor 
      delay(15); // Short delay to control the speed 
    } 

    digitalWrite(LP, LOW);
    digitalWrite(LN, HIGH);
    digitalWrite(RP, LOW);
    digitalWrite(RN, HIGH);
    Serial.println("Replacing"); // Print message to serial monitor
    delay(15000); // Reverse motors for 15 seconds

    actionTaken = true;
  }
}

bool compareUID(byte *UID1, byte *UID2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (UID1[i] != UID2[i]) {
      return false;
    }
  }
  return true;
}
