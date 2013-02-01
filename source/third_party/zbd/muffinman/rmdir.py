# Deletes the specified directories.
import shutil

if not 'dirs' in locals():
  muffin_error("No directories specified to delete.")

for d in dirs:
  path = muffin_resolve_path(d)
  if os.path.exists(path):

    if path == os.getcwd():
      muffin_error("You tried to delete your cwd.")

    if path == ParsedArgs.asset_dir:
      muffin_error("You tried to delete you asset directory.")

    if ParsedArgs.verbose:
      muffin_log("Deleting " + path)

    shutil.rmtree(path, True)
