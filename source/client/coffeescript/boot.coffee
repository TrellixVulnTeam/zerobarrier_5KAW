# First script executed after require.coffee.
Game = require('game')

# Required function, and the application entry point.
global.main = () ->
  Game.init()
  
  while !userHasQuit()
    VM.tick() # Garbage collect and process debugger messages.
    OS.tick() # Tick the OS object. (process window messages, etc.)
    Game.tick()
    GFX.tick() # Tick the graphics object. (push draw calls to render thread etc.)
  return true

# Required function and the application re-entry point if an exception is thrown.
global.onError = () ->
  # Go into a 'display the error' state, but keep everything running.
  while !userHasQuit()
    VM.tick()
    OS.tick()
    GFX.tick()
    return false if shouldRebootVM()
  return true

userHasQuit = () -> OS.hasQuit() or Controls.keyPressed(Keys.Escape) or Game.hasQuit()
shouldRebootVM = () -> Controls.keyPressed('r')
