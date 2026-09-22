/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

#ifndef LIBRARIES_TITUNE_H_
#define LIBRARIES_TITUNE_H_

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

#include "TiTune_LDQ.h"
#include "TiTune_J.h"
#include "TiTune_Rs.h"
#include "TiTune_SatLDQ.h"
#include "TiTune_Flux.h"

#include "pi.h"
#include "park.h"
#include "ipark.h"
#include <stdlib.h>
#include <stdbool.h>
#include "userParams.h"
// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "ti_math.h"
#include "types.h"

// **************************************************************************
// the defines
#define TI_TUNE_STATE_DELAY_COUNT       1000U  // Timer can be handled in background loop or ISR

//! \brief Defines the maximum board voltage based on max measurable ADC voltage after scaling down.
#define TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V         (76.04549098f)     // 110k/4.99k

#define TITUNE_USER_MOTOR1_RATED_VOLTAGE_PH_V         24.0f // Name Plate Rated Voltage / SQRT(3) for Star Connected Motor, Name Plate Rated Voltage for Delta

//Max Rate Phase current of the Motor
#define TI_TUNE_USER_MOTOR1_MAX_CURRENT_A          2.2f

// Rated Electrical Speed of the Motor in Hertz
#define TI_TUNE_ELECTRICAL_FREQ_BASE_HZ             (100)

// PWM Frequency /ISR execution to Current loop execution rate
#define TI_TUNE_USER_M1_NUM_ISR_TICKS_PER_TORQUE_TICK        (1)

// PWM Frequency/ ISR execution to speed loop execution rate
#define TI_TUNE_USER_M1_NUM_ISR_TICKS_PER_SPEED_TICK        (10)

//! \brief Defines the Pulse Width Modulation (PWM) period, usec , (1 / PWM_Frequency)
//!
#define TI_TUNE_USER_M1_PWM_PERIOD_sec     (0.0001f)

#define TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec     (TI_TUNE_USER_M1_PWM_PERIOD_sec * TI_TUNE_USER_M1_NUM_ISR_TICKS_PER_TORQUE_TICK)


#define TI_TUNE_SPEED_CUT_OFF_FREQ          (6.28f) // rad/sec - 10Hz

#define TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ    (MATH_TWO_PI * 100.0f)

/* Maximum Phase voltage Peak possible for SVPWM output */
#define USER_M1_MAX_VS_MAG_PU               (0.576f)

//Pole Pairs of Motor - Not mandatory
#define TI_TUNE_USER_MOTOR1_POLE_PAIRS              (4)
// **************************************************************************
// the typedefs

//! \brief Enumeration for the TiTune state machine
//!
typedef enum {
    TiTune_STATE_IDLE,            //!< idle state

    TiTune_STATE_LDQ_SETUP,       //!< LDQ set-up state
    TiTune_STATE_LDQ_EST,         //!< estimation of d�axis and q�axis inductance state, Ld/Lq
    TiTune_STATE_LDQ_DONE,        //!< LDQ done state

    TiTune_STATE_RS_SETUP,        //!< Rs set-up state
    TiTune_STATE_RS_EST,          //!< estimation of stator resistance state, Rs
    TiTune_STATE_RS_DONE,         //!< Rs done state

    TiTune_STATE_SAT_LDQ_SETUP,   //!< Saturated LDQ set-up state
    TiTune_STATE_SAT_LDQ_EST,     //!< estimation of saturated d�axis and q�axis inductance state, Ld/Lq
    TiTune_STATE_SAT_LDQ_DONE,    //!< Saturated LDQ done state

    TiTune_STATE_INERTIA_SETUP,   //!< motor inertia set-up state
    TiTune_STATE_INERTIA_EST,     //!< estimation of motor inertia state, J
    TiTune_STATE_INERTIA_DONE,    //!< motor inertia done state

    TiTune_STATE_FLUX_SETUP,      //!< motor flux linkage set-up state
    TiTune_STATE_FLUX_EST,        //!< estimation of motor flux linkage state
    TiTune_STATE_FLUX_DONE,       //!< motor flux linkage done state

    TiTune_STATE_OBSERVER_SETUP,  //!< Update the Observer Parameters

    TiTune_STATE_DONE,            //!< done state

    TiTune_NUMSTATES              //!< Number of TiTune routine states
} TiTune_State_e;

//*****************************************************************************
//
//! brief Defines the TiTune object
//
//*****************************************************************************
typedef struct _TiTune_Obj_
{
    bool TiTune_enable;

    // the handle for the speed PI controller
    PI_Obj        TiTune_PI_Speed;

    // the handle for the Id PI controller
    PI_Obj     TiTune_PI_Id;

    // the handle for the Iq PI controller
    PI_Obj     TiTune_PI_Iq;


    // the reference current on d&q rotation axis
    MATH_Vec2 IdqRef_A;

    // the reference output current on d&q rotation axis
    MATH_Vec2 Idq_out_A;


    /* Library Parameters */
    volatile TiTune_State_e TiTune_state;

    uint32_t TiTune_count;

    TiTune_LDQ_Obj TiTune_LDQ;
    TiTune_LDQ_Handle TiTune_LDQ_handler;

    TiTune_J_Obj TiTune_J;
    TiTune_J_Handle TiTune_J_handler;

    TiTune_Rs_Obj TiTune_Rs;
    TiTune_Rs_Handle TiTune_Rs_handler;

    TiTune_SatLDQ_Obj TiTune_SatLDQ;
    TiTune_SatLDQ_Handle TiTune_SatLDQ_handler;

    TiTune_Flux_Obj TiTune_Flux;
    TiTune_Flux_Handle TiTune_Flux_handler;


    /* Inputs of TI_Tune_Methods */

    float32_t VdcBus_V;                     // Supplied DC Bus voltage

    volatile float32_t TiTune_FOC_Angle_in_rad;      // Angle to be fed from the Estimated position of the rotor.

    float32_t TiTune_Speed_in_Hz;           // The Input Estimated Speed to Ti Tune Block

    MATH_Vec2 TiTune_Iab_in_A;              // the Alpha Beta Currents from Clarke are referenced with this pointer


    /* Outputs of TI_TUNE method */

    MATH_Vec2 Ti_Tune_Out_Vab_V;          // the output control voltage on Alpha beta Voltages


    /* Working variables of TI_TUNE method */

    MATH_Vec2 Ti_Tune_phasor;               // output phasor cos and sine angles

    float32_t TiTune_Rs_IPD_Angle;          // TODO : Angle to be fed from the Initial position of the rotor.

    MATH_Vec2 Ti_Tune_Out_Vdq_V;            // the output control voltage on d&q axis - need Global access for Ls

    MATH_Vec2  TiTune_Idq_in_A;             // the D & Q axis current are converter from 3-phase sampling input current of motor


} TiTune_Obj;

//*****************************************************************************
//
//! brief Defines the TiTune handle
//
//*****************************************************************************
typedef struct _TiTune_Obj_ *TiTune_Handle;

//*****************************************************************************
//
//! brief Object declarations
//
//*****************************************************************************
extern TiTune_Obj    TiTune_M1;

extern TiTune_Handle TiTune_handler;

//*****************************************************************************

//*****************************************************************************
// the function prototypes

//*****************************************************************************
//
//! brief     Initializes the TiTune
//!
//! param[in] pMemory   A pointer to the memory for the TiTune object
//!
//! param[in] numBytes  The number of bytes allocated for the TiTune
//!
//! return    The TiTune object handle
//
//*****************************************************************************
extern TiTune_Handle TiTune_init(void *pMemory, const size_t numBytes);

//*****************************************************************************
//
//! \brief     Sets the TiTune parameters
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_setParams(TiTune_Handle handle);

//*****************************************************************************
//
//! \brief     State machine handler function for TI Tune Routines
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_Run_stateMachine(TiTune_Handle handle);

//*****************************************************************************
//
//! \brief     Current Controller functions for TI Tune Routines
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_Run_Current_Control(TiTune_Handle handle);

//*****************************************************************************
//
//! \brief     Speed Controller functions for TI Tune Routines
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_Run_Speed_Control(TiTune_Handle handle);
//*****************************************************************************
//
//! \brief     Set parameters for Flux Estimation
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_Flux_setParams(TiTune_Flux_Handle flux_handle);
//*****************************************************************************
//
//! \brief     Set parameters for Inertia Estimation
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_J_setParams(TiTune_J_Handle handle);
//*****************************************************************************
//
//! \brief     Set parameters for Inductance Estimation
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_LDQ_setParams(TiTune_LDQ_Handle handle);
//*****************************************************************************
//
//! \brief     Set parameters for Resistance Estimation
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_Rs_setParams(TiTune_Rs_Handle handle);
//*****************************************************************************
//
//! \brief     Set parameters for Saturation Inductance Estimation
//!
//! \param[in] handle        The TiTune handle
//!
//*****************************************************************************
extern void TiTune_SatLDQ_setParams(TiTune_SatLDQ_Handle handle);

//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************


#endif /*LIBRARIES_TITUNE_H_*/
