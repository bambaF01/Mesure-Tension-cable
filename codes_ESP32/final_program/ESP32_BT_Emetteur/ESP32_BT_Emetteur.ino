#include <Arduino.h>
#include <BluetoothSerial.h>
#include <math.h>
#include "HX711.h"

// Configuration des broches HX711 pour ESP32 WROOM-32E
#define CLK_PIN   18  // Broche d'horloge pour HX711 (SCK)
#define DOUT_PIN_1  34  // Broche de données pour HX711 (MISO)
#define DOUT_PIN_2  35
#define DOUT_PIN_3  32
#define DOUT_PIN_4  33
#define SCALE_FACTOR_1 110.95757 //à determiner
#define SCALE_FACTOR_2 109.68881 //à determiner
#define SCALE_FACTOR_3 109.27245 //à determiner
#define SCALE_FACTOR_4 107.75708 //à determiner

// Bouton poussoir pour tare
#define BOUTON_TARE 13
bool dernierEtatBouton = HIGH;

// Création des objets
BluetoothSerial SerialBT;
HX711 scale1, scale2, scale3, scale4;

// IMPORTANT: Remplacer par l'adresse MAC de votre ESP32 Esclave
// Format: {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
// Vous l'obtiendrez depuis le Moniteur Série de l'Esclave
uint8_t slaveMAC[] = {0x8C, 0x4F, 0x00, 0x63, 0x4A, 0xF6};

void setup()
{
  Serial.begin(115200);
  
  // Attendre un peu pour la connexion du moniteur série
  delay(1000);

  // Initialiser les capteurs HX711
  Serial.println("Initializing HX711...");
  scale1.begin(DOUT_PIN_1, CLK_PIN);
  scale2.begin(DOUT_PIN_2, CLK_PIN);
  scale3.begin(DOUT_PIN_3, CLK_PIN);
  scale4.begin(DOUT_PIN_4, CLK_PIN);
  
  // Tarer les capteurs (définir le poids actuel comme zéro)
  Serial.println("Taring scale... Please ensure no load on sensor.");
  delay(2000);
  scale1.tare(10);
  scale2.tare(10);
  scale3.tare(10);
  scale4.tare(10);
  Serial.println("Tare complete!");
  Serial.print("Offset 1: ");
  Serial.println(scale1.get_offset());
  Serial.print("Offset 2: ");
  Serial.println(scale2.get_offset());
  Serial.print("Offset 3: ");
  Serial.println(scale3.get_offset());
  Serial.print("Offset 4: ");
  Serial.println(scale4.get_offset());
  
  // Définir le facteur de calibration
  scale1.set_scale(SCALE_FACTOR_1);
  scale2.set_scale(SCALE_FACTOR_2);
  scale3.set_scale(SCALE_FACTOR_3);
  scale4.set_scale(SCALE_FACTOR_4);
  Serial.println("Scale ready!");
  Serial.println();

  // Initialisation bouton
  pinMode(BOUTON_TARE, INPUT_PULLUP);
  dernierEtatBouton = digitalRead(BOUTON_TARE);

  // Démarrer la communication Bluetooth en mode Maître avec le nom "ESP32_Master"
  // Passer 'true' comme deuxième argument active le mode maître
  if (!SerialBT.begin("ESP32Master", true))
  {
    Serial.println("Bluetooth initialization failed!");
    while(1); // Arrêt ici
  }
  
  Serial.println("ESP32 Master Bluetooth Serial Started.");
  Serial.println("Make sure ESP32Slave is already running!");
  
  // Attendre un peu avant de tenter la connexion
  delay(3000);

  // Tentative de connexion au périphérique ESP32 esclave
  Serial.println("Attempting to connect to ESP32Slave...");
  Serial.print("Using MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", slaveMAC[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Tentative de connexion via l'adresse MAC (plus fiable que le nom)
  bool connected = SerialBT.connect(slaveMAC);
  
  if (connected)
  {
    Serial.println("Connected to ESP32Slave successfully!");
  }
  else
  {
    Serial.println("Failed initial connection.");
    Serial.println("Retrying connection every 3 seconds...");
    
    // Réessayer la connexion en boucle
    int attempts = 0;
    while (!SerialBT.connected())
    {
      attempts++;
      Serial.print("Attempt ");
      Serial.print(attempts);
      Serial.println(": Trying to connect to ESP32Slave...");
      
      SerialBT.connect(slaveMAC);
      delay(3000); // Attendre 3 secondes avant de réessayer
      
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
  // Vérifie si le bouton a été pressé pour lancer la tare
  bool etatActuelBouton = digitalRead(BOUTON_TARE);
  if (etatActuelBouton == LOW && dernierEtatBouton == HIGH) {
    // Appui détecté
    Serial.println("Tare en cours...");
    scale1.tare();
    scale2.tare();
    scale3.tare();
    scale4.tare();
    Serial.println("Tare effectuee!");
    delay(500); // anti-rebond
  }
  dernierEtatBouton = etatActuelBouton;

  if (scale1.is_ready() && scale2.is_ready() && scale3.is_ready() && scale4.is_ready())
  {
    float p1 = scale1.get_units(1);
    float p2 = scale2.get_units(1);
    float p3 = scale3.get_units(1);
    float p4 = scale4.get_units(1);

    float f1 = (p1 / 1000.0) * 9.80665;
    float f2 = (p2 / 1000.0) * 9.80665;
    float f3 = (p3 / 1000.0) * 9.80665;
    float f4 = (p4 / 1000.0) * 9.80665;

    Serial.print(f1, 2); Serial.print("\t");
    Serial.print(f2, 2); Serial.print("\t");
    Serial.print(f4, 2); Serial.print("\t");
    Serial.println(f3, 2);

    if (SerialBT.connected())
    {
      String data = String(f1, 2) + "\t" + String(f2, 2) + "\t" + String(f4, 2) + "\t" + String(f3, 2);
      SerialBT.println(data);
    }
    else
    {
      Serial.println("Warning: Bluetooth disconnected!");
    }
  }
  else
  {
    Serial.println("One or more HX711 not ready.");
  }

  if (SerialBT.available())
  {
    String received = SerialBT.readStringUntil('\n');
    Serial.println("Received from Slave: " + received);
  }

  delay(100);
  
}
