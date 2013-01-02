class vector2d
  constructor: (x, y) -> 
    @x = x or 0
    @y = y or 0

  set: (v) ->
    @x = v.x
    @y = v.y
    this

  negate: -> new vector2d(-@x, -@y)
  add: (v) -> new vector2d(@x + v.x, @y + v.y)
  sub: (v) -> new vector2d(@x - v.x, @y - v.y)
  addEq: (v) ->
    @x += v.x
    @y += v.y
    this
  subEq: (v) ->
    @x -= v.x
    @y -= v.y
    this

  mul: (f) -> new vector2d(@x*f, @y*f)
  div: (f) -> new vector2d(@x/f, @y/f)
  mulEq: (f) ->
    @x *= f
    @y *= f
    this
  divEq: (f) ->
    @x /= f
    @y /= f
    this

  length: -> Math.sqrt(@x*@x + @y*@y)
  lengthSq: -> @x*@x + @y*@y

  normalize: -> @divEq(@length())
  safeNormalize: (defaultVector) ->
    length = @length()
    if Math.abs(length) < Math.Epsilon
      @set(defaultVector)
    else
      @divEq(length)

  normal: -> @div(@length())
  equals: (v) -> @x == v.x and @y == v.y

dot2d = (a, b) -> a.x*b.x + a.y*b.y
wedge2d = (a, b) -> a.x*b.y - a.y*b.x
componentwise_mul2d = (a, b) -> new vector2d(a.x*b.x, a.y*b.y)
componentwise_div2d = (a, b) -> new vector2d(a.x/b.x, a.y/b.y)

class Renderer
  constructor: ->
    @primitives = []

  draw: ->
    null

Debug = 
  Renderer: Renderer

module.exports = Debug
global.Debug = Debug
