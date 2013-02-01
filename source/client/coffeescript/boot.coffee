# First script executed after require.coffee.
Game = require('game')

# Required function, and the application entry point.
global.main = () ->
  Game.init()

  while !userHasQuit()
    PROF.tick()

    PROF.start("VM::tick")
    VM.tick() # Garbage collect and process debugger messages.
    PROF.stop()

    PROF.start("OS::tick")
    OS.tick() # Tick the OS object. (process window messages, etc.)
    PROF.stop()

    PROF.start("Game::tick")
    Game.tick()
    PROF.stop()

    PROF.start("GFX::tick")
    GFX.tick() # Tick the graphics object. (push draw calls to render thread etc.)
    PROF.stop()

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

userHasQuit = () -> OS.hasQuit() or CONTROLS.keyPressed(Keys.Escape) or Game.hasQuit()
shouldRebootVM = () -> CONTROLS.keyPressed('r')
