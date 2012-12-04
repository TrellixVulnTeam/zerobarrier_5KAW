Game =
  cameraA: GFX.NewCamera(),
  cameraB: GFX.NewCamera(),
  init: () ->
    quarterWidth = GFX.windowWidth() / 4.0
    quarterHeight = GFX.windowHeight() / 4.0
    @cameraA.initialize(quarterWidth, quarterHeight, quarterWidth, quarterHeight, 0, 32)
    @cameraA.setLocation(0, 0, 32)

    @cameraB.initialize(3 * quarterWidth, 3 * quarterHeight, quarterWidth, quarterHeight, 0, 32)
    @cameraB.setLocation(0, 0, 32)

    @tStart = 0
    @tEnd = 100
    @tValue = 0
    @direction = 1

    help(GFX)
    null

  tick: () ->
    t = @tValue / (@tEnd - @tStart)

    GFX.setCamera(@cameraA)
    GFX.clear(@cameraA, 0, 0.5, 0.5, 1)
    GFX.ngon(t * 640 - 320, 0, 30, 20, 0, 0, 0.5, 1.0, 0.5 * t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 20, 5, 1, 0, 0, (1-t), t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 20, 5, 1, 0, 0, t, t * 2.0 * 3.14, false)
    GFX.ngon(t * 640 - 320, 0, 10, 3, 0, 1, 0, t, 2.0 * t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 10, 3, 0, 1, 0, (1-t), 2.0 * t * 2.0 * 3.14, false)
    GFX.point(t * 640 - 320, 0, 0, 0, 0, 1)

    GFX.ngon((1-t) * 640 - 320, 80, 25, 4, 1, 1, 1, 1, 0.5 * t * 2.0 * 3.14, false)
    GFX.ngon((1-t) * 640 - 320, 80, 24, 4, 0, 0, 0, t, 0.5 * t * 2.0 * 3.14, true)
    GFX.arrow(t * 640 - 320, 0, (1-t) * 640 - 320, 80, 1, 0, 0, 1, 8, 30)
    GFX.rect((1-t) * 640 - 320, -80, 40, 15, 1, 1, 1, 1, false)
    GFX.arrow(t * 640 - 320, 0, (1-t) * 640 - 320, -80, 0, 1, 0, 1, 8, 30)

    GFX.setCamera(@cameraB)
    GFX.clear(@cameraB, 0.5, 0.0, t*0.5, 1)
    
    @tValue += @direction
    if @tValue <= @tStart or @tValue >= @tEnd
      @direction *= -1

    null
  hasQuit: () -> false

module.exports = Game

# Handy globals to have around.
global.Game = Game
