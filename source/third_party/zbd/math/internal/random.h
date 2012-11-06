#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class MersenneTwister {
public:
  MersenneTwister(void);

  void Seed(i32 seed);
  
  i32 Rand(void);
  i32 RandRange(i32 min, i32 max);
  
  f32 RandF(f32 min, f32 max, i32 precision = 1000000);
  
  f32 Gaussian(void);
  f32 RandFGaussian(f32 min = 0.0f, f32 max = 1.0f, f32 deviations = 3.0f);

private:
  void Generate(void);

  const static i32 mt_size = 624;
  i32 mt[mt_size];
  i32 index;

  // Guassian
  f32 gaussian;
  bool gaussianAvailable;
};

extern MersenneTwister GlobalRandom;

ZB_NAMESPACE_END
