#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>




// Set up a new SoftwareSerial object
  
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"


const char* ssid = "SSID";
const char* password = "pwd lel";


int led_1 = 2;
int led_2 = 13;
int led_3 = 15;
int led_4 = 14;
int led_5 = 12;


void startCameraServer();
// Set web server port number to 80
WiFiServer server(80);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
// Variable to store the HTTP request
String header;
//String for storing server response
String response = "";
//JSON document
DynamicJsonDocument doc(2048);


void setup() {


    // Set the baud rate for the SoftwareSerial object



Serial.begin(9600); // Standard hardware serial port

  pinMode(led_1,OUTPUT);
  pinMode(led_2,OUTPUT);
  pinMode(led_3,OUTPUT);
  pinMode(led_4,OUTPUT);
  pinMode(led_5,OUTPUT);
pinMode(12,OUTPUT);
  
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
    server.begin();
}

void loop() {

//  // put your main code here, to run repeatedly:
   WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.println(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          Serial.println(currentLine.length());
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            Serial.print("HEADER ::  ");
            Serial.println(header);
            if (header.indexOf("GET /happy") >= 0) {
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Happy...");
              Serial.println("Happy...");
//              analogWrite(15,100);
//              Serial.write(100);
digitalWrite(led_1,HIGH);
delay(3000);
digitalWrite(led_1,LOW);

////
//  display.display();
//  delay(2000);
// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
            } else if (header.indexOf("GET /angry") >= 0) {
//                display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Angry...");
//  display.display();
//  delay(2000);
// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
digitalWrite(led_2,HIGH);
delay(3000);
digitalWrite(led_2,LOW);
            } else if (header.indexOf("GET /sad") >= 0) {
//              
//                 display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
////  display.println("Sad...");
////  display.display();
////  delay(2000);
digitalWrite(led_3,HIGH);
delay(3000);
digitalWrite(led_3,LOW);
////
//// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
            } else if (header.indexOf("GET /disgust") >= 0) {
//                 display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Disgusted...");
//  display.display();
//  delay(2000);

// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
//             
            }
            else if (header.indexOf("GET /surprise") >= 0) {
//             

//               display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Surprised...");
//  display.display();
//  delay(2000);
// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
              
              }
            
            else if (header.indexOf("GET /fear") >= 0) {
digitalWrite(12,HIGH);
delay(3000);
digitalWrite(12,LOW);
              
//                               display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Afraid...");
//  display.display();
//  delay(2000);
//  
// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
//              
              
              }
            else if (header.indexOf("GET /neutral") >= 0) {
//              
digitalWrite(led_4,HIGH);
delay(3000);
digitalWrite(led_4,LOW);
//               display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.println("Neutral...");
//  display.display();
//  delay(2000);
// display.clearDisplay();
//  display.drawBitmap(0,0,Frame1, 128, 64, 1); //this displays each frame hex value 
//  display.display();
}
     
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }

}
