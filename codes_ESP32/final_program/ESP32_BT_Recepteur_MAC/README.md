# 📟 ESP32 Bluetooth Récepteur (Esclave)

Code pour l'ESP32 esclave qui reçoit les données via Bluetooth et les affiche sur un écran OLED.

## 📋 Description

Cet ESP32 agit comme **esclave Bluetooth**. Il :
- Reçoit les données de force via Bluetooth
- Affiche les 4 forces sur un écran OLED
- Affiche l'état de la connexion Bluetooth
- Permet de mettre en pause l'affichage via bouton
- Affiche son adresse MAC au démarrage

## 🔌 Connexions matérielles

### Écran OLED SSD1306 (I2C)

| OLED | Broche ESP32 | Signal |
|------|--------------|--------|
| VCC | 3.3V | Alimentation |
| GND | GND | Masse |
| SDA | GPIO 21 | Données I2C |
| SCL | GPIO 22 | Horloge I2C |

**Adresse I2C** : 0x3C (par défaut)  
**Résolution** : 128x64 pixels

### Bouton pause/reprise

| Composant | Broche ESP32 |
|-----------|--------------|
| Bouton | GPIO 13 (INPUT_PULLUP) |

### Connexion USB (optionnelle)

- Port USB pour alimentation et débogage
- Pas nécessaire en fonctionnement normal

## ⚙️ Configuration

### 1. Adresse I2C de l'écran

Si votre écran utilise une adresse différente, modifier ligne 12 :

```cpp
#define SCREEN_ADDRESS 0x3C  // Changer si nécessaire (0x3D parfois)
```

Pour trouver l'adresse I2C :
1. Utiliser un scanner I2C
2. Ou tester 0x3C puis 0x3D

### 2. Nom Bluetooth

Ligne 66 :
```cpp
SerialBT.begin("ESP32Slave", false);  // Modifier le nom si désiré
```

## 🚀 Installation

### 1. Bibliothèques requises

Dans Arduino IDE, installer via Library Manager :
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `BluetoothSerial` (inclus avec ESP32)
- `esp_bt_device` (inclus avec ESP32)

### 2. Configuration Arduino IDE

- **Carte** : ESP32 Dev Module ou WROOM-32E
- **Port** : Sélectionner le port USB de l'ESP32
- **Vitesse** : 115200 bauds

### 3. Téléversement

1. Compiler et téléverser vers l'ESP32
2. Ouvrir le Moniteur Série (115200 bauds)
3. **Noter l'adresse MAC Bluetooth** affichée
4. Utiliser cette adresse dans le code du maître

## 📊 Fonctionnement

### Au démarrage

**Écran OLED affiche** :
```
ESP32Slave
MAC:
  8C:4F:00:63:4A:F6

Waiting for
Master...
```

**Moniteur Série affiche** :
```
ESP32 Slave Bluetooth Serial Started.
Device name: ESP32Slave
Bluetooth MAC Address: 8C:4F:00:63:4A:F6
Bluetooth is discoverable and waiting for connection...
.................
Connected to ESP32_Master successfully!
```

### Pendant la réception

**Écran OLED affiche** :
```
Forces (N):
─────────────────
C1: 12.3    C3: 34.5
C2: 23.4    C4: 45.6

BT: Connecte
```

**Moniteur Série affiche** :
```
Received data: 12.34    23.45   34.56   45.67
Received data: 12.35    23.46   34.57   45.68
...
```

### Utilisation du bouton pause

1. **Appuyer sur le bouton** (GPIO 13)
2. L'affichage se met en pause :
   ```
   
      ARRETE
   
   ```
3. **Appuyer à nouveau** pour reprendre l'affichage

## 🔧 Détails techniques

### Format d'affichage OLED

```
┌──────────────────────────┐
│ Forces (N):              │ ← En-tête
│ ─────────────────────── │ ← Ligne séparatrice
│ C1: 12.3    C3: 34.5    │ ← Valeurs (1 décimale)
│ C2: 23.4    C4: 45.6    │
│                          │
│ BT: Connecte            │ ← État Bluetooth
└──────────────────────────┘
```

### Parsing des données

```cpp
// Format reçu : "12.34\t23.45\t34.56\t45.67\n"
float f1, f2, f3, f4;
sscanf(incomingData.c_str(), "%f\t%f\t%f\t%f", &f1, &f2, &f3, &f4);
```

### Gestion de l'affichage

- **Rafraîchissement** : toutes les 200ms
- **État Bluetooth** : vérifié en continu
- **Perte de connexion** : affichage "-" au lieu des valeurs

### Communication I2C

- **Protocole** : I2C hardware
- **Fréquence** : Standard (100 kHz)
- **Broches** : SDA=21, SCL=22 (défaut ESP32)

## 🔍 Dépannage

### Problème : "SSD1306 allocation failed"

**Solutions** :
1. Vérifier les connexions I2C (SDA, SCL, VCC, GND)
2. Vérifier l'adresse I2C (essayer 0x3D si 0x3C ne fonctionne pas)
3. Tester avec un scanner I2C
4. Vérifier que l'écran est bien alimenté (3.3V)
5. Essayer un autre câble ou écran

### Problème : Écran affiche "Waiting for Master..." indéfiniment

**Solutions** :
1. Vérifier que l'ESP32 maître est démarré
2. Vérifier que l'adresse MAC dans le maître est correcte
3. Rapprocher les deux ESP32
4. Redémarrer les deux ESP32 (esclave d'abord)

### Problème : "Parsing error: expected 4 values"

**Solutions** :
- Vérifier le format de données envoyé par le maître
- Vérifier que les 4 valeurs sont bien séparées par tabulations
- Regarder les données brutes dans le Moniteur Série

### Problème : Affichage saccadé ou lent

**Solutions** :
- Modifier le délai de rafraîchissement (ligne 254) :
  ```cpp
  if (millis() - lastDisplayUpdate > 200) {  // Augmenter à 300-500
  ```
- Réduire la fréquence d'envoi du maître

## 📝 Personnalisation

### Modifier la mise en page OLED

Fonction `afficherValeurs()` (lignes 131-192) :

```cpp
// Position des textes
display.setCursor(x, y);  // Modifier x et y

// Taille du texte
display.setTextSize(1);   // 1 = petit, 2 = grand

// Nombre de décimales
display.print(lastForces[0], 1);  // Changer 1 à 0, 2, 3...
```

### Changer le délai de rafraîchissement

Ligne 254 :
```cpp
if (millis() - lastDisplayUpdate > 200) {  // Modifier 200 (en ms)
```

### Modifier le nom Bluetooth

Ligne 66 :
```cpp
SerialBT.begin("ESP32Slave", false);  // Changer le nom
```

### Ajouter plus de valeurs à afficher

1. Augmenter le tableau `lastForces[]`
2. Modifier le parsing (`sscanf`)
3. Adapter l'affichage OLED

## 💡 Conseils

- 🔋 L'écran OLED peut rester allumé en permanence
- 📡 Toujours démarrer l'esclave AVANT le maître
- 🔌 Le bouton pause économise la vie de l'OLED
- 🖥️ Le Moniteur Série aide au débogage
- 📝 Noter l'adresse MAC pour la configuration du maître

## 📚 Références

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [BluetoothSerial ESP32](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial)
- [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

## 🎨 Améliorations possibles

- Ajouter un graphique des forces en temps réel
- Afficher des statistiques (min, max, moyenne)
- Ajouter des seuils d'alerte visuels
- Mémoriser les valeurs maximales
- Ajouter un mode d'économie d'énergie

---

**Retour au projet** : [README principal](../README.md)
