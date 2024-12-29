#include <Arduino.h>
#include "secrets/wifi.h"
#include "secrets/mqtt.h"
#include "wifi_connect.h"
#include <WiFiClientSecure.h>
#include "ca_cert.h"
#include <PubSubClient.h>
#include <Ticker.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h> // Thư viện ESP32Servo để hỗ trợ servo trên ESP32

// Khởi tạo OLED với địa chỉ I2C (thay thế 0x3C nếu cần)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// RFID pins
#define SS_PIN 5       // Slave Select pin for RFID
#define RST_PIN 2      // Reset pin for RFID

// Servo pin
#define SERVO_PIN 13   // Servo control pin

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance
Servo doorServo;              // Tạo đối tượng servo

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C address 0x27, 16 columns, 2 rows
// // Define user data (replace with actual UIDs and names)
struct User {
  byte uid[4];    // UID in bytes
  const char *name; // User's name
 };
// List of authorized users
 User users[] = {
   {{0x8A, 0x8E, 0xFA, 0x03}, "Duong"},
   {{0x12, 0x34, 0x56, 0x78}, "Alice"},
   {{0xAB, 0xCD, 0xEF, 0x01}, "Bob"}
 };
const int userCount = sizeof(users) / sizeof(users[0]);
namespace
{
    const char *ssid = WiFiSecrets::ssid;
    const char *password = WiFiSecrets::pass;
    const char *relay_topic = "esp32/relay";       // Topic điều khiển relay từ Dashboard
    const char *soil_topic = "esp32/soil";         // Topic gửi độ ẩm đất
    const char *light_topic = "esp32/light";       // Topic gửi độ sáng
    const char *led_status_topic = "esp32/led";
    const char *status_topic = "esp32/status";     // Topic gửi trạng thái bơm
    const char *mode_topic = "esp32/mode";         // Topic gửi/nhận chế độ điều khiển
    const char *lwt_topic = "esp32/lwt";
    const char *lwt_message = "ESP32 disconnected unexpectedly.";
    const char *rfid_user_topic = "esp32/rfid_user";

    const int soilSensorPin = 34;
    const int ldrPin = 35;          // Pin kết nối quang trở (LDR)
    const int relayPin = 16;
    const int ledPin = 17;          // Pin kết nối đèn LED
    const long interval = 5000;
    bool isManualMode = false;    // Chế độ mặc định: AUTO
    bool manualPumpState = false;  // Trạng thái bơm trong chế độ MANUAL
    bool manualLedState = false;
}

WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);
Ticker soilPublishTicker;



// // Function to check user and return name
const char *getUserName(byte *uid, byte size) {
  for (int i = 0; i < userCount; i++) {
    if (size == 4 && memcmp(uid, users[i].uid, 4) == 0) {
      return users[i].name; // Return user's name if UID matches
    }
  }
  return nullptr; // Return null if no match found
}

// // Function to open door
void openDoor() {
  doorServo.write(90);  // Open door (rotate servo to 90 degrees)
  delay(5000);      // Keep door open for 5 seconds
  doorServo.write(0);   // Close door (rotate servo back to 0 degrees)
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("From %s:  ", topic);
    String message = "";
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
        Serial.print((char)payload[i]);
    }
    Serial.println();

    if (String(topic) == mode_topic)
    {
        if (message == "MANUAL")
        {
            isManualMode = true;
            Serial.println("Switched to MANUAL mode");
        }
        else if (message == "AUTO")
        {
            isManualMode = false;
            Serial.println("Switched to AUTO mode");
        }
    }

    if (isManualMode) {
        if (String(topic) == led_status_topic) {
            if (message == "true") {
                digitalWrite(ledPin, HIGH);
                manualLedState = true;
                mqttClient.publish(led_status_topic, "LED: ON (MANUAL)");
                Serial.println("LED turned ON manually.");
            } else if (message == "false") {
                digitalWrite(ledPin, LOW);
                manualLedState = false;
                mqttClient.publish(led_status_topic, "LED: OFF (MANUAL)");
                Serial.println("LED turned OFF manually.");
            }
        }

        if (message == "true")
        {
            digitalWrite(relayPin, LOW); // Bật bơm
            manualPumpState = true;
            mqttClient.publish(status_topic, "Pump: ON (MANUAL)");
            Serial.println("Pump turned ON manually.");
        }
        else if (message == "false")
        {
            digitalWrite(relayPin, HIGH); // Tắt bơm
            manualPumpState = false;
            mqttClient.publish(status_topic, "Pump: OFF (MANUAL)");
            Serial.println("Pump turned OFF manually.");
        }
    }
}


void checkRFIDAndOpenDoor() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    const char *userName = getUserName(rfid.uid.uidByte, rfid.uid.size);

    if (userName) {
        Serial.print("Authorized user: ");
        Serial.println(userName);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Welcome, ");
        lcd.print(userName);

        String rfidMessage = String("User: ") + userName + " accessed.";
        mqttClient.publish("esp32/rfid_user", rfidMessage.c_str(), true);

        openDoor();
    } else {
        Serial.println("Unauthorized card!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied!");
        lcd.setCursor(0, 1);
        lcd.print("Unauthorized");
        delay(2000);

        String rfidMessage = "Unauthorized access attempt!";
        mqttClient.publish("esp32/rfid_user", rfidMessage.c_str(), true);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to scan...");
    rfid.PICC_HaltA();
}



void publishSoilAndLightData() {
    int soilMoistureValue = analogRead(soilSensorPin);
    float soilPercentage = (100 - (soilMoistureValue / 4095.0) * 100);
    int lightValue = analogRead(ldrPin);
    float lightPercentage = (lightValue / 4095.0) * 100;

    bool isLedOn = false;
    if (!isManualMode) // Chỉ điều khiển bơm tự động khi ở chế độ AUTO
    {
        String pumpState;
        String ledState;
        if (soilPercentage < 40.0)
        {
            digitalWrite(relayPin, LOW); // Bật bơm
            pumpState = "ON (AUTO)";
            Serial.println("Relay ON (Soil Moisture < 40%)");
        }
        if (soilPercentage >=40.0)
        {
            digitalWrite(relayPin, HIGH); // Tắt bơm
            pumpState = "OFF (AUTO)";
            Serial.println("Relay OFF (Soil Moisture >= 40%)");
        }
        if (lightPercentage < 30.0) {
            digitalWrite(ledPin, HIGH);
            ledState = "ON (AUTO)";
            isLedOn = true;
        } else {
            digitalWrite(ledPin, LOW);
            isLedOn = false;
            ledState = "OFF (AUTO)";
        }
        mqttClient.publish(status_topic, pumpState.c_str(), true);
        mqttClient.publish(led_status_topic, ledState.c_str(), true);
    }

    String soilData = String(soilPercentage, 2);
    String lightData = String(lightPercentage, 2);
    

    mqttClient.publish(soil_topic, soilData.c_str(), true);
    mqttClient.publish(light_topic, lightData.c_str(), true);
    

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("Soil: ");
    display.print(soilPercentage, 1);
    display.println("%");
    display.print("Light: ");
    display.print(lightPercentage, 1);
    display.println("%");
    display.print("Pump: ");
    display.println(digitalRead(relayPin) == LOW ? "ON" : "OFF");
    display.print("Mode: ");
    display.print(isManualMode ? "MANUAL" : "AUTO");
    display.print(" |LED: ");
    display.println(digitalRead(ledPin) == LOW ? "ON" : "OFF");
    display.display();
}




void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        if (mqttClient.connect(client_id.c_str(), MQTT::username, MQTT::password, lwt_topic, 0, false, lwt_message))
        {
            Serial.println("MQTT connected");
            mqttClient.subscribe(relay_topic);
            mqttClient.subscribe(mode_topic);
        }
        else
        {
            Serial.print("MQTT connection failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(". Retrying in 1 second...");
            delay(1000);
        }
    }
}

void setup()
{   
  // Initialize RFID
  SPI.begin();              // Initialize SPI bus
  rfid.PCD_Init();          // Initialize RFID module
  Serial.println("Place your card near the RFID reader.");

  // Initialize Servo
  doorServo.attach(SERVO_PIN);
  doorServo.write(0); // Đặt servo ở vị trí ban đầu (cửa đóng)

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to scan...");

    // OLED khởi tạo
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Initializing...");
    display.display();
    delay(2000);

    // Cấu hình các chân
    Serial.begin(115200);
    pinMode(soilSensorPin, INPUT);
    pinMode(ldrPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPin, LOW);

    setup_wifi(ssid, password);
    tlsClient.setCACert(ca_cert);

    mqttClient.setCallback(mqttCallback);
    mqttClient.setServer(MQTT::broker, MQTT::port);

    soilPublishTicker.attach_ms(interval, publishSoilAndLightData);
}

void loop() {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }
    mqttClient.loop();

    // Kiểm tra RFID và xác thực người dùng
    checkRFIDAndOpenDoor();
}
