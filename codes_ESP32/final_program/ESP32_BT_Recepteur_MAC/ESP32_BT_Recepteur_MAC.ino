#include <Arduino.h>
#include <BluetoothSerial.h>
#include <esp_bt_device.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuration de l'écran OLED - SBCOLED01V2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Broche de réinitialisation (ou -1 si partage avec Arduino)
#define SCREEN_ADDRESS 0x3C  // Adresse I2C pour SBCOLED01V2 (généralement 0x3C)
// Broches I2C pour ESP32 WROOM-32E
#define SDA_PIN 21
#define SCL_PIN 22

// Bouton poussoir pour arrêt d'affichage
#define BOUTON_ARRET 13
bool dernierEtatBoutonArret = HIGH;
bool affichageActif = true;  // État de l'affichage

// Création des objets
BluetoothSerial SerialBT;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
float lastForces[4] = {0.0, 0.0, 0.0, 0.0};
unsigned long lastUpdate = 0;
unsigned long lastDisplayUpdate = 0;
bool hasData = false;


void setup()
{
  // Initialiser la communication série à 115200 bauds pour le débogage
  Serial.begin(115200);
 
  // Attendre un peu pour la connexion du moniteur série
  delay(1000);

  // Initialiser I2C avec les broches personnalisées pour ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
 
  // Initialiser l'écran OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // Arrêt ici si l'OLED échoue
  }
 
  // Effacer l'écran et afficher le message de démarrage
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 Slave");
  display.println("Initializing...");
  display.display();
  delay(1000);

  // Initialisation bouton d'arrêt
  pinMode(BOUTON_ARRET, INPUT_PULLUP);
  dernierEtatBoutonArret = digitalRead(BOUTON_ARRET);

  // Démarrer la communication Bluetooth et définir le nom "ESP32_Slave"
  // Utiliser un nom sans caractères spéciaux
  if (!SerialBT.begin("ESP32Slave", false)) // false = mode esclave
  {
    Serial.println("Bluetooth initialization failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BT Failed!");
    display.display();
    while(1); // Arrêt ici
  }
 
  Serial.println("ESP32 Slave Bluetooth Serial Started.");
  Serial.println("Device name: ESP32Slave");
 
  // Obtenir et afficher l'adresse MAC Bluetooth
  const uint8_t* mac = esp_bt_dev_get_address();
  Serial.print("Bluetooth MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
 
  // Afficher l'adresse MAC sur l'OLED
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

  // Attendre jusqu'à ce que le périphérique ESP32 Maître se connecte à cet ESP32 Esclave
  while (!SerialBT.connected())
  {
    delay(1000);  // Attendre 1 seconde avant de vérifier à nouveau
    Serial.print(".");
  }

  // Une fois connecté, afficher un message de confirmation
  Serial.println("");
  Serial.println("Connected to ESP32_Master successfully!");
 
  // Afficher l'état de connexion sur l'OLED
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

// Fonction pour afficher les valeurs sur l'OLED
void afficherValeurs() {
  if (!affichageActif) return;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Measures:");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  display.setTextSize(2);
  
  // Force 1
  display.setCursor(0, 18);
  if (hasData && SerialBT.connected()) {
    display.print(lastForces[0], 1);
  } else {
    display.print("-");
  }
  
  // Force 2
  display.setCursor(0, 36);
  if (hasData && SerialBT.connected()) {
    display.print(lastForces[1], 1);
  } else {
    display.print("-");
  }
  
  // Force 3
  display.setCursor(64, 18);
  if (hasData && SerialBT.connected()) {
    display.print(lastForces[2], 1);
  } else {
    display.print("-");
  }
  
  // Force 4
  display.setCursor(64, 36);
  if (hasData && SerialBT.connected()) {
    display.print(lastForces[3], 1);
  } else {
    display.print("-");
  }
  
  // État Bluetooth
  display.setTextSize(1);
  display.setCursor(0, 56);
  if (SerialBT.connected()) {
    display.print("BT: Connected");
  } else {
    display.print("BT: Disconnected");
  }
  
  display.display();
}

void loop()
{
  // Vérifie si le bouton d'arrêt d'affichage a été pressé
  bool etatActuelBoutonArret = digitalRead(BOUTON_ARRET);
  if (etatActuelBoutonArret == LOW && dernierEtatBoutonArret == HIGH) {
    // Appui détecté - inverse l'état d'affichage
    affichageActif = !affichageActif;
    if (affichageActif) {
      Serial.println("Display ON");
      // Rafraîchit l'affichage avec les dernières données
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Display ON");
      display.display();
    } else {
      Serial.println("OFF");
      // Affiche un message d'arrêt
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(10, 20);
      display.println("OFF");
      display.display();
    }
    delay(500); // anti-rebond
  }
  dernierEtatBoutonArret = etatActuelBoutonArret;

  if (SerialBT.available())
  {
    String incomingData = SerialBT.readStringUntil('\n');
    incomingData.trim();

    // N'envoyer au Serial que si l'affichage est actif
    if (affichageActif) {
      Serial.println(incomingData);
    }

    float f1, f2, f3, f4;
    int parsed = sscanf(incomingData.c_str(), "%f\t%f\t%f\t%f", &f1, &f2, &f3, &f4);

    if (parsed == 4)
    {
      lastForces[0] = f1;
      lastForces[1] = f2;
      lastForces[2] = f3;
      lastForces[3] = f4;
      lastUpdate = millis();
      hasData = true;
    }
    else
    {
      Serial.println("Parsing error: expected 4 values.");
    }
  }

  // Vérifier si la connexion est perdue
  if (!SerialBT.connected() && hasData) {
    hasData = false;
  }
  
  // Mettre à jour l'affichage toutes les 200ms
  if (millis() - lastDisplayUpdate > 200) {
    afficherValeurs();
    lastDisplayUpdate = millis();
  }

  delay(50);  // Petit délai pour éviter de surcharger la boucle
}
