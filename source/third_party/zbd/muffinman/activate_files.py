# Adds all files in the asset directory tree that match the specified criteria.

if not 'modified_only' in locals():
  locals()['modified_only'] = None

if not 'write_timestamps' in locals():
  locals()['write_timestamps'] = None

if not 'with_extensions' in locals():
  locals()['with_extensions'] = None

if with_extensions != None and not isinstance(with_extensions, list):
  muffin_error("Expecting array for 'with_extensions'")

if not 'without_extensions' in locals():
  locals()['without_extensions'] = None

if without_extensions != None and not isinstance(without_extensions, list):
  muffin_error("Expecting array for 'without_extensions'")

if not 'source_dirs' in locals():
  locals()['source_dirs'] = [ParsedArgs.asset_dir]
else:
  resolvedDirs = []
  for sourceDir in source_dirs[:]:
    resolvedDirs.append(muffin_resolve_path(sourceDir))

  source_dirs = resolvedDirs


tempDir = None
timestamps = {}

if write_timestamps:
  if not ParsedArgs.temp_dir:
    muffin_error("Can't write timestamps, no temp_dir specified.")

  tempDir = os.path.join(ParsedArgs.temp_dir, "activate_files/")
  if not os.path.exists(tempDir):
    os.makedirs(tempDir)

if write_timestamps or modified_only:
  timestampPath = os.path.join(tempDir, "stamps")
  if os.path.exists(timestampPath):
    timestampFile = open(timestampPath, 'r')
    timestamps = json.loads(timestampFile.read())
    timestampFile.close()

for sourceDir in source_dirs:
  for root, dirs, files in os.walk(sourceDir):
    for f in files:
      addFile = True

      if with_extensions != None:
        for ext in with_extensions:
          if not f.endswith(ext):
            addFile = False
            break

      if without_extensions != None:
        for ext in without_extensions:
          if f.endswith(ext):
            addFile = False
            break

      if addFile:
        fullPath = os.path.join(root, f)
        path = os.path.relpath(fullPath, ParsedArgs.asset_dir)

        lastAccess = os.stat(fullPath).st_mtime
        if modified_only:
          if path in timestamps:
            addFile = (lastAccess != timestamps[path])

        if write_timestamps:
          timestamps[path] = lastAccess

        if addFile:
          muffin_activate_file(path)

if write_timestamps:
  timestampPath = os.path.join(tempDir, "stamps")
  timestampFile = open(timestampPath, 'w')
  timestampFile.write(json.dumps(timestamps))
  timestampFile.close()
