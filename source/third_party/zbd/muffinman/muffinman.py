# Muffinman
# Asset baking and management tool.
# Copyright (c) 2013 David Siems
#
# Do you know the Muffinman?
#

import sys
import os
import argparse
import re
import json

# Attempt to make this work in Python 2.7.x as well as 3.
try:
    unicode = unicode
except NameError:
    # 'unicode' is undefined, must be Python 3
    str = str
    unicode = str
    bytes = bytes
    basestring = (str,bytes)
    def muffin_fix_string(s): return s
else:
    # 'unicode' exists, must be Python 2
    str = str
    unicode = unicode
    bytes = str
    basestring = basestring
    def muffin_fix_string(s): return s.encode('ascii')

#
# Global error function, available to all scripts.
def muffin_error(error):
  print(error)
  sys.exit()


#
# Global argument parsing, available to all scripts.
class EmptyClass: pass # Shell class to hold arguments.
ParsedArgs = EmptyClass()
UnparsedArgs = sys.argv[1:]
def muffin_parse_args(argParser):
  global ParsedArgs
  global UnparsedArgs

  args = argParser.parse_known_args(UnparsedArgs, namespace=ParsedArgs)

  UnparsedArgs = args[1]
  return ParsedArgs


#
# Global logging function, available to all scripts.
def muffin_log(message):
  if not ParsedArgs.silent:
    print(message)


# Keys are the relative paths, values are the metadata objects associated with the file.
ActiveFiles = {}
InactiveFiles = {}
QueuedFilesToRemove = {}


#
# Adds a file to the global ActiveFiles dictionary.
def muffin_activate_file(path):
  global ActiveFiles
  global InactiveFiles
  global ParsedArgs

  if path not in ActiveFiles:
    if path in InactiveFiles:
      if ParsedArgs.verbose:
        muffin_log("Re-adding " + path)

      ActiveFiles[path] = InactiveFiles[path]
      del InactiveFiles[path]
    else:
      if ParsedArgs.verbose:
        muffin_log("Activating " + path)

      ActiveFiles[path] = {}


#
# Removes a file from ActiveFiles and puts it in InactiveFiles.
def muffin_deactivate_file(path):
  global ActiveFiles
  global InactiveFiles
  global ParsedArgs

  if path in ActiveFiles:
    if ParsedArgs.verbose:
      muffin_log("Deactivating " + path)

    InactiveFiles[path] = ActiveFiles[path]
    del ActiveFiles[path]


#
# Queues the removal of a file from ActiveFiles, useful if you're iterating over ActiveFiles.
def muffin_queued_deactivate_file(path):
  global QueuedFilesToRemove
  QueuedFilesToRemove[path] = True


#
# Flushes the remove queue.
def muffin_flush_queued_deactivate_files():
  global QueuedFilesToRemove
  for f in QueuedFilesToRemove:
    muffin_deactivate_file(f)

  QueuedFilesToRemove = {}

#
# Resolves a path given with replacement strings like $(TEMP).
def muffin_resolve_path(path):
  if ParsedArgs.temp_dir:
    path = path.replace("$(TEMPDIR)", ParsedArgs.temp_dir)

  path = path.replace("$(OUTDIR)", ParsedArgs.output_dir)
  path = path.replace("$(ASSETDIR)", ParsedArgs.asset_dir)
  return os.path.normpath(path)


#
# Verifies the .muffin format.
def muffin_verify(config):
  for obj in config:
    if not 'command' in obj:
      muffin_error("Missing 'command' in " + json.dumps(obj))

    if not 'jobs' in obj:
      muffin_error("Missing 'jobs' in " + json.dumps(obj))

    for key in obj:
      if key == 'command':
        if not isinstance(obj[key], basestring):
          muffin_error("Expecting string for 'command', got: " + json.dumps(obj[key]) + " in " + json.dumps(obj))
      elif key == 'jobs':
        if not isinstance(obj[key], list):
          muffin_error("Expecting array for 'jobs', got: " + json.dumps(obj[key]) + " in " + json.dumps(obj))

        for job in obj[key]:
          if 'script' in job:
            for key in job:
              if key == 'script':
                if not isinstance(job[key], basestring):
                  muffin_error("Expecting string for 'script', got: " + json.dumps(job[key]) + " in " + json.dumps(job))
              elif key == 'vars':
                if not isinstance(job[key], dict):
                  muffin_error("Expecting object for 'vars', got: " + json.dumps(job[key]) + " in " + json.dumps(job))
              else:
                muffin_error("Ignored additional field '" + key + "' in " + json.dumps(job))

          elif 'command' in job:
            for key in job:
              if key == 'command':
                if not isinstance(job[key], basestring):
                  muffin_error("Expecting string for 'command', got: " + json.dumps(job[key]) + " in " + json.dumps(job))
              else:
                muffin_error("Ignored additional field '" + key + "' in " + json.dumps(job))
          else:
            muffin_error("Unrecognized job type: " + json.dumps(job))
      else:
        muffin_error("Ignored additional field '" + key + "' in " + json.dumps(obj))


#
# Executes an externally defined muffin script.
def muffin_script(job, parsedArgs):

  # Search the provided scripts directory and fall back on the muffinman directory.
  scriptPath = os.path.join(parsedArgs.scripts_dir, job['script'])
  if not os.path.exists(scriptPath):
    scriptPath = os.path.join(os.path.dirname(os.path.realpath(__file__)), job['script'])
    if not os.path.exists(scriptPath):
      muffin_error("Specified script " + job['script'] + " does not exist.")

  if parsedArgs.verbose:
    muffin_log("Executing script " + job['script'] + ":")

  ast = None
  try:
    ast = compile(open(scriptPath).read(), scriptPath, 'exec')
  except Exception as e:
    muffin_error("Error compiling " + scriptPath + "\n" + str(e))

  localvars = None
  if 'vars' in job:
    localvars = job['vars']
  else:
    localvars = {}

  exec(ast, globals(), localvars)


#
# Executes a .muffin command.
def muffin_command(config, command, parsedArgs):
  found = False
  for obj in config:
    if obj['command'] == command:

      if parsedArgs.verbose:
        muffin_log("Executing command '" + obj['command'] + "':")

      for job in obj['jobs']:
        if 'command' in job:
          muffin_command(config, job['command'], parsedArgs)
        elif 'script' in job:
          muffin_script(job, parsedArgs)

      found = True
      break

  muffin_flush_queued_deactivate_files()

  if not found:
    muffin_error("Command '" + command + "' has not been specified in the .muffin file.")
  elif parsedArgs.verbose:
    muffin_log("Command '" + obj['command'] + "' completed.")


#
# Reads in the .muffin file, parses it, and executes the specified command.
def muffin(parsedArgs):
  if parsedArgs.verbose:
    muffin_log("It's muffin time!")

  if not os.path.exists(parsedArgs.asset_dir):
    muffin_error("Specified asset_dir " + parsedArgs.asset_dir + " does not exist.")

  if parsedArgs.temp_dir and not os.path.exists(parsedArgs.temp_dir):
    os.makedirs(parsedArgs.temp_dir)

  if not os.path.exists(parsedArgs.output_dir):
    os.makedirs(parsedArgs.output_dir)

  # Open the .muffin file and read the contents.
  muffinPath = os.path.join(parsedArgs.asset_dir, ".muffin")
  if not os.path.exists(muffinPath):
    muffin_error("No '.muffin' file found at " + muffinPath)

  file = open(muffinPath, 'r')
  muffinConfig = file.read()
  file.close()

  # Strip out lines starting with #, which are used for commenting.
  try:
    muffinConfig = json.loads(re.sub("^\s*#.*$", '', muffinConfig, flags=re.MULTILINE))
  except ValueError as e:
    muffin_error("JSON parse error -- " + str(e))

  muffin_verify(muffinConfig)

  if parsedArgs.command == '':
    parsedArgs.command = muffinConfig[0]['command']

  muffin_command(muffinConfig, parsedArgs.command, parsedArgs)

  if parsedArgs.verbose:
    muffin_log("Success!")

  return


#
# Entry point.
def main():
  desc = 'Muffinman: Asset baking and management tool.'
  epi = 'Muffinman v0.01 Copyright 2013 David Siems'
  parser = argparse.ArgumentParser(description = desc, epilog = epi)

  parser.add_argument('-d', '--asset_dir', default = os.getcwd(), help = 'asset root directory, defaults to cwd')
  parser.add_argument('-s', '--scripts_dir', default = os.path.dirname(os.path.realpath(__file__)), help = "muffin scripts directory (this is checked first, if the script isn't found default folder is checked)")
  parser.add_argument('-o', '--output_dir', default = os.getcwd(), help = 'output directory, defaults to cwd')
  parser.add_argument('-c', '--command', default = '', help = 'operation to perform')
  parser.add_argument('-t', '--temp_dir', help = 'temp directory to use')
  parser.add_argument('-v', '--version', default = False, action='store_true', help = 'print version')
  parser.add_argument('--verbose', default = False, action='store_true', help = 'enable verbose mode')
  parser.add_argument('--silent', default = False, action='store_true', help = 'enable silent mode')
  parser.add_argument('--sing', default = False, action='store_true', help = 'sing a song')

  args = muffin_parse_args(parser)

  if args.version:
    muffin_log(epi)
    sys.exit()

  if args.sing:
    muffin_log('')
    muffin_log('Do you know the muffin man,')
    muffin_log('The muffin man, the muffin man,')
    muffin_log('Do you know the muffin man,')
    muffin_log('Who lives in Drury Lane?')
    muffin_log('')
    muffin_log('Yes, I know the muffin man,')
    muffin_log('The muffin man, the muffin man,')
    muffin_log('Yes, I know the muffin man,')
    muffin_log('Who lives in Drury Lane.')
    muffin_log('')
    sys.exit()

  if args.verbose:
    muffin_log('')
    muffin_log(epi)
    muffin_log('')

  muffin(args)

#
# Okay, the REAL entry point.
if __name__ == "__main__":
  main()
