
#include "application.h"
#include "user.h"
#include "zephyr/irq.h"

#include <zephyr/irq.h>
#include <zephyr/kernel.h>

extern struct k_sem do_foc_sem;

void motor1CtrlISR(void)
{

}

/* ADC Trigger Interrupt to execute the FOC state machine*/
// ISR_DIRECT_DECLARE(ADC0_INT1_IRQHandler)
void ADC0_INT1_IRQHandler(void)
{
    // Acknowledge ADC interrupt
    HAL_ackMtrADCInt(&motorVars_M1.halInterface->halFOCConfigHandle->intCfg);

    //HAL_setGPIOHigh((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);

    /* Runs the FOC Control for the Motor 1 */
    //application_run((MOTOR_Handle)&motorVars_M1);

    //HAL_setGPIOLow((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);
    k_sem_give(&do_foc_sem);

    systemVars.timerCntFOCCtrlISR++;

}