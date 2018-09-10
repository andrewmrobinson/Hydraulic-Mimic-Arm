/*******************************************************************************
* File Name: PWM_0_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PWM_0.h"

static PWM_0_backupStruct PWM_0_backup;


/*******************************************************************************
* Function Name: PWM_0_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_0_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_0_SaveConfig(void) 
{

    #if(!PWM_0_UsingFixedFunction)
        #if(!PWM_0_PWMModeIsCenterAligned)
            PWM_0_backup.PWMPeriod = PWM_0_ReadPeriod();
        #endif /* (!PWM_0_PWMModeIsCenterAligned) */
        PWM_0_backup.PWMUdb = PWM_0_ReadCounter();
        #if (PWM_0_UseStatus)
            PWM_0_backup.InterruptMaskValue = PWM_0_STATUS_MASK;
        #endif /* (PWM_0_UseStatus) */

        #if(PWM_0_DeadBandMode == PWM_0__B_PWM__DBM_256_CLOCKS || \
            PWM_0_DeadBandMode == PWM_0__B_PWM__DBM_2_4_CLOCKS)
            PWM_0_backup.PWMdeadBandValue = PWM_0_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_0_KillModeMinTime)
             PWM_0_backup.PWMKillCounterPeriod = PWM_0_ReadKillTime();
        #endif /* (PWM_0_KillModeMinTime) */

        #if(PWM_0_UseControl)
            PWM_0_backup.PWMControlRegister = PWM_0_ReadControlRegister();
        #endif /* (PWM_0_UseControl) */
    #endif  /* (!PWM_0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_0_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_0_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_0_RestoreConfig(void) 
{
        #if(!PWM_0_UsingFixedFunction)
            #if(!PWM_0_PWMModeIsCenterAligned)
                PWM_0_WritePeriod(PWM_0_backup.PWMPeriod);
            #endif /* (!PWM_0_PWMModeIsCenterAligned) */

            PWM_0_WriteCounter(PWM_0_backup.PWMUdb);

            #if (PWM_0_UseStatus)
                PWM_0_STATUS_MASK = PWM_0_backup.InterruptMaskValue;
            #endif /* (PWM_0_UseStatus) */

            #if(PWM_0_DeadBandMode == PWM_0__B_PWM__DBM_256_CLOCKS || \
                PWM_0_DeadBandMode == PWM_0__B_PWM__DBM_2_4_CLOCKS)
                PWM_0_WriteDeadTime(PWM_0_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_0_KillModeMinTime)
                PWM_0_WriteKillTime(PWM_0_backup.PWMKillCounterPeriod);
            #endif /* (PWM_0_KillModeMinTime) */

            #if(PWM_0_UseControl)
                PWM_0_WriteControlRegister(PWM_0_backup.PWMControlRegister);
            #endif /* (PWM_0_UseControl) */
        #endif  /* (!PWM_0_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_0_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_0_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_0_Sleep(void) 
{
    #if(PWM_0_UseControl)
        if(PWM_0_CTRL_ENABLE == (PWM_0_CONTROL & PWM_0_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_0_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_0_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_0_UseControl) */

    /* Stop component */
    PWM_0_Stop();

    /* Save registers configuration */
    PWM_0_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_0_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_0_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_0_Wakeup(void) 
{
     /* Restore registers values */
    PWM_0_RestoreConfig();

    if(PWM_0_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_0_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
