@echo off
REM Build Windows .exe with icon using PyInstaller
setlocal

cd /d "%~dp0"

REM Find Python
python --version >nul 2>&1
if %errorlevel% equ 0 (
    set PYTHON_CMD=python
) else (
    py --version >nul 2>&1
    if %errorlevel% equ 0 (
        set PYTHON_CMD=py
    ) else (
        echo Erreur: Python non trouve!
        pause
        exit /b 1
    )
)

REM Install dependencies
%PYTHON_CMD% -m pip install --user pyinstaller pyserial pillow

REM Build .ico if needed
if not exist "icone-APP.ico" (
    %PYTHON_CMD% make_ico.py
)

REM Build executable
%PYTHON_CMD% -m PyInstaller --noconsole --onefile --name "Mesure-Tension" --icon "icone-APP.ico" gui_enregistrement.py

echo.
echo Build termine. Fichier genere:
echo %CD%\\dist\\Mesure-Tension.exe
pause
