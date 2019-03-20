/*******************************************************************************
* File Name: CONS_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CONS.h"


/***************************************
* Local data allocation
***************************************/

static CONS_BACKUP_STRUCT  CONS_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: CONS_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the CONS_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  CONS_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void CONS_SaveConfig(void)
{
    #if(CONS_CONTROL_REG_REMOVED == 0u)
        CONS_backup.cr = CONS_CONTROL_REG;
    #endif /* End CONS_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: CONS_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  CONS_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling CONS_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void CONS_RestoreConfig(void)
{
    #if(CONS_CONTROL_REG_REMOVED == 0u)
        CONS_CONTROL_REG = CONS_backup.cr;
    #endif /* End CONS_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: CONS_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The CONS_Sleep() API saves the current component state. Then it
*  calls the CONS_Stop() function and calls 
*  CONS_SaveConfig() to save the hardware configuration.
*  Call the CONS_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  CONS_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void CONS_Sleep(void)
{
    #if(CONS_RX_ENABLED || CONS_HD_ENABLED)
        if((CONS_RXSTATUS_ACTL_REG  & CONS_INT_ENABLE) != 0u)
        {
            CONS_backup.enableState = 1u;
        }
        else
        {
            CONS_backup.enableState = 0u;
        }
    #else
        if((CONS_TXSTATUS_ACTL_REG  & CONS_INT_ENABLE) !=0u)
        {
            CONS_backup.enableState = 1u;
        }
        else
        {
            CONS_backup.enableState = 0u;
        }
    #endif /* End CONS_RX_ENABLED || CONS_HD_ENABLED*/

    CONS_Stop();
    CONS_SaveConfig();
}


/*******************************************************************************
* Function Name: CONS_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  CONS_Sleep() was called. The CONS_Wakeup() function
*  calls the CONS_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  CONS_Sleep() function was called, the CONS_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  CONS_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void CONS_Wakeup(void)
{
    CONS_RestoreConfig();
    #if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )
        CONS_ClearRxBuffer();
    #endif /* End (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */
    #if(CONS_TX_ENABLED || CONS_HD_ENABLED)
        CONS_ClearTxBuffer();
    #endif /* End CONS_TX_ENABLED || CONS_HD_ENABLED */

    if(CONS_backup.enableState != 0u)
    {
        CONS_Enable();
    }
}


/* [] END OF FILE */
