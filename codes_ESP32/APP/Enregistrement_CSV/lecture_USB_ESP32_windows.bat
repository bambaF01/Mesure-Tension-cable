@echo off
REM Script de lancement pour Windows
REM Double-cliquer ce fichier pour lancer l'application

cd /d "%~dp0"

color 0A
cls
echo.
echo ╔════════════════════════════════════════╗
echo ║   Enregistreur CSV - ESP32             ║
echo ╚════════════════════════════════════════╝
echo.

REM Trouver Python
python --version >nul 2>&1
if %errorlevel% equ 0 (
    set PYTHON_CMD=python
) else (
    py --version >nul 2>&1
    if %errorlevel% equ 0 (
        set PYTHON_CMD=py
    ) else (
        echo Erreur: Python non trouve!
        echo Installez Python depuis https://www.python.org
        pause
        exit /b 1
    )
)

REM Vérifier si pyserial est installé
%PYTHON_CMD% -c "import serial" >nul 2>&1
if %errorlevel% neq 0 (
    echo Installation de pyserial...
    REM Essayer d'abord avec --user (sans droits admin)
    %PYTHON_CMD% -m pip install --user pyserial >nul 2>&1
    if %errorlevel% neq 0 (
        REM Si ça échoue, essayer sans --user (peut demander admin)
        %PYTHON_CMD% -m pip install pyserial
    )
)

REM Vérifier que pyserial est bien installé avant de lancer
%PYTHON_CMD% -c "import serial" >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo Erreur: pyserial n'est pas installe!
    echo Installez-le manuellement avec: pip install --user pyserial
    echo.
    pause
    exit /b 1
)

REM Afficher les informations
echo [OK] Python trouve: %PYTHON_CMD%
echo [OK] Pyserial installe
echo.
echo Connectez l'ESP32 via USB...
echo ATTENTION: Appuyez sur Ctrl+C pour arreter l'enregistrement
echo.
echo ========================================
echo.

REM Lancer le script
%PYTHON_CMD% enregistrement_serial.py

echo.
echo ========================================
echo [OK] Programme termine.
echo [i] Vos donnees sont dans le dossier 'donnees_csv/'
echo.
echo Appuyez sur une touche pour fermer...
pause >nul
