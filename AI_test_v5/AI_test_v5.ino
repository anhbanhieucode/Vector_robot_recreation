#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <ESP32Servo.h>

// WiFi Credentials
const char *ssid = "Minh Hieu";
const char *password = "12345678";

// Camera Model
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Motor control
const int motorIn2 = 13;
const int motorIn4 = 2;
int motor_on = 4;

// Servo setup
Servo servo1, servo2;
#define servoPin1 12
#define servoPin2 4
int servoState = 0;
int minAngle = 30;
int maxAngle = 150;
int motorSpeed = 125;  // Default motor speed (0-255)
int emoji = 0;  // Variable to control emoji value

// Infrared Sensors
//#define irSensor1 1  // GPIO1 (TX0)
//#define irSensor2 3  // GPIO3 (RX0)

// OLED setup
#define i2c_Address 0x3C
#define SDA 15
#define SCL 14
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <FluxGarage_RoboEyes.h>
roboEyes roboEyes;

WebServer server(80);
bool isStreaming = false;
TaskHandle_t streamTaskHandle = NULL;

void controlMotor() {
  switch (motor_on) {
    case 1:  // Forward
      digitalWrite(motorIn2, HIGH);
      digitalWrite(motorIn4, HIGH);
      analogWrite(motorIn2, motorSpeed);
      analogWrite(motorIn4, motorSpeed);
      break;
    case 2:  // Reverse
      digitalWrite(motorIn2, HIGH);
      digitalWrite(motorIn4, LOW);
      analogWrite(motorIn2, motorSpeed);
      break;
    case 3:  // Right turn
      digitalWrite(motorIn2, LOW);
      digitalWrite(motorIn4, HIGH);
      analogWrite(motorIn4, motorSpeed);
      break;
    case 4:  // Left turn
      digitalWrite(motorIn2, LOW);
      digitalWrite(motorIn4, LOW);
      break;
    default:  // Stop
      digitalWrite(motorIn2, LOW);
      digitalWrite(motorIn4, LOW);
      analogWrite(motorIn2, 0);
      analogWrite(motorIn4, 0);
      break;
  }
}

void switchEmotion(int emoji) {
  switch (emoji) {
    
    case 0:
      roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(ON, 2, 2);
      roboEyes.setMood(DEFAULT);
      roboEyes.setHFlicker(OFF);
      roboEyes.setCuriosity(OFF);  
      break;
      
    case 1:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(OFF);
      roboEyes.setMood(HAPPY);
      break;
      
    case 2:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(OFF);
      roboEyes.setMood(TIRED);
      roboEyes.setHFlicker(ON, 2);
      break;
      
    case 3:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(OFF);     
      roboEyes.setMood(ANGRY);
      roboEyes.setHFlicker(ON, 10);
      break;
      
    case 4:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(OFF);
      roboEyes.setMood(HAPPY);
      roboEyes.anim_laugh();
      break;
      
    case 5:
      roboEyes.setWidth(32,52);
      roboEyes.setHeight(32,18);
      roboEyes.setBorderradius(8, 6);
      roboEyes.setSpacebetween(10);
      break;
      
    case 6:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setCuriosity(true);
      roboEyes.setAutoblinker(false);
      break;
      
    case 7:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(false);
      roboEyes.anim_confused();
      break;
      
    case 8:
          roboEyes.setWidth(32,32);
      roboEyes.setHeight(32,32);
      roboEyes.setBorderradius(8, 8);
      roboEyes.setSpacebetween(12);
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(ON, 3, 2);
      roboEyes.setCuriosity(ON);    
      break;
      
    default:
      roboEyes.setMood(DEFAULT);  
      roboEyes.setAutoblinker(true);  // Turn auto blinking back on
      roboEyes.setAutoblinker(ON, 3, 2);
      roboEyes.setIdleMode(ON, 2, 2);
      break;
  }
}


void handleMotorControl() {
  if (!server.hasArg("state") || !server.hasArg("speed") || !server.hasArg("time")) {
    server.send(400, "text/plain", "Missing 'state', 'speed', or 'time' parameter");
    return;
  }

  motor_on = server.arg("state").toInt();
  motorSpeed = server.arg("speed").toInt();
  int motorTime = server.arg("time").toInt(); // Extract time separately

  if (motorSpeed < 0 || motorSpeed > 255) {
    server.send(400, "text/plain", "Speed must be between 0 and 255");
    return;
  }

  // Run the motor
  controlMotor();

  // Delay for the specified time (converted to milliseconds)
  delay(motorTime);  // time 

  // Stop the motor after time expires
  motor_on = 0;
  controlMotor();

  server.send(200, "text/plain", 
    String("Motor ran with state ") + motor_on + 
    ", speed " + motorSpeed + 
    ", for " + motorTime + " second(s)");
}


void handleServoControl() {
  if (!server.hasArg("servo") || !server.hasArg("from") || !server.hasArg("to")) {
    server.send(400, "text/plain", "Missing parameters: servo (1 or 2), from, to");
    return;
  }

  servoState = server.arg("servo").toInt();
  int fromAngle = server.arg("from").toInt();
  int toAngle = server.arg("to").toInt();

  // Validate servo selection
  if (servoState != 1 && servoState != 2) {
    server.send(400, "text/plain", "Invalid servo. Use 1 or 2.");
    return;
  }

  // Move from 'fromAngle' to 'toAngle'
  if (fromAngle <= toAngle) {
    for (int angle = fromAngle; angle <= toAngle; angle++) {
      if (servoState == 1) {
        servo1.write(angle);
      } else {
        servo2.write(angle);
      }
      delay(20);
    }
  } else {
    for (int angle = fromAngle; angle >= toAngle; angle--) {
      if (servoState == 1) {
        servo1.write(angle);
      } else {
        servo2.write(angle);
      }
      delay(20);
    }
  }

  server.send(200, "text/plain", String("Servo ") + servoState + " moved from " + fromAngle + " to " + toAngle);
}

void handleEmojiControl() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing 'value' parameter");
    return;
  }

  emoji = server.arg("value").toInt();
  server.send(200, "text/plain", "Emoji value set to " + String(emoji));
}


void streamTask(void *param) {
  WiFiClient *client = (WiFiClient *)param;
  if (!client->connected()) {
    delete client;
    vTaskDelete(NULL);
    return;
  }

  String header = "HTTP/1.1 200 OK\r\n";
  header += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  client->print(header);

  while (isStreaming) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) break;

    client->print("--frame\r\n");
    client->print("Content-Type: image/jpeg\r\n");
    client->print("Content-Length: " + String(fb->len) + "\r\n\r\n");
    client->write(fb->buf, fb->len);
    client->print("\r\n");

    esp_camera_fb_return(fb);

    if (!client->connected()) break;
    delay(50);
  }

  client->stop();
  delete client;
  isStreaming = false;
  streamTaskHandle = NULL;
  vTaskDelete(NULL);
}

void handleStream() {
  if (isStreaming) {
    server.send(200, "text/plain", "Already streaming");
    return;
  }

  WiFiClient *client = new WiFiClient(server.client());
  if (!client->connected()) {
    delete client;
    server.send(500, "text/plain", "Client connection failed");
    return;
  }

  isStreaming = true;
  xTaskCreatePinnedToCore(streamTask, "StreamTask", 8192, client, 1, &streamTaskHandle, 1);
  server.send(200, "text/plain", "Streaming started");
}

void handleStopStream() {
  isStreaming = false;
  server.send(200, "text/plain", "Streaming stopped");
}

void setup() {
  Serial.begin(115200);

  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);

  // IR sensors
  //pinMode(irSensor1, INPUT);
  //pinMode(irSensor2, INPUT);

  // OLED
  Wire.begin(SDA, SCL);
  delay(250);
  display.begin(i2c_Address, true);
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  // Safe boot fix
  pinMode(12, INPUT_PULLDOWN);
  delay(100);

  // Servo
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  // Set initial servo position
  servo1.write(50);
  servo2.write(20);

  // Motor
  pinMode(motorIn2, OUTPUT);
  pinMode(motorIn4, OUTPUT);

  controlMotor();

  // Camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/motor", HTTP_GET, handleMotorControl);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/stop_stream", HTTP_GET, handleStopStream);
  server.on("/servo", HTTP_GET, handleServoControl);
  server.on("/emoji", HTTP_GET, handleEmojiControl);
  server.begin();
}

void loop() {
  roboEyes.update();
  switchEmotion(emoji);
  server.handleClient();
  
  // Example: read IR sensor values (optional logic)
  //int ir1 = digitalRead(irSensor1);
  //int ir2 = digitalRead(irSensor2);
  
  // You can use the sensor values here if needed
  //Serial.println(ir1);
  //Serial.println(ir2);
  //Serial.println();
  delay(10);
}
