/*******************************************************************************
* File Name: VDD1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_VDD1_H) /* Pins VDD1_H */
#define CY_PINS_VDD1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VDD1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VDD1__PORT == 15 && ((VDD1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    VDD1_Write(uint8 value);
void    VDD1_SetDriveMode(uint8 mode);
uint8   VDD1_ReadDataReg(void);
uint8   VDD1_Read(void);
void    VDD1_SetInterruptMode(uint16 position, uint16 mode);
uint8   VDD1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the VDD1_SetDriveMode() function.
     *  @{
     */
        #define VDD1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define VDD1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define VDD1_DM_RES_UP          PIN_DM_RES_UP
        #define VDD1_DM_RES_DWN         PIN_DM_RES_DWN
        #define VDD1_DM_OD_LO           PIN_DM_OD_LO
        #define VDD1_DM_OD_HI           PIN_DM_OD_HI
        #define VDD1_DM_STRONG          PIN_DM_STRONG
        #define VDD1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define VDD1_MASK               VDD1__MASK
#define VDD1_SHIFT              VDD1__SHIFT
#define VDD1_WIDTH              1u

/* Interrupt constants */
#if defined(VDD1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VDD1_SetInterruptMode() function.
     *  @{
     */
        #define VDD1_INTR_NONE      (uint16)(0x0000u)
        #define VDD1_INTR_RISING    (uint16)(0x0001u)
        #define VDD1_INTR_FALLING   (uint16)(0x0002u)
        #define VDD1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define VDD1_INTR_MASK      (0x01u) 
#endif /* (VDD1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VDD1_PS                     (* (reg8 *) VDD1__PS)
/* Data Register */
#define VDD1_DR                     (* (reg8 *) VDD1__DR)
/* Port Number */
#define VDD1_PRT_NUM                (* (reg8 *) VDD1__PRT) 
/* Connect to Analog Globals */                                                  
#define VDD1_AG                     (* (reg8 *) VDD1__AG)                       
/* Analog MUX bux enable */
#define VDD1_AMUX                   (* (reg8 *) VDD1__AMUX) 
/* Bidirectional Enable */                                                        
#define VDD1_BIE                    (* (reg8 *) VDD1__BIE)
/* Bit-mask for Aliased Register Access */
#define VDD1_BIT_MASK               (* (reg8 *) VDD1__BIT_MASK)
/* Bypass Enable */
#define VDD1_BYP                    (* (reg8 *) VDD1__BYP)
/* Port wide control signals */                                                   
#define VDD1_CTL                    (* (reg8 *) VDD1__CTL)
/* Drive Modes */
#define VDD1_DM0                    (* (reg8 *) VDD1__DM0) 
#define VDD1_DM1                    (* (reg8 *) VDD1__DM1)
#define VDD1_DM2                    (* (reg8 *) VDD1__DM2) 
/* Input Buffer Disable Override */
#define VDD1_INP_DIS                (* (reg8 *) VDD1__INP_DIS)
/* LCD Common or Segment Drive */
#define VDD1_LCD_COM_SEG            (* (reg8 *) VDD1__LCD_COM_SEG)
/* Enable Segment LCD */
#define VDD1_LCD_EN                 (* (reg8 *) VDD1__LCD_EN)
/* Slew Rate Control */
#define VDD1_SLW                    (* (reg8 *) VDD1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VDD1_PRTDSI__CAPS_SEL       (* (reg8 *) VDD1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VDD1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VDD1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VDD1_PRTDSI__OE_SEL0        (* (reg8 *) VDD1__PRTDSI__OE_SEL0) 
#define VDD1_PRTDSI__OE_SEL1        (* (reg8 *) VDD1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VDD1_PRTDSI__OUT_SEL0       (* (reg8 *) VDD1__PRTDSI__OUT_SEL0) 
#define VDD1_PRTDSI__OUT_SEL1       (* (reg8 *) VDD1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VDD1_PRTDSI__SYNC_OUT       (* (reg8 *) VDD1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(VDD1__SIO_CFG)
    #define VDD1_SIO_HYST_EN        (* (reg8 *) VDD1__SIO_HYST_EN)
    #define VDD1_SIO_REG_HIFREQ     (* (reg8 *) VDD1__SIO_REG_HIFREQ)
    #define VDD1_SIO_CFG            (* (reg8 *) VDD1__SIO_CFG)
    #define VDD1_SIO_DIFF           (* (reg8 *) VDD1__SIO_DIFF)
#endif /* (VDD1__SIO_CFG) */

/* Interrupt Registers */
#if defined(VDD1__INTSTAT)
    #define VDD1_INTSTAT            (* (reg8 *) VDD1__INTSTAT)
    #define VDD1_SNAP               (* (reg8 *) VDD1__SNAP)
    
	#define VDD1_0_INTTYPE_REG 		(* (reg8 *) VDD1__0__INTTYPE)
#endif /* (VDD1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VDD1_H */


/* [] END OF FILE */
