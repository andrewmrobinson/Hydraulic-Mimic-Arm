
#include <project.h>
#include <stdio.h>
#include "common.h"
#include <math.h>
#include <stdlib.h>     /* strtod */

#define	Frequency 1000000.0			// Frequency of PWMClock
#define	One_ms (Frequency/1000.0)	// 1 ms constant
#define filter_size 7

/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

uint8 errorStatus = 0u;

uint16 dutycyclelength(double angle)
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
double angle = 0;
double angletemp=0;
int16 pulse = 0;
int16 pulse_temp = 0;
int isNegative=0;
char sendValue[100];
uint16 adcValue1;
uint16 adcValue2;
char temp[20];
int nn=0;
double pid[3] = {0.003,0.000,0.001};
//double pid[3] = { -2.22,-0.0307,0.0 };
char help[100];
int tt = 0;
int pos=1600;
int new_pos;
double err;
int start_calib = 0;


uint16 moving_median[filter_size] = {0,0,0};
uint16 tempArray[filter_size];
uint16 tmp;

CY_ISR(adc_update){
    for(int j=0;j<filter_size-1;j++){
            moving_median[j] = moving_median[j+1]; 
    }
    ADC_SAR_1_StartConvert();
    ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        
    moving_median[filter_size-1] = ADC_SAR_1_GetResult16();
    
    for(int j=0;j<filter_size;j++){
            tempArray[j] = moving_median[j]; 
    }
    for(int i = 0; i < filter_size; i++){                     //Loop for ascending ordering
    	for (int j = 0; j < filter_size; j++)             //Loop for comparing other values
    	{
    		if (tempArray[j] > tempArray[i])                //Comparing other array elements
    		{
    			tmp = tempArray[i];         //Using temporary variable for storing last value
    			tempArray[i] = tempArray[j];            //replacing value
    			tempArray[j] = tmp;             //storing last value
    		}  
    	}
    }
    adcValue1 = tempArray[(filter_size-1)/2];
    if(adcValue1<4000){
        adcValue1 = adcValue1;
    }
    
    update_median_isr_ClearPending();
    
}


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
            if(tt==99){tt=0;}
            if(rxData=='b' ){start_calib=1;};
            switch(data_read_mode)
            {
                case 0:
                    if(rxData=='{' )
                    {
                        for(int pp=1;pp<20;pp++){
                            temp[pp] = ' ';
                        }
                        temp[0] = '\0';
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
                    
                        sprintf(sendValue,"%08d\t%8.2f\t%8d",adcValue1,err,pulse_temp);
                        UART_PutString(sendValue);
                        temp[0] = '\0';
                        
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
                        temp[0] = '\0';
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
                        temp[0] = '\0';
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
                        temp[0] = '\0';
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
    update_median_isr_StartEx(adc_update);
#endif /* INTERRUPT_CODE_ENABLED == ENABLED */
    
    CyGlobalIntEnable;      /* Enable global interrupts. */
    

    angle = -89;
    uint16 timercapture;
    double der, prev_err, pid_integral,dt;
    double time;
    
    der = 0; prev_err = 0; pid_integral = 0; 
    dt = 0.025; //assumes cydelay = 10 below for a 100Hz frequency.

    sprintf(sendValue,"%08d\t%08.0f\t%8d",adcValue1,err,pulse_temp);
    UART_PutString(sendValue);
    uint16 moving_avg[5] = {0,0,0,0,0};
    median_timer_Start();
    Timer_1_Start();
    for(;;)
    {
        Timer_1_WriteCounter(65535);
       
        
        if(new_pos_set){
            pos = new_pos;
            new_pos_set = 0;
            new_angle = 0;
            
            /*
            if(pos>45){pos=45;}
            if(pos<-45){pos=-45;}
            if(pos<46 & pos>-46){
                PWM_1_WriteCompare(dutycyclelength(pos));
            }
            */       
        }
        
        /* START PID CODE */

        err = -pos + adcValue1;
        der = err - prev_err;
        pid_integral = err + pid_integral;
        pulse_temp = pid[0] * err + ( pid[1] * pid_integral * dt) + ( pid[2] * der / dt );
        pulse=pulse_temp;
        //Limit angles of proportional valve
        if(pulse<0){pulse = pulse - 150;}
        if(pulse>0){pulse = pulse + 153;}
        if(pulse<-500){pulse=-500;}
        if(pulse>500){pulse=500;}
        if(pulse>-501 & pulse<501){
            //PWM_1_WriteCompare(dutycyclelength(angle));
            PWM_1_WriteCompare(pulse + 1500);//(uint16)One_ms + ((angle+45.0)/90.0) * One_ms;
        }
        prev_err = err;
        /* END PID CODE */
        
        
        
        CyDelay(10);
        time = Timer_1_ReadCounter();
        dt=(double)(65535-time)/1000000;

    }
}

/* [] END OF FILE */
