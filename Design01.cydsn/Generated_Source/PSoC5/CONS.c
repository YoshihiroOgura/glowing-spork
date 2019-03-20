/*******************************************************************************
* File Name: CONS.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
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
#if (CONS_INTERNAL_CLOCK_USED)
    #include "CONS_IntClock.h"
#endif /* End CONS_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 CONS_initVar = 0u;

#if (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED)
    volatile uint8 CONS_txBuffer[CONS_TX_BUFFER_SIZE];
    volatile uint8 CONS_txBufferRead = 0u;
    uint8 CONS_txBufferWrite = 0u;
#endif /* (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED) */

#if (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED))
    uint8 CONS_errorStatus = 0u;
    volatile uint8 CONS_rxBuffer[CONS_RX_BUFFER_SIZE];
    volatile uint8 CONS_rxBufferRead  = 0u;
    volatile uint8 CONS_rxBufferWrite = 0u;
    volatile uint8 CONS_rxBufferLoopDetect = 0u;
    volatile uint8 CONS_rxBufferOverflow   = 0u;
    #if (CONS_RXHW_ADDRESS_ENABLED)
        volatile uint8 CONS_rxAddressMode = CONS_RX_ADDRESS_MODE;
        volatile uint8 CONS_rxAddressDetected = 0u;
    #endif /* (CONS_RXHW_ADDRESS_ENABLED) */
#endif /* (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED)) */


/*******************************************************************************
* Function Name: CONS_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  CONS_Start() sets the initVar variable, calls the
*  CONS_Init() function, and then calls the
*  CONS_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The CONS_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time CONS_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the CONS_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void CONS_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(CONS_initVar == 0u)
    {
        CONS_Init();
        CONS_initVar = 1u;
    }

    CONS_Enable();
}


/*******************************************************************************
* Function Name: CONS_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call CONS_Init() because
*  the CONS_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void CONS_Init(void) 
{
    #if(CONS_RX_ENABLED || CONS_HD_ENABLED)

        #if (CONS_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(CONS_RX_VECT_NUM, &CONS_RXISR);
            CyIntSetPriority(CONS_RX_VECT_NUM, CONS_RX_PRIOR_NUM);
            CONS_errorStatus = 0u;
        #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        #if (CONS_RXHW_ADDRESS_ENABLED)
            CONS_SetRxAddressMode(CONS_RX_ADDRESS_MODE);
            CONS_SetRxAddress1(CONS_RX_HW_ADDRESS1);
            CONS_SetRxAddress2(CONS_RX_HW_ADDRESS2);
        #endif /* End CONS_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        CONS_RXBITCTR_PERIOD_REG = CONS_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        CONS_RXSTATUS_MASK_REG  = CONS_INIT_RX_INTERRUPTS_MASK;
    #endif /* End CONS_RX_ENABLED || CONS_HD_ENABLED*/

    #if(CONS_TX_ENABLED)
        #if (CONS_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(CONS_TX_VECT_NUM, &CONS_TXISR);
            CyIntSetPriority(CONS_TX_VECT_NUM, CONS_TX_PRIOR_NUM);
        #endif /* (CONS_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (CONS_TXCLKGEN_DP)
            CONS_TXBITCLKGEN_CTR_REG = CONS_BIT_CENTER;
            CONS_TXBITCLKTX_COMPLETE_REG = ((CONS_NUMBER_OF_DATA_BITS +
                        CONS_NUMBER_OF_START_BIT) * CONS_OVER_SAMPLE_COUNT) - 1u;
        #else
            CONS_TXBITCTR_PERIOD_REG = ((CONS_NUMBER_OF_DATA_BITS +
                        CONS_NUMBER_OF_START_BIT) * CONS_OVER_SAMPLE_8) - 1u;
        #endif /* End CONS_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (CONS_TX_INTERRUPT_ENABLED)
            CONS_TXSTATUS_MASK_REG = CONS_TX_STS_FIFO_EMPTY;
        #else
            CONS_TXSTATUS_MASK_REG = CONS_INIT_TX_INTERRUPTS_MASK;
        #endif /*End CONS_TX_INTERRUPT_ENABLED*/

    #endif /* End CONS_TX_ENABLED */

    #if(CONS_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        CONS_WriteControlRegister( \
            (CONS_ReadControlRegister() & (uint8)~CONS_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(CONS_PARITY_TYPE << CONS_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End CONS_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: CONS_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call CONS_Enable() because the CONS_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  CONS_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void CONS_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (CONS_RX_ENABLED || CONS_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        CONS_RXBITCTR_CONTROL_REG |= CONS_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        CONS_RXSTATUS_ACTL_REG  |= CONS_INT_ENABLE;

        #if (CONS_RX_INTERRUPT_ENABLED)
            CONS_EnableRxInt();

            #if (CONS_RXHW_ADDRESS_ENABLED)
                CONS_rxAddressDetected = 0u;
            #endif /* (CONS_RXHW_ADDRESS_ENABLED) */
        #endif /* (CONS_RX_INTERRUPT_ENABLED) */
    #endif /* (CONS_RX_ENABLED || CONS_HD_ENABLED) */

    #if(CONS_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!CONS_TXCLKGEN_DP)
            CONS_TXBITCTR_CONTROL_REG |= CONS_CNTR_ENABLE;
        #endif /* End CONS_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        CONS_TXSTATUS_ACTL_REG |= CONS_INT_ENABLE;
        #if (CONS_TX_INTERRUPT_ENABLED)
            CONS_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            CONS_EnableTxInt();
        #endif /* (CONS_TX_INTERRUPT_ENABLED) */
     #endif /* (CONS_TX_INTERRUPT_ENABLED) */

    #if (CONS_INTERNAL_CLOCK_USED)
        CONS_IntClock_Start();  /* Enable the clock */
    #endif /* (CONS_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: CONS_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void CONS_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (CONS_RX_ENABLED || CONS_HD_ENABLED)
        CONS_RXBITCTR_CONTROL_REG &= (uint8) ~CONS_CNTR_ENABLE;
    #endif /* (CONS_RX_ENABLED || CONS_HD_ENABLED) */

    #if (CONS_TX_ENABLED)
        #if(!CONS_TXCLKGEN_DP)
            CONS_TXBITCTR_CONTROL_REG &= (uint8) ~CONS_CNTR_ENABLE;
        #endif /* (!CONS_TXCLKGEN_DP) */
    #endif /* (CONS_TX_ENABLED) */

    #if (CONS_INTERNAL_CLOCK_USED)
        CONS_IntClock_Stop();   /* Disable the clock */
    #endif /* (CONS_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (CONS_RX_ENABLED || CONS_HD_ENABLED)
        CONS_RXSTATUS_ACTL_REG  &= (uint8) ~CONS_INT_ENABLE;

        #if (CONS_RX_INTERRUPT_ENABLED)
            CONS_DisableRxInt();
        #endif /* (CONS_RX_INTERRUPT_ENABLED) */
    #endif /* (CONS_RX_ENABLED || CONS_HD_ENABLED) */

    #if (CONS_TX_ENABLED)
        CONS_TXSTATUS_ACTL_REG &= (uint8) ~CONS_INT_ENABLE;

        #if (CONS_TX_INTERRUPT_ENABLED)
            CONS_DisableTxInt();
        #endif /* (CONS_TX_INTERRUPT_ENABLED) */
    #endif /* (CONS_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: CONS_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 CONS_ReadControlRegister(void) 
{
    #if (CONS_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(CONS_CONTROL_REG);
    #endif /* (CONS_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: CONS_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  CONS_WriteControlRegister(uint8 control) 
{
    #if (CONS_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       CONS_CONTROL_REG = control;
    #endif /* (CONS_CONTROL_REG_REMOVED) */
}


#if(CONS_RX_ENABLED || CONS_HD_ENABLED)
    /*******************************************************************************
    * Function Name: CONS_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      CONS_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      CONS_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      CONS_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      CONS_RX_STS_BREAK            Interrupt on break.
    *      CONS_RX_STS_OVERRUN          Interrupt on overrun error.
    *      CONS_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      CONS_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void CONS_SetRxInterruptMode(uint8 intSrc) 
    {
        CONS_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: CONS_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  CONS_rxBuffer - RAM buffer pointer for save received data.
    *  CONS_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  CONS_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  CONS_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 CONS_ReadRxData(void) 
    {
        uint8 rxData;

    #if (CONS_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        CONS_DisableRxInt();

        locRxBufferRead  = CONS_rxBufferRead;
        locRxBufferWrite = CONS_rxBufferWrite;

        if( (CONS_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = CONS_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= CONS_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            CONS_rxBufferRead = locRxBufferRead;

            if(CONS_rxBufferLoopDetect != 0u)
            {
                CONS_rxBufferLoopDetect = 0u;
                #if ((CONS_RX_INTERRUPT_ENABLED) && (CONS_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( CONS_HD_ENABLED )
                        if((CONS_CONTROL_REG & CONS_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            CONS_RXSTATUS_MASK_REG  |= CONS_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        CONS_RXSTATUS_MASK_REG  |= CONS_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end CONS_HD_ENABLED */
                #endif /* ((CONS_RX_INTERRUPT_ENABLED) && (CONS_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = CONS_RXDATA_REG;
        }

        CONS_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = CONS_RXDATA_REG;

    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: CONS_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  CONS_RX_STS_FIFO_NOTEMPTY.
    *  CONS_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  CONS_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   CONS_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   CONS_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 CONS_ReadRxStatus(void) 
    {
        uint8 status;

        status = CONS_RXSTATUS_REG & CONS_RX_HW_MASK;

    #if (CONS_RX_INTERRUPT_ENABLED)
        if(CONS_rxBufferOverflow != 0u)
        {
            status |= CONS_RX_STS_SOFT_BUFF_OVER;
            CONS_rxBufferOverflow = 0u;
        }
    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: CONS_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. CONS_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  CONS_rxBuffer - RAM buffer pointer for save received data.
    *  CONS_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  CONS_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  CONS_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 CONS_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (CONS_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        CONS_DisableRxInt();

        locRxBufferRead  = CONS_rxBufferRead;
        locRxBufferWrite = CONS_rxBufferWrite;

        if( (CONS_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = CONS_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= CONS_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            CONS_rxBufferRead = locRxBufferRead;

            if(CONS_rxBufferLoopDetect != 0u)
            {
                CONS_rxBufferLoopDetect = 0u;
                #if( (CONS_RX_INTERRUPT_ENABLED) && (CONS_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( CONS_HD_ENABLED )
                        if((CONS_CONTROL_REG & CONS_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            CONS_RXSTATUS_MASK_REG |= CONS_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        CONS_RXSTATUS_MASK_REG |= CONS_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end CONS_HD_ENABLED */
                #endif /* CONS_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = CONS_RXSTATUS_REG;
            if((rxStatus & CONS_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = CONS_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (CONS_RX_STS_BREAK | CONS_RX_STS_PAR_ERROR |
                                CONS_RX_STS_STOP_ERROR | CONS_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        CONS_EnableRxInt();

    #else

        rxStatus =CONS_RXSTATUS_REG;
        if((rxStatus & CONS_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = CONS_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (CONS_RX_STS_BREAK | CONS_RX_STS_PAR_ERROR |
                            CONS_RX_STS_STOP_ERROR | CONS_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: CONS_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 CONS_GetByte(void) 
    {
        
    #if (CONS_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        CONS_DisableRxInt();
        locErrorStatus = (uint16)CONS_errorStatus;
        CONS_errorStatus = 0u;
        CONS_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | CONS_ReadRxData() );
    #else
        return ( ((uint16)CONS_ReadRxStatus() << 8u) | CONS_ReadRxData() );
    #endif /* CONS_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: CONS_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  CONS_rxBufferWrite - used to calculate left bytes.
    *  CONS_rxBufferRead - used to calculate left bytes.
    *  CONS_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 CONS_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (CONS_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        CONS_DisableRxInt();

        if(CONS_rxBufferRead == CONS_rxBufferWrite)
        {
            if(CONS_rxBufferLoopDetect != 0u)
            {
                size = CONS_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(CONS_rxBufferRead < CONS_rxBufferWrite)
        {
            size = (CONS_rxBufferWrite - CONS_rxBufferRead);
        }
        else
        {
            size = (CONS_RX_BUFFER_SIZE - CONS_rxBufferRead) + CONS_rxBufferWrite;
        }

        CONS_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((CONS_RXSTATUS_REG & CONS_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: CONS_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_rxBufferWrite - cleared to zero.
    *  CONS_rxBufferRead - cleared to zero.
    *  CONS_rxBufferLoopDetect - cleared to zero.
    *  CONS_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void CONS_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        CONS_RXDATA_AUX_CTL_REG |= (uint8)  CONS_RX_FIFO_CLR;
        CONS_RXDATA_AUX_CTL_REG &= (uint8) ~CONS_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (CONS_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        CONS_DisableRxInt();

        CONS_rxBufferRead = 0u;
        CONS_rxBufferWrite = 0u;
        CONS_rxBufferLoopDetect = 0u;
        CONS_rxBufferOverflow = 0u;

        CONS_EnableRxInt();

    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: CONS_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  CONS__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  CONS__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  CONS__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  CONS__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  CONS__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  CONS_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void CONS_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(CONS_RXHW_ADDRESS_ENABLED)
            #if(CONS_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* CONS_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = CONS_CONTROL_REG & (uint8)~CONS_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << CONS_CTRL_RXADDR_MODE0_SHIFT);
                CONS_CONTROL_REG = tmpCtrl;

                #if(CONS_RX_INTERRUPT_ENABLED && \
                   (CONS_RXBUFFERSIZE > CONS_FIFO_LENGTH) )
                    CONS_rxAddressMode = addressMode;
                    CONS_rxAddressDetected = 0u;
                #endif /* End CONS_RXBUFFERSIZE > CONS_FIFO_LENGTH*/
            #endif /* End CONS_CONTROL_REG_REMOVED */
        #else /* CONS_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End CONS_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: CONS_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void CONS_SetRxAddress1(uint8 address) 
    {
        CONS_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: CONS_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void CONS_SetRxAddress2(uint8 address) 
    {
        CONS_RXADDRESS2_REG = address;
    }

#endif  /* CONS_RX_ENABLED || CONS_HD_ENABLED*/


#if( (CONS_TX_ENABLED) || (CONS_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: CONS_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   CONS_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   CONS_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   CONS_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   CONS_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void CONS_SetTxInterruptMode(uint8 intSrc) 
    {
        CONS_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: CONS_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  CONS_txBuffer - RAM buffer pointer for save data for transmission
    *  CONS_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  CONS_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  CONS_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void CONS_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(CONS_initVar != 0u)
        {
        #if (CONS_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            CONS_DisableTxInt();

            if( (CONS_txBufferRead == CONS_txBufferWrite) &&
                ((CONS_TXSTATUS_REG & CONS_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                CONS_TXDATA_REG = txDataByte;
            }
            else
            {
                if(CONS_txBufferWrite >= CONS_TX_BUFFER_SIZE)
                {
                    CONS_txBufferWrite = 0u;
                }

                CONS_txBuffer[CONS_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                CONS_txBufferWrite++;
            }

            CONS_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            CONS_TXDATA_REG = txDataByte;

        #endif /*(CONS_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: CONS_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 CONS_ReadTxStatus(void) 
    {
        return(CONS_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: CONS_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_txBuffer - RAM buffer pointer for save data for transmission
    *  CONS_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  CONS_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  CONS_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void CONS_PutChar(uint8 txDataByte) 
    {
    #if (CONS_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            CONS_DisableTxInt();
        #endif /* (CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = CONS_txBufferWrite;
            locTxBufferRead  = CONS_txBufferRead;

        #if ((CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            CONS_EnableTxInt();
        #endif /* (CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(CONS_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((CONS_TXSTATUS_REG & CONS_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            CONS_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= CONS_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            CONS_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3))
            CONS_DisableTxInt();
        #endif /* (CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3) */

            CONS_txBufferWrite = locTxBufferWrite;

        #if ((CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3))
            CONS_EnableTxInt();
        #endif /* (CONS_TX_BUFFER_SIZE > CONS_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (CONS_TXSTATUS_REG & CONS_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                CONS_SetPendingTxInt();
            }
        }

    #else

        while((CONS_TXSTATUS_REG & CONS_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        CONS_TXDATA_REG = txDataByte;

    #endif /* CONS_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: CONS_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void CONS_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(CONS_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                CONS_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: CONS_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void CONS_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(CONS_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                CONS_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: CONS_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void CONS_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(CONS_initVar != 0u)
        {
            CONS_PutChar(txDataByte);
            CONS_PutChar(0x0Du);
            CONS_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: CONS_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  CONS_txBufferWrite - used to calculate left space.
    *  CONS_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 CONS_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (CONS_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        CONS_DisableTxInt();

        if(CONS_txBufferRead == CONS_txBufferWrite)
        {
            size = 0u;
        }
        else if(CONS_txBufferRead < CONS_txBufferWrite)
        {
            size = (CONS_txBufferWrite - CONS_txBufferRead);
        }
        else
        {
            size = (CONS_TX_BUFFER_SIZE - CONS_txBufferRead) +
                    CONS_txBufferWrite;
        }

        CONS_EnableTxInt();

    #else

        size = CONS_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & CONS_TX_STS_FIFO_FULL) != 0u)
        {
            size = CONS_FIFO_LENGTH;
        }
        else if((size & CONS_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (CONS_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: CONS_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_txBufferWrite - cleared to zero.
    *  CONS_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void CONS_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        CONS_TXDATA_AUX_CTL_REG |= (uint8)  CONS_TX_FIFO_CLR;
        CONS_TXDATA_AUX_CTL_REG &= (uint8) ~CONS_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (CONS_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        CONS_DisableTxInt();

        CONS_txBufferRead = 0u;
        CONS_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        CONS_EnableTxInt();

    #endif /* (CONS_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: CONS_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   CONS_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   CONS_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   CONS_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   CONS_SEND_WAIT_REINIT - Performs both options: 
    *      CONS_SEND_BREAK and CONS_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  CONS_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with CONS_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The CONS_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void CONS_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(CONS_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(CONS_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == CONS_SEND_BREAK) ||
                (retMode == CONS_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                CONS_WriteControlRegister(CONS_ReadControlRegister() |
                                                      CONS_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                CONS_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = CONS_TXSTATUS_REG;
                }
                while((tmpStat & CONS_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == CONS_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == CONS_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = CONS_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & CONS_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == CONS_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == CONS_REINIT) ||
                (retMode == CONS_SEND_WAIT_REINIT) )
            {
                CONS_WriteControlRegister(CONS_ReadControlRegister() &
                                              (uint8)~CONS_CTRL_HD_SEND_BREAK);
            }

        #else /* CONS_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == CONS_SEND_BREAK) ||
                (retMode == CONS_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (CONS_PARITY_TYPE != CONS__B_UART__NONE_REVB) || \
                                    (CONS_PARITY_TYPE_SW != 0u) )
                    CONS_WriteControlRegister(CONS_ReadControlRegister() |
                                                          CONS_CTRL_HD_SEND_BREAK);
                #endif /* End CONS_PARITY_TYPE != CONS__B_UART__NONE_REVB  */

                #if(CONS_TXCLKGEN_DP)
                    txPeriod = CONS_TXBITCLKTX_COMPLETE_REG;
                    CONS_TXBITCLKTX_COMPLETE_REG = CONS_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = CONS_TXBITCTR_PERIOD_REG;
                    CONS_TXBITCTR_PERIOD_REG = CONS_TXBITCTR_BREAKBITS8X;
                #endif /* End CONS_TXCLKGEN_DP */

                /* Send zeros */
                CONS_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = CONS_TXSTATUS_REG;
                }
                while((tmpStat & CONS_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == CONS_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == CONS_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = CONS_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & CONS_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == CONS_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == CONS_REINIT) ||
                (retMode == CONS_SEND_WAIT_REINIT) )
            {

            #if(CONS_TXCLKGEN_DP)
                CONS_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                CONS_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End CONS_TXCLKGEN_DP */

            #if( (CONS_PARITY_TYPE != CONS__B_UART__NONE_REVB) || \
                 (CONS_PARITY_TYPE_SW != 0u) )
                CONS_WriteControlRegister(CONS_ReadControlRegister() &
                                                      (uint8) ~CONS_CTRL_HD_SEND_BREAK);
            #endif /* End CONS_PARITY_TYPE != NONE */
            }
        #endif    /* End CONS_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: CONS_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       CONS_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       CONS_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears CONS_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void CONS_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( CONS_CONTROL_REG_REMOVED == 0u )
            CONS_WriteControlRegister(CONS_ReadControlRegister() |
                                                  CONS_CTRL_MARK);
        #endif /* End CONS_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( CONS_CONTROL_REG_REMOVED == 0u )
            CONS_WriteControlRegister(CONS_ReadControlRegister() &
                                                  (uint8) ~CONS_CTRL_MARK);
        #endif /* End CONS_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndCONS_TX_ENABLED */

#if(CONS_HD_ENABLED)


    /*******************************************************************************
    * Function Name: CONS_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void CONS_LoadRxConfig(void) 
    {
        CONS_WriteControlRegister(CONS_ReadControlRegister() &
                                                (uint8)~CONS_CTRL_HD_SEND);
        CONS_RXBITCTR_PERIOD_REG = CONS_HD_RXBITCTR_INIT;

    #if (CONS_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        CONS_SetRxInterruptMode(CONS_INIT_RX_INTERRUPTS_MASK);
    #endif /* (CONS_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: CONS_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void CONS_LoadTxConfig(void) 
    {
    #if (CONS_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        CONS_SetRxInterruptMode(0u);
    #endif /* (CONS_RX_INTERRUPT_ENABLED) */

        CONS_WriteControlRegister(CONS_ReadControlRegister() | CONS_CTRL_HD_SEND);
        CONS_RXBITCTR_PERIOD_REG = CONS_HD_TXBITCTR_INIT;
    }

#endif  /* CONS_HD_ENABLED */


/* [] END OF FILE */
