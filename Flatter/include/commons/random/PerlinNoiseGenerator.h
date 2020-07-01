#pragma once

#include <algorithm>
#include <glm/gtx/compatibility.hpp>
#include <memory>
#include <numeric>
#include <random>

// Based off https://mrl.nyu.edu/~perlin/noise/
// https://adrianb.io/2014/08/09/perlinnoise.html

namespace Flatter {
template <class Float>
class PerlinNoiseGenerator {
 public:
  PerlinNoiseGenerator(unsigned int seed = 0XF147739);

  const Float getNoise(Float x, Float y, Float z = 0.0f) const;

  virtual ~PerlinNoiseGenerator() = default;

 private:
  const Float grad(unsigned int hash, Float x, Float y, Float z) const;
  const Float fade(Float t) const;

  static const unsigned int sPermutationsCount = 256;
  std::vector<unsigned int> mPermutations;
};

template <class Float>
PerlinNoiseGenerator<Float>::PerlinNoiseGenerator(unsigned int seed) {
  /*mPermutations.resize(sPermutationsCount);
  std::iota(mPermutations.begin(), mPermutations.end(), 0);
  std::default_random_engine engine(seed);
  std::shuffle(mPermutations.begin(), mPermutations.end(), engine);
  mPermutations.insert(mPermutations.end(), mPermutations.begin(),
                       mPermutations.end());*/
  mPermutations = {
      151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
      225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
      6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
      35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
      171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
      231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
      245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
      76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
      164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
      202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
      58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
      154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
      19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
      228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
      145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
      84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
      222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
      180};
  mPermutations.insert(mPermutations.end(), mPermutations.begin(),
                       mPermutations.end());
}

template <class Float>
const Float PerlinNoiseGenerator<Float>::getNoise(Float x,
                                                  Float y,
                                                  Float z) const {
  unsigned int X = (unsigned int)floor(x) & 255;
  unsigned int Y = (unsigned int)floor(y) & 255;
  unsigned int Z = (unsigned int)floor(z) & 255;

  x -= floor(x);
  y -= floor(y);
  z -= floor(z);

  Float u = fade(x);
  Float v = fade(y);
  Float w = fade(z);

  unsigned int A = mPermutations[X] + Y;
  unsigned int AA = mPermutations[A] + Z;
  unsigned int AB = mPermutations[A + 1] + Z;
  unsigned int B = mPermutations[X + 1] + Y;
  unsigned int BA = mPermutations[B] + Z;
  unsigned int BB = mPermutations[B + 1] + Z;

  Float res = glm::lerp<Float>(
      w,
      glm::lerp<Float>(
          v,
          glm::lerp<Float>(u, grad(mPermutations[AA], x, y, z),
                           grad(mPermutations[BA], x - 1, y, z)),
          glm::lerp<Float>(u, grad(mPermutations[AB], x, y - 1, z),
                           grad(mPermutations[BB], x - 1, y - 1, z))),
      glm::lerp<Float>(
          v,
          glm::lerp<Float>(u, grad(mPermutations[AA + 1], x, y, z - 1),
                           grad(mPermutations[BA + 1], x - 1, y, z - 1)),
          glm::lerp<Float>(u, grad(mPermutations[AB + 1], x, y - 1, z - 1),
                           grad(mPermutations[BB + 1], x - 1, y - 1, z - 1))));
  return (res + 1.0) / 2.0;
}

template <class Float>
const Float PerlinNoiseGenerator<Float>::grad(unsigned int hash,
                                              Float x,
                                              Float y,
                                              Float z) const {
  unsigned int h = hash & 15;
  Float u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

template <class Float>
const Float PerlinNoiseGenerator<Float>::fade(Float t) const {
  return t * t * t * (t * (t * 6 - 15) + 10);
}
}  // namespace Flatter
