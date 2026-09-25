#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

#include "ti_washing_machine.h"

/* Only lp_mspm33c321a's overlay instantiates door_btn; other boards
 * supported by this sample have no such node. */
#if DT_NODE_EXISTS(DT_NODELABEL(door_btn))

static void wash_door_cb(struct input_event * evt, void * user_data)
{
    LV_UNUSED(user_data);

    int curr_state = lv_subject_get_int(&subject_door);
    if(evt->code != INPUT_KEY_1) return;

    lv_subject_set_int(&subject_door, 1 - curr_state);
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(buttons)), wash_door_cb, NULL);

void wash_door_init(void)
{
    /* Callback is statically registered via INPUT_CALLBACK_DEFINE above. */
}

#else /* !DT_NODE_EXISTS(DT_NODELABEL(door_btn)) */

void wash_door_init(void)
{
    /* No door_btn node on this board target; subject_door stays UI-driven. */
}

#endif