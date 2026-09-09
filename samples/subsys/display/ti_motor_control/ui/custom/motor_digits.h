/**
 * @file motor_digits.h
 *
 * Projects subject_rpm_act onto the four odometer digit subjects.
 */
#ifndef MOTOR_DIGITS_H
#define MOTOR_DIGITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../motor_control.h"

/** Starts observing subject_rpm_act. Safe to call once, after init_gen. */
void motor_digits_init(void);

#ifdef __cplusplus
}
#endif
#endif /*MOTOR_DIGITS_H*/
