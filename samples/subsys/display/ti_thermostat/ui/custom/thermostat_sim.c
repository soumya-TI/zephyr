/**
 * @file thermostat_sim.c
 *
 * Simulated sensors, and the seam real ones use.
 *
 * FOR THE INTEGRATION TEAM: keep thermostat_inputs_publish() - it is the whole
 * interface and contains no simulation. Delete or disable everything under
 * THERMOSTAT_SIM_ENABLE, which is a toy model of a room so the demo has
 * something to show. Set the flag to 0 in thermostat_config.h and call
 * publish() from your own sensor read; nothing else changes.
 */

#include "thermostat_sim.h"
#include "thermostat_config.h"
#include "../thermostat.h"

/*======================= THE SEAM =======================*/

void thermostat_inputs_publish(int32_t indoor_tenths, int32_t outdoor_tenths, int32_t humidity_pct)
{
    /* These three subjects are the whole interface. thermostat_logic.c observes
     * them and recomputes the state and the display strings. */
    lv_subject_set_int(&subject_indoor,   indoor_tenths);
    lv_subject_set_int(&subject_outdoor,  outdoor_tenths);
    lv_subject_set_int(&subject_humidity, humidity_pct);
}

/*======================= THE SIMULATOR =======================*/
#if THERMOSTAT_SIM_ENABLE

/* A toy room model, fast enough to watch: ask for heat and the room takes a
 * while to get there, and Auto visibly switches sides as the outdoor sweep
 * crosses the setpoint. All state in tenths of a degree. */

/* Room response while running: 0.05 C/s. Far faster than a real room. */
#define SIM_HVAC_RATE  1
#define SIM_HVAC_TICKS 8

#define SIM_LEAK_TICKS 8

/* Outdoor sweeps 4-28 C, rising first, so an unattended demo runs heat -> idle -> cool. */
#define SIM_OUTDOOR_MIN   40    /*  4.0 C */
#define SIM_OUTDOOR_MAX  280    /* 28.0 C */
#define SIM_OUTDOOR_STEP   1
#define SIM_OUTDOOR_TICKS  2

#define STATE_HEATING 1
#define STATE_COOLING 2

static int32_t sim_indoor    = 202;   /* 20.2 C - see the opening-beat note in globals.xml */
static int32_t sim_outdoor   = 118;   /* 11.8 C */
static int32_t sim_humidity  = 44;
static int32_t sim_outdoor_dir = SIM_OUTDOOR_STEP;
static uint32_t sim_tick;

static void sim_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    /*--- Outdoor: a slow sweep between the two extremes ---------------------*/
    if((sim_tick % SIM_OUTDOOR_TICKS) == 0) {
        sim_outdoor += sim_outdoor_dir;
        if(sim_outdoor <= SIM_OUTDOOR_MIN) sim_outdoor_dir = SIM_OUTDOOR_STEP;
        if(sim_outdoor >= SIM_OUTDOOR_MAX) sim_outdoor_dir = -SIM_OUTDOOR_STEP;
    }

    const int32_t state = lv_subject_get_int(&subject_hvac_state);

    if(state == STATE_HEATING) {
        if((sim_tick % SIM_HVAC_TICKS) == 0) sim_indoor += SIM_HVAC_RATE;
    }
    else if(state == STATE_COOLING) {
        if((sim_tick % SIM_HVAC_TICKS) == 0) sim_indoor -= SIM_HVAC_RATE;
    }
    else if((sim_tick % SIM_LEAK_TICKS) == 0) {
        /* Idle: the room leaks toward outside, a tenth at a time. The dead zone
         * stops it oscillating by one tenth once the two are level. */
        const int32_t diff = sim_outdoor - sim_indoor;
        if(diff > 5)       sim_indoor += 1;
        else if(diff < -5) sim_indoor -= 1;
    }

    /* Humidity wanders gently so the reading is not suspiciously static. */
    if((sim_tick % 40) == 0) {
        sim_humidity += (sim_indoor > 230) ? -1 : 1;
        if(sim_humidity < 35) sim_humidity = 35;
        if(sim_humidity > 60) sim_humidity = 60;
    }

    sim_tick++;

    /* Out through the same door real sensors will use. */
    thermostat_inputs_publish(sim_indoor, sim_outdoor, sim_humidity);
}

void thermostat_sim_init(void)
{
    lv_timer_create(sim_timer_cb, THERMOSTAT_SIM_PERIOD_MS, NULL);

    /* Publish once so the first frame is not the XML defaults. */
    thermostat_inputs_publish(sim_indoor, sim_outdoor, sim_humidity);
}

#else /* THERMOSTAT_SIM_ENABLE */

void thermostat_sim_init(void)
{
    /* Simulator compiled out. The application is expected to call
     * thermostat_inputs_publish() from its own sensor code. */
}

#endif /* THERMOSTAT_SIM_ENABLE */
