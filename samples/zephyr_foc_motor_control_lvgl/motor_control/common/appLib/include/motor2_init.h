/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef MOTOR2_INIT_H
#define MOTOR2_INIT_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __TMS320C28XX_CLA__
#include "libraries/math/include/CLAmath.h"
#else
// includes
// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "ti_math.h"
#endif

#include "userParams.h"
#include "motor_common.h"
#include "user_mtr2.h"

//=============================================================================
// Global Variables for Motor 1
//=============================================================================


//! Motor control variables structure
extern MOTOR_Vars_t motorVars_M2;

//! Motor settings/configuration variables
extern MOTOR_SetVars_t motorSetVars_M2;

//#TAG_FAST_OBS_START#
//! User parameters structure
extern USER_Params userParams_M2;
//#TAG_FAST_OBS_END#
//=============================================================================
// Optional Feature Objects for Motor 1
//=============================================================================
// #TAG_ESMO_START#
extern ESMO_Obj esmo_M2;
// #TAG_ESMO_END#

// #TAG_IPD_START#
extern IPD_Obj ipd_M2;
// #TAG_IPD_END#

//#TAG_MTPA_START#
extern MTPA_Obj mtpa_M2;
//#TAG_MTPA_END#

//#TAG_RECONST_INV_VOLT_START#
extern VOLREC_Obj volrec_M2;
//#TAG_RECONST_INV_VOLT_START#

// #TAG_DEAD_TIME_COMP_START#
extern DTC_Obj deadTimeComp_M2;
// #TAG_DEAD_TIME_COMP_END#
//=============================================================================
// Function Prototypes
//=============================================================================

//! \brief The main interrupt service (ISR) routine for Motor 2
extern void motor2CtrlISR(void);

//! \brief Initialize motor control handles and HAL interface
//! \param[in] handle  The motor handle
extern void initMotor2Handles(MOTOR_Handle handle);

//! \brief Initialize all motor control parameters for Motor 2
//! This includes USER params, control params, FOC objects, and optional features
//! \param[in] handle  The motor handle
extern void initMotor2CtrlParameters(MOTOR_Handle handle);

//#TAG_FAST_OBS_START#
//! \brief Set USER parameters for Motor 2 based on macros in user_mtr2.h
//! \param[in] handle    The user parameters handle
//! \param[in] gdHandle  The gate driver data handle
extern void USER_setMotor2Params(userParams_Handle handle, const HAL_GD_DATA_Obj *gdHandle);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"

#endif // MOTOR2_INIT_H
