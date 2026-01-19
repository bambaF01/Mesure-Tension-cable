#include <Arduino.h>
#include <BluetoothSerial.h>
#include "HX711.h"

// HX711 pins configuration for ESP32 WROOM-32E
#define CLK_PIN   18  // Clock pin for HX711 (SCK)
#define DOUT_PIN  19  // Data pin for HX711 (MISO)
#define SCALE_FACTOR 420.0 // Calibration factor (adjust based on your sensor)

// Create objects
BluetoothSerial SerialBT;
HX711 scale;

// IMPORTANT: Replace with the MAC address of your ESP32 Slave
// Format: {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
// You will get this from the Slave's Serial Monitor
uint8_t slaveMAC[] = {0x8C, 0x4F, 0x00, 0x63, 0x4A, 0xF6};

void setup()
{
  Serial.begin(115200);
  
  // Wait a bit for serial monitor to connect
  delay(1000);

  // Initialize HX711 scale
  Serial.println("Initializing HX711...");
  scale.begin(DOUT_PIN, CLK_PIN);
  
  // Tare the scale (set current weight as zero)
  Serial.println("Taring scale... Please ensure no load on sensor.");
  delay(2000);
  scale.tare(10); // Average of 10 readings
  Serial.println("Tare complete!");
  Serial.print("Offset: ");
  Serial.println(scale.get_offset());
  
  // Set calibration factor
  scale.set_scale(SCALE_FACTOR);
  Serial.println("Scale ready!");
  Serial.println();

  // Start Bluetooth communication in Master mode with device name "ESP32_Master"
  // Passing 'true' as the second argument enables master mode
  if (!SerialBT.begin("ESP32Master", true))
  {
    Serial.println("Bluetooth initialization failed!");
    while(1); // Stop here
  }
  
  Serial.println("ESP32 Master Bluetooth Serial Started.");
  Serial.println("Make sure ESP32Slave is already running!");
  
  // Wait a bit before attempting connection
  delay(3000);

  // Attempt to connect to the slave ESP32 device
  Serial.println("Attempting to connect to ESP32Slave...");
  Serial.print("Using MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", slaveMAC[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Try to connect using MAC address (more reliable than name)
  bool connected = SerialBT.connect(slaveMAC);
  
  if (connected)
  {
    Serial.println("Connected to ESP32Slave successfully!");
  }
  else
  {
    Serial.println("Failed initial connection.");
    Serial.println("Retrying connection every 3 seconds...");
    
    // Retry connection in a loop
    int attempts = 0;
    while (!SerialBT.connected())
    {
      attempts++;
      Serial.print("Attempt ");
      Serial.print(attempts);
      Serial.println(": Trying to connect to ESP32Slave...");
      
      SerialBT.connect(slaveMAC);
      delay(3000); // Wait 3 seconds before retrying
      
      if (attempts >= 10)
      {
        Serial.println("Too many failed attempts. Restarting Bluetooth...");
        SerialBT.end();
        delay(1000);
        SerialBT.begin("ESP32Master", true);
        delay(2000);
        attempts = 0;
      }
    }
    Serial.println("Connected to ESP32Slave!");
  }
  
  Serial.println("System ready! Starting measurements...");
  Serial.println();
}

void loop()
{
  // Read tension from HX711 sensor
  float tension = scale.get_units(5); // Average of 5 readings for stability
  
  // Display on Serial Monitor
  Serial.print("Tension: ");
  Serial.print(tension, 2); // 2 decimal places
  Serial.println(" kg");
  
  // Send tension data to Slave via Bluetooth
  if (SerialBT.connected())
  {
    String data = String(tension, 2);
    SerialBT.println(data);
  }
  else
  {
    Serial.println("Warning: Bluetooth disconnected!");
  }
  
  // Check if data is received from the slave device
  if (SerialBT.available())
  {
    // Read the incoming data from the slave device
    String received = SerialBT.readStringUntil('\n');
    Serial.println("Received from Slave: " + received);
  }

  delay(500); // Read every 500ms
  
}
