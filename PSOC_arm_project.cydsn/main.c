
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
int data_read_mode = 0;
volatile int new_angle = 0;
volatile int new_pos_set = 0;
int angle = 0;
int isNegative=0;
char sendValue[100];
uint16 adcValue1;
uint16 adcValue2;
char temp[6];
int nn=0;
double pid[3] = {1,0,0};
char help[1000];
int tt = 0;
int pos=1600;
int new_pos;
double err;
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
            help[tt] = rxData;
            tt++;
            
            switch(data_read_mode)
            {
                case 0:
                    if(rxData=='{' )
                    {
                        new_angle = 0;
                        data_read_mode++;
                    }
                break;
                case 1:
                    if(rxData=='x'){
                        data_read_mode = 2;}
                    else if(rxData=='p'){
                        data_read_mode = 3;}
                    else if(rxData=='i'){
                        data_read_mode = 4;}
                    else if(rxData=='d'){
                        data_read_mode = 5;}
                break;
                case 2:
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        new_pos = (int) strtol(temp, (char **)NULL, 10);
                        new_pos_set = 1;
                        nn=0; 
                        
                        sprintf(sendValue,"%08d\t%08.0g\t%08d",adcValue1,err,angle);
                        UART_PutString(sendValue);
                        data_read_mode = 0;
                    }
                  
                break;
                case 3:
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        pid[0] = strtod(temp, NULL);
                        nn=0;
                        char temp[] = "      ";
                        data_read_mode = 0;
                    }
                    
                break;
                case 4:
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        pid[1] = strtod(temp, NULL);
                        nn=0;
                        char temp[] = "      ";
                        data_read_mode = 0;
                    }
                    
                break;
                case 5:
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        pid[2] = strtod(temp, NULL);
                        nn=0;
                        char temp[] = "      ";
                        data_read_mode = 0;
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
    //ADC_SAR_1_StartConvert(); 
    //ADC_SAR_2_Start(); 
    //ADC_SAR_2_StartConvert(); 
    
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

    double der, prev_err, pid_integral,dt;
    der = 0; prev_err = 0; pid_integral = 0; 
    dt = 0.01; //assumes cydelay = 10 below for a 100Hz frequency.
    for(;;)
    {
        
        /*adcValue1 = ADC_SAR_1_GetResult16() ;
        adcValue2 = ADC_SAR_2_GetResult16() ;
        sprintf(sendValue,"%04d \t %04d \n",adcValue1,adcValue2);
        UART_PutString(sendValue);
        */
        ADC_SAR_1_StartConvert();
        ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        adcValue1 = ADC_SAR_1_GetResult16();
        sprintf(sendValue,"%08d\t%08.0g\t%08d\n",adcValue1,err,angle);
        UART_PutString(sendValue);
                        
        /* ADC ERROR BANDAID */
        //ADC value for some reason is offset by 57232
        if(adcValue1>30000)
        {
            adcValue1 = adcValue1 - 57232;
        }
        
        /* END ADC ERROR BANDAID */
                 
        //PWM_1_WriteCompare(dutycyclelength(0));
        if(new_pos_set){
            pos = new_pos;
            new_pos_set = 0;
            new_angle = 0;
            
            err = pos - adcValue1;
            der = err - prev_err;
            pid_integral += err;
            angle = pid[0] * err + ( pid[1] * pid_integral * dt) + ( pid[2] * der / dt );
            //angle=angle*-1;
            //angle = ((float)pos/4096.00)*90.00 - 45.00;
            //angle = 0;
            //Limit angles of proportional valve
            if(angle>45){angle=45;}
            if(angle<-45){angle=-45;}
            if(angle<46 & angle>-46){
                PWM_1_WriteCompare(dutycyclelength(angle));
            }
            prev_err = err;
            
        }
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
