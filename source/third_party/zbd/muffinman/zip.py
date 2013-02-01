# Packages all ActiveFiles into a .zip with the given name.
import zipfile

if not 'output' in locals():
  muffin_error("No output file provided.")
else:
  output = muffin_resolve_path(output)
  if not os.path.isabs(output):
      os.path.join(ParsedArgs.output_dir, output)

if not 'relative_to' in locals():
  locals()['relative_to'] = ParsedArgs.asset_dir
else:
  relative_to = muffin_resolve_path(relative_to)


zf = zipfile.ZipFile(output, mode='w')
for f in ActiveFiles:
  path = os.path.join(ParsedArgs.asset_dir, f)

  if ParsedArgs.verbose:
    muffin_log("Zipping " + f + "...")

  zf.write(path, os.path.relpath(path, relative_to))
zf.close()
