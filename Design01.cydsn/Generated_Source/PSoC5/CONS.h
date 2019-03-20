/*******************************************************************************
* File Name: CONS.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_CONS_H)
#define CY_UART_CONS_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define CONS_RX_ENABLED                     (0u)
#define CONS_TX_ENABLED                     (1u)
#define CONS_HD_ENABLED                     (0u)
#define CONS_RX_INTERRUPT_ENABLED           (0u)
#define CONS_TX_INTERRUPT_ENABLED           (0u)
#define CONS_INTERNAL_CLOCK_USED            (1u)
#define CONS_RXHW_ADDRESS_ENABLED           (0u)
#define CONS_OVER_SAMPLE_COUNT              (8u)
#define CONS_PARITY_TYPE                    (0u)
#define CONS_PARITY_TYPE_SW                 (0u)
#define CONS_BREAK_DETECT                   (0u)
#define CONS_BREAK_BITS_TX                  (13u)
#define CONS_BREAK_BITS_RX                  (13u)
#define CONS_TXCLKGEN_DP                    (1u)
#define CONS_USE23POLLING                   (1u)
#define CONS_FLOW_CONTROL                   (0u)
#define CONS_CLK_FREQ                       (0u)
#define CONS_TX_BUFFER_SIZE                 (4u)
#define CONS_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define CONS_CONTROL_REG_REMOVED            (0u)
#else
    #define CONS_CONTROL_REG_REMOVED            (1u)
#endif /* End CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct CONS_backupStruct_
{
    uint8 enableState;

    #if(CONS_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End CONS_CONTROL_REG_REMOVED */

} CONS_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void CONS_Start(void) ;
void CONS_Stop(void) ;
uint8 CONS_ReadControlRegister(void) ;
void CONS_WriteControlRegister(uint8 control) ;

void CONS_Init(void) ;
void CONS_Enable(void) ;
void CONS_SaveConfig(void) ;
void CONS_RestoreConfig(void) ;
void CONS_Sleep(void) ;
void CONS_Wakeup(void) ;

/* Only if RX is enabled */
#if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )

    #if (CONS_RX_INTERRUPT_ENABLED)
        #define CONS_EnableRxInt()  CyIntEnable (CONS_RX_VECT_NUM)
        #define CONS_DisableRxInt() CyIntDisable(CONS_RX_VECT_NUM)
        CY_ISR_PROTO(CONS_RXISR);
    #endif /* CONS_RX_INTERRUPT_ENABLED */

    void CONS_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void CONS_SetRxAddress1(uint8 address) ;
    void CONS_SetRxAddress2(uint8 address) ;

    void  CONS_SetRxInterruptMode(uint8 intSrc) ;
    uint8 CONS_ReadRxData(void) ;
    uint8 CONS_ReadRxStatus(void) ;
    uint8 CONS_GetChar(void) ;
    uint16 CONS_GetByte(void) ;
    uint8 CONS_GetRxBufferSize(void)
                                                            ;
    void CONS_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define CONS_GetRxInterruptSource   CONS_ReadRxStatus

#endif /* End (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */

/* Only if TX is enabled */
#if(CONS_TX_ENABLED || CONS_HD_ENABLED)

    #if(CONS_TX_INTERRUPT_ENABLED)
        #define CONS_EnableTxInt()  CyIntEnable (CONS_TX_VECT_NUM)
        #define CONS_DisableTxInt() CyIntDisable(CONS_TX_VECT_NUM)
        #define CONS_SetPendingTxInt() CyIntSetPending(CONS_TX_VECT_NUM)
        #define CONS_ClearPendingTxInt() CyIntClearPending(CONS_TX_VECT_NUM)
        CY_ISR_PROTO(CONS_TXISR);
    #endif /* CONS_TX_INTERRUPT_ENABLED */

    void CONS_SetTxInterruptMode(uint8 intSrc) ;
    void CONS_WriteTxData(uint8 txDataByte) ;
    uint8 CONS_ReadTxStatus(void) ;
    void CONS_PutChar(uint8 txDataByte) ;
    void CONS_PutString(const char8 string[]) ;
    void CONS_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void CONS_PutCRLF(uint8 txDataByte) ;
    void CONS_ClearTxBuffer(void) ;
    void CONS_SetTxAddressMode(uint8 addressMode) ;
    void CONS_SendBreak(uint8 retMode) ;
    uint8 CONS_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define CONS_PutStringConst         CONS_PutString
    #define CONS_PutArrayConst          CONS_PutArray
    #define CONS_GetTxInterruptSource   CONS_ReadTxStatus

#endif /* End CONS_TX_ENABLED || CONS_HD_ENABLED */

#if(CONS_HD_ENABLED)
    void CONS_LoadRxConfig(void) ;
    void CONS_LoadTxConfig(void) ;
#endif /* End CONS_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_CONS) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    CONS_CyBtldrCommStart(void) CYSMALL ;
    void    CONS_CyBtldrCommStop(void) CYSMALL ;
    void    CONS_CyBtldrCommReset(void) CYSMALL ;
    cystatus CONS_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus CONS_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_CONS)
        #define CyBtldrCommStart    CONS_CyBtldrCommStart
        #define CyBtldrCommStop     CONS_CyBtldrCommStop
        #define CyBtldrCommReset    CONS_CyBtldrCommReset
        #define CyBtldrCommWrite    CONS_CyBtldrCommWrite
        #define CyBtldrCommRead     CONS_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_CONS) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define CONS_BYTE2BYTE_TIME_OUT (25u)
    #define CONS_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define CONS_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define CONS_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define CONS_SET_SPACE      (0x00u)
#define CONS_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (CONS_TX_ENABLED) || (CONS_HD_ENABLED) )
    #if(CONS_TX_INTERRUPT_ENABLED)
        #define CONS_TX_VECT_NUM            (uint8)CONS_TXInternalInterrupt__INTC_NUMBER
        #define CONS_TX_PRIOR_NUM           (uint8)CONS_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* CONS_TX_INTERRUPT_ENABLED */

    #define CONS_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define CONS_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define CONS_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(CONS_TX_ENABLED)
        #define CONS_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (CONS_HD_ENABLED) */
        #define CONS_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (CONS_TX_ENABLED) */

    #define CONS_TX_STS_COMPLETE            (uint8)(0x01u << CONS_TX_STS_COMPLETE_SHIFT)
    #define CONS_TX_STS_FIFO_EMPTY          (uint8)(0x01u << CONS_TX_STS_FIFO_EMPTY_SHIFT)
    #define CONS_TX_STS_FIFO_FULL           (uint8)(0x01u << CONS_TX_STS_FIFO_FULL_SHIFT)
    #define CONS_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << CONS_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (CONS_TX_ENABLED) || (CONS_HD_ENABLED)*/

#if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )
    #if(CONS_RX_INTERRUPT_ENABLED)
        #define CONS_RX_VECT_NUM            (uint8)CONS_RXInternalInterrupt__INTC_NUMBER
        #define CONS_RX_PRIOR_NUM           (uint8)CONS_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* CONS_RX_INTERRUPT_ENABLED */
    #define CONS_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define CONS_RX_STS_BREAK_SHIFT             (0x01u)
    #define CONS_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define CONS_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define CONS_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define CONS_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define CONS_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define CONS_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define CONS_RX_STS_MRKSPC           (uint8)(0x01u << CONS_RX_STS_MRKSPC_SHIFT)
    #define CONS_RX_STS_BREAK            (uint8)(0x01u << CONS_RX_STS_BREAK_SHIFT)
    #define CONS_RX_STS_PAR_ERROR        (uint8)(0x01u << CONS_RX_STS_PAR_ERROR_SHIFT)
    #define CONS_RX_STS_STOP_ERROR       (uint8)(0x01u << CONS_RX_STS_STOP_ERROR_SHIFT)
    #define CONS_RX_STS_OVERRUN          (uint8)(0x01u << CONS_RX_STS_OVERRUN_SHIFT)
    #define CONS_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << CONS_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define CONS_RX_STS_ADDR_MATCH       (uint8)(0x01u << CONS_RX_STS_ADDR_MATCH_SHIFT)
    #define CONS_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << CONS_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define CONS_RX_HW_MASK                     (0x7Fu)
#endif /* End (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */

/* Control Register definitions */
#define CONS_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define CONS_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define CONS_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define CONS_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define CONS_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define CONS_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define CONS_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define CONS_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define CONS_CTRL_HD_SEND               (uint8)(0x01u << CONS_CTRL_HD_SEND_SHIFT)
#define CONS_CTRL_HD_SEND_BREAK         (uint8)(0x01u << CONS_CTRL_HD_SEND_BREAK_SHIFT)
#define CONS_CTRL_MARK                  (uint8)(0x01u << CONS_CTRL_MARK_SHIFT)
#define CONS_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << CONS_CTRL_PARITY_TYPE0_SHIFT)
#define CONS_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << CONS_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define CONS_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define CONS_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define CONS_SEND_BREAK                         (0x00u)
#define CONS_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define CONS_REINIT                             (0x02u)
#define CONS_SEND_WAIT_REINIT                   (0x03u)

#define CONS_OVER_SAMPLE_8                      (8u)
#define CONS_OVER_SAMPLE_16                     (16u)

#define CONS_BIT_CENTER                         (CONS_OVER_SAMPLE_COUNT - 2u)

#define CONS_FIFO_LENGTH                        (4u)
#define CONS_NUMBER_OF_START_BIT                (1u)
#define CONS_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define CONS_TXBITCTR_BREAKBITS8X   ((CONS_BREAK_BITS_TX * CONS_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define CONS_TXBITCTR_BREAKBITS ((CONS_BREAK_BITS_TX * CONS_OVER_SAMPLE_COUNT) - 1u)

#define CONS_HALF_BIT_COUNT   \
                            (((CONS_OVER_SAMPLE_COUNT / 2u) + (CONS_USE23POLLING * 1u)) - 2u)
#if (CONS_OVER_SAMPLE_COUNT == CONS_OVER_SAMPLE_8)
    #define CONS_HD_TXBITCTR_INIT   (((CONS_BREAK_BITS_TX + \
                            CONS_NUMBER_OF_START_BIT) * CONS_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define CONS_RXBITCTR_INIT  ((((CONS_BREAK_BITS_RX + CONS_NUMBER_OF_START_BIT) \
                            * CONS_OVER_SAMPLE_COUNT) + CONS_HALF_BIT_COUNT) - 1u)

#else /* CONS_OVER_SAMPLE_COUNT == CONS_OVER_SAMPLE_16 */
    #define CONS_HD_TXBITCTR_INIT   ((8u * CONS_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define CONS_RXBITCTR_INIT      (((7u * CONS_OVER_SAMPLE_COUNT) - 1u) + \
                                                      CONS_HALF_BIT_COUNT)
#endif /* End CONS_OVER_SAMPLE_COUNT */

#define CONS_HD_RXBITCTR_INIT                   CONS_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 CONS_initVar;
#if (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED)
    extern volatile uint8 CONS_txBuffer[CONS_TX_BUFFER_SIZE];
    extern volatile uint8 CONS_txBufferRead;
    extern uint8 CONS_txBufferWrite;
#endif /* (CONS_TX_INTERRUPT_ENABLED && CONS_TX_ENABLED) */
#if (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED))
    extern uint8 CONS_errorStatus;
    extern volatile uint8 CONS_rxBuffer[CONS_RX_BUFFER_SIZE];
    extern volatile uint8 CONS_rxBufferRead;
    extern volatile uint8 CONS_rxBufferWrite;
    extern volatile uint8 CONS_rxBufferLoopDetect;
    extern volatile uint8 CONS_rxBufferOverflow;
    #if (CONS_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 CONS_rxAddressMode;
        extern volatile uint8 CONS_rxAddressDetected;
    #endif /* (CONS_RXHW_ADDRESS_ENABLED) */
#endif /* (CONS_RX_INTERRUPT_ENABLED && (CONS_RX_ENABLED || CONS_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define CONS__B_UART__AM_SW_BYTE_BYTE 1
#define CONS__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define CONS__B_UART__AM_HW_BYTE_BY_BYTE 3
#define CONS__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define CONS__B_UART__AM_NONE 0

#define CONS__B_UART__NONE_REVB 0
#define CONS__B_UART__EVEN_REVB 1
#define CONS__B_UART__ODD_REVB 2
#define CONS__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define CONS_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define CONS_NUMBER_OF_STOP_BITS    (1u)

#if (CONS_RXHW_ADDRESS_ENABLED)
    #define CONS_RX_ADDRESS_MODE    (0u)
    #define CONS_RX_HW_ADDRESS1     (0u)
    #define CONS_RX_HW_ADDRESS2     (0u)
#endif /* (CONS_RXHW_ADDRESS_ENABLED) */

#define CONS_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((0 << CONS_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << CONS_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << CONS_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << CONS_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << CONS_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << CONS_RX_STS_BREAK_SHIFT) \
                                        | (0 << CONS_RX_STS_OVERRUN_SHIFT))

#define CONS_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << CONS_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << CONS_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << CONS_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << CONS_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define CONS_CONTROL_REG \
                            (* (reg8 *) CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define CONS_CONTROL_PTR \
                            (  (reg8 *) CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(CONS_TX_ENABLED)
    #define CONS_TXDATA_REG          (* (reg8 *) CONS_BUART_sTX_TxShifter_u0__F0_REG)
    #define CONS_TXDATA_PTR          (  (reg8 *) CONS_BUART_sTX_TxShifter_u0__F0_REG)
    #define CONS_TXDATA_AUX_CTL_REG  (* (reg8 *) CONS_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define CONS_TXDATA_AUX_CTL_PTR  (  (reg8 *) CONS_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define CONS_TXSTATUS_REG        (* (reg8 *) CONS_BUART_sTX_TxSts__STATUS_REG)
    #define CONS_TXSTATUS_PTR        (  (reg8 *) CONS_BUART_sTX_TxSts__STATUS_REG)
    #define CONS_TXSTATUS_MASK_REG   (* (reg8 *) CONS_BUART_sTX_TxSts__MASK_REG)
    #define CONS_TXSTATUS_MASK_PTR   (  (reg8 *) CONS_BUART_sTX_TxSts__MASK_REG)
    #define CONS_TXSTATUS_ACTL_REG   (* (reg8 *) CONS_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define CONS_TXSTATUS_ACTL_PTR   (  (reg8 *) CONS_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(CONS_TXCLKGEN_DP)
        #define CONS_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define CONS_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define CONS_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define CONS_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define CONS_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define CONS_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define CONS_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define CONS_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define CONS_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define CONS_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) CONS_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* CONS_TXCLKGEN_DP */

#endif /* End CONS_TX_ENABLED */

#if(CONS_HD_ENABLED)

    #define CONS_TXDATA_REG             (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__F1_REG )
    #define CONS_TXDATA_PTR             (  (reg8 *) CONS_BUART_sRX_RxShifter_u0__F1_REG )
    #define CONS_TXDATA_AUX_CTL_REG     (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define CONS_TXDATA_AUX_CTL_PTR     (  (reg8 *) CONS_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define CONS_TXSTATUS_REG           (* (reg8 *) CONS_BUART_sRX_RxSts__STATUS_REG )
    #define CONS_TXSTATUS_PTR           (  (reg8 *) CONS_BUART_sRX_RxSts__STATUS_REG )
    #define CONS_TXSTATUS_MASK_REG      (* (reg8 *) CONS_BUART_sRX_RxSts__MASK_REG )
    #define CONS_TXSTATUS_MASK_PTR      (  (reg8 *) CONS_BUART_sRX_RxSts__MASK_REG )
    #define CONS_TXSTATUS_ACTL_REG      (* (reg8 *) CONS_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define CONS_TXSTATUS_ACTL_PTR      (  (reg8 *) CONS_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End CONS_HD_ENABLED */

#if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )
    #define CONS_RXDATA_REG             (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__F0_REG )
    #define CONS_RXDATA_PTR             (  (reg8 *) CONS_BUART_sRX_RxShifter_u0__F0_REG )
    #define CONS_RXADDRESS1_REG         (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__D0_REG )
    #define CONS_RXADDRESS1_PTR         (  (reg8 *) CONS_BUART_sRX_RxShifter_u0__D0_REG )
    #define CONS_RXADDRESS2_REG         (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__D1_REG )
    #define CONS_RXADDRESS2_PTR         (  (reg8 *) CONS_BUART_sRX_RxShifter_u0__D1_REG )
    #define CONS_RXDATA_AUX_CTL_REG     (* (reg8 *) CONS_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define CONS_RXBITCTR_PERIOD_REG    (* (reg8 *) CONS_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define CONS_RXBITCTR_PERIOD_PTR    (  (reg8 *) CONS_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define CONS_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) CONS_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define CONS_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) CONS_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define CONS_RXBITCTR_COUNTER_REG   (* (reg8 *) CONS_BUART_sRX_RxBitCounter__COUNT_REG )
    #define CONS_RXBITCTR_COUNTER_PTR   (  (reg8 *) CONS_BUART_sRX_RxBitCounter__COUNT_REG )

    #define CONS_RXSTATUS_REG           (* (reg8 *) CONS_BUART_sRX_RxSts__STATUS_REG )
    #define CONS_RXSTATUS_PTR           (  (reg8 *) CONS_BUART_sRX_RxSts__STATUS_REG )
    #define CONS_RXSTATUS_MASK_REG      (* (reg8 *) CONS_BUART_sRX_RxSts__MASK_REG )
    #define CONS_RXSTATUS_MASK_PTR      (  (reg8 *) CONS_BUART_sRX_RxSts__MASK_REG )
    #define CONS_RXSTATUS_ACTL_REG      (* (reg8 *) CONS_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define CONS_RXSTATUS_ACTL_PTR      (  (reg8 *) CONS_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */

#if(CONS_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define CONS_INTCLOCK_CLKEN_REG     (* (reg8 *) CONS_IntClock__PM_ACT_CFG)
    #define CONS_INTCLOCK_CLKEN_PTR     (  (reg8 *) CONS_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define CONS_INTCLOCK_CLKEN_MASK    CONS_IntClock__PM_ACT_MSK
#endif /* End CONS_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(CONS_TX_ENABLED)
    #define CONS_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End CONS_TX_ENABLED */

#if(CONS_HD_ENABLED)
    #define CONS_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End CONS_HD_ENABLED */

#if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )
    #define CONS_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define CONS_WAIT_1_MS      CONS_BL_CHK_DELAY_MS   

#define CONS_TXBUFFERSIZE   CONS_TX_BUFFER_SIZE
#define CONS_RXBUFFERSIZE   CONS_RX_BUFFER_SIZE

#if (CONS_RXHW_ADDRESS_ENABLED)
    #define CONS_RXADDRESSMODE  CONS_RX_ADDRESS_MODE
    #define CONS_RXHWADDRESS1   CONS_RX_HW_ADDRESS1
    #define CONS_RXHWADDRESS2   CONS_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define CONS_RXAddressMode  CONS_RXADDRESSMODE
#endif /* (CONS_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define CONS_initvar                    CONS_initVar

#define CONS_RX_Enabled                 CONS_RX_ENABLED
#define CONS_TX_Enabled                 CONS_TX_ENABLED
#define CONS_HD_Enabled                 CONS_HD_ENABLED
#define CONS_RX_IntInterruptEnabled     CONS_RX_INTERRUPT_ENABLED
#define CONS_TX_IntInterruptEnabled     CONS_TX_INTERRUPT_ENABLED
#define CONS_InternalClockUsed          CONS_INTERNAL_CLOCK_USED
#define CONS_RXHW_Address_Enabled       CONS_RXHW_ADDRESS_ENABLED
#define CONS_OverSampleCount            CONS_OVER_SAMPLE_COUNT
#define CONS_ParityType                 CONS_PARITY_TYPE

#if( CONS_TX_ENABLED && (CONS_TXBUFFERSIZE > CONS_FIFO_LENGTH))
    #define CONS_TXBUFFER               CONS_txBuffer
    #define CONS_TXBUFFERREAD           CONS_txBufferRead
    #define CONS_TXBUFFERWRITE          CONS_txBufferWrite
#endif /* End CONS_TX_ENABLED */
#if( ( CONS_RX_ENABLED || CONS_HD_ENABLED ) && \
     (CONS_RXBUFFERSIZE > CONS_FIFO_LENGTH) )
    #define CONS_RXBUFFER               CONS_rxBuffer
    #define CONS_RXBUFFERREAD           CONS_rxBufferRead
    #define CONS_RXBUFFERWRITE          CONS_rxBufferWrite
    #define CONS_RXBUFFERLOOPDETECT     CONS_rxBufferLoopDetect
    #define CONS_RXBUFFER_OVERFLOW      CONS_rxBufferOverflow
#endif /* End CONS_RX_ENABLED */

#ifdef CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define CONS_CONTROL                CONS_CONTROL_REG
#endif /* End CONS_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(CONS_TX_ENABLED)
    #define CONS_TXDATA                 CONS_TXDATA_REG
    #define CONS_TXSTATUS               CONS_TXSTATUS_REG
    #define CONS_TXSTATUS_MASK          CONS_TXSTATUS_MASK_REG
    #define CONS_TXSTATUS_ACTL          CONS_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(CONS_TXCLKGEN_DP)
        #define CONS_TXBITCLKGEN_CTR        CONS_TXBITCLKGEN_CTR_REG
        #define CONS_TXBITCLKTX_COMPLETE    CONS_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define CONS_TXBITCTR_PERIOD        CONS_TXBITCTR_PERIOD_REG
        #define CONS_TXBITCTR_CONTROL       CONS_TXBITCTR_CONTROL_REG
        #define CONS_TXBITCTR_COUNTER       CONS_TXBITCTR_COUNTER_REG
    #endif /* CONS_TXCLKGEN_DP */
#endif /* End CONS_TX_ENABLED */

#if(CONS_HD_ENABLED)
    #define CONS_TXDATA                 CONS_TXDATA_REG
    #define CONS_TXSTATUS               CONS_TXSTATUS_REG
    #define CONS_TXSTATUS_MASK          CONS_TXSTATUS_MASK_REG
    #define CONS_TXSTATUS_ACTL          CONS_TXSTATUS_ACTL_REG
#endif /* End CONS_HD_ENABLED */

#if( (CONS_RX_ENABLED) || (CONS_HD_ENABLED) )
    #define CONS_RXDATA                 CONS_RXDATA_REG
    #define CONS_RXADDRESS1             CONS_RXADDRESS1_REG
    #define CONS_RXADDRESS2             CONS_RXADDRESS2_REG
    #define CONS_RXBITCTR_PERIOD        CONS_RXBITCTR_PERIOD_REG
    #define CONS_RXBITCTR_CONTROL       CONS_RXBITCTR_CONTROL_REG
    #define CONS_RXBITCTR_COUNTER       CONS_RXBITCTR_COUNTER_REG
    #define CONS_RXSTATUS               CONS_RXSTATUS_REG
    #define CONS_RXSTATUS_MASK          CONS_RXSTATUS_MASK_REG
    #define CONS_RXSTATUS_ACTL          CONS_RXSTATUS_ACTL_REG
#endif /* End  (CONS_RX_ENABLED) || (CONS_HD_ENABLED) */

#if(CONS_INTERNAL_CLOCK_USED)
    #define CONS_INTCLOCK_CLKEN         CONS_INTCLOCK_CLKEN_REG
#endif /* End CONS_INTERNAL_CLOCK_USED */

#define CONS_WAIT_FOR_COMLETE_REINIT    CONS_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_CONS_H */


/* [] END OF FILE */
