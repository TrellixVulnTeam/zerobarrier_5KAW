# Compiles all .coffee files in ActiveFiles, outputs the .js files to the output directory.

import shlex
import subprocess

if not 'path_to_nodejs' in locals():
  muffin_error("No path to nodejs specified.")
else:
  path_to_nodejs = muffin_resolve_path(path_to_nodejs)

if not 'path_to_coffee' in locals():
  muffin_error("No path to coffee compiler specified.")
else:
  path_to_coffee = muffin_resolve_path(path_to_coffee)

if not 'output_subdir' in locals():
  output_subdir = ""

def quote(str): return '"' + str + '"'

baseCommand = quote(path_to_nodejs) + " " + quote(path_to_coffee) + " " + "--output " + quote(os.path.join(ParsedArgs.output_dir, output_subdir))

for f in ActiveFiles:
  if f.endswith(".coffee"):
    pathToFile = os.path.normpath(os.path.join(ParsedArgs.asset_dir, f))

    if ParsedArgs.verbose:
      muffin_log("Compiling " + f + "...")

    subprocess.call(shlex.split(muffin_fix_string(baseCommand + ' ' + quote(pathToFile))))
    muffin_queued_deactivate_file(f)

# Okay, this is awesome. The coffee compiler calls mkdir -p, which under windows creates a -p folder.
# Nuke it.
if os.path.exists("-p"):
  os.rmdir("-p")
