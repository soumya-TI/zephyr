/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <hal_FOCInterface.h>

#include "ti_sdk_dl_config.h"

DCLINK_SS_Obj  dcLinkObj;
// TODO: Replace with M33-compatible section placement
// #pragma DATA_SECTION(dcLinkObj,"foc_data");


static inline void HAL_1S_ClearCompStatus(const HAL_3S_CMP_Config_t *cmpCfg)
{
    uint8_t index =0;

    if(cmpCfg->compType[index] == COMP_HIGH)
    {
        HAL_clear_CMPHigh_FaultStatus(cmpCfg->cmpssHandle[index]);
    }
    else
    {
        HAL_clear_CMPLow_FaultStatus(cmpCfg->cmpssHandle[index]);
    }
}

static inline void HAL_1S_setCompIlimConfig(float32_t currentLimit,
                             const HAL_FOC_Config_t *configHandle, const HAL_FOC_Data_Obj *dataHandle)
{
    //TODO: Calibrate the Comparators and Identify the Individual Comparator Offsets

    int16_t cmpValue =  dataHandle->adcData.offset_Idc_ad + (int16_t)(currentLimit/ dataHandle->adcData.current_sf);


       if(cmpValue > 4095)
           cmpValue = 4095;
       else if(cmpValue <= 10)
           cmpValue = 10;

       /* Read the IPD Phase Current */

       /* Convert into the Comparator Value */
           /* If the gain is Positive , Invert the comparator direction, look for -ve current */

       if(configHandle->cmpCfg.compType[0] == COMP_HIGH)
       {
           HAL_Set_PhHighCompVal(configHandle->cmpCfg.cmpssHandle[0],cmpValue);
       }
       else
       {
           HAL_Set_PhLowCompVal(configHandle->cmpCfg.cmpssHandle[0],cmpValue);
       }
}
__attribute__((always_inline))
inline void HAL_1S_FOC_SetPhaseVoltages(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

     HAL_writePWMData(&configHandle->pwmCfg, &dataHandle->pwmData);

     HAL_runSingleShuntCompensation(&configHandle->pwmCfg, configHandle->dclinkHandle, dataHandle->pwmData.pVab_out_V,
                    &dataHandle->pwmData, dataHandle->adcData.VdcBus_V);

//     HAL_runSingleShuntCompensation(configHandle,dataHandle);
}
__attribute__((always_inline))
inline void HAL_1S_FOC_ReadPhaseCurrents(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

    float32_t value;

    // TODO: initialize the adcCurrentCfg with appropriate ADC base address and PPB number.
    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0]);

    dataHandle->adcData.Idc1_A.value[0] = (value - dataHandle->adcData.offset_Idc_ad) * dataHandle->adcData.current_sf;

    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1]);

    dataHandle->adcData.Idc1_A.value[1] = (value - dataHandle->adcData.offset_Idc_ad) * dataHandle->adcData.current_sf;

    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2]);

    dataHandle->adcData.Idc2_A.value[0] = (value - dataHandle->adcData.offset_Idc_ad) * dataHandle->adcData.current_sf;

    value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[3]);

    dataHandle->adcData.Idc2_A.value[1] = (value - dataHandle->adcData.offset_Idc_ad) * dataHandle->adcData.current_sf;


    // run single-shunt current reconstruction
    DCLINK_SS_runCurrentReconstruction(configHandle->dclinkHandle,
                                     &dataHandle->adcData.Idc1_A, &dataHandle->adcData.Idc2_A);

    dataHandle->adcData.I_A.value[0] = DCLINK_SS_getIa(configHandle->dclinkHandle);
    dataHandle->adcData.I_A.value[1] = DCLINK_SS_getIb(configHandle->dclinkHandle);
    dataHandle->adcData.I_A.value[2] = DCLINK_SS_getIc(configHandle->dclinkHandle);

}
__attribute__((always_inline))
inline void HAL_1S_FOC_ReadInverterVoltages(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
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

void HAL_1S_FOC_DClinkInit(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{
    DCLINK_SS_setInitialConditions(configHandle->dclinkHandle,
                                     configHandle->pwmCfg.pwmPeriod, 0.5f);

    //disable full sampling
//    DCLINK_SS_setFlag_enableFullSampling(halFOCHandle->halFOCHandle->dclinkHandle, false);     // default
    DCLINK_SS_setFlag_enableFullSampling(configHandle->dclinkHandle, true);    // test, not recommend in most cases

    //enable sequence control
//    DCLINK_SS_setFlag_enableSequenceControl(halMtrHandle->halFOCHandle->dclinkHandle, false);  // default
    DCLINK_SS_setFlag_enableSequenceControl(configHandle->dclinkHandle, true); // test, not recommend in most cases

    DCLINK_SS_setMinAVDuration(configHandle->dclinkHandle, dataHandle->gdDataHandle->gdCurrSensDelay);
    DCLINK_SS_setSampleDelay(configHandle->dclinkHandle, dataHandle->gdDataHandle->gdCurrSampDelay);
}

static void HAL_1S_FOC_SetCompOCLimits(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

    HAL_1S_setCompIlimConfig(*dataHandle->pOverCurLimA, configHandle ,dataHandle);

    return;
}
static void HAL_1S_FOC_Init(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

}

static void HAL_1S_FOC_RunOffsetCal(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{

    dataHandle->adcData.offset_Idc_ad = dataHandle->gdDataHandle->gdCurrOffset;

    // Offsets in phase voltage sensing
    dataHandle->adcData.offset_V_sf.value[0]  = dataHandle->gdDataHandle->gdPhVoltOffsetSF;
    dataHandle->adcData.offset_V_sf.value[1]  = dataHandle->gdDataHandle->gdPhVoltOffsetSF;;
    dataHandle->adcData.offset_V_sf.value[2]  = dataHandle->gdDataHandle->gdPhVoltOffsetSF;;

        float32_t offsetK1 = 0.998001f;  // Offset filter coefficient K1: 0.05/(T+0.05);
        float32_t offsetK2 = 0.001999f;  // Offset filter coefficient K2: T/(T+0.05);
        float32_t invCurrentSf = 1.0f / dataHandle->adcData.current_sf;
        float32_t invVdcbus;

        uint16_t offsetCnt;

        // TODO: Replace with M33-compatible delay function
        // DEVICE_DELAY_US(2.0f);   // delay 2us

        HAL_setOffsetTrigger(&configHandle->pwmCfg);

//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0],0);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1],0);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2],0);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[3],0);

        dataHandle->adcData.offset_Idc_ad  = dataHandle->gdDataHandle->gdCurrOffset * dataHandle->gdDataHandle->gdCurrentSF;

        // Set the 3-phase output PWMs to 50% duty cycle
        dataHandle->pwmData.Vabc_pu.value[0] = 0.0f;
        dataHandle->pwmData.Vabc_pu.value[1] = 0.0f;
        dataHandle->pwmData.Vabc_pu.value[2] = 0.0f;

        // write the PWM compare values
        HAL_writePWMData(&configHandle->pwmCfg, &dataHandle->pwmData);
        // enable the PWM
        HAL_enablePWM(&configHandle->pwmCfg ,  &dataHandle->pwmData);

        for(offsetCnt = 0; offsetCnt < 32000; offsetCnt++)
        {
            // TODO: Replace with M33-compatible ADC interrupt handling
            // clear the ADC interrupt flag
            // ADC_clearInterruptStatus(configHandle->intCfg.intPeripheralBase,  configHandle->intCfg.intPeripheralNum);

            // while(ADC_getInterruptStatus(configHandle->intCfg.intPeripheralBase,  configHandle->intCfg.intPeripheralNum) == false);

//            HAL_readMtr1ADCData(&obj->adcData);

            float32_t value;
//
//            // TODO: initialize the adcCurrentCfg with appropriate ADC base address and PPB number.
            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0]);

            dataHandle->adcData.Idc1_A.value[0] = value * dataHandle->adcData.current_sf;

            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1]);

            dataHandle->adcData.Idc1_A.value[1] = value * dataHandle->adcData.current_sf;

            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2]);

            dataHandle->adcData.Idc2_A.value[0] = value * dataHandle->adcData.current_sf;

            value = HAL_ControllerReadMtrCurrentAdcData((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[3]);

            dataHandle->adcData.Idc2_A.value[1] = value * dataHandle->adcData.current_sf;


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
                dataHandle->adcData.offset_Idc_ad = offsetK1 * dataHandle->adcData.offset_Idc_ad +
                               0.25f * offsetK2 *(dataHandle->adcData.Idc1_A.value[0] +
                                                  dataHandle->adcData.Idc1_A.value[1] +
                                                  dataHandle->adcData.Idc2_A.value[0] +
                                                  dataHandle->adcData.Idc2_A.value[1]);

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
                HAL_enablePWM(&configHandle->pwmCfg,&dataHandle->pwmData);
            }
        } // for()

        // disable the PWM
        HAL_disablePWM(&configHandle->pwmCfg,&dataHandle->pwmData);

        dataHandle->adcData.offset_Idc_ad = dataHandle->adcData.offset_Idc_ad * invCurrentSf;

//        HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[0],dataHandle->adcData.offset_Idc_ad);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[1],dataHandle->adcData.offset_Idc_ad);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[2],dataHandle->adcData.offset_Idc_ad);
//           HAL_ControllerSetADCCurrentOffset((HAL_ADC_Result_Config_Handle)&configHandle->adcCurrentCfg[3],dataHandle->adcData.offset_Idc_ad);

    // Check current and voltage offset

    // Check current and voltage offset
//    if( (dataHandle->adcData.offset_Idc_ad > USER_M1_IDC_OFFSET_AD_MAX) ||
//        (dataHandle->adcData.offset_Idc_ad < USER_M1_IDC_OFFSET_AD_MIN) )
//    {
//        halFOCHandle->faultMtrNow.bit.currentOffset = 1;
//    }
//
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
//            (halFOCHandle->faultMtrNow.bit.currentOffset == 0))
//    {
//        halFOCHandle->flagEnableOffsetCalc = false;
//    }

//TODO: Move the Current offset and Voltage Offset computations to independent modules and feedback the Faults.
    return;
} // end of runMotor1OffsetsCalculation() function
//! \brief      Configuration of PWM's Period and Compare Units
//! \details    Provides the correct timing to enable the gate driver
//! \param[in]  handle  The Gate Driver hardware abstraction layer (HAL) handle

static void HAL_1S_FOC_ConfigPWM(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle)
{

    HAL_setupPWMs(&configHandle->pwmCfg);

    HAL_SetupSingleShuntPWMConfig(&configHandle->pwmCfg);

    HAL_1S_FOC_DClinkInit(configHandle,dataHandle);

    HAL_StartIPDPulseCount(&configHandle->ecapIPDConfig);

}

static void HAL_1S_FOC_clearMtrFaultStatus(HAL_FOC_Config_t *configHandle)
{
    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);
    HAL_1S_ClearCompStatus(&configHandle->cmpCfg);
}

void HAL_1S_IPDExit(HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);
    HAL_ResetPWM(&configHandle->pwmCfg,&dataHandle->pwmData);
    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);
    HAL_enablePWM(&configHandle->pwmCfg,&dataHandle->pwmData);
    HAL_1S_setCompIlimConfig(*dataHandle->pOverCurLimA, configHandle ,dataHandle);
}
void HAL_1S_IPDPulseConfig(VOLTAGE_VECTOR  voltageVector, HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    HAL_ResetIPDPulseCount(&configHandle->ecapIPDConfig);

    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);

    HAL_forceVoltageSector(voltageVector, &configHandle->pwmCfg);

    HAL_StartIPDPulseCount(&configHandle->ecapIPDConfig);
}
void HAL_1S_IPDInit(HAL_FOC_Config_t *configHandle , HAL_FOC_Data_Obj *dataHandle)
{
    /* Clear the 3 Phase trips */
    HAL_IPDForcePWMLow(&configHandle->pwmCfg);

    HAL_1S_ClearCompStatus(&configHandle->cmpCfg);

    HAL_clearPWMFaultStatus(&configHandle->pwmCfg);

    HAL_1S_setCompIlimConfig(dataHandle->ipdCurrentLimit,configHandle,dataHandle);

    HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);

}
uint8_t HAL_1S_IPDPulseDone(HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle)
{
    if(HAL_getMtrTripFaults(&configHandle->pwmCfg))
    {
       HAL_IPDForcePWMLow(&configHandle->pwmCfg);
       dataHandle->ecapIPDPulseTime = HAL_getIPDPulseTime(&configHandle->ecapIPDConfig);
       HAL_1S_ClearCompStatus(&configHandle->cmpCfg);
       HAL_StopIPDPulseCount(&configHandle->ecapIPDConfig);
       return 1;
    }
    else
    {
        return 0;
    }
}

const HAL_FOC_vTable_t hal1SFOCFnTable =
{
     .Init                 = HAL_1S_FOC_Init,
     .SetPhaseVoltages     = HAL_1S_FOC_SetPhaseVoltages,
     .ReadPhaseCurrents    = HAL_1S_FOC_ReadPhaseCurrents,
     .ReadInverterVoltages = HAL_1S_FOC_ReadInverterVoltages,
     .SetCompOCLimits      = HAL_1S_FOC_SetCompOCLimits,
     .RunOffsetCal         = HAL_1S_FOC_RunOffsetCal,
     .ConfigPWM            = HAL_1S_FOC_ConfigPWM,
     .clearMtrFaultStatus  = HAL_1S_FOC_clearMtrFaultStatus,
     .ipdInit              = HAL_1S_IPDInit,
     .ipdExit              = HAL_1S_IPDExit,
     .ipdPulseConfig       = HAL_1S_IPDPulseConfig,
     .ipdPulseDone         = HAL_1S_IPDPulseDone,
};
