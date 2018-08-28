/*******************************************************************************
* File Name: pwm1.h  
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

#if !defined(CY_PINS_pwm1_H) /* Pins pwm1_H */
#define CY_PINS_pwm1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "pwm1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 pwm1__PORT == 15 && ((pwm1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    pwm1_Write(uint8 value);
void    pwm1_SetDriveMode(uint8 mode);
uint8   pwm1_ReadDataReg(void);
uint8   pwm1_Read(void);
void    pwm1_SetInterruptMode(uint16 position, uint16 mode);
uint8   pwm1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the pwm1_SetDriveMode() function.
     *  @{
     */
        #define pwm1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define pwm1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define pwm1_DM_RES_UP          PIN_DM_RES_UP
        #define pwm1_DM_RES_DWN         PIN_DM_RES_DWN
        #define pwm1_DM_OD_LO           PIN_DM_OD_LO
        #define pwm1_DM_OD_HI           PIN_DM_OD_HI
        #define pwm1_DM_STRONG          PIN_DM_STRONG
        #define pwm1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define pwm1_MASK               pwm1__MASK
#define pwm1_SHIFT              pwm1__SHIFT
#define pwm1_WIDTH              1u

/* Interrupt constants */
#if defined(pwm1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in pwm1_SetInterruptMode() function.
     *  @{
     */
        #define pwm1_INTR_NONE      (uint16)(0x0000u)
        #define pwm1_INTR_RISING    (uint16)(0x0001u)
        #define pwm1_INTR_FALLING   (uint16)(0x0002u)
        #define pwm1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define pwm1_INTR_MASK      (0x01u) 
#endif /* (pwm1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define pwm1_PS                     (* (reg8 *) pwm1__PS)
/* Data Register */
#define pwm1_DR                     (* (reg8 *) pwm1__DR)
/* Port Number */
#define pwm1_PRT_NUM                (* (reg8 *) pwm1__PRT) 
/* Connect to Analog Globals */                                                  
#define pwm1_AG                     (* (reg8 *) pwm1__AG)                       
/* Analog MUX bux enable */
#define pwm1_AMUX                   (* (reg8 *) pwm1__AMUX) 
/* Bidirectional Enable */                                                        
#define pwm1_BIE                    (* (reg8 *) pwm1__BIE)
/* Bit-mask for Aliased Register Access */
#define pwm1_BIT_MASK               (* (reg8 *) pwm1__BIT_MASK)
/* Bypass Enable */
#define pwm1_BYP                    (* (reg8 *) pwm1__BYP)
/* Port wide control signals */                                                   
#define pwm1_CTL                    (* (reg8 *) pwm1__CTL)
/* Drive Modes */
#define pwm1_DM0                    (* (reg8 *) pwm1__DM0) 
#define pwm1_DM1                    (* (reg8 *) pwm1__DM1)
#define pwm1_DM2                    (* (reg8 *) pwm1__DM2) 
/* Input Buffer Disable Override */
#define pwm1_INP_DIS                (* (reg8 *) pwm1__INP_DIS)
/* LCD Common or Segment Drive */
#define pwm1_LCD_COM_SEG            (* (reg8 *) pwm1__LCD_COM_SEG)
/* Enable Segment LCD */
#define pwm1_LCD_EN                 (* (reg8 *) pwm1__LCD_EN)
/* Slew Rate Control */
#define pwm1_SLW                    (* (reg8 *) pwm1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define pwm1_PRTDSI__CAPS_SEL       (* (reg8 *) pwm1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define pwm1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) pwm1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define pwm1_PRTDSI__OE_SEL0        (* (reg8 *) pwm1__PRTDSI__OE_SEL0) 
#define pwm1_PRTDSI__OE_SEL1        (* (reg8 *) pwm1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define pwm1_PRTDSI__OUT_SEL0       (* (reg8 *) pwm1__PRTDSI__OUT_SEL0) 
#define pwm1_PRTDSI__OUT_SEL1       (* (reg8 *) pwm1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define pwm1_PRTDSI__SYNC_OUT       (* (reg8 *) pwm1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(pwm1__SIO_CFG)
    #define pwm1_SIO_HYST_EN        (* (reg8 *) pwm1__SIO_HYST_EN)
    #define pwm1_SIO_REG_HIFREQ     (* (reg8 *) pwm1__SIO_REG_HIFREQ)
    #define pwm1_SIO_CFG            (* (reg8 *) pwm1__SIO_CFG)
    #define pwm1_SIO_DIFF           (* (reg8 *) pwm1__SIO_DIFF)
#endif /* (pwm1__SIO_CFG) */

/* Interrupt Registers */
#if defined(pwm1__INTSTAT)
    #define pwm1_INTSTAT            (* (reg8 *) pwm1__INTSTAT)
    #define pwm1_SNAP               (* (reg8 *) pwm1__SNAP)
    
	#define pwm1_0_INTTYPE_REG 		(* (reg8 *) pwm1__0__INTTYPE)
#endif /* (pwm1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_pwm1_H */


/* [] END OF FILE */
