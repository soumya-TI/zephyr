/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \defgroup MATHLIB_FASTRTS FAST RTS TMU API
 * \ingroup MATHLIB
 *
 * TMU intrinsic redefinitions of standard math library (math.h) functions,
 * include this file to replace the standard functions listed below
 *
 * @{
 */

#ifndef _FASTRTS_TMU_
#define _FASTRTS_TMU_

#include <arm_cde.h>
#include "arm_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/** \brief sinf - remaped RTS functions to TMU CDE intrinsics */
#define sinf   sinf_tmu
/** \brief cosf - remaped RTS functions to TMU CDE intrinsics */
#define cosf   cosf_tmu
/** \brief asinf - remaped RTS functions to TMU CDE intrinsics */
#define asinf  asinf_tmu
/** \brief acosf - remaped RTS functions to TMU CDE intrinsics */
#define acosf  acosf_tmu
/** \brief atanf - remaped RTS functions to TMU CDE intrinsics */
#define atanf  atanf_tmu
/** \brief atan2f - remaped RTS functions to TMU CDE intrinsics */
#define atan2f atan2f_tmu
/** \brief sinf - remaped RTS functions to TMU CDE intrinsics */
#define expf   expf_tmu
/** \brief logf - remaped RTS functions to TMU CDE intrinsics */
#define logf   logf_tmu
/** \brief sqrtf - remaped RTS functions to TMU CDE intrinsics */
#define sqrtf  sqrtf_tmu

/** \brief TMU CDE LUF flag */
#define FASTRTS_TMU_FLAGS_LUF ((uint32_t)0x00000001U)
/** \brief TMU CDE LVF flag */
#define FASTRTS_TMU_FLAGS_LVF ((uint32_t)0x00000002U)

/** \brief 2*PI const define. Not be used directly by end application. */
#define FASTRTS_TMU_TWO_PI     (6.28318530717959F)
/** \brief PI const define. Not be used directly by end application. */
#define FASTRTS_TMU_PI         (3.14159265358979F)
/** \brief PI/2 const define. Not be used directly by end application. */
#define FASTRTS_TMU_HALF_PI    (1.57079632679490F)
/** \brief Inverse 2*PI const define. Not be used directly by end application. */
#define FASTRTS_TMU_INV_2PI    (0.15915494309190F)
/** \brief Scale value to apply for logf output. Not be used directly by end application. */
#define FASTRTS_TMU_LOGF_SCALE (0.6931471805F)

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/** \brief Union type to use with TMU intrinsics. Not be used directly by end application.
 *
 * TMU CDE intrinsics use uint32_t inputs/outputs whereas actual operations
 * are floating-point, so we use union for bit-exactness without bit-conversion
 */
typedef union
{
    /** \brief 32b float representation */
    float    f32;
    /** \brief 32b unsigedn integer representation */
    uint32_t u32;
} FASTRTS_TMU_FloatUInt32;

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

/** \brief sinf operation */
__STATIC_FORCEINLINE float sinf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in, out;
    // TMU sin intrinsic operates on per-unit value
    in.f32  = x * FASTRTS_TMU_INV_2PI;
    out.u32 = __arm_vcx2_u32(0, in.u32, 0);
    return out.f32;
}

/** \brief cosf operation */
__STATIC_FORCEINLINE float cosf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in, out;
    // TMU cos intrinsic operates on per-unit value
    in.f32  = x * FASTRTS_TMU_INV_2PI;
    out.u32 = __arm_vcx2_u32(0, in.u32, 1);
    return out.f32;
}

/** \brief acosf operation */
__STATIC_FORCEINLINE float acosf_tmu(float x)
{
    // Range check is not done on input
    FASTRTS_TMU_FloatUInt32 in, temp1, temp2, out;
    in.f32    = x;
    temp2.f32 = 1.0f - (in.f32 * in.f32);
    // sqrt (1-x^2)
    temp2.u32 = __arm_vcx2_u32(0, temp2.u32, 3);

    // sqrt(1-x^2)/x
    temp1.f32 = temp2.f32 / x;
    // acos(x) = atan2(sqrt(1-x^2),x) which is computed using
    // quadf + puatanf
    out.u32   = (__arm_vcx3_u32(0, temp2.u32, in.u32, 1) + __arm_vcx2_u32(0, temp1.u32, 2));
    // scale to rad
    return (out.f32 * FASTRTS_TMU_TWO_PI);
}

/** \brief asinf operation */
__STATIC_FORCEINLINE float asinf_tmu(float x)
{
    // Range check is not done on input
    FASTRTS_TMU_FloatUInt32 in, temp1, temp2, out;
    in.f32    = x;
    temp2.f32 = 1.0f - (in.f32 * in.f32);
    // sqrt (1-x^2)
    temp2.u32 = __arm_vcx2_u32(0, temp2.u32, 3);

    // x/sqrt(1-x^2)
    temp1.f32 = x / temp2.f32;
    // asin(x) = atan2(x,sqrt(1-x^2)) which is computed using
    // quadf + puatanf
    out.u32   = (__arm_vcx3_u32(0, in.u32, temp2.u32, 1) + __arm_vcx2_u32(0, temp1.u32, 2));
    // scale to rad
    return (out.f32 * FASTRTS_TMU_TWO_PI);
}

/** \brief atanf operation */
__STATIC_FORCEINLINE float atanf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in, out;
    in.f32  = x;
    out.u32 = __arm_vcx2_u32(0, in.u32, 2);
    // scale to rad
    return (out.f32 * FASTRTS_TMU_TWO_PI);
}

/** \brief atan2f operation using ATANPUF32 with manual quadrant handling
 *
 * Uses TMU ATANPUF32 with manual quadrant correction.
 */
__STATIC_FORCEINLINE float atan2f_tmu(float y, float x)
{
    FASTRTS_TMU_FloatUInt32 ratio, atan_pu;

    /* Branchless absolute values */
    FASTRTS_TMU_FloatUInt32 ay, ax;
    ay.f32 = y; ax.f32 = x;
    ay.u32 &= 0x7FFFFFFFU;
    ax.u32 &= 0x7FFFFFFFU;

    /* Handle both-zero case */
    if ((ax.u32 | ay.u32) < 0x00800000U) return 0.0f;

    /* Keep ratio in [-1, 1] */
    uint32_t swap = (ay.f32 > ax.f32) ? 1U : 0U;
    ratio.f32 = swap ? (x / y) : (y / x);

    /* TMU ATANPUF32 only - avoid QUADF32 */
    atan_pu.u32 = __arm_vcx2_u32(0, ratio.u32, 2);
    float atan_rad = atan_pu.f32 * FASTRTS_TMU_TWO_PI;

    /* Manual quadrant correction */
    if (swap) {
        return (y >= 0.0f) ? (FASTRTS_TMU_HALF_PI - atan_rad) : (-FASTRTS_TMU_HALF_PI - atan_rad);
    } else {
        if (x >= 0.0f) return atan_rad;
        return (y >= 0.0f) ? (atan_rad + FASTRTS_TMU_PI) : (atan_rad - FASTRTS_TMU_PI);
    }
}

/** \brief expf operation */
__STATIC_FORCEINLINE float expf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in_scale, out;
    in_scale.f32 = x * -1.44269504f;  // scale input
    out.u32      = __arm_vcx2_u32(0, in_scale.u32, 4);
    if (x > 0.0f)
    {
        out.f32 = (1.0f / out.f32);
    }
    return (out.f32);
}

/** \brief logf operation */
__STATIC_FORCEINLINE float logf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in, out;
    in.f32  = x;
    // Generates log2f output
    out.u32 = __arm_vcx2_u32(0, in.u32, 5);
    // Scale to logf
    return (out.f32 * FASTRTS_TMU_LOGF_SCALE);
}

/** \brief sqrtf operation */
__STATIC_FORCEINLINE float sqrtf_tmu(float x)
{
    FASTRTS_TMU_FloatUInt32 in, out;
    in.f32  = x;
    out.u32 = __arm_vcx2_u32(0, in.u32, 3);
    return out.f32;
}

/** \brief divf operation */
__STATIC_FORCEINLINE float divf_tmu(float y, float x)
{
    FASTRTS_TMU_FloatUInt32 y_in, x_in, out;
    y_in.f32 = y;
    x_in.f32 = x;
    out.u32  = __arm_vcx3_u32(0, y_in.u32, x_in.u32, 0);
    return out.f32;
}

/**
 * @brief Clear LUF and LVF flags.
 *
 * @param[in] flags Bit mask of \ref FASTRTS_TMU_FLAGS_LUF, \ref FASTRTS_TMU_FLAGS_LVF
 *
 * @return new flags value
 */
__STATIC_FORCEINLINE uint32_t FASTRTS_TMU_clearFlags(uint32_t flags)
{
    return __arm_cx3(0, ~flags, flags, 0);
}

/**
 * @brief Sets LUF and LVF flags.
 *
 * @param[in] flags Bit mask of \ref FASTRTS_TMU_FLAGS_LUF, \ref FASTRTS_TMU_FLAGS_LVF
 *
 * @return new flags value
 */
__STATIC_FORCEINLINE uint32_t FASTRTS_TMU_setFlags(uint32_t flags)
{
    return __arm_cx3(0, flags, flags, 0);
}

/**
 * @brief Return current LUF and LVF flag value
 */
__STATIC_FORCEINLINE uint32_t FASTRTS_TMU_getFlags()
{
    return __arm_cx3(0, FASTRTS_TMU_FLAGS_LUF | FASTRTS_TMU_FLAGS_LVF, 0, 0);
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _FASTRTS_TMU_*/

/**
 * \defgroup MATHLIB MATHLIB API
 */
