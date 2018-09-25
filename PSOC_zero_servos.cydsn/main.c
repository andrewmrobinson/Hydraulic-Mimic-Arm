
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

void writePWM(int cylinder, int pulse){
    switch(cylinder){
                case 0:
                    PWM_0_WriteCompare(pulse + 1500);
                    break;
            }  
}


int main()
{
    //initializing variables
    Clock_1_Start();
    PWM_0_Start();
    
#if(INTERRUPT_CODE_ENABLED == ENABLED)
#endif /* INTERRUPT_CODE_ENABLED == ENABLED */
    
    CyGlobalIntEnable;      /* Enable global interrupts. */
    
    
    for(;;)
    {
        for(int i = 0;i<4;i++){
            writePWM(i,499);
            CyDelay(500);
            writePWM(i,-499);
            CyDelay(500);
            writePWM(i,0);
            CyDelay(1000);
        }
            
    }
}

/*  END OF FILE */
