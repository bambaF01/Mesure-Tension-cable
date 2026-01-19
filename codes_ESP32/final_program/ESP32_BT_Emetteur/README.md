#  ESP32 Bluetooth Émetteur (Maître)

Code pour l'ESP32 maître qui lit 4 capteurs HX711 et transmet les données via Bluetooth.

##  Description

Cet ESP32 agit comme **maître Bluetooth**. Il :
- Lit 4 capteurs de force HX711
- Convertit les mesures en Newtons
- Transmet les données via Bluetooth à l'esclave
- Affiche les valeurs sur le Moniteur Série
- Permet la tare manuelle via bouton

##  Connexions matérielles

### Capteurs HX711

| HX711 | Broche ESP32 | Signal |
|-------|--------------|--------|
| HX711_1 | GPIO 34 | DOUT_1 |
| HX711_2 | GPIO 35 | DOUT_2 |
| HX711_3 | GPIO 32 | DOUT_3 |
| HX711_4 | GPIO 33 | DOUT_4 |
| Tous | GPIO 18 | CLK (horloge) |

### Bouton de tare

| Composant | Broche ESP32 |
|-----------|--------------|
| Bouton | GPIO 13 (INPUT_PULLUP) |

### Connexion USB

- Port USB pour alimentation, débogage et enregistrement des données

##  Configuration

### 1. Facteurs de calibration

Modifier les lignes 12-15 selon vos capteurs :

```cpp
#define SCALE_FACTOR_1 110.95757
#define SCALE_FACTOR_2 109.68881
#define SCALE_FACTOR_3 109.27245
#define SCALE_FACTOR_4 107.75708
```

**Pour calibrer** :
1. Placer un poids connu (ex: 1000g)
2. Lire la valeur brute sur le Moniteur Série
3. Calculer : `SCALE_FACTOR = valeur_brute / 1000`

### 2. Adresse MAC de l'esclave

**IMPORTANT** : Remplacer ligne 28 par l'adresse MAC de votre ESP32 esclave :

```cpp
uint8_t slaveMAC[] = {0x8C, 0x4F, 0x00, 0x63, 0x4A, 0xF6};
```

Pour obtenir l'adresse MAC :
1. Flasher et démarrer l'ESP32 esclave
2. Ouvrir le Moniteur Série de l'esclave
3. Noter l'adresse MAC affichée au démarrage

##  Installation

### 1. Bibliothèques requises

Dans Arduino IDE, installer :
- `HX711` par Bogdan Necula (via Library Manager)
- `BluetoothSerial` (inclus avec ESP32)

### 2. Configuration Arduino IDE

- **Carte** : ESP32 Dev Module ou WROOM-32E
- **Port** : Sélectionner le port USB de l'ESP32
- **Vitesse** : 115200 bauds

### 3. Téléversement

1. Modifier l'adresse MAC de l'esclave (ligne 28)
2. Ajuster les facteurs de calibration si nécessaire
3. Compiler et téléverser vers l'ESP32

##  Fonctionnement

### Au démarrage

```
Initializing HX711...
Taring scale... Please ensure no load on sensor.
Tare complete!
Offset 1: 8388607
Offset 2: 8388607
Offset 3: 8388607
Offset 4: 8388607
Scale ready!

ESP32 Master Bluetooth Serial Started.
Make sure ESP32Slave is already running!
Attempting to connect to ESP32Slave...
Using MAC Address: 8C:4F:00:63:4A:F6
Connected to ESP32Slave successfully!
System ready! Starting measurements...
```

### En fonctionnement

Le Moniteur Série affiche continuellement :
```
12.34    23.45    34.56    45.67
12.35    23.46    34.57    45.68
12.36    23.47    34.59    45.69
...
```
(Forces en Newtons, séparées par tabulations)

### Utilisation du bouton de tare

1. Appuyer sur le bouton (GPIO 13)
2. Message : `Tare en cours...`
3. Les 4 capteurs sont remis à zéro
4. Message : `Tare effectuee!`

##  Détails techniques

### Calcul des forces

```cpp
// Lecture brute des capteurs
float p1 = scale1.get_units(1);

// Conversion en Newtons (masse en kg × g)
float f1 = (p1 / 1000.0) * 9.80665;
```


### Format de transmission Bluetooth

```
F1\tF2\tF3\tF4\n
```
- 4 valeurs séparées par tabulations
- 2 décimales de précision
- Terminé par retour à la ligne

### Gestion de la connexion

- **Connexion automatique** via adresse MAC au démarrage
- **Reconnexion automatique** si déconnexion détectée
- **10 tentatives** avant de redémarrer le Bluetooth
- **Délai de 3 secondes** entre chaque tentative

##  Dépannage

### Problème : "Bluetooth initialization failed!"

**Solutions** :
- Vérifier que le module Bluetooth n'est pas utilisé ailleurs
- Redémarrer l'ESP32
- Vérifier la version de la bibliothèque ESP32

### Problème : "Failed initial connection"

**Solutions** :
1. Vérifier que l'ESP32 esclave est démarré
2. Vérifier l'adresse MAC dans le code
3. Rapprocher les deux ESP32
4. Vérifier les logs de l'esclave

### Problème : "One or more HX711 not ready"

**Solutions** :
- Vérifier les connexions des HX711
- Vérifier l'alimentation des HX711 (3.3V ou 5V)
- Vérifier les broches DOUT et CLK
- Attendre quelques secondes après le démarrage

### Problème : Valeurs instables ou aberrantes

**Solutions** :
1. Effectuer une nouvelle tare
2. Vérifier les câblages
3. Recalibrer les capteurs
4. Améliorer l'alimentation électrique

##  Personnalisation

### Modifier la fréquence d'échantillonnage

Ligne 191 :
```cpp
delay(100);  // Changer à 50, 200, etc. (en ms)
```

### Changer le nombre de lectures moyennées

Ligne 155 :
```cpp
float p1 = scale1.get_units(1);  // Changer 1 à 5, 10, etc.
```
Plus de lectures = plus stable mais plus lent

### Modifier le nom Bluetooth

Ligne 75 :
```cpp
SerialBT.begin("ESP32Master", true);  // Changer le nom
```

##  Conseils

-  Alimenter les HX711 avec une source stable
-  Effectuer la tare au démarrage sans charge
-  Attendre la stabilisation avant les mesures
-  Démarrer l'esclave AVANT le maître
-  Les données sont aussi disponibles via USB série

##  Références

- [Bibliothèque HX711](https://github.com/bogde/HX711)
- [BluetoothSerial ESP32](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial)
- [Datasheet HX711](https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf)

---
