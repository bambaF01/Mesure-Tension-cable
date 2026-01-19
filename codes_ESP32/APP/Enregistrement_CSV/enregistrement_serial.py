import serial
import serial.tools.list_ports
import csv
from datetime import datetime
import os

# Configuration
BAUDRATE = 115200

# Créer le dossier pour les données CSV s'il n'existe pas
DOSSIER_CSV = 'donnees_csv'
if not os.path.exists(DOSSIER_CSV):
    os.makedirs(DOSSIER_CSV)
    print(f"📁 Dossier '{DOSSIER_CSV}' créé.")

# Chemin complet du fichier CSV dans le dossier dédié
nom_fichier = f'donnees_forces_{datetime.now().strftime("%Y%m%d_%H%M%S")}.csv'
FICHIER_CSV = os.path.join(DOSSIER_CSV, nom_fichier)

def trouver_port_esp32():
    """Détecte automatiquement le port de l'ESP32"""
    ports = serial.tools.list_ports.comports()
    
    print("\n=== Ports série détectés ===")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device}")
        print(f"   Description: {port.description}")
        print(f"   Fabricant: {port.manufacturer}")
        
        # Recherche automatique d'ESP32 ou CP210x (puce USB de l'ESP32)
        keywords = ['ESP32', 'CP210', 'CH340', 'USB Serial', 'UART', 'Silicon Labs']
        if any(keyword.lower() in str(port.description).lower() or 
               keyword.lower() in str(port.manufacturer).lower() 
               for keyword in keywords):
            print(f"   >>> ESP32 détecté ici! <<<")
            return port.device
    
    # Si aucun port automatique trouvé, demander à l'utilisateur
    if len(ports) == 0:
        print("Aucun port série détecté!")
        return None
    
    print("\n  ESP32 non détecté automatiquement.")
    choix = input(f"Choisissez le numéro du port (1-{len(ports)}) ou Entrée pour annuler: ")
    
    if choix.isdigit() and 1 <= int(choix) <= len(ports):
        return ports[int(choix)-1].device
    
    return None

print(f"\n📊 Enregistrement des données dans: {FICHIER_CSV}")
print("Recherche de l'ESP32...")

PORT = trouver_port_esp32()

if PORT is None:
    print("❌ Aucun port sélectionné. Arrêt du programme.")
    exit(1)

print(f"\n✅ Utilisation du port: {PORT}")

try:
    # Ouvrir le port série
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
    print(f"✓ Connecté au port {PORT}")
    
    # Créer le fichier CSV avec en-tête
    with open(FICHIER_CSV, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['Force_1_N', 'Force_2_N', 'Force_3_N', 'Force_4_N'])
        
        print("Enregistrement en cours...")
        
        while True:
            if ser.in_waiting > 0:
                ligne = ser.readline().decode('utf-8').strip()
                
                # Vérifier si c'est une ligne de données (4 valeurs séparées par des tabulations)
                if '\t' in ligne:
                    try:
                        valeurs = ligne.split('\t')
                        if len(valeurs) == 4:
                            writer.writerow(valeurs)
                            csvfile.flush()  # Écrire immédiatement
                            print(f"{valeurs}")
                    except:
                        pass
                        
except KeyboardInterrupt:
    print("\nEnregistrement arrêté.")
except Exception as e:
    print(f"Erreur: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
    print(f"Données sauvegardées dans {FICHIER_CSV}")
