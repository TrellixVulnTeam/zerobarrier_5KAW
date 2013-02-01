# Copies active files to the output directory.

import shutil

for f in ActiveFiles:
  sourcePath = os.path.join(ParsedArgs.asset_dir, f)
  outputPath = os.path.join(ParsedArgs.output_dir, f)
  outputFolder = os.path.dirname(outputPath)

  if not os.path.exists(outputFolder):
    os.makedirs(outputFolder)

  if ParsedArgs.verbose:
    muffin_log("Copying " + sourcePath + " to " + outputPath)

  shutil.copy(sourcePath, outputPath)

  muffin_queued_deactivate_file(f)
