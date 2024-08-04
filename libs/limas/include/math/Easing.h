#pragma once

namespace rs {
namespace math {
namespace easing {

//! Easing equation for a simple linear tweening with no easing.
inline float none(float t) { return t; }

//! Easing equation for a quadratic (t^2) ease-in, accelerating from zero
//! velocity.
inline float quadIn(float t) { return t * t; }

//! Easing equation for a quadratic (t^2) ease-out, decelerating to zero
//! velocity.
inline float quadOut(float t) { return -t * (t - 2); }

//! Easing equation for a quadratic (t^2) ease-in/out, accelerating until
//! halfway, then decelerating.
inline float quadInOut(float t) {
  t *= 2;
  if (t < 1) return 0.5f * t * t;

  t -= 1;
  return -0.5f * ((t) * (t - 2) - 1);
}

//! Easing equation for a quadratic (t^2) ease-out/in, decelerating until
//! halfway, then accelerating.
inline float quadOutIn(float t) {
  if (t < 0.5f) return quadOut(t * 2) * 0.5f;
  return quadIn((2 * t) - 1) * 0.5f + 0.5f;
}

//! Easing equation function for a cubic (t^3) ease-in, accelerating from zero
//! velocity.
inline float cubicIn(float t) { return t * t * t; }

//! Easing equation for a cubic (t^3) ease-out, decelerating to zero velocity.
inline float cubicOut(float t) {
  t -= 1;
  return t * t * t + 1;
}

//! Easing equation for a cubic (t^3) ease-in/out, accelerating until halfway,
//! then decelerating.
inline float cubicInOut(float t) {
  t *= 2;
  if (t < 1) return 0.5f * t * t * t;
  t -= 2;
  return 0.5f * (t * t * t + 2);
}

//! Easing equation for a cubic (t^3) ease-out/in, decelerating until halfway,
//! then accelerating.
inline float cubicOutIn(float t) {
  if (t < 0.5f) return cubicOut(2 * t) / 2;
  return cubicIn(2 * t - 1) / 2 + 0.5f;
}

//! Easing equation for a quartic (t^4) ease-in, accelerating from zero
//! velocity.
inline float quartIn(float t) { return t * t * t * t; }

//! Easing equation for a quartic (t^4) ease-out, decelerating to zero velocity.
inline float quartOut(float t) {
  t -= 1;
  return -(t * t * t * t - 1);
}

//! Easing equation for a quartic (t^4) ease-in/out, accelerating until halfway,
//! then decelerating.
inline float quartInOut(float t) {
  t *= 2;
  if (t < 1)
    return 0.5f * t * t * t * t;
  else {
    t -= 2;
    return -0.5f * (t * t * t * t - 2);
  }
}

//! Easing equation for a quartic (t^4) ease-out/in, decelerating until halfway,
//! then accelerating.
inline float quartOutIn(float t) {
  if (t < 0.5f) return quartOut(2 * t) / 2;
  return quartIn(2 * t - 1) / 2 + 0.5f;
}

//! Easing equation function for a quintic (t^5) ease-in, accelerating from zero
//! velocity.
inline float quintIn(float t) { return t * t * t * t * t; }

//! Easing equation for a quintic (t^5) ease-out, decelerating to zero velocity.
inline float quintOut(float t) {
  t -= 1;
  return t * t * t * t * t + 1;
}

//! Easing equation for a quintic (t^5) ease-in/out, accelerating until halfway,
//! then decelerating.
inline float quintInOut(float t) {
  t *= 2;
  if (t < 1)
    return 0.5f * t * t * t * t * t;
  else {
    t -= 2;
    return 0.5f * (t * t * t * t * t + 2);
  }
}

//! Easing equation for a quintic (t^5) ease-out/in, decelerating until halfway,
//! then accelerating.
inline float quintOutIn(float t) {
  if (t < 0.5f) return quintOut(2 * t) / 2;
  return quintIn(2 * t - 1) / 2 + 0.5f;
}

//! Easing equation for a sinusoidal (sin(t)) ease-in, accelerating from zero
//! velocity.
inline float sineIn(float t) { return -cos(t * (float)M_PI / 2) + 1; }

//! Easing equation for a sinusoidal (sin(t)) ease-out, decelerating from zero
//! velocity.
inline float sineOut(float t) { return sin(t * (float)M_PI / 2); }

//! Easing equation for a sinusoidal (sin(t)) ease-in/out, accelerating until
//! halfway, then decelerating.
inline float sineInOut(float t) { return -0.5f * (cos((float)M_PI * t) - 1); }

//! Easing equation for a sinusoidal (sin(t)) ease-out/in, decelerating until
//! halfway, then accelerating.
inline float sineOutIn(float t) {
  if (t < 0.5f) return sineOut(2 * t) / 2;
  return sineIn(2 * t - 1) / 2 + 0.5f;
}

//! Easing equation for an exponential (2^t) ease-in, accelerating from zero
//! velocity.
inline float expoIn(float t) { return t == 0 ? 0 : powf(2, 10 * (t - 1)); }

//! Easing equation for an exponential (2^t) ease-out, decelerating from zero
//! velocity.
inline float expoOut(float t) { return t == 1 ? 1 : -powf(2, -10 * t) + 1; }

//! Easing equation for an exponential (2^t) ease-in/out, accelerating until
//! halfway, then decelerating.
inline float expoInOut(float t) {
  if (t == 0) return 0;
  if (t == 1) return 1;
  t *= 2;
  if (t < 1) return 0.5f * powf(2, 10 * (t - 1));
  return 0.5f * (-powf(2, -10 * (t - 1)) + 2);
}

//! Easing equation for an exponential (2^t) ease-out/in, decelerating until
//! halfway, then accelerating.
inline float expoOutIn(float t) {
  if (t < 0.5f) return expoOut(2 * t) / 2;
  return expoIn(2 * t - 1) / 2 + 0.5f;
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-in, accelerating from zero
//! velocity.
inline float circIn(float t) { return -(sqrt(1 - t * t) - 1); }

//! Easing equation for a circular (sqrt(1-t^2)) ease-out, decelerating from
//! zero velocity.
inline float circOut(float t) {
  t -= 1;
  return sqrt(1 - t * t);
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-in/out, accelerating until
//! halfway, then decelerating.
inline float circInOut(float t) {
  t *= 2;
  if (t < 1) {
    return -0.5f * (sqrt(1 - t * t) - 1);
  } else {
    t -= 2;
    return 0.5f * (sqrt(1 - t * t) + 1);
  }
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-out/in, decelerating until
//! halfway, then accelerating.
inline float circOutIn(float t) {
  if (t < 0.5f) return circOut(2 * t) / 2;
  return circIn(2 * t - 1) / 2 + 0.5f;
}

// Bounce
inline float bounceOutHelper_(float t, float c, float a) {
  if (t == 1) return c;
  if (t < (4 / 11.0f)) {
    return c * (7.5625f * t * t);
  } else if (t < (8 / 11.0f)) {
    t -= (6 / 11.0f);
    return -a * (1 - (7.5625f * t * t + 0.75f)) + c;
  } else if (t < (10 / 11.0f)) {
    t -= (9 / 11.0f);
    return -a * (1 - (7.5625f * t * t + 0.9375f)) + c;
  } else {
    t -= (21 / 22.0f);
    return -a * (1 - (7.5625f * t * t + 0.984375f)) + c;
  }
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce)
//! ease-in, accelerating from zero velocity. The \a a parameter controls
//! overshoot, the default producing a 10% overshoot.
inline float bounceIn(float t, float a = 1.70158f) {
  return 1 - bounceOutHelper_(1 - t, 1, a);
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce)
//! ease-out, decelerating from zero velocity. The \a a parameter controls
//! overshoot, the default producing a 10% overshoot.
inline float bounceOut(float t, float a = 1.70158f) {
  return bounceOutHelper_(t, 1, a);
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce)
//! ease-in/out, accelerating until halfway, then decelerating. The \a a
//! parameter controls overshoot, the default producing a 10% overshoot.
inline float bounceInOut(float t, float a = 1.70158f) {
  if (t < 0.5f)
    return bounceIn(2 * t, a) / 2;
  else
    return (t == 1) ? 1 : bounceOut(2 * t - 1, a) / 2 + 0.5f;
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce)
//! ease-out/in, decelerating until halfway, then accelerating. The \a a
//! parameter controls overshoot, the default producing a 10% overshoot.
inline float bounceOutIn(float t, float a = 1.70158f) {
  if (t < 0.5f) return bounceOutHelper_(t * 2, 0.5, a);
  return 1 - bounceOutHelper_(2 - 2 * t, 0.5, a);
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2)
//! ease-in, accelerating from zero velocity. The \a a parameter controls
//! overshoot, the default producing a 10% overshoot.
inline float backIn(float t) {
  float s = 1.70158f;
  return t * t * ((s + 1) * t - s);
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2)
//! ease-out, decelerating from zero velocity. The \a a parameter controls
//! overshoot, the default producing a 10% overshoot.
inline float backOut(float t) {
  t -= 1;
  float s = 1.70158f;
  return (t * t * ((s + 1) * t + s) + 1);
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2)
//! ease-in/out, accelerating until halfway, then decelerating. The \a a
//! parameter controls overshoot, the default producing a 10% overshoot.
inline float backInOut(float t, float s = 1.70158f) {
  t *= 2;
  if (t < 1) {
    s *= 1.525f;
    return 0.5f * (t * t * ((s + 1) * t - s));
  } else {
    t -= 2;
    s *= 1.525f;
    return 0.5f * (t * t * ((s + 1) * t + s) + 2);
  }
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2)
//! ease-out/in, decelerating until halfway, then accelerating. The \a a
//! parameter controls overshoot, the default producing a 10% overshoot.
inline float backOutIn(float t, float s) {
  if (t < 0.5f) return backOut(2 * t) / 2;
  return backIn(2 * t - 1) / 2 + 0.5f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Elastic

//! \cond
inline float elasticInHelper_(float t, float b, float c, float d, float a,
                              float p) {
  if (t == 0) return b;
  float t_adj = t / d;
  if (t_adj == 1) return b + c;

  float s;
  if (a < abs(c)) {
    a = c;
    s = p / 4.0f;
  } else {
    s = p / (2 * (float)M_PI) * asin(c / a);
  }

  t_adj -= 1;
  return -(a * powf(2, 10 * t_adj) *
           sin((t_adj * d - s) * (2 * (float)M_PI) / p)) +
         b;
}

inline float elasticOutHelper_(float t, float /*b*/, float c, float /*d*/,
                               float a, float p) {
  if (t == 0) return 0;
  if (t == 1) return c;

  float s;
  if (a < c) {
    a = c;
    s = p / 4;
  } else {
    s = p / (2 * (float)M_PI) * asin(c / a);
  }

  return a * powf(2, -10 * t) * sin((t - s) * (2 * (float)M_PI) / p) + c;
}
//! \endcond

//! Easing equation for an elastic (exponentially decaying sine wave) ease-in,
//! accelerating from zero velocity.
inline float elasticIn(float t, float amplitude, float period) {
  return elasticInHelper_(t, 0, 1, 1, amplitude, period);
}

//! Easing equation for an elastic (exponentially decaying sine wave) ease-out,
//! decelerating from zero velocity.
inline float elasticOut(float t, float amplitude, float period) {
  return elasticOutHelper_(t, 0, 1, 1, amplitude, period);
}

//! Easing equation for an elastic (exponentially decaying sine wave)
//! ease-in/out, accelerating until halfway, then decelerating.
inline float elasticInOut(float t, float amplitude, float period) {
  if (t == 0) return 0;
  t *= 2;
  if (t == 2) return 1;

  float s;
  if (amplitude < 1) {
    amplitude = 1;
    s = period / 4;
  } else {
    s = period / (2 * (float)M_PI) * asin(1 / amplitude);
  }

  if (t < 1)
    return -0.5f * (amplitude * powf(2.0f, 10 * (t - 1)) *
                    sin((t - 1 - s) * (2 * (float)M_PI) / period));
  return amplitude * powf(2, -10 * (t - 1)) *
             sin((t - 1 - s) * (2 * (float)M_PI) / period) * 0.5f +
         1;
}

//! Easing equation for an elastic (exponentially decaying sine wave)
//! ease-out/in, decelerating until halfway, then accelerating.
inline float elasticOutIn(float t, float amplitude, float period) {
  if (t < 0.5) return elasticOutHelper_(t * 2, 0, 0.5, 1.0, amplitude, period);
  return elasticInHelper_(2 * t - 1, 0.5f, 0.5f, 1, amplitude, period);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Atan

//! Easing equation for an atan ease-in, accelerating from zero velocity. Used
//! by permssion from Chris McKenzie.
inline float atanIn(float t, float a = 15) {
  float m = atan(a);
  return (atan((t - 1) * a) / m) + 1;
}

//! Easing equation for an atan ease-out, decelerating from zero velocity. Used
//! by permssion from Chris McKenzie.
inline float atanOut(float t, float a = 15) {
  float m = atan(a);
  return atan(t * a) / m;
}

//! Easing equation for an atan ease-in/out, accelerating until halfway, then
//! decelerating. Used by permssion from Chris McKenzie.
inline float atanInOut(float t, float a = 15) {
  float m = atan(0.5f * a);
  return (atan((t - 0.5f) * a) / (2 * m)) + 0.5f;
}

}  // namespace easing
}  // namespace math
}  // namespace rs
