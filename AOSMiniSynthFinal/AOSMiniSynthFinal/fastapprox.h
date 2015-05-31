/*=====================================================================*
 *                   Copyright (C) 2012 Paul Mineiro                   *
 * All rights reserved.                                                *
 *                                                                     *
 * Redistribution and use in source and binary forms, with             *
 * or without modification, are permitted provided that the            *
 * following conditions are met:                                       *
 *                                                                     *
 *     * Redistributions of source code must retain the                *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer.                                       *
 *                                                                     *
 *     * Redistributions in binary form must reproduce the             *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer in the documentation and/or            *
 *     other materials provided with the distribution.                 *
 *                                                                     *
 *     * Neither the name of Paul Mineiro nor the names                *
 *     of other contributors may be used to endorse or promote         *
 *     products derived from this software without specific            *
 *     prior written permission.                                       *
 *                                                                     *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND              *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,         *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES               *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE             *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER               *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,                 *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES            *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE           *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR                *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF          *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY              *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             *
 * POSSIBILITY OF SUCH DAMAGE.                                         *
 *                                                                     *
 * Contact: Paul Mineiro <paul@mineiro.com>                            *
 *=====================================================================*/
typedef unsigned int uint32_t;

#ifndef __CAST_H_

#ifdef __cplusplus
#define cast_uint32_t static_cast<uint32_t>
#else
#define cast_uint32_t (uint32_t)
#endif

#endif // __CAST_H_

#ifndef __SSE_H_
#define __SSE_H_
#endif // __SSE_H_

#ifndef __FAST_TRIG_H_
#define __FAST_TRIG_H_

#include "stdint.h"

// http://www.devmaster.net/forums/showthread.php?t=5784
// fast sine variants are for x \in [ -\pi, pi ]
// fast cosine variants are for x \in [ -\pi, pi ]
// fast tangent variants are for x \in [ -\pi / 2, pi / 2 ]
// "full" versions of functions handle the entire range of inputs
// although the range reduction technique used here will be hopelessly
// inaccurate for |x| >> 1000
//
// WARNING: fastsinfull, fastcosfull, and fasttanfull can be slower than
// libc calls on older machines (!) and on newer machines are only 
// slighly faster.  however:
//   * vectorized versions are competitive
//   * faster full versions are competitive

static inline float
fastsin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.78444488374548933f;
  union { float f; uint32_t i; } p = { 0.20363937680730309f };
  union { float f; uint32_t i; } r = { 0.015124940802184233f };
  union { float f; uint32_t i; } s = { -0.0032225901625579573f };

  union { float f; uint32_t i; } vx = { x };
  uint32_t sign = vx.i & 0x80000000;
  vx.i = vx.i & 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;
  float qpproxsq = qpprox * qpprox;

  p.i |= sign;
  r.i |= sign;
  s.i ^= sign;

  return q * qpprox + qpproxsq * (p.f + qpproxsq * (r.f + qpproxsq * s.f));
}

static inline float
fastersin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.77633023248007499f;
  union { float f; uint32_t i; } p = { 0.22308510060189463f };

  union { float f; uint32_t i; } vx = { x };
  uint32_t sign = vx.i & 0x80000000;
  vx.i &= 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;

  p.i |= sign;

  return qpprox * (q + p.f * qpprox);
}

static inline float
fastsinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastsin ((half + k) * twopi - x);
}

static inline float
fastersinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastersin ((half + k) * twopi - x);
}

static inline float
fastcos (float x)
{
  static const float halfpi = 1.5707963267948966f;
  static const float halfpiminustwopi = -4.7123889803846899f;
  float offset = (x > halfpi) ? halfpiminustwopi : halfpi;
  return fastsin (x + offset);
}

static inline float
fastercos (float x)
{
  static const float twooverpi = 0.63661977236758134f;
  static const float p = 0.54641335845679634f;

  union { float f; uint32_t i; } vx = { x };
  vx.i &= 0x7FFFFFFF;

  float qpprox = 1.0f - twooverpi * vx.f;

  return qpprox + p * qpprox * (1.0f - qpprox * qpprox);
}

static inline float
fastcosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsinfull (x + halfpi);
}

static inline float
fastercosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastersinfull (x + halfpi);
}

static inline float
fasttan (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsin (x) / fastsin (x + halfpi);
}

static inline float
fastertan (float x)
{
  return fastersin (x) / fastercos (x);
}

static inline float
fasttanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastsin (xnew) / fastcos (xnew);
}

static inline float
fastertanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastersin (xnew) / fastercos (xnew);
}

#endif // __FAST_TRIG_H_
#ifndef __FAST_EXP_H_
#define __FAST_EXP_H_

#include "stdint.h"

// Underflow of exponential is common practice in numerical routines,
// so handle it here.

static inline float fastpow2 (float p)
{
  float offset = (p < 0) ? 1.0f : 0.0f;
  float clipp = (p < -126) ? -126.0f : p;
  int w = clipp;
  float z = clipp - w + offset;
  union { uint32_t i; float f; } v = { cast_uint32_t ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

  return v.f;
}

static inline float fastexp (float p)
{
  return fastpow2 (1.442695040f * p);
}

static inline float fasterpow2 (float p)
{
  float clipp = (p < -126) ? -126.0f : p;
  union { uint32_t i; float f; } v = { cast_uint32_t ( (1 << 23) * (clipp + 126.94269504f) ) };
  return v.f;
}

static inline float fasterexp (float p)
{
  return fasterpow2 (1.442695040f * p);
}

#endif // __FAST_EXP_H_

#ifndef __FAST_LOG_H_
#define __FAST_LOG_H_

#include "stdint.h"

static inline float fastlog2 (float x)
{
  union { float f; uint32_t i; } vx = { x };
  union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f
           - 1.498030302f * mx.f 
           - 1.72587999f / (0.3520887068f + mx.f);
}

static inline float fastlog (float x)
{
  return 0.69314718f * fastlog2 (x);
}

static inline float  fasterlog2 (float x)
{
  union { float f; uint32_t i; } vx = { x };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;
  return y - 126.94269504f;
}

static inline float fasterlog (float x)
{
//  return 0.69314718f * fasterlog2 (x);

  union { float f; uint32_t i; } vx = { x };
  float y = vx.i;
  y *= 8.2629582881927490e-8f;
  return y - 87.989971088f;
}

#endif // __FAST_LOG_H_

#ifndef __FAST_HYPERBOLIC_H_
#define __FAST_HYPERBOLIC_H_

#include "stdint.h"

static inline float
fastsinh (float p)
{
  return 0.5f * (fastexp (p) - fastexp (-p));
}

static inline float
fastersinh (float p)
{
  return 0.5f * (fasterexp (p) - fasterexp (-p));
}

static inline float
fastcosh (float p)
{
  return 0.5f * (fastexp (p) + fastexp (-p));
}

static inline float
fastercosh (float p)
{
  return 0.5f * (fasterexp (p) + fasterexp (-p));
}

static inline float
fasttanh (float p)
{
  return -1.0f + 2.0f / (1.0f + fastexp (-2.0f * p));
}

static inline float
fastertanh (float p)
{
  return -1.0f + 2.0f / (1.0f + fasterexp (-2.0f * p));
}

#endif // __FAST_HYPERBOLIC_H_