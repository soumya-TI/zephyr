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

#ifndef USER_MTR1_H
#define USER_MTR1_H

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

//
// the includes
// platforms
#include "user_common.h"


//******************************************************************************
// List Of Fixed Common Configurations
//------------------------------------------------------------------------------

//
//! \brief Defines the number of current sensors // Not shunt, The Shunt value is used to extract further
#define USER_M1_NUM_CURRENT_SENSORS                 (3)
//! \brief Defines the number of voltage sensors Including DC Bus
//!
#define USER_M1_NUM_VOLTAGE_SENSORS                 (3)  // Fixed.

//******************************************************************************
//------------------------------------------------------------------------------
//! \brief motor lost phase current threshold
#define USER_M1_LOST_PHASE_CURRENT_A        (0.01f)


//! \brief motor unbalance ratio percent threshold
#define USER_M1_UNBALANCE_RATIO             (0.2f)

//! \brief motor over load power threshold
#define USER_M1_OVER_LOAD_POWER_W           (90.0f)

//! \brief motor stall current threshold
#define USER_M1_STALL_CURRENT_A             (10.0f)

//! \brief motor fault check current threshold
#define USER_M1_FAULT_CHECK_CURRENT_A       (0.2f)

//! \brief motor failed maximum speed threshold
#define USER_M1_FAIL_SPEED_MAX_HZ           (1800.0f)

//! \brief motor failed minimum speed threshold
#define USER_M1_FAIL_SPEED_MIN_HZ           (5.0f)

//! \brief Defines the number of failed torque
//!
#define USER_M1_TORQUE_FAILED_SET           (0.000001f)

//! \brief Defines the maximum negative current to be applied in Id reference
//!
#define USER_M1_MAX_NEGATIVE_ID_REF_CURRENT_A       (-2)


//! \brief Defines the number of pwm clock ticks per isr clock tick
//!        Note: Valid values are 1, 2 or 3 only
#define USER_M1_NUM_PWM_TICKS_PER_ISR_TICK          (1)


//! \brief Defines the number of ISR clock ticks per current controller clock tick
//!
#define USER_M1_NUM_ISR_TICKS_PER_CURRENT_TICK      (1)


//! \brief Defines the number of ISR clock ticks per speed controller clock tick
//!
#define USER_M1_NUM_ISR_TICKS_PER_SPEED_TICK        (10)


//! \brief Defines the Pulse Width Modulation (PWM) frequency, kHz
//!
#define USER_PWM_FREQ_kHz                         (10)
#define USER_M1_PWM_FREQ_kHz        (10)
#define USER_M1_PWM_TBPRD_NUM       (uint16_t)(USER_SYSTEM_FREQ_MHz * 1000.0f / USER_M1_PWM_FREQ_kHz / 2.0f)

//! \brief Defines the Pulse Width Modulation (PWM) period, usec
//!
#define USER_M1_PWM_PERIOD_usec     (1000.0f / USER_M1_PWM_FREQ_kHz)


//! \brief Defines the Interrupt Service Routine (ISR) frequency, Hz
//!
#define USER_M1_ISR_FREQ_Hz         (USER_M1_PWM_FREQ_kHz * 1000.0f / (float32_t)USER_M1_NUM_PWM_TICKS_PER_ISR_TICK)

//! \brief Defines the Interrupt Service Routine (ISR) period, usec
//!
#define USER_M1_ISR_PERIOD_usec     (USER_M1_PWM_PERIOD_usec * (float32_t)USER_M1_NUM_PWM_TICKS_PER_ISR_TICK)


//! \brief Defines the direct voltage (Vd) scale factor
//!
#define USER_M1_VD_SF               (0.95)


//! \brief Defines the maximum Vs magnitude in per units allowed
//! \brief This value sets the maximum magnitude for the output of the Id and
//! \brief Iq PI current controllers. The Id and Iq current controller outputs
//! \brief are Vd and Vq. The relationship between Vs, Vd, and Vq is:
//! \brief Vs = sqrt(Vd^2 + Vq^2).  In this FOC controller, the Vd value is set
//! \brief equal to USER_MAX_VS_MAG*USER_VD_MAG_FACTOR.
//! \brief so the Vq value is set equal to sqrt(USER_MAX_VS_MAG^2 - Vd^2).
//!
//! \brief Set USER_MAX_VS_MAG = 0.5 for a pure sinewave with a peak at
//! \brief SQRT(3)/2 = 86.6% duty cycle.  No current reconstruction
//! \brief is needed for this scenario.
//!
//! \brief Set USER_MAX_VS_MAG = 1/SQRT(3) = 0.5774 for a pure sinewave
//! \brief with a peak at 100% duty cycle.  Current reconstruction
//! \brief will be needed for this scenario (Lab08).
//!
//! \brief Set USER_MAX_VS_MAG = 2/3 = 0.6666 to create a trapezoidal
//! \brief voltage waveform.  Current reconstruction will be needed
//! \brief for this scenario (Lab08).
//!
//! \brief For space vector over-modulation, see lab08 for details on
//! \brief system requirements that will allow the SVM generator to
//! \brief go all the way to trapezoidal.
//!
#define USER_M1_MAX_VS_MAG_PU               (0.576f)


//! \brief Defines the reference Vs magnitude in per units allowed
//! \      Set the value equal from 0.5 to 0.95 of the maximum Vs magnitude
#define USER_M1_VS_REF_MAG_PU               (0.8f * USER_M1_MAX_VS_MAG_PU)

//! \brief Defines the R/L excitation frequency, Hz
//!
#define USER_M1_R_OVER_L_EXC_FREQ_Hz  (300)


//! \brief Defines the R/L Kp scale factor, pu
//! \brief Kp used during R/L is USER_M1_R_OVER_L_KP_SF * USER_M1_NOMINAL_DC_BUS_VOLTAGE_V / USER_M1_MAX_CURRENT_A;
//!
#define USER_M1_R_OVER_L_KP_SF        (0.02f)


//! \brief Defines maximum acceleration for the estimation speed profiles, Hz/sec
//!
#define USER_M1_MAX_ACCEL_Hzps        (2)



//! \brief Defines the controller execution period, usec
//!
#define USER_M1_CTRL_PERIOD_usec            ((float32_t)USER_M1_ISR_PERIOD_usec)


//! \brief Defines the controller execution period, sec
//!
#define USER_M1_CTRL_PERIOD_sec             ((float32_t)USER_M1_CTRL_PERIOD_usec / 1000000.0f)

//! \brief Defines the IdRated delta to use during estimation
//!
#define USER_M1_IDRATED_DELTA_A                 (0.0001f)

//! \brief Defines the forced angle frequency, Hz
#define USER_M1_FORCE_ANGLE_FREQ_Hz             (1)

//! \brief Defines the forced angle acceleration, Hz
#define USER_M1_FORCE_ANGLE_ACCEL_Hzps          (10)

//! \brief Defines the near zero speed limit for electrical frequency estimation, Hz
//!        The flux integrator uses this limit to regulate flux integration
#define USER_M1_FREQ_NEARZEROSPEEDLIMIT_Hz      (0)

//! \brief Defines the fraction of IdRated to use during inductance estimation
//!
#define USER_M1_IDRATED_FRACTION_FOR_L_IDENT    (0.5f)


//! \brief Defines the fraction of SpeedMax to use during inductance estimation
//!
#define USER_M1_SPEEDMAX_FRACTION_FOR_L_IDENT  (1)


//! \brief Defines the Power Warp gain for computing Id reference
//! \brief If motor parameters are known, set this gain to:
//! \brief USER_M1_PW_GAIN = SQRT(1.0 + USER_MOTOR1_Rr_Ohm / USER_MOTOR1_Rs_Ohm)
//!
#define USER_M1_PW_GAIN                        (1)


//! \brief Defines the pole location for the DC bus filter, rad/sec
//!
#define USER_M1_DCBUS_POLE_rps                  (100)


//! \brief Defines the pole location for the voltage and current offset estimation, rad/s
//!
#define USER_M1_OFFSET_POLE_rps                 (20)


//! \brief Defines the pole location for the speed control filter, rad/sec
//!
#define USER_M1_SPEED_POLE_rps                  (100)


//! \brief Defines the pole location for the direction filter, rad/sec
//!
#define USER_M1_DIRECTION_POLE_rps              (MATH_TWO_PI * 10.0f)


//! \brief Defines the pole location for the second direction filter, rad/sec
//!
#define USER_M1_DIRECTION_POLE_2_rps           (628.3185307179587f)


//! \brief Defines the pole location for the flux estimation, rad/sec
//!
#define USER_M1_FLUX_POLE_rps                  (10)

//! \brief Defines the pole location for the R/L estimation, rad/sec
//!
#define USER_M1_R_OVER_L_POLE_rps               (MATH_TWO_PI * 3.2f)

//! \brief Defines the convergence factor for the estimator
//!
#define USER_M1_EST_KAPPAQ                          (1.5f)


//! \brief Defines the scale factor for the flux estimation
//! the default value is 1.0f, change the value between 0.1f and 1.25f
//!
#define USER_M1_EST_FLUX_HF_SF                     (0.25f)

//! \brief Defines the scale factor for the frequency estimation
//! the default value is 1.0f, change the value between 0.5f and 1.5f
//!
#define USER_M1_EST_FREQ_HF_SF                     (1)

//! \brief Defines the scale factor for the bemf estimation
//! the default value is 1.0f, change the value between 0.50f and 1.25f
//!
#define USER_M1_EST_BEMF_HF_SF                     (1)

//! brief Define the Kp gain for Field Weakening Control
#define USER_M1_FWC_KP                 (0.0225f)

//! brief Define the Ki gain for Field Weakening Control
#define USER_M1_FWC_KI                 (0.00225f)

//! brief Define the maximum current vector angle for Field Weakening Control
#define USER_M1_FWC_MAX_ANGLE          -15.0f                        // degree
#define USER_M1_FWC_MAX_ANGLE_RAD      USER_M1_FWC_MAX_ANGLE /180.0f * MATH_PI  // rad

//! brief Define the minimum current vector angle for Field Weakening Control
#define USER_M1_FWC_MIN_ANGLE          0.0f                          // degree
#define USER_M1_FWC_MIN_ANGLE_RAD      USER_M1_FWC_MIN_ANGLE /180.0f * MATH_PI  // rad

//! \brief Defines the number of DC bus over/under voltage setting time
//!  timer base = 5ms
#define USER_M1_VOLTAGE_FAULT_TIME_SET          (500)

//! \brief Defines the number of motor over load setting time
//!  timer base = 5ms, 1s
#define USER_M1_OVER_LOAD_TIME_SET              (200)

//! \brief Defines the number of motor stall setting time
//!  timer base = 5ms, 1s
#define USER_M1_STALL_TIME_SET                  (200)

//! \brief Defines the number of phase unbalanced setting time
//!  timer base = 5ms, 5s
#define USER_M1_UNBALANCE_TIME_SET              (1000)

//! \brief Defines the number of lost phase setting time
//!  timer base = 5ms, 10s
#define USER_M1_LOST_PHASE_TIME_SET             (2000)

//! \brief Defines the number of over speed setting time
//!  timer base = 5ms
#define USER_M1_OVER_SPEED_TIME_SET             (600)

//! \brief Defines the number of startup failed setting time
//!  timer base = 5ms, 10s
#define USER_M1_STARTUP_FAIL_TIME_SET           (2000)

//! \brief Defines the number of over load setting times
//!
#define USER_M1_OVER_CURRENT_TIMES_SET          (5)

//! \brief Defines the number of stop wait time
//!  timer base = 5ms, 10s
#define USER_M1_STOP_WAIT_TIME_SET              (2000)

//! \brief Defines the number of restart wait time
//!  timer base = 5ms, 10s
#define USER_M1_RESTART_WAIT_TIME_SET           (2000)

//! \brief Defines the number of restart time
//!
#define USER_M1_START_TIMES_SET                 (3)

//! \brief Defines the alignment time
//!
#define USER_M1_ALIGN_TIMES_SET                 (2000)

//! \brief Defines the QEP unit ticks
#define USER_M1_QEP_UNIT_TIMER_TICKS        (uint32_t)(USER_SYSTEM_FREQ_MHz/(2.0f * USER_M1_ISR_FREQ_Hz) * 1000000.0f)

//! \brief Defines the current filter pole location, Hz
#define USER_M1_IS_FILTER_POLE_Hz           (7500.0f)      // 7.5kHz

//! \brief Defines the current filter pole location, rad/s
//!
#define USER_M1_IS_FILTER_POLE_rps          (MATH_TWO_PI * USER_M1_IS_FILTER_POLE_Hz)


//! \brief Defines the voltage filter pole location, Hz
#define USER_M1_VS_FILTER_POLE_Hz           (30000.0f)     // 30.0kHz

//! \brief Defines the voltage filter pole location, rad/s
//!
#define USER_M1_VS_FILTER_POLE_rps          (MATH_TWO_PI * USER_M1_VS_FILTER_POLE_Hz)

//==============================================================================
// Motor defines

#define MOTOR1_TEKNIC

// #define MOTOR1_PANASONIC_BLOWER

#ifdef MOTOR1_TEKNIC

#define USER_MOTOR1_TYPE                   	(MOTOR_TYPE_PM)
#define USER_MOTOR1_NUM_POLE_PAIRS         	(4)
#define USER_MOTOR1_Rr_Ohm                 	(0)
#define USER_MOTOR1_Rs_Ohm                 	(0.393955578f)
#define USER_MOTOR1_Ls_d_H                 	(0.000190442806f)
#define USER_MOTOR1_Ls_q_H                 	(0.000190442806f)
#define USER_MOTOR1_RATED_FLUX_VpHz        	(0.0399353318f)
#define USER_MOTOR1_BW_SPD_LOOP_RATIO       (12.0f)
#define USER_MOTOR1_MAGNETIZING_CURRENT_A  	(0)
#define USER_MOTOR1_RES_EST_CURRENT_A      	(1.5f)
#define USER_MOTOR1_IND_EST_CURRENT_A      	(-1)
#define USER_MOTOR1_MAX_CURRENT_A          	(6.6f)
#define USER_MOTOR1_FLUX_EXC_FREQ_Hz       	(60)
#define USER_MOTOR1_NUM_ENC_SLOTS          	(1000U)
#define USER_MOTOR1_INERTIA_Kgm2           	(0.00000706154f)

#define USER_MOTOR1_FREQ_NEARZEROLIMIT_Hz  	(5)

// Ls compensation coefficient
#define USER_MOTOR1_Ls_d_COMP_COEF         (0.15f)          // 0.0f~0.5f
#define USER_MOTOR1_Ls_q_COMP_COEF         (0.35f)          // 0.0f~0.5f
#define USER_MOTOR1_Ls_MIN_NUM_COEF        (0.55f)          // 0.5f~1.0f

#define USER_MOTOR1_RATED_VOLTAGE_V        	(24)
#define USER_MOTOR1_RATED_SPEED_KRPM       	(3)

#define USER_MOTOR1_FREQ_MIN_HZ            	(9)
#define USER_MOTOR1_FREQ_MAX_HZ            	(600)

#define USER_MOTOR1_FREQ_LOW_HZ            	(5)
#define USER_MOTOR1_FREQ_HIGH_HZ           	(400)
#define USER_MOTOR1_VOLT_MIN_V             	(1)
#define USER_MOTOR1_VOLT_MAX_V             	(24)

// the maximum/base rotation frequency of the motor (Hz)
#define USER_MOTOR1_FREQ_MAX_Hz            (600.0f)         // Hz

// V/f Profile Parameters for open-loop in build level 2
// the low frequency f_low  of V/f profile, in Hz,
// set to 10% of rated motor frequency
#define USER_MOTOR1_FREQ_LOW_Hz            (5.0f)           // Hz

// the high frequency f_high of V/f profile, in Hz,
// set to 100% of rated motor frequency
#define USER_MOTOR1_FREQ_HIGH_Hz           (400.0f)         // Hz


#define USER_MOTOR1_FORCE_DELTA_A          	(0.05f)
#define USER_MOTOR1_ALIGN_DELTA_A          	(0.01f)
#define USER_MOTOR1_FLUX_CURRENT_A         	(0.5f)
#define USER_MOTOR1_ALIGN_CURRENT_A        	(1)
#define USER_MOTOR1_STARTUP_CURRENT_A      	(0.5f)
#define USER_MOTOR1_TORQUE_CURRENT_A       	(3)
#define USER_MOTOR1_OVER_CURRENT_A         	(7.5f)

#define USER_MOTOR1_BRAKE_CURRENT_A        	(1)
#define USER_MOTOR1_BRAKE_TIME_DELAY       	(12000)

#define USER_MOTOR1_SPEED_START_Hz         	(35)
#define USER_MOTOR1_SPEED_FORCE_Hz         	(30)
#define USER_MOTOR1_ACCEL_START_Hzps       	(10)
#define USER_MOTOR1_ACCEL_MAX_Hzps         	(20)

#define USER_MOTOR1_SPEED_FS_Hz            	(3)




// for Rs online calibration
#define USER_MOTOR1_RSONLINE_WAIT_TIME     	(60000U)
#define USER_MOTOR1_RSONLINE_WORK_TIME     	(24000U)

//! \brief Defines the maximum current slope for Id trajectory
//!
#define USER_M1_MAX_CURRENT_DELTA_A        (USER_MOTOR1_RES_EST_CURRENT_A / USER_M1_ISR_FREQ_Hz)


//! \brief Defines the maximum current slope for Id trajectory during power warp mode
//!
#define USER_M1_MAX_CURRENT_DELTA_PW_A    (0.3f * USER_MOTOR1_RES_EST_CURRENT_A / USER_M1_ISR_FREQ_Hz)

#define USER_M1_IPD_FAULT_TIME_SEC          (5.0f)

#endif

#ifdef MOTOR1_PANASONIC_BLOWER


#define USER_MOTOR1_TYPE                    (MOTOR_TYPE_PM)
#define USER_MOTOR1_NUM_POLE_PAIRS          (4)

//#TAG_MOTOR_PARAMS_START#
#define USER_MOTOR1_Rr_Ohm                  (0)
#define USER_MOTOR1_Rs_Ohm                  (0.800f)
#define USER_MOTOR1_Ls_d_H                  (0.0014423047f)
#define USER_MOTOR1_Ls_q_H                  (0.0014423047f)
#define USER_MOTOR1_RATED_FLUX_VpHz         (0.0384637527)
#define USER_MOTOR1_MAX_CURRENT_A           (2.2f)

#define USER_MOTOR1_BW_SPD_LOOP_RATIO       (12.0f)
//#TAG_MOTOR_PARAMS_END#
#define USER_MOTOR1_MAGNETIZING_CURRENT_A   (0)
#define USER_MOTOR1_RES_EST_CURRENT_A       (1.5f)
#define USER_MOTOR1_IND_EST_CURRENT_A       (-1)

#define USER_MOTOR1_FLUX_EXC_FREQ_Hz        (60)
#define USER_MOTOR1_NUM_ENC_SLOTS           (1000U)
#define USER_MOTOR1_INERTIA_Kgm2            (0.00000706154f)




#define USER_MOTOR1_FREQ_NEARZEROLIMIT_Hz   (5)

// Ls compensation coefficient
#define USER_MOTOR1_Ls_d_COMP_COEF         (0.15f)          // 0.0f~0.5f
#define USER_MOTOR1_Ls_q_COMP_COEF         (0.35f)          // 0.0f~0.5f
#define USER_MOTOR1_Ls_MIN_NUM_COEF        (0.55f)          // 0.5f~1.0f

#define USER_MOTOR1_RATED_VOLTAGE_V         (24)
#define USER_MOTOR1_RATED_SPEED_KRPM        (3)

#define USER_MOTOR1_FREQ_MIN_HZ             (9)
#define USER_MOTOR1_FREQ_MAX_HZ             (100)

#define USER_MOTOR1_FREQ_LOW_HZ             (5)
#define USER_MOTOR1_FREQ_HIGH_HZ            (100)
#define USER_MOTOR1_VOLT_MIN_V              (1)
#define USER_MOTOR1_VOLT_MAX_V              (4)

// the maximum/base rotation frequency of the motor (Hz)
#define USER_MOTOR1_FREQ_MAX_Hz            (600.0f)         // Hz

// V/f Profile Parameters for open-loop in build level 2
// the low frequency f_low  of V/f profile, in Hz,
// set to 10% of rated motor frequency
#define USER_MOTOR1_FREQ_LOW_Hz            (5.0f)           // Hz

// the high frequency f_high of V/f profile, in Hz,
// set to 100% of rated motor frequency
#define USER_MOTOR1_FREQ_HIGH_Hz           (100.0f)         // Hz


#define USER_MOTOR1_FORCE_DELTA_A           (0.05f)
#define USER_MOTOR1_ALIGN_DELTA_A           (0.01f)
#define USER_MOTOR1_FLUX_CURRENT_A          (0.5f)
#define USER_MOTOR1_ALIGN_CURRENT_A         (1)
#define USER_MOTOR1_STARTUP_CURRENT_A       (1.0f)
#define USER_MOTOR1_TORQUE_CURRENT_A        (2.0f)
#define USER_MOTOR1_OVER_CURRENT_A          (4.0f)

#define USER_MOTOR1_BRAKE_CURRENT_A         (1)
#define USER_MOTOR1_BRAKE_TIME_DELAY        (12000)

#define USER_MOTOR1_SPEED_START_Hz          (35)
#define USER_MOTOR1_SPEED_FORCE_Hz          (30)
#define USER_MOTOR1_ACCEL_START_Hzps        (10)
#define USER_MOTOR1_ACCEL_MAX_Hzps          (20)

#define USER_MOTOR1_SPEED_FS_Hz             (3)




// for Rs online calibration
#define USER_MOTOR1_RSONLINE_WAIT_TIME      (60000U)
#define USER_MOTOR1_RSONLINE_WORK_TIME      (24000U)

//! \brief Defines the maximum current slope for Id trajectory
//!
#define USER_M1_MAX_CURRENT_DELTA_A        (USER_MOTOR1_RES_EST_CURRENT_A / USER_M1_ISR_FREQ_Hz)


//! \brief Defines the maximum current slope for Id trajectory during power warp mode
//!
#define USER_M1_MAX_CURRENT_DELTA_PW_A    (0.3f * USER_MOTOR1_RES_EST_CURRENT_A / USER_M1_ISR_FREQ_Hz)

#define USER_M1_IPD_FAULT_TIME_SEC          (5.0f)

#endif

// Only for eSMO
// PLL (phase-locked loop)
// PID proportional, integral, derivative
// the sliding mode control maximum gain that equals to Ke*fmax/vscale/sqrt(2)*factor(max),
//  tune the factor(0.1~10) based the test status
#define USER_MOTOR1_KSLIDE_MAX             (0.5f)

// the sliding mode control minimum gain that equals to Ke*fmin/vscale/sqrt(2)*factor(min),
//  tune the factor(0.1~10) based the test status
#define USER_MOTOR1_KSLIDE_MIN             (0.1f)

// the PLL control maximum gain that equals to 2*(Damping factor)*(Natural frequency)*factor(max),
// tune the factor(0.1~10) based the test status
#define USER_MOTOR1_PLL_KP_MAX             (10.0f)

// the PLL control minimum gain that equals to 2*(Damping factor)*(Natural frequency)*factor(min),
// tune the factor(0.1~5) based the test status
#define USER_MOTOR1_PLL_KP_MIN             (1.50f)

// the PLL control gain adjusting coefficient that
// equals to (Kpll_max-Kpll_min)/fscale/fmax
#define USER_MOTOR1_PLL_KP_SF              (5.0f)

// the phase-locked loop control integration gain that
// equals to (Natural frequency)*(Natural frequency)*Ts
#define USER_MOTOR1_PLL_KI                 (2.8125E-06f)    // Not used, reserve

// the threshold of the estimated current error for sliding mode control that
// equals to (motor maximum BEMF voltage / rated voltage), (0.3~0.5 )
#define USER_MOTOR1_BEMF_THRESHOLD         (0.5f)

// the parameters of the low-pass filter for the estimated back EMF,
// Kslf equal to (fc*2*PI()*Ts), (0.5~2.5)
#define USER_MOTOR1_BEMF_KSLF_FC_SF        (2.0f)

// the offset coefficient to compensate the error by using
// the low-pass filter that equals to 1.0, or [0.5~1.5]
#define USER_MOTOR1_THETA_OFFSET_SF        (1.0f)

// the cut-off frequency of the low-pass filter to calculate the estimated speed, (100~400)
#define USER_MOTOR1_SPEED_LPF_FC_Hz        (200.0f)

// the THeta Ramp rate during Closed Loop Transition in deg/sec
#define USER_MOTOR1_THETA_RAMP_RATE       (360.0f)

// the globals
extern USER_Params userParams_M1;


// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"

#endif
