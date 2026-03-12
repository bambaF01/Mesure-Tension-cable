#!/bin/bash
# Script de lancement pour Linux
# Double-cliquer ce fichier pour lancer l'application

# Obtenir le répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

# Trouver Python
if command -v python3 &> /dev/null; then
    PYTHON_CMD=python3
else
    echo " Python 3 non trouvé!"
    echo "Appuyez sur Entrée pour quitter..."
    read
    exit 1
fi

# Vérifier si pyserial est installé
$PYTHON_CMD -c "import serial" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Installation de pyserial..."
    # Essayer différentes méthodes d'installation
    $PYTHON_CMD -m pip install --user pyserial 2>/dev/null || \
    $PYTHON_CMD -m pip install pyserial --break-system-packages 2>/dev/null || \
    sudo $PYTHON_CMD -m pip install pyserial 2>/dev/null || \
    echo " Impossible d'installer pyserial automatiquement."
    echo "Veuillez exécuter: pip3 install --user pyserial"
fi

# Vérifier que pyserial est bien installé avant de lancer
$PYTHON_CMD -c "import serial" 2>/dev/null
if [ $? -ne 0 ]; then
    echo " pyserial n'est pas installé!"
    echo "Installez-le manuellement avec: pip3 install --user pyserial"
    read
    exit 1
fi

# Lancer directement la GUI
$PYTHON_CMD gui_enregistrement.py
