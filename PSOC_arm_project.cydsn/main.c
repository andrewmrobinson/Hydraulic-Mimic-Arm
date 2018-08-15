
#include <project.h>
#include <stdio.h>
#include "common.h"
#include <math.h>

#define	Frequency 1000000.0			// Frequency of PWMClock
#define	One_ms (Frequency/1000.0)	// 1 ms constant

/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

uint8 errorStatus = 0u;

uint16 dutycyclelength(int angle)
{
    uint16 pulselength=(uint16)One_ms + ((angle+45.0)/90.0) * One_ms;
    return pulselength;
}

/*******************************************************************************
* Function Name: RxIsr
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
*******************************************************************************/
int angle_read_mode = 0;
volatile int new_angle = 0;
volatile int new_angle_set = 0;
int angle = 0;
int isNegative=0;
char sendValue[12];
int16 adcValue1;
int16 adcValue2;

CY_ISR(RxIsr)
{
    uint8 rxStatus;         
    uint8 rxData;           
    
    do
    {
        /* Read receiver status register */
        rxStatus = UART_RXSTATUS_REG;

        if((rxStatus & (UART_RX_STS_BREAK      | UART_RX_STS_PAR_ERROR |
                        UART_RX_STS_STOP_ERROR | UART_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            errorStatus |= rxStatus & ( UART_RX_STS_BREAK      | UART_RX_STS_PAR_ERROR | 
                                        UART_RX_STS_STOP_ERROR | UART_RX_STS_OVERRUN);
        }
        
        if((rxStatus & UART_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read data from the RX data register */
            rxData = UART_RXDATA_REG;
            
            switch(angle_read_mode)
            {
                case 0:
                    if(rxData=='{' )
                    {
                        new_angle = 0;
                        angle_read_mode++;
                    }
                break;
                case 1:
                    if(rxData<=0x39 & rxData>=0x30)
                    {
                        if(new_angle_set==0)
                            new_angle = new_angle*10 + (rxData-48);
                    }
                    else if(rxData == '-')
                    {
                        isNegative = 1;
                    }
                    else if(rxData=='}')
                    {
                        if(isNegative){new_angle = -new_angle;}
                        if(new_angle_set==0)
                            new_angle_set = 1;
                        isNegative = 0;
                        //adcValue = ADC_SAR_1_GetResult16() ;
                        //sprintf(sendValue,"%08d",adcValue);
                        UART_PutString(sendValue);
                        angle_read_mode=0;
                    }
                    else 
                    {
                        new_angle = 0;
                        angle_read_mode=0;
                        isNegative = 0;
                    }
                break;
            }
            
            if(errorStatus == 0u)
            {
                /* Send data backward */
                //UART_TXDATA_REG = rxData;
            }
        }
    }while((rxStatus & UART_RX_STS_FIFO_NOTEMPTY) != 0u);

}
    

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Theory:
*  The function starts UART and interrupt components.
*
*******************************************************************************/
int led_switch = 0;
int main()
{
    
    
    //PWM variables

    PWM_1_Start();
    ADC_SAR_1_Start(); 
    ADC_SAR_1_StartConvert(); 
    ADC_SAR_2_Start(); 
    ADC_SAR_2_StartConvert(); 
    
    uint8 button = 0u;
    uint8 buttonPre = 0u;
    
    LED_Write(LED_OFF);     /* Clear LED */

    UART_Start();           /* Start communication component */

#if(INTERRUPT_CODE_ENABLED == ENABLED)
    isr_rx_StartEx(RxIsr);
#endif /* INTERRUPT_CODE_ENABLED == ENABLED */
    
    CyGlobalIntEnable;      /* Enable global interrupts. */
    

    angle = -89;
    int up = 1;
    int cycle = 1000;

    for(;;)
    {
        
        adcValue1 = ADC_SAR_1_GetResult16() ;
        adcValue2 = ADC_SAR_2_GetResult16() ;
        sprintf(sendValue,"%04d \t %04d \n",adcValue1,adcValue2);
        UART_PutString(sendValue);
        
        //angle = ((float)adcValue/4096.00)*90.00 - 45.00;
        //PWM_1_WriteCompare(dutycyclelength(angle));
        
        //PWM_1_WriteCompare(dutycyclelength(0));
        /*
        if(new_angle_set){
            angle = new_angle;
            new_angle_set = 0;
            new_angle = 0;
            
            if(angle<46 & angle>-46){
                PWM_1_WriteCompare(dutycyclelength(angle));
            }
        }
        */
        
        //PWM_1_WriteCompare(dutycyclelength(angle))
        CyDelay(100);
        
        
        /***********************************************************************
        * Handle SW2 press. 
        ***********************************************************************/
        button = SW2_Read();
        if((button == 0u) && (buttonPre != 0u))
        {
            if(led_switch)
            {
                LED_Write(LED_OFF);     /* Clear LED */
                led_switch--;
            }
            else
            {
                LED_Write(LED_ON);     /* Clear LED */
                led_switch++;
            }
        }
        buttonPre = button;
        
       
    }
}

/* [] END OF FILE */
