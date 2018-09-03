/*******************************************************************************
* File Name: median_timer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "median_timer.h"

static median_timer_backupStruct median_timer_backup;


/*******************************************************************************
* Function Name: median_timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  median_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void median_timer_SaveConfig(void) 
{
    #if (!median_timer_UsingFixedFunction)
        median_timer_backup.TimerUdb = median_timer_ReadCounter();
        median_timer_backup.InterruptMaskValue = median_timer_STATUS_MASK;
        #if (median_timer_UsingHWCaptureCounter)
            median_timer_backup.TimerCaptureCounter = median_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!median_timer_UDB_CONTROL_REG_REMOVED)
            median_timer_backup.TimerControlRegister = median_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: median_timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  median_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void median_timer_RestoreConfig(void) 
{   
    #if (!median_timer_UsingFixedFunction)

        median_timer_WriteCounter(median_timer_backup.TimerUdb);
        median_timer_STATUS_MASK =median_timer_backup.InterruptMaskValue;
        #if (median_timer_UsingHWCaptureCounter)
            median_timer_SetCaptureCount(median_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!median_timer_UDB_CONTROL_REG_REMOVED)
            median_timer_WriteControlRegister(median_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: median_timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  median_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void median_timer_Sleep(void) 
{
    #if(!median_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(median_timer_CTRL_ENABLE == (median_timer_CONTROL & median_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            median_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            median_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    median_timer_Stop();
    median_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: median_timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  median_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void median_timer_Wakeup(void) 
{
    median_timer_RestoreConfig();
    #if(!median_timer_UDB_CONTROL_REG_REMOVED)
        if(median_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                median_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
