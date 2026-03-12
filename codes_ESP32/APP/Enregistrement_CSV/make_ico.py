from pathlib import Path

try:
    from PIL import Image
except ImportError as exc:
    raise SystemExit("Pillow n'est pas installe. Lancez: pip install pillow") from exc

src = Path(__file__).with_name("icone-APP.png")
dst = Path(__file__).with_name("icone-APP.ico")

if not src.exists():
    raise SystemExit(f"Fichier manquant: {src}")

img = Image.open(src)
# Tailles classiques pour Windows
sizes = [(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]
img.save(dst, sizes=sizes)
print(f"Icone generee: {dst}")
