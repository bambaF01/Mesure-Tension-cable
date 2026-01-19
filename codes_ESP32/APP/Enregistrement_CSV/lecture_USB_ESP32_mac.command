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

# Afficher l'en-tête
clear
echo "╔════════════════════════════════════════╗"
echo "║    Enregistreur CSV - ESP32      	 ║"
echo "╚════════════════════════════════════════╝"
echo ""
echo "✓ Python trouvé: $PYTHON_CMD"
echo "✓ Pyserial installé"
echo ""
echo " Connectez l'ESP32 via USB..."
echo "  Appuyez sur Ctrl+C pour arrêter l'enregistrement"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Lancer le script
$PYTHON_CMD enregistrement_serial.py

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " Programme terminé."
echo " Vos données sont dans le dossier 'donnees_csv/'"
echo ""
echo "Appuyez sur Entrée pour fermer..."
read
