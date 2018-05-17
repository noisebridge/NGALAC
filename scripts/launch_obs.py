import os
import subprocess
import sys


OBS_LOC=r"C:\Program Files (x86)\obs-studio\bin\64bit\\"
os.chdir(OBS_LOC)
subprocess.Popen(["obs64.exe", OBS_LOC])
sys.exit(0)

# os.system("obs64.exe")
