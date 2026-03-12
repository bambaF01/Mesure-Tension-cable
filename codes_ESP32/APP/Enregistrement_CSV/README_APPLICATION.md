#  Enregistreur ESP32 vers CSV

Scripts de lancement pour enregistrer automatiquement les données ESP32 en CSV.

##  Structure du projet

```
Enregistrement_CSV/
├── lecture_USB_ESP32_mac.command     #  Lanceur macOS
├── lecture_USB_ESP32_linux.sh        #  Lanceur Linux
├── lecture_USB_ESP32_windows.bat     #  Lanceur Windows
├── build_windows_pyinstaller.bat     #  Build .exe Windows (PyInstaller)
├── enregistrement_serial.py          #  Script Python principal
├── gui_enregistrement.py             #  Interface graphique locale
├── make_ico.py                        #  Conversion PNG -> ICO (Windows)
├── requirements.txt                  #  Dépendances Python
├── README_APPLICATION.md             #  Ce fichier
└── donnees_csv/                      #  Exemple de dossier (voir emplacement réel ci-dessous)
    └── README.md
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

##  Windows (EXE PyInstaller)

Pour distribuer un exécutable Windows avec icône :

1.  Ouvrir un terminal **Windows** dans ce dossier
2.  Lancer : **`build_windows_pyinstaller.bat`**
3.  L'exe sera dans **`dist/Mesure-Tension.exe`**

---

##  Fonctionnement

Quand vous lancez l'application :

1.  L'interface graphique se lance directement
1.  Détection automatique du port ESP32
2.  Connexion automatique
3.  Création d'un fichier CSV avec horodatage
4.  Enregistrement en temps réel des 4 forces
5.  Ctrl+C pour arrêter et sauvegarder

---

##  Fonctionnement (GUI)

L'interface graphique permet :

1.  Selection du port serie (auto-detect possible)
2.  Choix du dossier de sortie
3.  Bouton Start pour lancer l'enregistrement
4.  Bouton Stop pour terminer et sauvegarder
5.  Les valeurs recentes s'affichent en direct

---

##  Fichiers CSV générés

**Emplacement** : Tous les fichiers CSV sont automatiquement enregistrés dans le dossier :

- Linux/macOS : `~/MesureTension/donnees_csv`
- Windows : `C:\\Users\\<user>\\MesureTension\\donnees_csv`

**Format** : `donnees_forces_YYYYMMDD_HHMMSS.csv`

**Contenu** :
```csv
12.34,23.45,34.56,45.67
12.35,23.46,34.57,45.68
12.36,23.47,34.59,45.69
...
```

Le dossier est créé automatiquement au premier lancement.

---

##  Conseils

- Les fichiers CSV sont automatiquement enregistrés dans le dossier `MesureTension/donnees_csv` de l'utilisateur
- Les lanceurs fonctionnent sur **n'importe quel ordinateur** sans modification
- La détection de l'ESP32 est automatique, pas besoin de configuration

---

Profitez de votre enregistreur ESP32 ! 
