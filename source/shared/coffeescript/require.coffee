# various global helpers
modules = []
vmLoadFile = (path) -> VM.require('js/' + path + '.js')
global.require = (path) -> modules[path] ?= vmLoadFile(path)
global.reload = (path) -> modules[path] = vmLoadFile(path)

configs = []
global.config = (path) ->  configs[path] ?= JSON.readfile('config/' + path + '.json')

global.log = (message) -> VM.log(message)

global.gc = (hint) -> VM.collectGarbage(hint)

global.Debugger = {
  "tick" : () -> 
}

VM.tick = () ->
  Debugger.tick()
  gc()
  return null
