#pragma once
#include "FastNoiseLite.h"

namespace rs {
namespace math {

enum class NoiseType {
  Simplex = FastNoiseLite::
      NoiseType_OpenSimplex2,  // As opposed to TYPE_PERLIN, gradients exist in
                               // a simplex lattice rather than a grid lattice,
                               // avoiding directional artifacts.

  SimplexSmooth =
      FastNoiseLite::NoiseType_OpenSimplex2S,  // Modified, higher quality
                                               // version of TYPE_SIMPLEX, but
                                               // slower.

  Perlin = FastNoiseLite::NoiseType_Perlin,  // A lattice of random gradients.
                                             // Their dot products are
                                             // interpolated to obtain values in
                                             // between the lattices.

  Value = FastNoiseLite::NoiseType_Value,  // A lattice of points are assigned
                                           // random values then interpolated
                                           // based on neighboring values.

  ValueCubic =
      FastNoiseLite::NoiseType_ValueCubic  // Similar to Value noise, but
                                           // slower. Has more variance in
                                           // peaks and valleys.

  //   Cellular = FastNoiseLite::NoiseType_Cellular, // unsupported now
};

enum FractalType {
  None = FastNoiseLite::FractalType_None,  // No fractal noise.

  Fbm = FastNoiseLite::FractalType_FBm,  // Method using Fractional Brownian
                                         // Motion to combine octaves into a
                                         // fractal.

  Rigid = FastNoiseLite::FractalType_Ridged,  // Method of combining octaves
                                              // into a fractal resulting in a
                                              // "ridged" look.

  PingPong = FastNoiseLite::FractalType_PingPong,  // Method of combining
                                                   // octaves into a fractal
                                                   // with a ping pong effect.
};

template <NoiseType noise_type>
class Noise {
 protected:
  FastNoiseLite noise_;
  int seed_ =
      1337;  // Using different seeds will cause the noise output to change
  float frequency_ =
      0.01;  // Affects how coarse the patterns in the noise output area

 public:
  Noise() {
    noise_.SetNoiseType((FastNoiseLite::NoiseType)noise_type);
    noise_.SetSeed(seed_);
    noise_.SetFrequency(frequency_);
  }

  void setSeed(int seed) {
    seed_ = seed;
    noise_.SetSeed(seed_);
  }

  void setFrequency(float frequency) {
    frequency_ = frequency;
    noise_.SetFrequency(frequency_);
  }

  float getSigned(float x, float y) { return noise_.GetNoise(x, y); }
  float getSigned(float x, float y, float z) {
    return noise_.GetNoise(x, y, z);
  }

  float get(float x, float y) { return getSigned(x, y) * 0.5 + 0.5; }
  float get(float x, float y, float z) {
    return getSigned(x, y, z) * 0.5 + 0.5;
  }

  int getSeed() const { return seed_; }
  float getFrequency() const { return frequency_; }
};

using PerlinNoise = Noise<NoiseType::Perlin>;
using SimplexNoise = Noise<NoiseType::Simplex>;
using SimplexSmoothNoise = Noise<NoiseType::SimplexSmooth>;
using ValueNoise = Noise<NoiseType::Value>;

template <NoiseType noise_type, FractalType fractal_type>
class FractalNoise : public Noise<noise_type> {
 protected:
  int octaves_ = 3;  // The amount of noise layers used to create the fractal
  float lacunarity_ = 2.0;  // The frequency multiplier between each octave
  float gain_ = 0.5;  // The relative strength of noise from each layer when
                      // compared to the last
 public:
  FractalNoise() : Noise<noise_type>() {
    Noise<noise_type>::noise_.SetFractalType(
        (FastNoiseLite::FractalType)fractal_type);
    Noise<noise_type>::noise_.SetFractalOctaves(octaves_);
    Noise<noise_type>::noise_.SetFractalLacunarity(lacunarity_);
    Noise<noise_type>::noise_.SetFractalGain(gain_);
  }

  void setOctaves(int octaves) {
    octaves_ = octaves;
    Noise<noise_type>::noise_.SetFractalOctaves(octaves_);
  }

  void setLacunarity(float lacunarity) {
    lacunarity_ = lacunarity;
    Noise<noise_type>::noise_.SetFractalLacunarity(lacunarity_);
  }

  void setGain(float gain) {
    gain_ = gain;
    Noise<noise_type>::noise_.SetFractalGain(gain_);
  }

  int setOctaves() const { return octaves_; }
  float geLacunarity() const { return lacunarity_; }
  float getGain() const { return gain_; }
};

using PerlinNoiseFbm = FractalNoise<NoiseType::Perlin, FractalType::Fbm>;
using SimplexNoiseFbm = FractalNoise<NoiseType::Simplex, FractalType::Fbm>;
using SimplexSmoothNoiseFbm =
    FractalNoise<NoiseType::SimplexSmooth, FractalType::Fbm>;
using ValueNoiseFbm = FractalNoise<NoiseType::Value, FractalType::Fbm>;

}  // namespace math
}  // namespace rs