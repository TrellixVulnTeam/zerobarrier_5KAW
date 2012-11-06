#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

bool Parallel(const line2d a, const line2d b);
bool Intersect(const line2d a, const line2d b, f32 *tOut, f32 *sOut);
bool Intersect(const segment2d a, const segment2d b, f32 *tOut, f32 *sOut);

bool Test(const line2d l, const circle c);
bool Intersect(const line2d l, const circle c, f32 *t1, f32 *t2);

bool Test(const circle a, const circle b);
bool Test(const aabb2d a, const aabb2d b);

bool Intersect(const aabb2d aabb, const segment2d segment, segment2d *intersectionOut);

bool Contains(const circle c, vector2d p);
bool Contains(const aabb2d b, vector2d p);

aabb2d ToAABB2d(const segment2d segment);

ZB_NAMESPACE_END
