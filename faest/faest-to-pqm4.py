#!/usr/bin/env python3
# This script uses the prepare_nist.py script to export the
# specified implementation to the pqm4 project.
import os
import sys
import subprocess
import shutil

TARGET_DIR = "target"


all = ['faest_128f', 'faest_128s', 'faest_192f', 'faest_192s', 'faest_256f', 'faest_256s',\
        'faest_em_128f', 'faest_em_128s', 'faest_em_192f', 'faest_em_192s', 'faest_em_256f', 'faest_em_256s']


def prepare_single(name: str):
    print(f"Preparing {name}")
    cmd = ["python3",
           "faest-ref/tools/prepare_nist.py",
           "faest-ref",
           "faest-ref/build",
           TARGET_DIR,
           name]
    r = subprocess.run(cmd, text=True)
    if r.returncode != 0:
        exit(1)


def prepare_all():
    for e in all:
        prepare_single(e)


def move_single_to_pqm4(name: str):
    folder = f"../crypto_sign/{name}/ref"

    # Remove existing folder with content
    try:
        shutil.rmtree(folder)
    except FileNotFoundError:
        pass

    # Copy to folder
    shutil.copytree(f"target/Reference_Implementation/{name}", folder)


def move_all_to_pqm4():
    for e in all:
        move_single_to_pqm4(e)


if __name__ == '__main__':
    # Create target dir for temporary files
    os.makedirs(TARGET_DIR, exist_ok=True)

    # Prepare files
    if len(sys.argv) == 1:
        prepare_all()
        move_all_to_pqm4()
    elif len(sys.argv) == 2:
        name = sys.argv[1]
        prepare_single(name)
        move_single_to_pqm4(name)
    
