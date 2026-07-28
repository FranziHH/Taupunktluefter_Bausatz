import os
import shutil

# Beide Dateien liegen im Hauptverzeichnis (Root)
source_file = "config.h.example"
source_file_private = "config.h.private"
target_file = "config.h"

def check_wifi_config():
        if os.path.isfile(source_file_private):
            print(f"--- [INFO] Kopiere {source_file_private} -> {target_file} im Hauptverzeichnis ---")
            shutil.copyfile(source_file_private, target_file)
        else:
            if os.path.isfile(source_file):
                print(f"--- [INFO] Kopiere {source_file} -> {target_file} im Hauptverzeichnis ---")
                shutil.copyfile(source_file, target_file)
            else:
                print(f"--- [FEHLER] {source_file} fehlt! ---")

check_wifi_config()