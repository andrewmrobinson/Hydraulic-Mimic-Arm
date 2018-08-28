
#include <project.h>
#include <stdio.h>
#include "common.h"
#include <math.h>
#include <stdlib.h>     /* strtod */

#define	Frequency 1000000.0			// Frequency of PWMClock
#define	One_ms (Frequency/1000.0)	// 1 ms constant

/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

uint8 errorStatus = 0u;


char sendValue[100];
uint16 proximal_adc;
uint16 mid_adc;

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

            if(errorStatus == 0u)
            {
                /* Send data backward */
                //UART_TXDATA_REG = rxData;
            }
        }
    }while((rxStatus & UART_RX_STS_FIFO_NOTEMPTY) != 0u);

}
    
int main()
{
    ADC_SAR_1_Start(); 
    ADC_SAR_2_Start(); 
    UART_Start();

    #if(INTERRUPT_CODE_ENABLED == ENABLED)
        isr_rx_StartEx(RxIsr);
    #endif /* INTERRUPT_CODE_ENABLED == ENABLED */
        
    CyGlobalIntEnable;      /* Enable global interrupts. */
    
    //Send headers
    sprintf(sendValue,"Prox\tMid\n");
    UART_PutString(sendValue);
    
    for(;;)
    {
        
        ADC_SAR_1_StartConvert();
        ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        ADC_SAR_2_StartConvert();
        ADC_SAR_2_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        
        proximal_adc = ADC_SAR_1_GetResult16();
        mid_adc = ADC_SAR_2_GetResult16();
        sprintf(sendValue,"%08.0d\t%08.0d\n",proximal_adc,mid_adc);
        UART_PutString(sendValue);

        CyDelay(250);
    }
}

