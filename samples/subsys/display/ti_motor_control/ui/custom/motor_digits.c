/**
 * @file motor_digits.c
 *
 * The one piece of DISPLAY-side C in the project, and it exists for exactly one
 * reason: the XML DSL cannot divide.
 *
 * The odometer on the run screen is four lv_rollers, each of which selects an
 * option by index. To show 1250 RPM as four wheels, something has to turn 1250
 * into {1, 2, 5, 0}. There is no arithmetic in the DSL - `bind_text-fmt` can
 * print a subject, and `bind_value` can select by it, but neither can compute
 * from it - so the projection happens here and the XML binds each wheel to its
 * own digit subject.
 *
 * That is the whole file. It reads one subject and writes four. It must not
 * decide anything about the motor; see motor_sim.c for that.
 */

#include "motor_digits.h"

/*======================= STATIC PROTOTYPES =======================*/

static void rpm_changed_cb(lv_observer_t * observer, lv_subject_t * subject);

/*======================= GLOBAL FUNCTIONS =======================*/

void motor_digits_init(void)
{
    /* A plain single-subject observer fires once on registration, so the wheels
     * are correct before the first frame without priming them by hand. */
    lv_subject_add_observer(&subject_rpm_act, rpm_changed_cb, NULL);
}

/*======================= STATIC FUNCTIONS =======================*/

static void rpm_changed_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);

    int32_t rpm = lv_subject_get_int(&subject_rpm_act);

    /* Clamp rather than modulo the top end. A speed above 9999 is a bug
     * somewhere upstream, and wrapping it to a small number would hide that on
     * the one display an operator trusts. */
    if(rpm < 0) rpm = 0;
    if(rpm > 9999) rpm = 9999;

    lv_subject_set_int(&subject_rpm_d1000, (rpm / 1000) % 10);
    lv_subject_set_int(&subject_rpm_d100,  (rpm / 100)  % 10);
    lv_subject_set_int(&subject_rpm_d10,   (rpm / 10)   % 10);
    lv_subject_set_int(&subject_rpm_d1,     rpm         % 10);
}
