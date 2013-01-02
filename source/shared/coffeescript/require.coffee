# various global helpers
modules = []
vmLoadFile = (path) -> VM.require('js/' + path + '.js')
global.require = (path) -> modules[path] ?= vmLoadFile(path)
global.reload = (path) -> modules[path] = vmLoadFile(path)

configs = []
global.config = (path) -> configs[path] ?= JSON.readfile('config/' + path + '.json')

global.log = (message) -> VM.log(message)

VM.tick = () ->
  @processDebugMessages()
  @collectGarbage(20)
  return null

global.help = (obj) ->
  for key, value of obj
    log(key + ":")
    log(value + "\n")
