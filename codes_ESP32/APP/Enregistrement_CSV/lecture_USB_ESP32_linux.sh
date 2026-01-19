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

# Fonction pour afficher l'interface
show_interface() {
    clear
    echo "╔════════════════════════════════════════╗"
    echo "║    Enregistreur CSV - ESP32 🔌	     ║"
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
}

# Fonction pour lancer le script
run_script() {
    show_interface
    $PYTHON_CMD enregistrement_serial.py
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo " Programme terminé."
    echo " Vos données sont dans le dossier 'donnees_csv/'"
    echo ""
    echo "Appuyez sur Entrée pour fermer..."
    read
}

# Lancer le script dans un nouveau terminal
if command -v gnome-terminal &> /dev/null; then
    gnome-terminal -- bash -c "cd '$DIR' && export PYTHON_CMD='$PYTHON_CMD' && $(declare -f show_interface run_script) && run_script"
elif command -v konsole &> /dev/null; then
    konsole -e bash -c "cd '$DIR' && export PYTHON_CMD='$PYTHON_CMD' && $(declare -f show_interface run_script) && run_script"
elif command -v xterm &> /dev/null; then
    xterm -e bash -c "cd '$DIR' && export PYTHON_CMD='$PYTHON_CMD' && $(declare -f show_interface run_script) && run_script"
elif command -v x-terminal-emulator &> /dev/null; then
    x-terminal-emulator -e bash -c "cd '$DIR' && export PYTHON_CMD='$PYTHON_CMD' && $(declare -f show_interface run_script) && run_script"
else
    # Fallback: lancer dans le terminal actuel
    run_script
fi
