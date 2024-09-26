#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)
#define IN_5  12          // INPUT FROM IR SENSOR 1           GPIO12(D6)
#define IN_6  14          // OUTPUT TO SERVO                  GPIO14(D5)
#define IN_7  16          // OUTPUT TO OLED                   GPIO16(D0)
#define IN_8  5           // OUTPUT TO SCL OF DISPLAY         GPIO5(D1)
#define IN_9  4           // OUTPUT TO SDA OF DISPLAY         GPIO4(D2)
#define IN_10 10          // INPUT FROM IR SENSOR 2           GPIO10 (SD3)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myservo;  // create servo object to control a servo
int pos = 90;   // variable to store the servo position
String command; // String to store app command state.
const char* ssid = "NodeMCU Car";
ESP8266WebServer server(80);

void HTTP_handleRoot(void);

void setup() {
  myservo.attach(IN_6);  // attaches the servo on pin 14 (D5) to the servo object
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT); 
  pinMode(IN_5, INPUT);  // IR Sensor 1
  pinMode(IN_10, INPUT); // IR Sensor 2
  Serial.begin(9600);
  
  // initialize the OLED object
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();
  
  // Connecting WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("APP IP address: ");
  Serial.println(myIP);
 
  // Starting WEB-server 
  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();    
}

void goAhead() { 
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.write(0x03);
  display.println("MOVING FORWARD");
  display.display();
  display.clearDisplay();
}

void goBack() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.write(0x03);
  display.println("MOVING BACKWARD");
  display.display();
  display.clearDisplay();
}

void goRight() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.write(0x03);
  display.println("TURNING RIGHT");
  display.display();
  display.clearDisplay();
}

void goLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.write(0x03);
  display.println("TURNING LEFT");
  display.display();
  display.clearDisplay();
}

void BatterySwap() {
  // If IR Sensor 2 is triggered
  if (digitalRead(IN_10) == HIGH) {
    for (pos = 90; pos >= 0; pos -= 1) {
      myservo.write(pos);
      delay(100);
    }
    for (pos = 0; pos <= 90; pos += 1) {
      myservo.write(pos);
      delay(100);
    }
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.write(0x03);
    display.println("BATTERY SWAPPING");
    display.display();
    display.clearDisplay();
  }
}

void WirelessCharging() {
  // If IR Sensor 1 is triggered
  if (digitalRead(IN_5) == HIGH) {
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.write(0x03);
    display.println("WIRELESS CHARGING");
    display.display();
    display.clearDisplay();
  }
}

void loop() {
  server.handleClient();
  
  command = server.arg("State");
  if (command == "F") goAhead();
  else if (command == "B") goBack();
  else if (command == "L") goLeft();
  else if (command == "R") goRight();
  else if (command == "S") BatterySwap();
  
  WirelessCharging(); // Check for wireless charging
  BatterySwap();      // Check for battery swapping
}

void HTTP_handleRoot(void) {
  if (server.hasArg("State")) {
    Serial.println(server.arg("State"));
  }
  server.send(200, "text/html", "");
  delay(1);
}
