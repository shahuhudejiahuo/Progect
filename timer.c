/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of AutoChips Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */
#define _TIM_    1
#include "includes.h"
/**
*
* @param[in] none
*
* @return none
*
* @brief TIM_GpioInit; Init GPIO,  Change GPIO Value in timer interrupt
*
*/
void TIM_GpioInit(void)
{
    GPIO_SetFunc(PWM_GPIO, 0);//Set PWM3_CH0 as GPIO
    GPIO_SetDir(PWM_GPIO, 1);   
}
/**
*
* @param[in] timerIndex: timer Index from 0 to 7
*
* @return none
*
* @brief TIMx CallBack
*
*/
void TIMx_Task(uint8_t timerIndex)
{
    TIMER_Type *TIMx = TIMER_GetTimerCtrl(timerIndex);
    if (TIMER_GetIntFlag(TIMx))
    {
        TIMER_ClrIntFlag(TIMx);   
        g_timerCount ++;
        if (g_timerCount % 2)
        {
            GPIO_HIGH; //High time: 2 * DelayTime
			printf("ADC0 RESULT= %d\n",*(uint32_t *)0x4000304c);
        }
        else
        {
            GPIO_LOW; //Low Time: DelayTime            
        }
        if (g_timerCount > MAX_COUNT)
        {
            g_timerCount = 0;
        }
    }
}
/**
*
* @param[in] TIMx: TIMER_Type, from TIMER0 to TIMER7
* @param[in] loadValue: timer loadValue, timer frequency = APB_BUS_FREQ / (loadValue)
* @return none
*
* @brief TIM_Initilaize
*
*/
void TIM_Initilaize(TIMER_Type *TIMx, uint32_t loadValue)
{
    TIMER_ConfigType  timerConfig0 = {0};

    /* configure TIMER channel 0, only enable timer */
    timerConfig0.loadValue      = loadValue;  ///1s timeout
    timerConfig0.interruptEn      = 1;
    timerConfig0.linkMode        = 0;
    timerConfig0.timerEn          = 1;

    TIMER_SetCallback(TIMx, TIMx_Task);
    TIMER_Init(TIMx, &timerConfig0);       
}
/**
*
* @param[in] TIMx: TIMER_Type, from TIMER0 to TIMER7
* @param[in] loadValue: timer loadValue, timer frequency = APB_BUS_FREQ / (loadValue)
* @return none
*
* @brief TIM_Initilaize
*
*/
void TIM_ChangePeriod(TIMER_Type *TIMx, uint32_t loadValue)
{
    TIMER_SetLoadVal(TIMx, loadValue);
}

