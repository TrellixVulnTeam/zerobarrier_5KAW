//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

bool Parallel(const line2d a, const line2d b) {
  defend (a.v != vector2d::Zero);
  defend (b.v != vector2d::Zero);
  return fabs(wedge(a.v, b.v)) < Epsilon_Medium;
}

bool Intersect(const line2d a, const line2d b, f32 *tOut, f32 *sOut) {
  defend (tOut != 0x0);
  defend (sOut != 0x0);
  defend (tOut != sOut);
  defend (a.v != vector2d::Zero);
  defend (b.v != vector2d::Zero);

  // Pretend parallel lines never intersect.  
  // Because it's easier and it doesn't matter. Most of the time.
  const f32 denominator = wedge(a.v, b.v);
  if (fabs(denominator) < Epsilon_Medium) {
    return false;
  }

  const vector2d x = b.p - a.p;
  const f32 s = wedge(x, a.v) / denominator;
  const f32 t = wedge(x, b.v) / denominator;

  *sOut = s;
  *tOut = t;
  return true;
}

bool Intersect(const segment2d a, const segment2d b, f32 *tOut, f32 *sOut) {
  line2d aLine(a.a, a.b - a.a);
  line2d bLine(b.a, b.b - b.a);

  if (!Intersect(aLine, bLine, tOut, sOut)) {
    return false;
  }

  return *tOut >= 0.0f && *tOut <= 1.0f && *sOut >= 0.0f && *sOut <= 1.0f;
}

bool Test(const circle a, const circle b) {
  const vector2d displacement = a.center - b.center;
  const f32 radii_sum = a.radius + b.radius;
  return dot(displacement, displacement) < radii_sum * radii_sum;
}

bool Test(const aabb2d a, const aabb2d b) {
  if (fabs(a.center.x - b.center.x) < a.extents.x + b.extents.x &&
      fabs(a.center.y - b.center.y) < a.extents.y + b.extents.y) {
      return true;
  }

  return false;
}

bool Intersect(const aabb2d aabb, const segment2d segment, segment2d *intersectionOut) {
  defend (intersectionOut != 0x0);

  const vector2d toCenter = (segment.b - segment.a) * 0.5f;
  const aabb2d segBox(segment.a + toCenter, vector2d(fabs(toCenter.x), fabs(toCenter.y)));

  if (!Test(aabb, segBox)) {
    return false;
  }

  if (Contains(aabb, segment.a) && Contains(aabb, segment.b)) {
    *intersectionOut = segment;
    return true;
  }

  const vector2d tr(aabb.center.x + aabb.extents.x, aabb.center.y + aabb.extents.y);
  const vector2d bl(aabb.center.x - aabb.extents.x, aabb.center.y - aabb.extents.y);
  const vector2d tl(aabb.center.x - aabb.extents.x, aabb.center.y + aabb.extents.y);
  const vector2d br(aabb.center.x + aabb.extents.x, aabb.center.y - aabb.extents.y);

  const segment2d xMin(tl, bl);
  const segment2d xMax(br, tr);
  const segment2d yMin(bl, br);
  const segment2d yMax(tr, tl);

  f32 dummy = 0.0f;
  f32 xMinProj = 0.0f;
  f32 xMaxProj = 1.0f;
  f32 yMinProj = 0.0f;
  f32 yMaxProj = 1.0f;

  Intersect(segment, xMin, &xMinProj, &dummy);
  Intersect(segment, xMax, &xMaxProj, &dummy);
  Intersect(segment, yMin, &yMinProj, &dummy);
  Intersect(segment, yMax, &yMaxProj, &dummy);

  const f32 xMinT = min(xMinProj, xMaxProj);
  const f32 xMaxT = max(xMinProj, xMaxProj);
  const f32 yMinT = min(yMinProj, yMaxProj);
  const f32 yMaxT = max(yMinProj, yMaxProj);

  if (xMaxT < yMinT || yMaxT < xMinT) {
    return false;
  }

  const f32 tMin = max(xMinT, yMinT);
  const f32 tMax = min(xMaxT, yMaxT);

  if (tMax < 0.0f || tMin > 1.0f) {
    return false;
  }

  vector2d line(segment.b - segment.a);
  segment2d result(segment.a + line * max(tMin, 0.0f), segment.a + line * min(tMax, 1.0f));
  *intersectionOut = result;

  return true;
}

bool Contains(const circle c, const vector2d p) {
  const vector2d displacement = c.center - p;
  return dot(displacement, displacement) < c.radius * c.radius;
}

bool Contains(const aabb2d b, const vector2d p) {
  if (fabs(b.center.x - p.x) < b.extents.x &&
      fabs(b.center.y - p.y) < b.extents.y) {
      return true;
  }

  return false;
}

aabb2d ToAABB2d(const segment2d segment) {
  vector2d half = (segment.b - segment.a) * 0.5f;
  return aabb2d(segment.a + half, vector2d(fabs(half.x), fabs(half.y)));
}

ZB_NAMESPACE_END
