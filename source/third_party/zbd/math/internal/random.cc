//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

MersenneTwister GlobalRandom;

MersenneTwister::MersenneTwister(void)
: index(0)
, gaussian(0.0f)
, gaussianAvailable(false){
  // Do nothing.
}

void MersenneTwister::Seed(i32 seed) {
  mt[0] = seed;
  for (i32 i = 1; i < mt_size; ++i) {
    mt[i] = 0x6c078965 * (mt[i-1] ^ (mt[i-1] >> 30)) + i;
  }

  index = 0;
  gaussian = 0.0f;
  gaussianAvailable = false;
}

i32 MersenneTwister::Rand(void) {
  if (index == 0) {
    Generate();
  }

  i32 x = mt[index];
  index = (index + 1) % mt_size;

  x ^= x >> 11;
  x ^= (x << 7) & 0x9d2c5680;
  x ^= (x << 15) & 0xefc60000;
  x ^= x >> 18;

  return x;
}

i32 MersenneTwister::RandRange(i32 min, i32 max) {
  defend (max >= min);
  return min + Rand() % (max - min + 1);
}

f32 MersenneTwister::RandF(f32 min, f32 max, i32 precision) {
  defend (max >= min);
  return min + (max - min) * (static_cast<f32>(RandRange(0, precision)) / static_cast<f32>(precision));
}

// From Numerical Recipes in C.
f32 MersenneTwister::Gaussian(void) {
  if (gaussianAvailable) {
    gaussianAvailable = false;
    return gaussian;
  }

  f32 rsq = 0.0f;
  f32 v1 = 0.0f;
  f32 v2 = 0.0f;
  do {
    v1 = 2.0f * RandF(-1.0f, 1.0f);
    v2 = 2.0f * RandF(-1.0f, 1.0f);
    rsq = v1 * v1 + v2 * v2;
  } while (rsq >= 1.0f || rsq == 0.0f);

  const f32 fac = sqrtf(-2.0f * logf(rsq) / rsq);
  gaussian = v1 * fac;
  gaussianAvailable = true;

  return v2 * fac;
}

f32 MersenneTwister::RandFGaussian(f32 min, f32 max, f32 deviations) {
  defend (max >= min);
  defend (deviations > 0.0f);

  // To modify a normal distribution to have a mean M and deviation S
  // multiply by S and add M.

  // Here, we want to get a distribution that is [0,1] and multiply that
  // by the specified range. Since the distribution is theoretically infinite,
  // we allow the number of valid deviations out to be specified.

  const f32 halfWidth = (max - min) / 2.0f; // Compute the half-width of the interval.
  const f32 M = min + halfWidth;            // Mean directly in the middle of max and min.
  const f32 S = halfWidth / deviations;     // Compute the new standard deviation using the half-width.
  const f32 value = Gaussian() * S + M;
  return clamp(value, min, max);
}

void MersenneTwister::Generate(void) {
  for (i32 i = 0; i < mt_size; ++i) {
    const i32 j = (i + 1) % mt_size;
    const i32 k = (i + 397) % mt_size;

    i32 x = (mt[i] & 0x80000000) + (mt[j] & 0x7fffffff);
    mt[i] = mt[k] ^ (x >> 1);
    if (x % 2 == 1) {
      mt[i] ^= 0x9908b0df;
    }
  }
}

ZB_NAMESPACE_END
