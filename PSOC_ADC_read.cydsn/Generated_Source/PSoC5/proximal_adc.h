/*******************************************************************************
* File Name: proximal_adc.h  
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

#if !defined(CY_PINS_proximal_adc_H) /* Pins proximal_adc_H */
#define CY_PINS_proximal_adc_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "proximal_adc_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 proximal_adc__PORT == 15 && ((proximal_adc__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    proximal_adc_Write(uint8 value);
void    proximal_adc_SetDriveMode(uint8 mode);
uint8   proximal_adc_ReadDataReg(void);
uint8   proximal_adc_Read(void);
void    proximal_adc_SetInterruptMode(uint16 position, uint16 mode);
uint8   proximal_adc_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the proximal_adc_SetDriveMode() function.
     *  @{
     */
        #define proximal_adc_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define proximal_adc_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define proximal_adc_DM_RES_UP          PIN_DM_RES_UP
        #define proximal_adc_DM_RES_DWN         PIN_DM_RES_DWN
        #define proximal_adc_DM_OD_LO           PIN_DM_OD_LO
        #define proximal_adc_DM_OD_HI           PIN_DM_OD_HI
        #define proximal_adc_DM_STRONG          PIN_DM_STRONG
        #define proximal_adc_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define proximal_adc_MASK               proximal_adc__MASK
#define proximal_adc_SHIFT              proximal_adc__SHIFT
#define proximal_adc_WIDTH              1u

/* Interrupt constants */
#if defined(proximal_adc__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in proximal_adc_SetInterruptMode() function.
     *  @{
     */
        #define proximal_adc_INTR_NONE      (uint16)(0x0000u)
        #define proximal_adc_INTR_RISING    (uint16)(0x0001u)
        #define proximal_adc_INTR_FALLING   (uint16)(0x0002u)
        #define proximal_adc_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define proximal_adc_INTR_MASK      (0x01u) 
#endif /* (proximal_adc__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define proximal_adc_PS                     (* (reg8 *) proximal_adc__PS)
/* Data Register */
#define proximal_adc_DR                     (* (reg8 *) proximal_adc__DR)
/* Port Number */
#define proximal_adc_PRT_NUM                (* (reg8 *) proximal_adc__PRT) 
/* Connect to Analog Globals */                                                  
#define proximal_adc_AG                     (* (reg8 *) proximal_adc__AG)                       
/* Analog MUX bux enable */
#define proximal_adc_AMUX                   (* (reg8 *) proximal_adc__AMUX) 
/* Bidirectional Enable */                                                        
#define proximal_adc_BIE                    (* (reg8 *) proximal_adc__BIE)
/* Bit-mask for Aliased Register Access */
#define proximal_adc_BIT_MASK               (* (reg8 *) proximal_adc__BIT_MASK)
/* Bypass Enable */
#define proximal_adc_BYP                    (* (reg8 *) proximal_adc__BYP)
/* Port wide control signals */                                                   
#define proximal_adc_CTL                    (* (reg8 *) proximal_adc__CTL)
/* Drive Modes */
#define proximal_adc_DM0                    (* (reg8 *) proximal_adc__DM0) 
#define proximal_adc_DM1                    (* (reg8 *) proximal_adc__DM1)
#define proximal_adc_DM2                    (* (reg8 *) proximal_adc__DM2) 
/* Input Buffer Disable Override */
#define proximal_adc_INP_DIS                (* (reg8 *) proximal_adc__INP_DIS)
/* LCD Common or Segment Drive */
#define proximal_adc_LCD_COM_SEG            (* (reg8 *) proximal_adc__LCD_COM_SEG)
/* Enable Segment LCD */
#define proximal_adc_LCD_EN                 (* (reg8 *) proximal_adc__LCD_EN)
/* Slew Rate Control */
#define proximal_adc_SLW                    (* (reg8 *) proximal_adc__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define proximal_adc_PRTDSI__CAPS_SEL       (* (reg8 *) proximal_adc__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define proximal_adc_PRTDSI__DBL_SYNC_IN    (* (reg8 *) proximal_adc__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define proximal_adc_PRTDSI__OE_SEL0        (* (reg8 *) proximal_adc__PRTDSI__OE_SEL0) 
#define proximal_adc_PRTDSI__OE_SEL1        (* (reg8 *) proximal_adc__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define proximal_adc_PRTDSI__OUT_SEL0       (* (reg8 *) proximal_adc__PRTDSI__OUT_SEL0) 
#define proximal_adc_PRTDSI__OUT_SEL1       (* (reg8 *) proximal_adc__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define proximal_adc_PRTDSI__SYNC_OUT       (* (reg8 *) proximal_adc__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(proximal_adc__SIO_CFG)
    #define proximal_adc_SIO_HYST_EN        (* (reg8 *) proximal_adc__SIO_HYST_EN)
    #define proximal_adc_SIO_REG_HIFREQ     (* (reg8 *) proximal_adc__SIO_REG_HIFREQ)
    #define proximal_adc_SIO_CFG            (* (reg8 *) proximal_adc__SIO_CFG)
    #define proximal_adc_SIO_DIFF           (* (reg8 *) proximal_adc__SIO_DIFF)
#endif /* (proximal_adc__SIO_CFG) */

/* Interrupt Registers */
#if defined(proximal_adc__INTSTAT)
    #define proximal_adc_INTSTAT            (* (reg8 *) proximal_adc__INTSTAT)
    #define proximal_adc_SNAP               (* (reg8 *) proximal_adc__SNAP)
    
	#define proximal_adc_0_INTTYPE_REG 		(* (reg8 *) proximal_adc__0__INTTYPE)
#endif /* (proximal_adc__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_proximal_adc_H */


/* [] END OF FILE */
