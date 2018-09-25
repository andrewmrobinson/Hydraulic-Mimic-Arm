
#include <project.h>
#include <stdio.h>
#include "common.h"
#include <math.h>
#include <stdlib.h>     /* strtod */

#define	Frequency 1000000.0			// Frequency of PWMClock
#define	One_ms (Frequency/1000.0)	// 1 ms constant
#define filter_size 7
#define CYL_NO 4                    // Number of connected cylinders

/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

uint8 errorStatus = 0u;

void writePWM(int cylinder, int pulse){
    switch(cylinder){
                case 0:
                    PWM_0_WriteCompare(pulse + 1500);
                    break;
                case 1:
                    PWM_1_WriteCompare(pulse + 1500);
                    break;
                case 2:
                    PWM_2_WriteCompare(pulse + 1500);
                    break;
                case 3:
                    PWM_3_WriteCompare(pulse + 1500);
                    break;
            }  
}

int pulseCheck(int pulse, int offset_upper, int offset_lower) {
    if(pulse<0){pulse = pulse - offset_lower;}
        if(pulse>0){pulse = pulse + offset_upper;}
        if(pulse<-500){pulse=-500;}
        if(pulse>500){pulse=500;}
    return pulse;
}


int data_read_mode = 0;

volatile int new_pos_set[CYL_NO];
int new_pos[CYL_NO];
int pos[CYL_NO];
int pulse[CYL_NO];
int pulse_temp[CYL_NO];
double err[CYL_NO];
uint16 adcValue[CYL_NO];
double der[CYL_NO];
double prev_err[CYL_NO];
double pid_integral[CYL_NO];

int cyl_set = 0;
char cyl_tmp[1];
double dts[30];
int dts_ct = 0;

int offsets[4][2] = {{125,179},{132,176},{185,172},{195,169}}; //{lower,upper} - both positive

char sendValue[100];
char temp[20];
int nn=0;
double pid[3] = {0.3,0.001,0.0};

char help[100];
int tt = 0;

int start_calib = 0;
int cyl_no = 0;

uint16 moving_median[CYL_NO][filter_size] = {0};
uint16 tempArray[filter_size];
uint16 tmp;

CY_ISR(adc_update){
    for(int c = 0;c<CYL_NO;c++){
        AMux_0_FastSelect(c);
        
        for(int j=0;j<filter_size-1;j++){
                moving_median[c][j] = moving_median[c][j+1]; 
        }
        
        moving_median[c][filter_size-1] = ADC_0_GetResult16();
        moving_median[c][filter_size-1] = ADC_0_GetResult16();
        ADC_0_IsEndConversion(ADC_0_WAIT_FOR_RESULT);
        moving_median[c][filter_size-1] = ADC_0_GetResult16();
        
        for(int j=0;j<filter_size;j++){
                tempArray[j] = moving_median[c][j]; 
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
        adcValue[c] = tempArray[(filter_size-1)/2];
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
                        //aaaa
                        for(int pp=1;pp<20;pp++){
                            temp[pp] = ' ';
                        }
                        temp[0] = '\0';
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
                    else if(rxData=='c'){
                        data_read_mode = 6;}
                break;
                case 2: //x
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        
                        new_pos[cyl_no] = (int) strtol(temp, (char **)NULL, 10);
                        new_pos_set[cyl_no] = 1;
                        nn=0; 
                        temp[0] = '\0';
                        data_read_mode = 0;
                        
                    }
                  
                break;
                case 3: //p
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
                case 4: //i
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
                case 5: //d
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
                case 6: //c
                    if(rxData != '}'){
                        temp[nn] = rxData;
                        nn++;
                    }
                    else {
                        cyl_no = (int) strtol(temp, (char **)NULL, 10);
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

int main()
{
    //initializing variables
    for(int g = 0;g<CYL_NO;g++){
        new_pos_set[g]  = 0;
        new_pos[g]      = 0;
        pos[g]          = 4000;
        pulse[g]        = 0;
        pulse_temp[g]   = 0;
        err[g]          = 0;
        adcValue[g]     = 0;
        der[g]          = 0;
        prev_err[g]     = 0;
        pid_integral[g] = 0;
    }
    //PWM variables
    PWM_0_Start();
    PWM_1_Start();
    PWM_2_Start();
    PWM_3_Start();
    ADC_0_Start();
    ADC_0_StartConvert();
    AMux_0_Init();
    
    UART_Start();           /* Start communication component */

#if(INTERRUPT_CODE_ENABLED == ENABLED)
    isr_rx_StartEx(RxIsr);
    update_median_isr_StartEx(adc_update);
#endif /* INTERRUPT_CODE_ENABLED == ENABLED */
    
    CyGlobalIntEnable;      /* Enable global interrupts. */
    
    double dt;
    double time;
    
    dt = 0.025; //initialised dt, corrected by timer  below

    sprintf(sendValue,"%08d\t%08.2f\t%08d\t%08.2f",adcValue[0],err[0],adcValue[1],err[1]);
    UART_PutString(sendValue);
    
    median_timer_Start();
    Timer_1_Start();
    
    for(;;)
    {
        for(;;){
            for(int i = 0;i<4;i++){
                writePWM(i,499);
            }
                CyDelay(500);
                for(int i = 0;i<4;i++){
                writePWM(i,-499);
            }
                CyDelay(500);
                for(int i = 0;i<4;i++){
                writePWM(i,0);
            }
                CyDelay(1000);
        }
        
        Timer_1_WriteCounter(65535);
        //sprintf(sendValue,"%08d\t%08.2f\t%08d\t%08.2f",adcValue[0],err[0],adcValue[1],err[1]);
        sprintf(sendValue,"%08d\t%08.2d\t%08d\t%08.2d\n",adcValue[0],adcValue[1],adcValue[2],adcValue[3]);
        
        UART_PutString(sendValue);
        //sprintf(sendValue,"%08d\t%08.2f\t%08d\t%08.2f",adcValue[0],err[0],adcValue[1],err[1]);
        //UART_PutString(sendValue);
        /* START PID CODE */
        for(int cyl = 0;cyl<CYL_NO;cyl++){
            if(new_pos_set[cyl]){
                pos[cyl] = new_pos[cyl];
                new_pos_set[cyl] = 0;
            }
            
            err[cyl] = -pos[cyl] + adcValue[cyl];
            der[cyl] = err[cyl] - prev_err[cyl];
            pid_integral[cyl] = err[cyl]* dt + pid_integral[cyl];
            pulse_temp[cyl] = pid[0] * err[cyl] + ( pid[1] * pid_integral[cyl] ) + ( pid[2] * der[cyl] / dt );
            
            pulse[cyl] = pulseCheck(pulse_temp[cyl],offsets[cyl][1],offsets[cyl][0]);
            
            writePWM(cyl,pulse[cyl]);
            prev_err[cyl] = err[cyl];
        }
        
        
        /* END PID CODE */
        if(start_calib){
                
            int calib_cyl = 1;
            int upper_offset_set = 0;
            int lower_offset_set = 0;
            int upper_offset = 160;
            int lower_offset = -110;
            int lower_check_no = 0;
            int upper_check_no = 0;
            uint16 prev_adcValue = adcValue[0];
            uint16 diff = 0;
            for(int g = 0;g<CYL_NO;g++)
                writePWM(g,0);
                
            CyDelay(1000);
        
            while(upper_offset_set==0 || lower_offset_set==0){
                
                prev_adcValue = adcValue[calib_cyl];
                if(upper_offset_set==0){
                    writePWM(calib_cyl,upper_offset);
                    CyDelay(200);
                    diff = abs(prev_adcValue-adcValue[calib_cyl]);
                    if(diff>10){
                        upper_check_no ++;;
                    }else{
                        upper_offset = upper_offset + 1;
                        upper_check_no=0;
                    }
                    sprintf(sendValue,"UPPER:\tLower Offset: %d \tUpper Offset: %d \t ADC Diff: %d\n",lower_offset,upper_offset,diff);
                    UART_PutString(sendValue);
                }
                writePWM(calib_cyl,0);
                CyDelay(500);
                
                prev_adcValue = adcValue[calib_cyl];
                if(lower_offset_set==0){
                    writePWM(calib_cyl,lower_offset);
                    CyDelay(200);
                    diff = abs(prev_adcValue-adcValue[calib_cyl]);
                    if(diff>10){
                        lower_check_no++;
                    }else{
                        lower_offset = lower_offset - 1;
                        lower_check_no = 0;
                    }
                    sprintf(sendValue,"LOWER:\tLower Offset: %d \tUpper Offset: %d \t ADC Diff: %d\n",lower_offset,upper_offset,diff);
                    UART_PutString(sendValue);
            
                }
                
                if(upper_check_no>2){
                    upper_offset_set=1;}
                if(lower_check_no>2){
                    lower_offset_set=1;}
                
                writePWM(calib_cyl,0);
                CyDelay(500);
                
            }
            
            sprintf(sendValue,"Lower Offset: %d \t Upper Offset: %d \n",lower_offset,upper_offset);
            UART_PutString(sendValue);
            
            //while(1){CyDelay(100);};
            start_calib = 0;
            CyDelay(15000);
        }
        CyDelay(5);
        time = Timer_1_ReadCounter();
        dt=(double)(65535-time)/1000000;
        dts[dts_ct] = dt;
        dts_ct++;
        if(dts_ct>29){
            dts_ct=0;
        }
    }
}

/*  END OF FILE */
