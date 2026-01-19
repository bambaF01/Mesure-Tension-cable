#include <Arduino.h>
#include <BluetoothSerial.h>
#include <esp_bt_device.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration - SBCOLED01V2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C address for SBCOLED01V2 (usually 0x3C)
// I2C pins for ESP32 WROOM-32E
#define SDA_PIN 21
#define SCL_PIN 22

// Create objects
BluetoothSerial SerialBT;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
float lastTension = 0.0;
unsigned long lastUpdate = 0;

void setup()
{
  // Initialize serial communication at 115200 baud rate for debugging
  Serial.begin(115200);
  
  // Wait a bit for serial monitor to connect
  delay(1000);

  // Initialize I2C with custom pins for ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // Stop here if OLED fails
  }
  
  // Clear display and show startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 Slave");
  display.println("Initializing...");
  display.display();
  delay(1000);

  // Start Bluetooth communication and set the device name to "ESP32_Slave"
  // Use the device name without special characters
  if (!SerialBT.begin("ESP32Slave", false)) // false = slave mode
  {
    Serial.println("Bluetooth initialization failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BT Failed!");
    display.display();
    while(1); // Stop here
  }
  
  Serial.println("ESP32 Slave Bluetooth Serial Started.");
  Serial.println("Device name: ESP32Slave");
  
  // Get and print the Bluetooth MAC address
  const uint8_t* mac = esp_bt_dev_get_address();
  Serial.print("Bluetooth MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Display MAC address on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("ESP32Slave");
  display.println("MAC:");
  display.print("  ");
  for (int i = 0; i < 6; i++) {
    display.printf("%02X", mac[i]);
    if (i < 5) display.print(":");
  }
  display.println();
  display.println();
  display.println("Waiting for");
  display.println("Master...");
  display.display();
  
  Serial.println("Bluetooth is discoverable and waiting for connection...");

  // Wait until the ESP32 Master device connects to this ESP32 Slave
  while (!SerialBT.connected())
  {
    delay(1000);  // Wait for 1 second before checking again
    Serial.print(".");
  }

  // Once connected, print a confirmation message
  Serial.println("");
  Serial.println("Connected to ESP32_Master successfully!");
  
  // Display connection status on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("Connected!");
  display.setTextSize(1);
  display.println();
  display.println("Ready to receive");
  display.println("tension data...");
  display.display();
  delay(2000);
}

void loop()
{
  // Check if data is received from the master device via Bluetooth
  if (SerialBT.available())
  {
    // Read the incoming data as a string
    String incomingData = SerialBT.readStringUntil('\n');
    incomingData.trim(); // Remove whitespace
    
    Serial.print("Received tension: ");
    Serial.print(incomingData);
    Serial.println(" kg");
    
    // Convert to float
    lastTension = incomingData.toFloat();
    lastUpdate = millis();
    
    // Update OLED display
    display.clearDisplay();
    
    // Title
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Tension du fil:");
    display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
    
    // Tension value (large)
    display.setTextSize(3);
    display.setCursor(5, 20);
    display.print(lastTension, 2);
    
    // Unit
    display.setTextSize(2);
    display.setCursor(95, 30);
    display.println("kg");
    
    // Status bar at bottom
    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("BT: Connected");
    
    display.display();
  }
  
  // Check if connection is lost
  if (!SerialBT.connected() && (millis() - lastUpdate > 5000))
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Connection Lost!");
    display.println();
    display.println("Waiting for");
    display.println("Master...");
    display.display();
    lastUpdate = millis();
  }

  delay(50);  // Small delay to avoid overwhelming the loop
}
