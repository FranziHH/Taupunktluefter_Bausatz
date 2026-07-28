Import('env')
import os
import re
import shutil
import gzip
from datetime import datetime

OUTPUT_DIR = "build_output{}".format(os.path.sep)
FIRMWARE_DIR = "firmware{}".format(os.path.sep)

project_dir = env.get("PROJECT_DIR")

# Wir bauen den Pfad explizit zusammen
ino_path = os.path.join(project_dir, "ESP32_Taupunktluefter_V7.0.ino")

def get_version_from_ino(file_path):
    version_pattern = r'String\s+Software_version\s*=\s*"([^"]+)"'

    if not os.path.exists(file_path):
        return "Unknown"

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
        match = re.search(version_pattern, content)
        if match:
            return match.group(1)

    return "Unknown"

def _get_cpp_define_value(env, define):
    define_list = [item[-1] for item in env["CPPDEFINES"] if item[0] == define]

    if define_list:
        return define_list[0]

    return None

def _create_dirs(dirs=["map", "release", "firmware"]):
    os.makedirs(FIRMWARE_DIR, exist_ok=True)
    for d in dirs:
        os.makedirs(os.path.join(OUTPUT_DIR, d), exist_ok=True)

def create_release(source):
    release_name_def = _get_cpp_define_value(env, "RELEASE_NAME")
    current_date = datetime.now().strftime("%Y-%m-%d")
    if release_name_def:
        release_name = release_name_def.replace("\\\"", "")
        version = get_version_from_ino(ino_path)
        release_file = os.path.join(OUTPUT_DIR, "release", f"{current_date}_{version}_{release_name}.bin")
        firmware_release_file = os.path.join(FIRMWARE_DIR, f"{version}_{release_name}.bin")
        print(f"Copying {source} to {release_file}")
        shutil.copy(source, release_file)
        shutil.copy(source, firmware_release_file)

        full_release_file = os.path.join(OUTPUT_DIR, "release", f"{current_date}_{version}_{release_name}_full.bin")
        firmware_full_release_file = os.path.join(FIRMWARE_DIR, f"{version}_{release_name}_full.bin")
        factory_bin = os.path.join(env.subst("$BUILD_DIR"), "firmware.factory.bin")
        shutil.copy(factory_bin, full_release_file)
        shutil.copy(factory_bin, firmware_full_release_file)

    else:
        print(f"Copying: there is no RELEASE_NAME")
        # variant = env["PIOENV"]
        # bin_file = "{}firmware{}{}.bin".format(OUTPUT_DIR, os.path.sep, variant)
        # print(f"Copying {source} to {bin_file}")
        # shutil.copy(source, bin_file)

def bin_rename_copy(source, target, env):
    _create_dirs()
    variant = env["PIOENV"]
    builddir = os.path.join(env["PROJECT_BUILD_DIR"],  variant)
    source_map = os.path.join(builddir, env["PROGNAME"] + ".map")

    # create string with location and file names based on variant
    map_file = "{}map{}{}.map".format(OUTPUT_DIR, os.path.sep, variant)

    create_release(str(target[0]))

    # copy firmware.map to map/<variant>.map
    if os.path.isfile("firmware.map"):
        print("Found linker mapfile firmware.map")
        shutil.copy("firmware.map", map_file)
    if os.path.isfile(source_map):
        print(f"Found linker mapfile {source_map}")
        shutil.copy(source_map, map_file)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", bin_rename_copy)
env.AlwaysBuild("$BUILD_DIR/${PROGNAME}.bin")
