# First script executed after require.coffee.
Game = require('game')

# Required function, and the application entry point.
global.main = () ->
  while !userHasQuit()
    tickMandatorySystems()
    Game.tick()
  return true

# Required function and the application re-entry point if an exception is thrown.
global.onError = () ->
  # Go into a 'display the error' state, but keep everything running.
  while !userHasQuit()
    tickMandatorySystems()
    return false if shouldRebootVM()
  return true

userHasQuit = () -> OS.hasQuit() or Controls.keyPressed(Keys.Escape) or Game.hasQuit()
shouldRebootVM = () -> Controls.keyPressed('r')
tickMandatorySystems = () ->
  VM.tick() # Garbage collect and process debugger messages.
  OS.tick() # Tick the OS object. (process window messages, etc.)
  Graphics.tick() # Tick the graphics object.
