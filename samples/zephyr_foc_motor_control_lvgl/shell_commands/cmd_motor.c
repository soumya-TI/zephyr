/*
 * Motor Emulator Shell Command with State Machine
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <motor_common.h>
#include <user.h>


extern uint32_t isr_counter;

/*============================================================================
 * Fixed-Point & Float Conversion from String
 *============================================================================*/

#define FP_SHIFT    16
#define FP_ONE      (1 << FP_SHIFT)

static int32_t str_to_fp(const char *s)
{
    int32_t whole = 0, frac = 0;
    int neg = 0, frac_div = 1;
    
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') {
        whole = whole * 10 + (*s - '0');
        s++;
    }
    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9' && frac_div < 100000) {
            frac = frac * 10 + (*s - '0');
            frac_div *= 10;
            s++;
        }
    }
    int32_t result = (whole << FP_SHIFT) + ((frac << FP_SHIFT) / frac_div);
    return neg ? -result : result;
}

static float str_to_float(const char *s)
{
    float result = 0.0f;
    float frac_mult = 0.1f;
    int neg = 0;
    
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') {
        result = result * 10.0f + (*s - '0');
        s++;
    }
    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9') {
            result += (*s - '0') * frac_mult;
            frac_mult *= 0.1f;
            s++;
        }
    }
    return neg ? -result : result;
}

/*============================================================================
 * data stream helper functions
 *============================================================================*/

static bool get_plot_state(void)
{
    extern bool data_stream;
    return data_stream;
}

static bool pause_plot_state (void)
{   
    extern bool data_stream;
    bool state = get_plot_state();
    data_stream = false;

    return state;
}

static void resume_plot_state (bool state) {
    extern bool data_stream;
    data_stream = state;
}

/*============================================================================
 * Shell Commands
 *============================================================================*/

 /* Plot commands */
static int cmd_plot_on (const struct shell *sh, size_t argc, char **argv) 
{
    extern bool data_stream;
    data_stream = true;

    return 0;
}

static int cmd_plot_off (const struct shell *sh, size_t argc, char **argv) 
{
    extern bool data_stream;
    data_stream = false;

    return 0;
}

static int cmd_plot_format_toggle (const struct shell *sh, size_t argc, char **argv) 
{
    extern bool data_bin;
    data_bin = !data_bin;

    return 0;
}

/* update data1, data2, to different items 
    assuming the initial ones are angle and 0
*/
static int cmd_plot_update (const struct shell *sh, size_t argc, char **argv)
{   
    extern SYSTEM_Vars_t systemVars;
    extern MOTOR_Vars_t motorVars_M1;

    typedef enum {
        speed,
        speed_ref,
        idq_ref,
        requests_max
    } requests;

    typedef enum {
        d1, 
        d2,
        d_max
    } plot_data;

    requests rq;
    plot_data d;

    /* plot details */
    char* plot = argv[1];
    char* request = argv[2];

    float32_t* requested_data_addr;

    if (!strcmp(plot, "data1")){
        d = d1;
    } else if (!strcmp(plot, "data2")) {
        d = d2;
    }

    if (d >= d_max)
    {
        /* plot is not data1 or data2 */
        shell_error(sh, "Error! data1 or data2 are supported, %s is not supported.", plot);

    } else {
        /* supported requesets 
            1. speed
            2. speed_ref
            3. idq_ref
        */
        if( !strcmp(request, "speed")) {
            rq = speed;
        } else if ( !strcmp(request, "speed_ref") ) {
            rq = speed_ref;
        } else if ( !strcmp(request, "idq_ref") ) {
            rq = idq_ref;
        }

        if (rq >= requests_max) {
            /* invalid request */
            shell_error(sh, "Error! speed, speed_ref, idq_ref are supported. %s is not supported.", request);
        } else {
            /* valid request */
            switch (rq) {
                case speed : 
                    requested_data_addr = &(motorVars_M1.speed_Hz);
                    break;
                case speed_ref :
                    requested_data_addr = &(motorVars_M1.speedRef_Hz);
                    break;
                case idq_ref :
                    requested_data_addr = &(motorVars_M1.IdqRef_A.value[1]);
                    break;
                default :
                    /* speed */
                    requested_data_addr = &(motorVars_M1.speed_Hz);
                    break;
            }

            switch(d) {
                case d1 : 
                    systemVars.dacCtrl1.pDacOutAddr = requested_data_addr;
                    break;
                case d2 :
                    systemVars.dacCtrl2.pDacOutAddr = requested_data_addr;
                    break;
                default :
                    break;
            }
        }
    }

    return 0;
};

/* Motor Commands */

/* start  */
static int cmd_motor_start (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    motorVars_M1.flagEnableRunAndIdentify = 1;

    return 0;
}
/* stop */
static int cmd_motor_stop (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    motorVars_M1.flagEnableRunAndIdentify = 0;

    return 0;
}

/* clear faults */
static int cmd_motor_clear (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    motorVars_M1.flagClearFaults = 1;

    return 0;
}

/* set speed */
static int cmd_motor_set_speed (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    
    float speed_ref = str_to_float(argv[1]);

    if (speed_ref < 0.0f || speed_ref >= 3000.0f) {
        shell_error (sh, "invalid speed");
        return 0;
    }

    motorVars_M1.speedRef_Hz = speed_ref;
    
    return 0;
}

/* get speed */
static int cmd_motor_get_speed (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    
    float speed_ref = str_to_float(argv[1]);

    shell_fprintf(sh, SHELL_NORMAL, "speed : %f\n", motorVars_M1.speed_Hz);
    
    return 0;
}

/* get Speed ref  */
static int cmd_motor_get_speedRef (const struct shell *sh, size_t argc, char **argv)
{
    extern MOTOR_Vars_t motorVars_M1;
    
    float speed_ref = str_to_float(argv[1]);

    shell_fprintf(sh, SHELL_NORMAL, "speedRef : %f\n", motorVars_M1.speedRef_Hz);
    
    return 0;
}

/*============================================================================
 * Shell Command Registration
 *============================================================================*/

SHELL_STATIC_SUBCMD_SET_CREATE(sub_plot,
    SHELL_CMD_ARG(on, NULL, "Start Plotting Data Streams", cmd_plot_on, 0, 0),
    SHELL_CMD_ARG(off, NULL, "Stop Plotting Data Streams", cmd_plot_off, 0, 0),
    SHELL_CMD_ARG(fmt, NULL, "Toggle Plot Data Format between Binary and ASCII", cmd_plot_format_toggle, 0, 0),
    SHELL_CMD_ARG(update, NULL, "Update the data streams data1, data2", cmd_plot_update, 3, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(plot, &sub_plot, "Serial Plot Commands", NULL);


SHELL_STATIC_SUBCMD_SET_CREATE(sub_motor_get,
    SHELL_CMD_ARG(speed, NULL, "Get Motor Speed.", cmd_motor_get_speed, 0, 0),
    SHELL_CMD_ARG(speedRef, NULL, "Get Motor SpeedRef.", cmd_motor_get_speedRef, 0, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_motor_set,
    SHELL_CMD_ARG(speed, NULL, "Set Motor Speed. Valid range : 10.0 to 3000.0 Hz", cmd_motor_set_speed, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_motor,
    SHELL_CMD_ARG(start, NULL, "Start Motor", cmd_motor_start, 0, 0),
    SHELL_CMD_ARG(stop, NULL, "Stop Motor", cmd_motor_stop, 0, 0),
    SHELL_CMD_ARG(clear, NULL, "Clear Motor Faults", cmd_motor_clear, 0, 0),
    SHELL_CMD(set, &sub_motor_set, "Update the Motor Parameters", NULL),
    SHELL_CMD(get, &sub_motor_get, "Get the Motor Parameters", NULL),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(motor, &sub_motor, "Motor Commands", NULL);

