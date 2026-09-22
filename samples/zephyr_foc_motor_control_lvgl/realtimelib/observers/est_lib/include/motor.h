//##############################################################################
// $TI Release: $
// $Release Date: $
// $Copyright: $
//##############################################################################

//------------------------------------------------------------------------------
//! MotorControl SDK
//!
//! \file   libraries/motor/include/motor.h for software library
//! \brief  Contains motor related definitions
//!
//------------------------------------------------------------------------------

#ifndef MOTOR_H
#define MOTOR_H

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

//*****************************************************************************
//
//! \defgroup MOTOR_LIB MOTOR_LIB
//! @{
//
//*****************************************************************************

// the includes

#include "types.h"


// **************************************************************************
// the defines


// **************************************************************************
// the typedefs


//
//! \brief Enumeration for the motor types
//!
typedef enum
{
  MOTOR_TYPE_INDUCTION = 0,   //!< induction
  MOTOR_TYPE_PM               //!< permanent magnet
} MOTOR_Type_e;

//
//! \brief Enumeration for the braking Mode
//
typedef enum
{
    FREE_STOP_MODE           = 0,       //!< Free stop mode without braking
    HARDSWITCH_BRAKE_MODE    = 1,       //!< Hard switch braking mode
    FORCESTOP_BRAKE_MODE     = 2,       //!< Force alignment braking mode
    DYNAMIC_BRAKE_MODE       = 3        //!< N/A, Dynamic braking mode
} Braking_Mode_e;

typedef Braking_Mode_e BRAKE_Mode_e;

//
//! \brief Enumeration for the control mode
//
typedef enum
{
    OPERATE_MODE_SPEED  = 0,        //!< Speed closed-loop running mode
    OPERATE_MODE_TORQUE = 1,        //!< Torque control running mode
    OPERATE_MODE_POWER  = 2,        //!< Constant power running mode
    OPERATE_MODE_VQCTRL = 3,        //!< Vq direction control
    OPERATE_MODE_SCALAR = 4         //!< V/f scalar control
} Operation_Mode_e;

typedef Operation_Mode_e OPERATE_Mode_e;

//! \brief Enumeration for the current sampling mode
//
typedef enum
{
    SAMPLE_MODE_DCSS2  = 0,         //!< dclink_ss2
    SAMPLE_MODE_DCSS4  = 1,         //!< dclink_ss4
    SAMPLE_MODE_DCLINK = 2,         //!< dclink_ss
    SAMPLE_MODE_3LSR   = 3,         //!< three_shunt
    SAMPLE_MODE_3INL   = 4,         //!< inline
    SAMPLE_MODE_SDFM   = 5          //!< sdfm
} SAMPLE_Mode_e;

//
//! \brief Enumeration for the Flying Start Mode
//
typedef enum
{
    FLYINGSTART_MODE_HALT    = 0,       //!< Halt Mode
    FLYINGSTART_MODE_STANDBY = 1        //!< Standby Mode
} FlyingStart_Mode_e;

//! \brief Enumeration for the current sampling mode
//
typedef enum
{
    LSC_TYPE_THREE_SHUNT  = 0,         //!< three_shunt
    LSC_TYPE_TWO_SHUNT    = 1,         //!< two_shunt
    LSC_TYPE_SINGLE_SHUNT = 2,         //!< dclink_ss
    LSC_TYPE_INLINE_SHUNT = 3          //!< inline 3/2 shunt
} CurrentShunt_Type_e;

//! \brief Enumeration for the RsOnline mode
//
typedef enum
{
    RSONLINE_CONTINUE     = 0,         //!< RsOnline Continue
    RSONLINE_INTERVAL     = 1          //!< RsOnline Interval
} RsOnline_Mode_e;

//! \brief Enumeration for the current direction mode
//
typedef enum
{
    CS_DIR_POSTIVE     = 0,              //!< Positive input direction
    CS_DIR_NEGATIVE    = 1               //!< Negative input direction
} CurrentSense_Dir_e;

//! \brief Enumeration for the current direction mode
//
typedef enum
{
    STARTUP_FREE     = 0,              //!< Startup mode
    STARTUP_HFI      = 1               //!< Startup mode
} Startup_Mode_e;

//
//! \brief Enumeration for the using estimator algorithm
//
typedef enum
{
    SLEST_TYPE_FAST_ONLY     = 0,     //!< the estimator is only FAST
    SLEST_TYPE_ESMO_ONLY     = 1,     //!< the estimator is only ESMO
} Estimator_Type_e;


// State machine typedef for motor running status
typedef enum
{
    MOTOR_STOP_IDLE      = 0,       // 0x00
    MOTOR_FAULT_STOP     = 1,       // 0x01
    MOTOR_BRAKE_STOP     = 2,       // 0x02
    MOTOR_NORM_STOP      = 3,       // 0x03
    MOTOR_CHARGE         = 4,       // 0x04
    MOTOR_SEEK_POS       = 5,       // 0x05
    MOTOR_ALIGNMENT      = 6,       // 0x06
    MOTOR_PARAM_IDENT    = 7,       // 0x07, Motor parameter identification (Rs, Ls, Ke, J)
    MOTOR_IPD            = 8,       // 0x08, Initial Position Detection (renamed from MOTOR_IPD_HFI)
    MOTOR_IPD_HFI        = 8,       // 0x08, Alias for backward compatibility
    MOTOR_OL_START       = 9,       // 0x09, Open Loop Start (renumbered)
    MOTOR_OL_END         = 10,      // 0x0A, Open Loop End transition
    MOTOR_CL_RUNNING     = 11,      // 0x0B, Closed Loop Running (renumbered)
    MOTOR_CTRL_RUN       = 12,      // 0x0C, Control Run (renumbered)
    MOTOR_FWC_RUN        = 13,      // 0x0D, Field Weakening Control Run (renumbered)
    MOTOR_BRAKE_RUN      = 14,      // 0x0E, Brake Run (renumbered)
    MOTOR_ID_START       = 15,      // 0x0F, Motor ID Start (renumbered)
    MOTOR_ID_RUN         = 16,      // 0x10, Motor ID Run (renumbered)
    MOTOR_ID_DONE        = 17,      // 0x11, Motor ID Done (renumbered)
    MOTOR_RS_START       = 18,      // 0x12, Rs Calibration Start (renumbered)
    MOTOR_RS_RUN         = 19,      // 0x13, Rs Calibration Run (renumbered)
    MOTOR_RS_DONE        = 20,      // 0x14, Rs Calibration Done (renumbered)
    MOTOR_PRMS_STORE     = 21,      // 0x15, Parameters Store (renumbered, only do this at motor stop)
    MOTOR_CODE_UPDATE    = 22       // 0x16, Code Update (renumbered, only do this at motor stop)
} MOTOR_Status_e;

//! \brief Enumeration for the motor drive control state
//
typedef enum
{
    MOTOR_CTRL_IDLE_STATE   = 0,    //!< (0000), Idle state, no any response
    MOTOR_CTRL_FREE_STOP    = 1,    //!< (0001), Stop the motor with free mode
    MOTOR_CTRL_BRAKE_STOP   = 2,    //!< (0010), Stop the motor with braking mode
    MOTOR_CTRL_URGENT_STOP  = 3,    //!< (0011), urgent stop
    MOTOR_CTRL_SPEED_CW     = 4,    //!< (0100), CW spin with speed closed-loop
    MOTOR_CTRL_SPEED_CCW    = 5,    //!< (0101), CCW spin with speed closed-loop
    MOTOR_CTRL_TORQUE_CW    = 6,    //!< (0110), CW spin with torque control
    MOTOR_CTRL_TORQUE_CCW   = 7,    //!< (0111), CCW spin with torque control
    MOTOR_CTRL_POWER_CW     = 8,    //!< (1000), CW spin with power control
    MOTOR_CTRL_POWER_CCW    = 9,    //!< (1001), CCW spin with power control
    MOTOR_CTRL_ID_AUTO      = 10,   //!< (1010), Motor parameters identification
    MOTOR_CTRL_ID_SET       = 11,   //!< (1011), Motor parameters identification
    MOTOR_CTRL_RS_AUTO      = 12,   //!< (1100), Rs recalculation
    MOTOR_CTRL_RS_SET       = 13,   //!< (1101), Rs recalculation
    MOTOR_CTRL_PRMS_RESET   = 14,   //!< (1110), Reset control parameters
    MOTOR_CTRL_DEBUG_CHECK  = 15,   //!< (1111), debug monitor
    MOTOR_CTRL_PRMS_STORE   = 0x40, //!< (0100), only do this at motor stop
    MOTOR_CTRL_CODE_UPDATE  = 0x80  //!< (1000), only do this at motor stop
} MOTOR_CtrlMode_e;

//! \brief Defines the motor parameters
//!
typedef struct _MOTOR_Params_
{
  MOTOR_Type_e type;            //!< Motor type (induction or permanent magnet)
  uint16_t     numPolePairs;    //!< Number of pole pairs for the motor
  float32_t    Lmag_H;          //!< Magnetizing inductance, H
  float32_t    Ls_d_H;          //!< Direct axis stator inductance, H
  float32_t    Ls_q_H;          //!< Quadrature axis stator inductance, H
  float32_t    Rr_d_Ohm;        //!< Direct axis rotor resistance, Ohm
  float32_t    Rr_q_Ohm;        //!< Quadrature axis rotor resistance, Ohm
  float32_t    Rs_d_Ohm;        //!< Direct axis stator resistance, Ohm
  float32_t    Rs_q_Ohm;        //!< Quadrature axis stator resistance, Ohm
  float32_t    ratedFlux_Wb;    //!< Rated flux linkage of the motor, Wb
} MOTOR_Params;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // end of MOTOR_LIB_H definition
