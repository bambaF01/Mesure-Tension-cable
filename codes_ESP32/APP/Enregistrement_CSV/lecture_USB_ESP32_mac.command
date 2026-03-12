#!/bin/bash
# Script de lancement pour macOS
# Double-cliquer ce fichier pour lancer l'application

# Obtenir le répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

# Trouver Python - Priorité à pyenv si disponible
if [ -f "$HOME/.pyenv/versions/3.11.9/bin/python" ]; then
    PYTHON_CMD="$HOME/.pyenv/versions/3.11.9/bin/python"
elif command -v python3 &> /dev/null; then
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
    $PYTHON_CMD -m pip install --user pyserial 2>/dev/null || \
    $PYTHON_CMD -m pip install pyserial --break-system-packages 2>/dev/null || \
    $PYTHON_CMD -m pip install pyserial
fi

# Lancer directement la GUI
$PYTHON_CMD gui_enregistrement.py
