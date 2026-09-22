/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Ti_Tune.h"

//*****************************************************************************
//
//! brief Object declarations
//
//*****************************************************************************


//*****************************************************************************
//
// TiTune_init
//
//*****************************************************************************
TiTune_Handle TiTune_init(void *pMemory, const size_t numBytes)
{
    TiTune_Handle handle;

    if((int16_t)numBytes < (int16_t)sizeof(TiTune_Obj))
    {
        return((TiTune_Handle)NULL);
    }

    //
    // Assign the handle
    //
    handle = (TiTune_Handle)pMemory;

    return(handle);
} // end of TiTune_init() function

//*****************************************************************************
//
// TiTune_setParams
//
//*****************************************************************************
void TiTune_setParams(TiTune_Handle handle)
{
    TiTune_Obj *obj = (TiTune_Obj *)handle;

    obj->TiTune_state = TiTune_STATE_IDLE;

    obj->TiTune_enable = 0;

    obj->TiTune_count = 0;

    obj->TiTune_Rs_IPD_Angle = 0.0f; //TODO: to be initialized with the IPD angle

    obj->TiTune_LDQ_handler = TiTune_LDQ_init(&obj->TiTune_LDQ, sizeof(obj->TiTune_LDQ));
    TiTune_LDQ_setParams(obj->TiTune_LDQ_handler);

    obj->TiTune_J_handler = TiTune_J_init(&obj->TiTune_J, sizeof(obj->TiTune_J));
    TiTune_J_setParams(obj->TiTune_J_handler);

    obj-> TiTune_Rs_handler = TiTune_Rs_init(&obj->TiTune_Rs, sizeof(obj->TiTune_Rs));
    TiTune_Rs_setParams(obj->TiTune_Rs_handler);

    obj->TiTune_SatLDQ_handler = TiTune_SatLDQ_init(&obj->TiTune_SatLDQ, sizeof(obj->TiTune_SatLDQ));
    TiTune_SatLDQ_setParams(obj->TiTune_SatLDQ_handler);

    obj->TiTune_Flux_handler = TiTune_Flux_init(&obj->TiTune_Flux, sizeof(obj->TiTune_Flux));
    TiTune_Flux_setParams(obj->TiTune_Flux_handler);

    return;
} // end of TiTune_setParams() function
////*****************************************************************************
////
//// TiTune_stateMachineHandler
////
////*****************************************************************************
void TiTune_Run_stateMachine(TiTune_Handle handle)
{
    TiTune_Obj *obj = (TiTune_Obj *)handle;
    TiTune_LDQ_Handle obj_ldq = handle->TiTune_LDQ_handler;

    PARK_Obj  TiTune_park_I;      // Handle for computing the D-Q currents from I A-B


    /* Run the Park Transform on Alpha beta Currents */

   if((obj->TiTune_state == TiTune_STATE_INERTIA_EST) ||
           (obj->TiTune_state == TiTune_STATE_FLUX_EST))
   {
       /* Compute the Estimated Rotor Angle */
       // Set the Phasor Values
       obj->Ti_Tune_phasor.value[0] = __cos(obj->TiTune_FOC_Angle_in_rad);
       obj->Ti_Tune_phasor.value[1] = __sin(obj->TiTune_FOC_Angle_in_rad);
   }
   // For Rs and Ls Estimation , the angles are pre computed */

    //TODO: the Phasor to be updated with next estimated angle after Tpwm.

   // Park & inverse Park transform working Local variables
   IPARK_Obj  TiTune_ipark_V;     // Handle for computing the iPark voltages

    PARK_setPhasor(&TiTune_park_I, &obj->Ti_Tune_phasor);

    // run the Park transform on the Alpha beta Currents.

    PARK_run(&TiTune_park_I, &obj->TiTune_Iab_in_A,
             &(obj->TiTune_Idq_in_A));

    switch (obj->TiTune_state)
    {
        case TiTune_STATE_IDLE:
            obj->TiTune_state = TiTune_STATE_LDQ_SETUP;
            break;

/**********************Start of Ls Estimation ************************************************/

        case TiTune_STATE_LDQ_SETUP:
            obj->TiTune_state = TiTune_STATE_LDQ_EST;
            obj->Ti_Tune_Out_Vdq_V.value[1] = 0.0f;

            break;
        case TiTune_STATE_LDQ_EST:


            /* Read the Feedback currents and estimate the L based on Phase and magnitude*/

            TiTune_LDQ_run(obj->TiTune_LDQ_handler, obj->VdcBus_V,
                           obj->TiTune_Idq_in_A.value[0], obj->TiTune_Idq_in_A.value[1]);

            /* Run the forced Angle Open loop model with increasing Frequency */

            obj->TiTune_FOC_Angle_in_rad = obj_ldq->ElecTheta * MATH_TWO_PI;


            obj->Ti_Tune_Out_Vdq_V.value[0] = obj->TiTune_LDQ_handler->InjVol;

            obj->Ti_Tune_Out_Vdq_V.value[1] = 0.0f;

            obj_ldq->InjVol = 0.0f;


            if(obj_ldq->Finish == true)
            {

                obj->TiTune_state = TiTune_STATE_LDQ_DONE;
                obj_ldq -> ld_Final = obj_ldq -> LminApprox * obj_ldq -> inductance_base;
                obj_ldq -> lq_Final = obj_ldq -> LmaxApprox * obj_ldq -> inductance_base;
            }

            break;

        case TiTune_STATE_LDQ_DONE:

            obj->IdqRef_A.value[0] = 0;
            obj->IdqRef_A.value[1] = 0;

            obj->Ti_Tune_Out_Vdq_V.value[0] = 0.0f;
            obj->Ti_Tune_Out_Vdq_V.value[1] = 0.0f;


            obj->TiTune_state = TiTune_STATE_RS_SETUP;
                break;

/**********************Start of Rs Estimation ************************************************/
        case TiTune_STATE_RS_SETUP:

            if(++obj->TiTune_count > TI_TUNE_STATE_DELAY_COUNT)
            {
                    obj->TiTune_count = 0;
                obj->TiTune_PI_Id.Kp = obj_ldq -> ld_Final * TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ;

                obj->TiTune_PI_Id.Ki = obj->TiTune_PI_Id.Kp * TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

                obj->TiTune_PI_Iq.Kp = obj_ldq -> lq_Final * TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ;

                obj->TiTune_PI_Iq.Ki = obj->TiTune_PI_Iq.Kp * TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

                /* Initial Rotor angle based on the LMin */

                float32_t angle_rad;

                angle_rad = obj_ldq -> InitialRotorAngle * MATH_TWO_PI;

                if(angle_rad > MATH_PI)
                {
                    angle_rad -= MATH_TWO_PI;
                }

                obj->TiTune_Rs_IPD_Angle = angle_rad;

                obj->TiTune_Rs_handler->VdistGain1 = (2.0f / 3.0f) * (MATH_abs(cosf(obj->TiTune_Rs_IPD_Angle)) +
                        MATH_abs(cosf(obj->TiTune_Rs_IPD_Angle - (1.0f/3.0f)*MATH_TWO_PI))
                        + MATH_abs(cosf(obj->TiTune_Rs_IPD_Angle - (2.0f/3.0f)*MATH_TWO_PI)));

                obj->TiTune_state = TiTune_STATE_RS_EST;
            }
            break;
        case TiTune_STATE_RS_EST:
//            TiTune_Rs_Align_Count

             TiTune_Rs_run(obj->TiTune_Rs_handler, obj->TiTune_Rs_handler->VdistGain1 , obj->TiTune_Idq_in_A.value[0], obj->Ti_Tune_Out_Vdq_V.value[0]);

            /* Set the Reference Idq currents for Current Controller */
            obj->IdqRef_A.value[0] = obj->TiTune_Rs.IdRef * TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
            obj->IdqRef_A.value[1] = 0;

            TiTune_Run_Current_Control(handle);


            obj->TiTune_FOC_Angle_in_rad =  obj->TiTune_Rs_IPD_Angle;

            /* When Rs Estimastion is completed Switch to Rs Done State */
            if (obj->TiTune_Rs_handler->Finish == true)
            {
                obj->TiTune_state = TiTune_STATE_RS_DONE;
                obj->TiTune_Rs_handler -> Rs_Final = obj->TiTune_Rs_handler -> Rs * obj->TiTune_Rs_handler -> impedance_base;
            }
            break;
        case TiTune_STATE_RS_DONE:

            obj->TiTune_state = TiTune_STATE_OBSERVER_SETUP;
//            obj->TiTune_state = TiTune_STATE_DONE;
            break;
/**************************End of Rs Estimation********************************************/

        case TiTune_STATE_OBSERVER_SETUP:

            obj->TiTune_state = TiTune_STATE_INERTIA_SETUP;

            break;
/**************************Start of Inertia over Flux Estimation********************************************/
        case TiTune_STATE_INERTIA_SETUP:   //!< motor inertia set-up state

            if(++obj->TiTune_count > TI_TUNE_STATE_DELAY_COUNT)
            {
                obj->TiTune_count = 0;

                float32_t RbyLd = (obj->TiTune_Rs_handler->Rs_Final) / (obj->TiTune_LDQ_handler->ld_Final);

                float32_t RbyLq = (obj->TiTune_Rs_handler->Rs_Final) / (obj->TiTune_LDQ_handler->lq_Final);

                obj->TiTune_PI_Id.Ki = 0.25f * RbyLd * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

                obj->TiTune_PI_Iq.Ki = 0.25f * RbyLq * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

//              obj->TiTune_PI_Id.Ki = TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ * obj->TiTune_Rs_handler->Rs_Final * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

//              obj->TiTune_PI_Iq.Ki = TI_TUNE_TORQUE_LOOP_CUT_OFF_FREQ * obj->TiTune_Rs_handler->Rs_Final * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec;

                obj->TiTune_state = TiTune_STATE_INERTIA_EST;
            }

            break;

        case TiTune_STATE_INERTIA_EST:     //!< estimation of motor inertia state, J

            TiTune_J_run(obj->TiTune_J_handler, obj->TiTune_Speed_in_Hz, obj->TiTune_Idq_in_A.value[1]);

            if (obj->TiTune_J_handler->TorqueSign > 0)
           {
               obj->IdqRef_A.value[1]= obj->TiTune_J_handler->IqRef * TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
           }
           else
           {
               obj->IdqRef_A.value[1]= - obj->TiTune_J_handler->IqRef * TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
           }

            obj->IdqRef_A.value[0] = 0;

            TiTune_Run_Current_Control(handle);

            /* reset the Reference currents to Zero */
            if (obj->TiTune_J_handler->Finish == true)
            {

                obj->TiTune_state = TiTune_STATE_INERTIA_DONE;
           }
        break;

        case TiTune_STATE_INERTIA_DONE:    //!< motor inertia done state

            // TODO : Ke Estimation is enabled : Continue to spin the Motor.

            obj->IdqRef_A.value[1]= obj->TiTune_J_handler->IqRef * TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;

            obj->TiTune_state = TiTune_STATE_FLUX_SETUP;
            // TODO : IF ke Is not enabled Set the Current Reference to Zero
            //obj->IdqRef_A.value[1]= 0.0;
           // obj->IdqRef_A.value[0]= 0.0;

        break;;

/**************************End of Inertia over Flux Estimation********************************************/

/**************************Start of Flux Estimation********************************************/
        case TiTune_STATE_FLUX_SETUP:   //!< motor inertia set-up state

            if(++obj->TiTune_count > TI_TUNE_STATE_DELAY_COUNT)
            {
                obj->TiTune_count = 0;

                obj->TiTune_Flux.Rs = obj->TiTune_Rs.Rs_Final / obj->TiTune_Flux.X_base;
                obj->TiTune_Flux.Lq = obj->TiTune_LDQ.lq_Final / (obj->TiTune_Flux.X_base / (MATH_TWO_PI * obj->TiTune_Flux.F_Base));

                //Computed J/kt is in P.U => actual value of J/Kt = P.u * (Ibase / Wbase) , Also the J/Kt computation is done with Omega on P.U To be compensated with wbase

                float32_t inertiaGain = (TI_TUNE_USER_MOTOR1_MAX_CURRENT_A /((TI_TUNE_ELECTRICAL_FREQ_BASE_HZ) * (MATH_TWO_PI * TI_TUNE_ELECTRICAL_FREQ_BASE_HZ)));

                obj->TiTune_PI_Speed.Kp = TI_TUNE_SPEED_CUT_OFF_FREQ * obj->TiTune_J_handler->InertiaOverFlux * inertiaGain;

                obj->TiTune_PI_Speed.Ki = TI_TUNE_SPEED_CUT_OFF_FREQ * obj->TiTune_PI_Speed.Kp * TI_TUNE_USER_M1_TORQUE_LOOP_PERIOD_sec * TI_TUNE_USER_M1_NUM_ISR_TICKS_PER_SPEED_TICK;

                /* Initialize the Speed Integrator with the Current reference */

                PI_setUi(&obj->TiTune_PI_Speed, obj->IdqRef_A.value[1]);

                /* initialize the Maximum and Minimum Currents */

                PI_setMinMax(&obj->TiTune_PI_Speed,
                                0.0,
                                TI_TUNE_USER_MOTOR1_MAX_CURRENT_A);

                obj->TiTune_state = TiTune_STATE_FLUX_EST;
            }

            TiTune_Run_Current_Control(handle);

            break;

        case TiTune_STATE_FLUX_EST:     //!< estimation of motor inertia state, J

            if (obj->TiTune_Flux_handler->SlowDownEn == 0.0f)
            {
                if (MATH_abs(obj->TiTune_Speed_in_Hz * obj->TiTune_Flux_handler -> F_Base) > MATH_abs((0.95f * (obj->TiTune_Flux_handler -> TestSpeed * obj->TiTune_Flux_handler -> F_Base)  )) )
                {
                obj->TiTune_Flux_handler -> SpeedRampArrives = 1.0f;
                }

                else
                {
                obj->TiTune_Flux_handler -> SpeedRampArrives = 0.0f;
                }
             }

            else
            {
               if ((obj->TiTune_Flux_handler ->SpeedRef * obj->TiTune_Flux_handler -> F_Base) < MATH_abs((0.5f * (obj->TiTune_Flux_handler -> TestSpeed * obj->TiTune_Flux_handler -> F_Base)  )) )
               {
                   obj->TiTune_Flux_handler -> SpeedRampArrives = 1.0f;
               }
               else
               {
                   obj->TiTune_Flux_handler -> SpeedRampArrives = 0.0f;
               }
            }

           // Execute
                  TiTune_Flux_run(obj -> TiTune_Flux_handler, obj->TiTune_Speed_in_Hz,  obj->TiTune_Idq_in_A.value[1], obj->Ti_Tune_Out_Vdq_V.value[1]);

                  obj->TiTune_count++;
                 //  Run Speed Loop at Slower rates.
                 if(obj->TiTune_count >= TI_TUNE_USER_M1_NUM_ISR_TICKS_PER_SPEED_TICK)
                 {
                     TiTune_Run_Speed_Control(handle);
                     obj->TiTune_count = 0;
                 }

                 TiTune_Run_Current_Control(handle);

          // Finish
          if (obj->TiTune_Flux_handler -> Finish == 1.0f)
         {
              obj->Ti_Tune_Out_Vdq_V.value[0] = 0.0f;
              obj->Ti_Tune_Out_Vdq_V.value[1] = 0.0f;

              obj->TiTune_state = TiTune_STATE_FLUX_DONE;
         }
        break;

        case TiTune_STATE_FLUX_DONE:    //!< motor inertia done state

            obj->TiTune_state = TiTune_STATE_DONE;

        break;;

/**************************End of Inertia over Flux Estimation********************************************/

        case TiTune_STATE_DONE:

            obj->Ti_Tune_Out_Vdq_V.value[1]= 0.0;
            obj->Ti_Tune_Out_Vdq_V.value[0]= 0.0;
            break;

        default:
            break;

    }

    // Set the Phasor Values
    obj->Ti_Tune_phasor.value[0] = __cos(obj->TiTune_FOC_Angle_in_rad);
    obj->Ti_Tune_phasor.value[1] = __sin(obj->TiTune_FOC_Angle_in_rad);
//
    // set the phasor in the inverse Park transform
    IPARK_setPhasor(&TiTune_ipark_V, &obj->Ti_Tune_phasor);

    // run the inverse Park module
    IPARK_run(&TiTune_ipark_V,
              &obj->Ti_Tune_Out_Vdq_V, &obj->Ti_Tune_Out_Vab_V);
}
void TiTune_Run_Current_Control(TiTune_Handle handle)
{
    TiTune_Obj *obj = (TiTune_Obj *)handle;

    float32_t maxVsMag_V = USER_M1_MAX_VS_MAG_PU * obj->VdcBus_V;

    PI_setMinMax(&obj->TiTune_PI_Id,
                 -maxVsMag_V, maxVsMag_V);


    // run the Id controller
    PI_run(&obj->TiTune_PI_Id, obj->IdqRef_A.value[0],
           obj->TiTune_Idq_in_A.value[0], (float32_t*)&obj->Ti_Tune_Out_Vdq_V.value[0]);

    // calculate Iq controller limits
    float32_t outMax_V = __sqrt((maxVsMag_V * maxVsMag_V) -
                      (obj->Ti_Tune_Out_Vdq_V.value[0] * obj->Ti_Tune_Out_Vdq_V.value[0]));

    PI_setMinMax(&obj->TiTune_PI_Iq, -outMax_V, outMax_V);

    // run the Iq controller
    PI_run(&obj->TiTune_PI_Iq, obj->IdqRef_A.value[1],
           obj->TiTune_Idq_in_A.value[1], (float32_t*)&obj->Ti_Tune_Out_Vdq_V.value[1]);

    //TODO Update the Idq reference to Fast Library
}
void TiTune_Run_Speed_Control(TiTune_Handle handle)
{
    TiTune_Obj *obj = (TiTune_Obj *)handle;

    float32_t speedRef_Hz;

    speedRef_Hz = ((obj->TiTune_Flux_handler -> SpeedRef) * (obj->TiTune_Flux_handler -> F_Base)) ;

    PI_run(&obj->TiTune_PI_Speed,
                       speedRef_Hz,
                       obj->TiTune_Speed_in_Hz,
                      (float32_t *)&obj->IdqRef_A.value[1]);
}
//
// end of file
//

void TiTune_Flux_setParams(TiTune_Flux_Handle flux_handle)
{
    TiTune_Flux_Obj *obj = (TiTune_Flux_Obj *)flux_handle;

    obj->Vph_Base = (TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V /1.732f); //USER_MOTOR1_RATED_VOLTAGE_V;
    obj->I_Base = TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
    obj->F_Base = TI_TUNE_ELECTRICAL_FREQ_BASE_HZ;
    obj->X_base = obj->Vph_Base / obj->I_Base;
    obj->flux_base = obj->Vph_Base / obj->F_Base; // for conversion to Hz, will need to divide to two PI if rad/s was needed

    obj->VariableInit = true;
    obj->AveragingSize = 20000;
    obj->WaitingTime2Settle_Speed = 50000.0f;
    obj->MaxTime4Settle = 100000.0f;
    obj->TestSpeed = 0.5f;
    obj->SpeedErrLim4Settle = 0.05f;// Not so small causing convergence problem, not so high resulting in pre-mature pass to estimation part.

    obj->TimePU = MATH_TWO_PI *  obj->F_Base * TI_TUNE_USER_M1_PWM_PERIOD_sec;
    obj->LPF_IqDeriv_Gain = (((5.0f) * obj->TimePU) / ( 1.0f + (5.0f * obj->TimePU))); // LPF with 500-Hz cut-off - Backward Euler
    obj->LPF_SpeedErr_Gain = ((0.01f * obj->TimePU) / (1.0f + (0.01f * obj->TimePU))); // LPF with 1-Hz cut-off - Backward Euler

    return;
} // end of TiTune_Flux_setParams() function

// Sets up parameters for angle generation
void TiTune_J_setParams(TiTune_J_Handle handle)
{
    TiTune_J_Obj *obj = (TiTune_J_Obj *)handle;

    obj->VariableInit = true;
    obj->RiseTimeMin = 4000;                                    // Parameter: Number. Allowed minimum sample number for speed rise from  'SpdMinLim' to 'SpdMaxLim'.
    obj->RiseTimeMax = 8000;                                    // Parameter: Number. Allowed maximum sample number for speed rise from  'SpdMinLim' to 'SpdMaxLim'.
    obj->SettlePeriod = 2;                                      // Parameter: Number. Speed period number for settling; before estimation begins and after suitable torque is found.
    obj->MaxSampleSizeDFT = 14000;                              // Parameter: Number. Maximum sample size for DFT. Reason is purely numerical, for better numerical accuracy in DFT calculaion.

    obj->Vdc_Base = TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V;
    obj->Vph_Base = TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V/1.732f; // USER_MOTOR1_RATED_VOLTAGE_V;
    obj->I_Base = TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
    obj->F_Base = 100;
    obj->P_Base = TI_TUNE_USER_MOTOR1_POLE_PAIRS * 2.0f;
//    obj->Lam_PM = USER_MOTOR1_RATED_FLUX_VpHz * MATH_ONE_OVER_TWO_PI;

    obj->Nbase = 1/(TI_TUNE_USER_M1_PWM_PERIOD_sec * obj->F_Base); //(USER_M1_PWM_FREQ_kHz * 1000) / obj->F_Base;   // Parameter: Number. Base sample size: Fsample/Fbase.
    obj->SpdMinLim = 0.1f;                                     // Parameter: Per-Unit. Allowed min value for speed magnitude.
    obj->SpdMaxLim = 0.3f;                                     // Parameter: Per-Unit. Allowed max value for speed magnitude.
    obj->IqMaxLim = 1.0f;//0.4f;                                       // Parameter: Per-Unit. Allowed max value for Iq magnitude.
    obj->IqInit = 0.001f;                                       // Parameter: Per-Unit. Initialization for Iq value.

    obj->X_base = obj->Vph_Base/obj->I_Base;
    obj->flux_base = obj->Vph_Base/(MATH_TWO_PI*obj->F_Base);
    obj->inductance_base = obj->X_base/(MATH_TWO_PI*obj->F_Base);
    obj->power_base = 1.5f * obj->Vph_Base * obj->I_Base;
    obj->torque_base = (0.5f * obj->P_Base * obj->power_base )  / (MATH_TWO_PI*obj->F_Base);
    obj->friction_base = (0.5f * obj->P_Base * obj->torque_base )  / (MATH_TWO_PI*obj->F_Base);
    obj->inertia_base = obj->friction_base   / (MATH_TWO_PI*obj->F_Base);
    obj->J_eq_base = obj->inertia_base * obj->I_Base / obj->torque_base;



    return;
} // end of TiTune_J_init() function

void TiTune_LDQ_setParams(TiTune_LDQ_Handle handle)
{
    TiTune_LDQ_Obj *obj = (TiTune_LDQ_Obj *)handle;

    obj->Vdc_Base = TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V;
    obj->Vph_Base = (TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V/1.732f); //Per Phase Peak Voltage  //USER_MOTOR1_RATED_VOLTAGE_V;
    obj->I_Base = TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
    obj->F_Base = 100.0f;
    obj->X_base = obj->Vph_Base / obj->I_Base;
    obj->flux_base = obj->Vph_Base / (MATH_TWO_PI * obj->F_Base);
    obj->inductance_base = obj->X_base / (MATH_TWO_PI * obj->F_Base);

    obj->VariableInit = true;
    obj->InjVolHalfPrdSampleNoMax = 30.0f;
    obj->InjVolHalfPrdSampleNoInit = 8.0f;
    obj->InjSettlePrd = 2.0f;
    obj->AngIntgGain = obj->F_Base * TI_TUNE_USER_M1_PWM_PERIOD_sec;
    obj->ElecThetaStep = 0.004f;
    obj->InjVolMagnInit = 0.002f;
    obj->IsMagnMinLim = 0.02f;
    obj->IsMagnMaxLim = 0.5f;
    obj->InjVolMagnMaxLim = 0.98f;
//    obj->i12 = 0;

    return;
} // end of TiTune_LDQ_setParams() function
// Sets up parameters for angle generation
void TiTune_Rs_setParams(TiTune_Rs_Handle handle)
{
    TiTune_Rs_Obj *obj = (TiTune_Rs_Obj *)handle;

    obj->Vdc_Base = TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V;
    obj->Vph_Base = (TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V/1.732f); //Per Phase Peak Voltage  //USER_MOTOR1_RATED_VOLTAGE_V;
    obj->I_Base = TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
    obj->F_Base = 100.0f;
    obj->impedance_base = obj->Vph_Base / obj->I_Base;

    obj->VariableInit = true;
    obj->AveragingSize = 5000;
    obj->Wait4SettleCurrent = 15000;
    obj->MaxTime4Settle = 30000;
    obj->TestCurrent1 = 0.4f;
    obj->TestCurrent2 = 0.5f;
    obj->IdErrLim4Settle = 0.05f;
    obj->LPF_IdErr_Gain = ((1.0f * 2 * MATH_PI * obj->F_Base * TI_TUNE_USER_M1_PWM_PERIOD_sec) / (1.0f +  1.0f * 2 * MATH_PI * obj->F_Base * TI_TUNE_USER_M1_PWM_PERIOD_sec));
    obj->i12 = 0;

    return;
} // end of TiTune_Rs_setParams() function
// Sets up parameters for angle generation
void TiTune_SatLDQ_setParams(TiTune_SatLDQ_Handle handle)
{
    TiTune_SatLDQ_Obj *obj = (TiTune_SatLDQ_Obj *)handle;
    obj->Vdc_Base = TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V;
    obj->Vph_Base = (TI_TUNE_USER_M1_ADC_FULL_SCALE_VOLTAGE_V/1.732f); //Per Phase Peak Voltage  //USER_MOTOR1_RATED_VOLTAGE_V;
    obj->I_Base = TI_TUNE_USER_MOTOR1_MAX_CURRENT_A;
    obj->F_Base = 100.0f;
    obj->X_base = obj->Vph_Base / obj->I_Base;
    obj->flux_base = obj->Vph_Base / (MATH_TWO_PI * obj->F_Base);
    obj->inductance_base = obj->X_base / (MATH_TWO_PI * obj->F_Base);

    obj->VariableInit = true;
    obj->MotorType = MtrType_IPMSM;
    obj->Nbase = 1/(TI_TUNE_USER_M1_PWM_PERIOD_sec * obj->F_Base); //(USER_M1_PWM_FREQ_kHz * 1000) / obj->F_Base;   // Parameter: Number. Base sample size: Fsample/Fbase.
    obj->InjSettleWait_PrdNo = 1;
    obj->dcSettleWait_SampleNo = 20;
    obj->InjCurFreqInit = (8.0f);
    obj->InjCurMagn = (0.05f);
    obj->IrefDCStep = (0.1f);

    if (obj->MotorType == MtrType_INDUCTION)
    {
     obj->InjCurFreqInit = (0.4f);
     obj->InjSettleWait_PrdNo = 50;
    }

    return;
} // end of TiTune_SatLDQ_setParams() function
