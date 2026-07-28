import os
import shutil

# Beide Dateien liegen im Hauptverzeichnis (Root)
source_file = "config.h.example"
target_file = "config.h"

def check_config():
    if os.path.isfile(source_file):
        print(f"--- [INFO] Kopiere {source_file} -> {target_file} im Hauptverzeichnis ---")
        shutil.copyfile(source_file, target_file)
    else:
        print(f"--- [FEHLER] {source_file} fehlt! ---")

check_config()