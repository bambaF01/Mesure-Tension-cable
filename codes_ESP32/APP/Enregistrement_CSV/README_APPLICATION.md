#  Enregistreur ESP32 vers CSV

Scripts de lancement pour enregistrer automatiquement les données ESP32 en CSV.

##  Structure du projet

```
Enregistrement_CSV/
├── lecture_USB_ESP32_mac.command     #  Lanceur macOS
├── lecture_USB_ESP32_linux.sh        #  Lanceur Linux
├── lecture_USB_ESP32_windows.bat     #  Lanceur Windows
├── enregistrement_serial.py          #  Script Python principal
├── requirements.txt                  #  Dépendances Python
├── README_APPLICATION.md             #  Ce fichier
└── donnees_csv/                      #  Dossier contenant tous les fichiers CSV
    ├── donnees_forces_20251022_152345.csv
    ├── donnees_forces_20251022_154210.csv
    └── ...
```

##  macOS

**Double-cliquez** sur : **`lecture_USB_ESP32_mac.command`**

Ou depuis le terminal :
```bash
./lecture_USB_ESP32_mac.command
```

---

##  Linux

**Double-cliquez** sur : **`lecture_USB_ESP32_linux.sh`**

Ou depuis le terminal :
```bash
./lecture_USB_ESP32_linux.sh
```

---

##  Windows

**Double-cliquez** sur : **`lecture_USB_ESP32_windows.bat`**

---

##  Fonctionnement

Quand vous lancez l'application :

1.  Détection automatique du port ESP32
2.  Connexion automatique
3.  Création d'un fichier CSV avec horodatage
4.  Enregistrement en temps réel des 4 forces
5.  Ctrl+C pour arrêter et sauvegarder

---

##  Fichiers CSV générés

**Emplacement** : Tous les fichiers CSV sont automatiquement enregistrés dans le dossier `donnees_csv/`

**Format** : `donnees_forces_YYYYMMDD_HHMMSS.csv`

**Contenu** :
```csv
12.34,23.45,34.56,45.67
12.35,23.46,34.57,45.68
12.36,23.47,34.59,45.69
...
```

Le dossier `donnees_csv/` est créé automatiquement au premier lancement.

---

##  Conseils

- Les fichiers CSV sont automatiquement enregistrés dans `donnees_csv/`
- Les lanceurs fonctionnent sur **n'importe quel ordinateur** sans modification
- La détection de l'ESP32 est automatique, pas besoin de configuration

---

Profitez de votre enregistreur ESP32 ! 
