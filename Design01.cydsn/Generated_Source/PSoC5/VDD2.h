/*******************************************************************************
* File Name: VDD2.h  
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

#if !defined(CY_PINS_VDD2_H) /* Pins VDD2_H */
#define CY_PINS_VDD2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VDD2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VDD2__PORT == 15 && ((VDD2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    VDD2_Write(uint8 value);
void    VDD2_SetDriveMode(uint8 mode);
uint8   VDD2_ReadDataReg(void);
uint8   VDD2_Read(void);
void    VDD2_SetInterruptMode(uint16 position, uint16 mode);
uint8   VDD2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the VDD2_SetDriveMode() function.
     *  @{
     */
        #define VDD2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define VDD2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define VDD2_DM_RES_UP          PIN_DM_RES_UP
        #define VDD2_DM_RES_DWN         PIN_DM_RES_DWN
        #define VDD2_DM_OD_LO           PIN_DM_OD_LO
        #define VDD2_DM_OD_HI           PIN_DM_OD_HI
        #define VDD2_DM_STRONG          PIN_DM_STRONG
        #define VDD2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define VDD2_MASK               VDD2__MASK
#define VDD2_SHIFT              VDD2__SHIFT
#define VDD2_WIDTH              1u

/* Interrupt constants */
#if defined(VDD2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VDD2_SetInterruptMode() function.
     *  @{
     */
        #define VDD2_INTR_NONE      (uint16)(0x0000u)
        #define VDD2_INTR_RISING    (uint16)(0x0001u)
        #define VDD2_INTR_FALLING   (uint16)(0x0002u)
        #define VDD2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define VDD2_INTR_MASK      (0x01u) 
#endif /* (VDD2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VDD2_PS                     (* (reg8 *) VDD2__PS)
/* Data Register */
#define VDD2_DR                     (* (reg8 *) VDD2__DR)
/* Port Number */
#define VDD2_PRT_NUM                (* (reg8 *) VDD2__PRT) 
/* Connect to Analog Globals */                                                  
#define VDD2_AG                     (* (reg8 *) VDD2__AG)                       
/* Analog MUX bux enable */
#define VDD2_AMUX                   (* (reg8 *) VDD2__AMUX) 
/* Bidirectional Enable */                                                        
#define VDD2_BIE                    (* (reg8 *) VDD2__BIE)
/* Bit-mask for Aliased Register Access */
#define VDD2_BIT_MASK               (* (reg8 *) VDD2__BIT_MASK)
/* Bypass Enable */
#define VDD2_BYP                    (* (reg8 *) VDD2__BYP)
/* Port wide control signals */                                                   
#define VDD2_CTL                    (* (reg8 *) VDD2__CTL)
/* Drive Modes */
#define VDD2_DM0                    (* (reg8 *) VDD2__DM0) 
#define VDD2_DM1                    (* (reg8 *) VDD2__DM1)
#define VDD2_DM2                    (* (reg8 *) VDD2__DM2) 
/* Input Buffer Disable Override */
#define VDD2_INP_DIS                (* (reg8 *) VDD2__INP_DIS)
/* LCD Common or Segment Drive */
#define VDD2_LCD_COM_SEG            (* (reg8 *) VDD2__LCD_COM_SEG)
/* Enable Segment LCD */
#define VDD2_LCD_EN                 (* (reg8 *) VDD2__LCD_EN)
/* Slew Rate Control */
#define VDD2_SLW                    (* (reg8 *) VDD2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VDD2_PRTDSI__CAPS_SEL       (* (reg8 *) VDD2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VDD2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VDD2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VDD2_PRTDSI__OE_SEL0        (* (reg8 *) VDD2__PRTDSI__OE_SEL0) 
#define VDD2_PRTDSI__OE_SEL1        (* (reg8 *) VDD2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VDD2_PRTDSI__OUT_SEL0       (* (reg8 *) VDD2__PRTDSI__OUT_SEL0) 
#define VDD2_PRTDSI__OUT_SEL1       (* (reg8 *) VDD2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VDD2_PRTDSI__SYNC_OUT       (* (reg8 *) VDD2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(VDD2__SIO_CFG)
    #define VDD2_SIO_HYST_EN        (* (reg8 *) VDD2__SIO_HYST_EN)
    #define VDD2_SIO_REG_HIFREQ     (* (reg8 *) VDD2__SIO_REG_HIFREQ)
    #define VDD2_SIO_CFG            (* (reg8 *) VDD2__SIO_CFG)
    #define VDD2_SIO_DIFF           (* (reg8 *) VDD2__SIO_DIFF)
#endif /* (VDD2__SIO_CFG) */

/* Interrupt Registers */
#if defined(VDD2__INTSTAT)
    #define VDD2_INTSTAT            (* (reg8 *) VDD2__INTSTAT)
    #define VDD2_SNAP               (* (reg8 *) VDD2__SNAP)
    
	#define VDD2_0_INTTYPE_REG 		(* (reg8 *) VDD2__0__INTTYPE)
#endif /* (VDD2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VDD2_H */


/* [] END OF FILE */
