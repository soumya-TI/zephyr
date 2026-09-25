/* wash_sensor - polls the APDS9960 proximity channel onto subject_water_level. */
#ifndef WASH_SENSOR_H
#define WASH_SENSOR_H

/** Register the module. Call after washing_machine_init_gen(). */
void wash_sensor_init(void);

#endif /* WASH_SENSOR_H */