# Passes all *.coffee files in the target directories to the coffee command and directs the output to the destination directory.

import sys
import os
import subprocess
import shlex
import shutil

def quote(str):
  return '"' + str + '"'

def main():
  pathToNode = os.path.abspath(sys.argv[1])
  pathToCoffee = os.path.abspath(sys.argv[2])
  outDir = os.path.abspath(sys.argv[3])

  buildDirs = []
  for i in range(4, len(sys.argv)):
    buildDirs.append(os.path.abspath(sys.argv[i]))
  
  print("Nuking output directory...")
  shutil.rmtree(outDir, True)

  baseCommand = quote(pathToNode) + " " + quote(pathToCoffee) + " " + "--output " + quote(outDir)
  for buildDir in buildDirs:
    for root, dirs, files in os.walk(buildDir):
      for file in files:
        filename = os.path.join(root, file)
        print("Compiling " + filename)
        sys.stdout.flush()
        subprocess.call(shlex.split(baseCommand + ' ' + quote(filename)))

  # Okay, this is awesome. The coffee compiler calls mkdir -p, which under windows creates a -p folder.
  # Nuke it.
  shutil.rmtree("-p", True)

if __name__ == "__main__":
  main()
