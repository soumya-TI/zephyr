/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Motor Configuration Defines
 * This file MUST be included FIRST before any other headers to ensure
 * the optimization macros are visible to hal_FOCInterface.h
 */

#ifndef MOTOR_CONFIG_DEFINES_H_
#define MOTOR_CONFIG_DEFINES_H_

// ============================================================================
// OPTIMIZATION CONFIGURATION
// ============================================================================
// This bypasses function pointer indirection for time-critical ISR functions
// in Single motor Configurations

//#define DIRECT_FUNCTION_CALLS

// ============================================================================
// MOTOR SHUNT CONFIGURATION
// ============================================================================
// Define ONE of the following based on your Single motor configuration:

#if defined DIRECT_FUNCTION_CALLS
#if defined(DRV8329AEVM_REVA)
    // Single-shunt configuration
    #define MOTOR_SINGLE_SHUNT
#elif defined(BSXL_3PHGANINV) || defined(DRV8323RS_REVA)
    // Three-shunt configuration
    #define MOTOR_THREE_SHUNT
#else
    // Default to three-shunt if no board is specified
    #define MOTOR_THREE_SHUNT
#endif
#endif

#endif /* MOTOR_CONFIG_DEFINES_H_ */
