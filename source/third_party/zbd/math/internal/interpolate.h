#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

template<typename T, typename U> 
ZB_INLINE T lerp(T start, T stop, U t) {
  return static_cast<T>(start + t * (stop - start));
}

template<typename T> 
ZB_INLINE T smoother_step(T start, T stop, T x) {
  x = clamp<T>((x - start) / (stop - start), T(0), T(1));
  return x*x*x*(x*(x*T(6) - T(15)) + T(10));
}

template<typename T, typename U> 
ZB_INLINE T smoother_lerp(T start, T stop, U tstart, U tstop, U t) {
  return lerp<T,U>(start, stop, smoother_step<U>(tstart, tstop, t));
}

template<typename T>
bool AnalyticIK2Bone2d(T targetX, T targetY, T rootX, T rootY, T l1, T l2, T *r1Out, T *r2Out, bool mirrored = false, bool asCloseAsPossible = false) {
  targetX -= rootX;
  targetY -= rootY;

  if (asCloseAsPossible) {
    const T minDistance = abs(l1 - l2);
    const T maxDistance = l1 + l2;

    const T targetDistanceSq = targetX * targetX + targetY * targetY;
    if (targetDistanceSq < minDistance * minDistance) {
      const T targetDistance = sqrt(targetDistanceSq);
      const T scale = minDistance / targetDistance;
      targetX *= scale;
      targetY *= scale;
    }
    else if (targetDistanceSq > maxDistance * maxDistance) {
      const T targetDistance = sqrt(targetDistanceSq);
      const T scale = maxDistance / targetDistance;
      targetX *= scale;
      targetY *= scale;
    }
  }

  const T targetDistanceSq = targetX * targetX + targetY * targetY;
  T cosR2 = (targetDistanceSq - l1*l1 - l2*l2) / (2.0f * l1 * l2);

  if (asCloseAsPossible) {
    cosR2 = clamp(cosR2, -1.0f, 1.0f);
  }

  if (cosR2 >= -1.0f && cosR2 <= 1.0f) {
    const T r2 = acos(cosR2) * (mirrored ? -1.0f : 1.0f);
    
    const T sinR2 = sin(r2);
    const T common = l1 + l2 * cosR2;
    const T r1 = atan2(-l2 * sinR2 * targetX + common * targetY, l2 * sinR2 * targetY + common * targetX);

    *r1Out = r1;
    *r2Out = r2;
    return true;
  }

  defend (!asCloseAsPossible);
  return false;
}

ZB_NAMESPACE_END
