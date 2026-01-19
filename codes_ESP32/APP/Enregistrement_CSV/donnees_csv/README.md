#  Dossier des données CSV

Ce dossier contient tous les fichiers CSV enregistrés lors des sessions de mesure.

##  Format des fichiers

### Nom des fichiers

```
donnees_forces_YYYYMMDD_HHMMSS.csv
```

**Exemple** : `donnees_forces_20251022_152345.csv`

- **YYYYMMDD** : Date (Année-Mois-Jour)
- **HHMMSS** : Heure (Heure-Minute-Seconde)

Chaque session d'enregistrement crée un nouveau fichier unique, aucun risque d'écrasement.

### Structure du fichier CSV

```csv
12.34,23.45,34.56,45.67
12.35,23.46,34.57,45.68
12.36,23.47,34.59,45.69
...
```

**Données** :
- Valeurs séparées par virgules
- Précision : 2 décimales
- Une ligne = une mesure instantanée
- Fréquence : ~10 mesures/seconde

##  Gestion des fichiers

### Création automatique

Le dossier `donnees_csv/` est créé automatiquement au premier lancement du script d'enregistrement s'il n'existe pas.

### Emplacement

```
Enregistrement_CSV/
└── donnees_csv/
    ├── donnees_forces_20251022_141230.csv
    ├── donnees_forces_20251022_143015.csv
    ├── donnees_forces_20251022_150542.csv
    └── ...
```

### Conservation

- Les fichiers ne sont jamais écrasés
- Chaque session génère un nouveau fichier
- Pensez à nettoyer régulièrement si nécessaire
- Pas de limite de nombre de fichiers

##  Utilisation des données

### Ouvrir dans Excel / LibreOffice

1. Double-cliquer sur le fichier CSV
2. Les colonnes sont automatiquement séparées
3. Créer des graphiques à partir des données

### Ouvrir dans Python

```python
import pandas as pd

# Charger le fichier
df = pd.read_csv('donnees_forces_20251022_152345.csv')

# Afficher les premières lignes
print(df.head())

# Calculer des statistiques
print(df.describe())

# Tracer les forces
import matplotlib.pyplot as plt
df.plot()
plt.show()
```

### Ouvrir dans MATLAB

```matlab
% Charger le fichier
data = readtable('donnees_forces_20251022_152345.csv');

% Afficher les premières lignes
head(data)

% Tracer les forces
plot(data.Force_1_N)
hold on
plot(data.Force_2_N)
plot(data.Force_3_N)
plot(data.Force_4_N)
legend('Force 1', 'Force 2', 'Force 3', 'Force 4')
xlabel('Échantillon')
ylabel('Force (N)')
```

##  Maintenance

**macOS/Linux** :
```bash
# Supprimer les fichiers de plus de 30 jours
find donnees_csv/ -name "*.csv" -mtime +30 -delete
```

**Windows** :
```batch
# Supprimer manuellement ou utiliser PowerShell
Get-ChildItem -Path "donnees_csv" -Filter "*.csv" | Where-Object {$_.LastWriteTime -lt (Get-Date).AddDays(-30)} | Remove-Item
```


##  Notes

-  Les fichiers sont créés uniquement pendant l'enregistrement
-  Format CSV standard, compatible avec tous les logiciels
-  Encodage UTF-8 pour compatibilité maximale
-  Ne pas modifier manuellement les fichiers en cours d'écriture

---

**Retour à la documentation** : [README Enregistrement](../README_APPLICATION.md) | [README principal](../../README.md)
