/*
 * hal_3S_FOCInterface.c
 *
 *  Created on: Jul 8, 2025
 *      Author: a0132555
 */
#include <hal_FOCInterface.h>
/* Syscfg includes */
#include "ti_sdk_dl_config.h"

// ZEPHYR_EDIT (removed inline)
void HAL_3S_ClearCompStatus(const HAL_3S_CMP_Config_t *cmpCfg)
{
    uint8_t index =0;
    for(; index <=2 ; index++)
    {
        if(cmpCfg->compType[index] == COMP_HIGH)
        {
            HAL_clear_CMPHigh_FaultStatus(cmpCfg->cmpssHandle[index]);
        }
        else
        {
            HAL_clear_CMPLow_FaultStatus(cmpCfg->cmpssHandle[index]);
        }
    }

}

// ZEPHYR_EDIT (removed inline)
void HAL_3S_setCompIlimConfig(float32_t currentLimit,
                             const HAL_FOC_Config_t *configHandle, const HAL_FOC_Data_Obj *dataHandle)
{
       //TODO: Calibrate the Comparators and Identify the Individual Comparator Offsets

       /* Read the IPD Phase Current */

       /* Convert into the Comparator Value */
           /* If the gain is Positive , Invert the comparator direction, look for -ve current */
        uint8_t index =0;

        for(; index <=2 ; index++)
        {
            int16_t cmpValue =  dataHandle->adcData.offset_I_ad.value[index] - (int16_t)(currentLimit/ dataHandle->adcData.current_sf);

            if(cmpValue > 4095)
                cmpValue = 4095;
            else if(cmpValue <= 10)
                cmpValue = 10;

            if(configHandle->cmpCfg.compType[index] == COMP_HIGH)
            {
                HAL_Set_PhHighCompVal(configHandle->cmpCfg.cmpssHandle[index],cmpValue);
            }
            else
            {
                HAL_Set_PhLowCompVal(configHandle->cmpCfg.cmpssHandle[index],cmpValue);
            }
         }
}
__attribute__((always_inline)) inline
void HAL_3S_FOC_SetPhaseVoltages(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

     HAL_writePWMData(&configHandle->pwmCfg, &dataHandle->pwmData);
}
__attribute__((always_inline)) inline
void HAL_3S_FOC_ReadPhaseCurrents( HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    float32_t value;

    // convert phase A current
    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0]);
    dataHandle->adcData.I_A.value[0] = (value - dataHandle->adcData.offset_I_ad.value[0]) * dataHandle->adcData.current_sf;

    // convert phase B current
    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1]);
    dataHandle->adcData.I_A.value[1] = (value - dataHandle->adcData.offset_I_ad.value[1]) * dataHandle->adcData.current_sf;

    // convert phase C current
    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2]);
    dataHandle->adcData.I_A.value[2] = (value - dataHandle->adcData.offset_I_ad.value[2]) * dataHandle->adcData.current_sf;

}
__attribute__((always_inline)) inline
void HAL_3S_FOC_ReadInverterVoltages(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{
    float32_t value;

    value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[0]);

    dataHandle->adcData.V_V.value[0] = value * dataHandle->adcData.voltage_sf;

    value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[1]);

    dataHandle->adcData.V_V.value[1] = value * dataHandle->adcData.voltage_sf;

    value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[2]);

    dataHandle->adcData.V_V.value[2] = value * dataHandle->adcData.voltage_sf;

    value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[3]);

    dataHandle->adcData.VdcBus_V = value * dataHandle->adcData.voltage_sf;
}

void HAL_3S_FOC_SetCompOCLimits(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{
    HAL_3S_setCompIlimConfig(*dataHandle->pOverCurLimA, configHandle ,dataHandle);

    return;
}
void HAL_3S_FOC_Init(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

}

void HAL_3S_FOC_RunOffsetCal(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

    /*Initialize the Phase Current Offsets */
    dataHandle->adcData.offset_I_ad.value[0]  = dataHandle->gdDataHandle->gdCurrOffset;
    dataHandle->adcData.offset_I_ad.value[1]  = dataHandle->gdDataHandle->gdCurrOffset;
    dataHandle->adcData.offset_I_ad.value[2]  = dataHandle->gdDataHandle->gdCurrOffset;

    // Initialize offsets in phase voltage sensing
    dataHandle->adcData.offset_V_sf.value[0]  = dataHandle->gdDataHandle->gdVolSF;
    dataHandle->adcData.offset_V_sf.value[1]  = dataHandle->gdDataHandle->gdVolSF;
    dataHandle->adcData.offset_V_sf.value[2]  = dataHandle->gdDataHandle->gdVolSF;

        float32_t offsetK1 = 0.998001f;  // Offset filter coefficient K1: 0.05/(T+0.05);
        float32_t offsetK2 = 0.001999f;  // Offset filter coefficient K2: T/(T+0.05);
        float32_t invCurrentSf = 1.0f / dataHandle->adcData.current_sf;
        float32_t invVdcbus;

        uint16_t offsetCnt;

        //TODO: Set the PWM's to Off and move hal mtr halFOCHandle to GD
        // Set the 3-phase output PWMs to 50% duty cycle
        dataHandle->pwmData.Vabc_pu.value[0] = 0.0f;
        dataHandle->pwmData.Vabc_pu.value[1] = 0.0f;
        dataHandle->pwmData.Vabc_pu.value[2] = 0.0f;

        // write the PWM compare values
        HAL_writePWMData(&configHandle->pwmCfg, &dataHandle->pwmData);

        HAL_3S_ClearCompStatus(&configHandle->cmpCfg);

        // enable the PWM
        HAL_enablePWM(&configHandle->pwmCfg,&dataHandle->pwmData);

        // TODO: Replace with M33-compatible delay function
        // DEVICE_DELAY_US(2.0f);   // delay 2us

//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0],0);
//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1],0);
//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2],0);

        dataHandle->adcData.offset_I_ad.value[0] =
                dataHandle->adcData.offset_I_ad.value[0] * dataHandle->adcData.current_sf;
        dataHandle->adcData.offset_I_ad.value[1] =
                dataHandle->adcData.offset_I_ad.value[1] * dataHandle->adcData.current_sf;
        dataHandle->adcData.offset_I_ad.value[2] =
                dataHandle->adcData.offset_I_ad.value[2] * dataHandle->adcData.current_sf;

        for(offsetCnt = 0; offsetCnt < 32000; offsetCnt++)
        {
            // TODO: Replace with M33-compatible ADC interrupt handling
            // clear the ADC interrupt flag
            // ADC_clearInterruptStatus(configHandle->intCfg.intPeripheralBase,  configHandle->intCfg.intPeripheralNum);

            // while(ADC_getInterruptStatus(configHandle->intCfg.intPeripheralBase,  configHandle->intCfg.intPeripheralNum) == false);
            (void)configHandle;

            float32_t value;

            // convert phase A current
            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0]);
            dataHandle->adcData.I_A.value[0] = value * dataHandle->adcData.current_sf;

            // convert phase B current
            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1]);
            dataHandle->adcData.I_A.value[1] = value * dataHandle->adcData.current_sf;

            // convert phase C current
            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2]);
            dataHandle->adcData.I_A.value[2] = value * dataHandle->adcData.current_sf;

            value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[0]);

            dataHandle->adcData.V_V.value[0] = value * dataHandle->adcData.voltage_sf;

              value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[1]);

              dataHandle->adcData.V_V.value[1] = value * dataHandle->adcData.voltage_sf;

              value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[2]);

              dataHandle->adcData.V_V.value[2] = value * dataHandle->adcData.voltage_sf;

              value = HAL_ControllerReadMtrVoltageAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcVoltageCfg[3]);

              dataHandle->adcData.VdcBus_V = value * dataHandle->adcData.dcBusvoltage_sf;

            if(offsetCnt >= 2000)       // Ignore the first 2000 times
            {

                // Offsets in phase current sensing
                dataHandle->adcData.offset_I_ad.value[0] =
                        offsetK1 * dataHandle->adcData.offset_I_ad.value[0] +
                        dataHandle->adcData.I_A.value[0] * offsetK2;

                dataHandle->adcData.offset_I_ad.value[1] =
                        offsetK1 * dataHandle->adcData.offset_I_ad.value[1] +
                        dataHandle->adcData.I_A.value[1] * offsetK2;

                dataHandle->adcData.offset_I_ad.value[2] =
                        offsetK1 * dataHandle->adcData.offset_I_ad.value[2] +
                        dataHandle->adcData.I_A.value[2] * offsetK2;

                invVdcbus = 1.0f / dataHandle->adcData.VdcBus_V;

                // Offsets in phase voltage sensing
                dataHandle->adcData.offset_V_sf.value[0] =
                         offsetK1 * dataHandle->adcData.offset_V_sf.value[0] +
                         (invVdcbus * dataHandle->adcData.V_V.value[0]) * offsetK2;

                dataHandle->adcData.offset_V_sf.value[1] =
                         offsetK1 * dataHandle->adcData.offset_V_sf.value[1] +
                         (invVdcbus * dataHandle->adcData.V_V.value[1]) * offsetK2;

                dataHandle->adcData.offset_V_sf.value[2] =
                         offsetK1 * dataHandle->adcData.offset_V_sf.value[2] +
                         (invVdcbus * dataHandle->adcData.V_V.value[2]) * offsetK2;
            }
            else if (offsetCnt <= 1000)
            {
                // enable the PWM
                HAL_enablePWM(&configHandle->pwmCfg, &dataHandle->pwmData);
            }
        } // for()

        // disable the PWM
        HAL_disablePWM(&configHandle->pwmCfg, &dataHandle->pwmData);

        dataHandle->adcData.offset_I_ad.value[0] =
                dataHandle->adcData.offset_I_ad.value[0] * invCurrentSf;
        dataHandle->adcData.offset_I_ad.value[1] =
                dataHandle->adcData.offset_I_ad.value[1] * invCurrentSf;
        dataHandle->adcData.offset_I_ad.value[2] =
                dataHandle->adcData.offset_I_ad.value[2] * invCurrentSf;

//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0],(uint16_t)dataHandle->adcData.offset_I_ad.value[0]);
//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1],(uint16_t)dataHandle->adcData.offset_I_ad.value[1]);
//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2],(uint16_t)dataHandle->adcData.offset_I_ad.value[2]);

    // Check current and voltage offset

//    if( (dataHandle->adcData.offset_V_sf.value[0] > USER_M1_VA_OFFSET_SF_MAX) ||
//        (dataHandle->adcData.offset_V_sf.value[0] < USER_M1_VA_OFFSET_SF_MIN) )
//    {
//        halFOCHandle->faultMtrNow.bit.voltageOffset = 1;
//    }
//
//    if( (dataHandle->adcData.offset_V_sf.value[1] > USER_M1_VB_OFFSET_SF_MAX) ||
//        (dataHandle->adcData.offset_V_sf.value[1] < USER_M1_VB_OFFSET_SF_MIN) )
//    {
//        halFOCHandle->faultMtrNow.bit.voltageOffset = 1;
//    }
//
//    if( (dataHandle->adcData.offset_V_sf.value[2] > USER_M1_VC_OFFSET_SF_MAX) ||
//        (dataHandle->adcData.offset_V_sf.value[2] < USER_M1_VC_OFFSET_SF_MIN) )
//    {
//        halFOCHandle->faultMtrNow.bit.voltageOffset = 1;
//    }
//
//    if((halFOCHandle->faultMtrNow.bit.voltageOffset == 0) &&
//            (obj->faultMtrNow.bit.currentOffset == 0))
//    {
//        halFOCHandle->flagEnableOffsetCalc = false;
//    }
//TODO: Move the Current offset and Voltage Offset computations to independent modules and feedback the Faults.
    return;
} // end of runMotor1OffsetsCalculation() function
//! \brief      Configuration of PWM's Period and Compare Units
//! \details    Provides the correct timing to enable the gate driver
//! \param[in]  handle  The Gate Driver hardware abstraction layer (HAL) handle

void HAL_3S_FOC_ConfigPWM(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle)
{

    HAL_setupPWMs(&configHandle->pwmCfg);

    HAL_SetupThreeShuntPWMConfig(&configHandle->pwmCfg);

    HAL_StartIPDPulseCount(&configHandle->ecapIPDConfig);

}

void HAL_3S_FOC_clearMtrFaultStatus(HAL_FOC_Config_t *configHandle)
{
    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);
    HAL_3S_ClearCompStatus(&configHandle->cmpCfg);
}

void HAL_3S_IPDExit(HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);
    HAL_ResetPWM(&configHandle->pwmCfg,&dataHandle->pwmData);
    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);
    HAL_enablePWM(&configHandle->pwmCfg,&dataHandle->pwmData);
    HAL_3S_setCompIlimConfig(*dataHandle->pOverCurLimA, configHandle ,dataHandle);
}
void HAL_3S_IPDPulseConfig(VOLTAGE_VECTOR  voltageVector, HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    HAL_ResetIPDPulseCount(&configHandle->ecapIPDConfig);
    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);
    HAL_forceVoltageSector(voltageVector, &configHandle->pwmCfg);
    HAL_StartIPDPulseCount(&configHandle->ecapIPDConfig);
}
void HAL_3S_IPDInit(HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    /* Clear the 3 Phase trips */
    HAL_IPDForcePWMLow(&configHandle->pwmCfg);

    HAL_3S_ClearCompStatus(&configHandle->cmpCfg);

    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);

    HAL_3S_setCompIlimConfig(dataHandle->ipdCurrentLimit,configHandle,dataHandle);

    HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);

}
uint8_t HAL_3S_IPDPulseDone(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{
    if(HAL_getMtrTripFaults(&configHandle->pwmCfg))
    {
        HAL_IPDForcePWMLow(&configHandle->pwmCfg);
        dataHandle->ecapIPDPulseTime = HAL_getIPDPulseTime(&configHandle->ecapIPDConfig);
       HAL_3S_ClearCompStatus(&configHandle->cmpCfg);
       HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);

       return 1;
    }
    else
    {
        return 0;
    }
}
const HAL_FOC_vTable_t hal3SFOCFnTable =
{
     .Init                 = HAL_3S_FOC_Init,
     .SetPhaseVoltages     = HAL_3S_FOC_SetPhaseVoltages,
     .ReadPhaseCurrents    = HAL_3S_FOC_ReadPhaseCurrents,
     .ReadInverterVoltages = HAL_3S_FOC_ReadInverterVoltages,
     .SetCompOCLimits      = HAL_3S_FOC_SetCompOCLimits,
     .RunOffsetCal         = HAL_3S_FOC_RunOffsetCal,
     .ConfigPWM            = HAL_3S_FOC_ConfigPWM,
     .clearMtrFaultStatus  = HAL_3S_FOC_clearMtrFaultStatus,
     .ipdInit              = HAL_3S_IPDInit,
     .ipdExit              = HAL_3S_IPDExit,
     .ipdPulseConfig       = HAL_3S_IPDPulseConfig,
     .ipdPulseDone         = HAL_3S_IPDPulseDone,
};
