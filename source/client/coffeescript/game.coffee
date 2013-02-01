Game =
  cameraA: GFX.NewCamera(),
  physicsEditor: PHYSICS.NewEditor(),

  init: () ->
    halfWidth = GFX.windowWidth() / 2.0
    halfHeight = GFX.windowHeight() / 2.0
    @cameraA.initialize(halfWidth, halfHeight, halfWidth, halfHeight, 0, 32)
    @cameraA.setLocation(0, 0, 32)

    @physicsEditor.initialize(@cameraA)
    @physicsEditorEnabled = false

    @tStart = 0
    @tEnd = 100
    @tValue = 0
    @direction = 1

    @startTime = TIME.stamp()

    help(GFX)
    null

  tick: () ->
    if CONTROLS.keyPressed(Keys.F1)
      @physicsEditorEnabled = !@physicsEditorEnabled

    if @physicsEditorEnabled
      @physicsEditor.tick()

    t = @tValue / (@tEnd - @tStart)

    GFX.setCamera(@cameraA)
    GFX.clear(@cameraA, 0, 0.5, 0.5, 1)
    GFX.setTechnique(0)
    GFX.ngon(t * 640 - 320, 0, 30, 18 * t + 3, 0, 0, 0.5, 1.0, 0.5 * t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 20, 5, 1, 0, 0, (1-t), t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 20, 5, 1, 0, 0, t, t * 2.0 * 3.14, false)
    GFX.ngon(t * 640 - 320, 0, 10, 3, 0, 1, 0, t, 2.0 * t * 2.0 * 3.14, true)
    GFX.ngon(t * 640 - 320, 0, 10, 3, 0, 1, 0, (1-t), 2.0 * t * 2.0 * 3.14, false)
    GFX.point(t * 640 - 320, 0, 0, 0, 0, 1)

    GFX.ngon((1-t) * 640 - 320, 80, 25, 4, 1, 1, 1, 1, 0.5 * t * 2.0 * 3.14, false)
    GFX.ngon((1-t) * 640 - 320, 80, 24, 4, 0, 0, 0, t, 0.5 * t * 2.0 * 3.14, true)
    GFX.thick_line(t * 640 - 320, 0, (1-t) * 640 - 320, 80, 5, 1, 0, 0, 1)
    GFX.rect((1-t) * 640 - 320, -80, 40, 15, 1, 1, 1, 1, false)
    GFX.arrow(t * 640 - 320, 0, (1-t) * 640 - 320, -80, 0, 1, 0, 1, 8, 30)

    @tValue += @direction
    if @tValue <= @tStart or @tValue >= @tEnd
      @direction *= -1

    if @physicsEditorEnabled
      @physicsEditor.draw()

    PROF.draw(@cameraA)

    null
  hasQuit: () -> false

module.exports = Game

# Handy globals to have around.
global.Game = Game
