/*******************************************************************************
* File Name: CONSINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CONS.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED))
    /*******************************************************************************
    * Function Name: CONS_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_rxBuffer - RAM buffer pointer for save received data.
    *  CONS_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  CONS_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  CONS_rxBufferOverflow - software overflow flag. Set to one
    *     when CONS_rxBufferWrite index overtakes
    *     CONS_rxBufferRead index.
    *  CONS_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when CONS_rxBufferWrite is equal to
    *    CONS_rxBufferRead
    *  CONS_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  CONS_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(CONS_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef CONS_RXISR_ENTRY_CALLBACK
        CONS_RXISR_EntryCallback();
    #endif /* CONS_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START CONS_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = CONS_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in CONS_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (CONS_RX_STS_BREAK | 
                            CONS_RX_STS_PAR_ERROR |
                            CONS_RX_STS_STOP_ERROR | 
                            CONS_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                CONS_errorStatus |= readStatus & ( CONS_RX_STS_BREAK | 
                                                            CONS_RX_STS_PAR_ERROR | 
                                                            CONS_RX_STS_STOP_ERROR | 
                                                            CONS_RX_STS_OVERRUN);
                /* `#START CONS_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef CONS_RXISR_ERROR_CALLBACK
                CONS_RXISR_ERROR_Callback();
            #endif /* CONS_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & CONS_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = CONS_RXDATA_REG;
            #if (CONS_RXHW_ADDRESS_ENABLED)
                if(CONS_rxAddressMode == (uint8)CONS__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & CONS_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & CONS_RX_STS_ADDR_MATCH) != 0u)
                        {
                            CONS_rxAddressDetected = 1u;
                        }
                        else
                        {
                            CONS_rxAddressDetected = 0u;
                        }
                    }
                    if(CONS_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        CONS_rxBuffer[CONS_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    CONS_rxBuffer[CONS_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                CONS_rxBuffer[CONS_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (CONS_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(CONS_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        CONS_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    CONS_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(CONS_rxBufferWrite >= CONS_RX_BUFFER_SIZE)
                    {
                        CONS_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(CONS_rxBufferWrite == CONS_rxBufferRead)
                    {
                        CONS_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (CONS_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            CONS_RXSTATUS_MASK_REG  &= (uint8)~CONS_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(CONS_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (CONS_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & CONS_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START CONS_RXISR_END` */

        /* `#END` */

    #ifdef CONS_RXISR_EXIT_CALLBACK
        CONS_RXISR_ExitCallback();
    #endif /* CONS_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED)) */


#if (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED)
    /*******************************************************************************
    * Function Name: CONS_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_txBuffer - RAM buffer pointer for transmit data from.
    *  CONS_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  CONS_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(CONS_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef CONS_TXISR_ENTRY_CALLBACK
        CONS_TXISR_EntryCallback();
    #endif /* CONS_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START CONS_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((CONS_txBufferRead != CONS_txBufferWrite) &&
             ((CONS_TXSTATUS_REG & CONS_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(CONS_txBufferRead >= CONS_TX_BUFFER_SIZE)
            {
                CONS_txBufferRead = 0u;
            }

            CONS_TXDATA_REG = CONS_txBuffer[CONS_txBufferRead];

            /* Set next pointer */
            CONS_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START CONS_TXISR_END` */

        /* `#END` */

    #ifdef CONS_TXISR_EXIT_CALLBACK
        CONS_TXISR_ExitCallback();
    #endif /* CONS_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED) */


/* [] END OF FILE */
