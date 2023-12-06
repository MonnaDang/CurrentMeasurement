/**
 * main.c
 * Name: Dang Hoang Anh Chuong - UTE
 * Code: 20142473
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

uint32_t Buff[100];
char String1[20] = "BEGIN 100 Character:"; // 20
char String2[18] = "END 100 Character.";   // 18
uint8_t  scount,i;

void Send_DEC(uint32_t dec){
    UARTCharPut(UART0_BASE, 0x0A); // Line Feed (New Line)
    UARTCharPut(UART0_BASE, 0x0D); // Carriage Return (Move Cursor to the beginning of new line)
    UARTCharPut(UART0_BASE, dec/1000%10+0x30); // Convert dec character to ascii
    UARTCharPut(UART0_BASE, dec/100%10+0x30);
    UARTCharPut(UART0_BASE, dec/10%10+0x30);
    UARTCharPut(UART0_BASE, dec%10+0x30);
}

// Config ADC1, Channel AIN2, Sequencer 3.
void Init_ADC1_PE1(void){
    // Based on datasheet page 817
    //Step 1: Enable the ADC clock
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R1; // Using ADC1
    //Step 2: Enable clock to GPIO modules
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; //portE
    GPIO_PORTE_DIR_R ^= ~GPIO_PIN_1; // Config PinE1 as input
    //Step 3: Select GPIO pin for  Alternate function(ADC)
    GPIO_PORTE_AFSEL_R |= GPIO_PIN_1;
    //Step 4: Clear DEN, to config ADC input
    GPIO_PORTE_DEN_R &= ~GPIO_PIN_1;
    //Step 5: Disable analog isolation circuit
    GPIO_PORTE_AMSEL_R |= GPIO_PIN_1;
    // Config Sample sequencer  page 818
    //Step 1: Disable sequencer
    ADC1_ACTSS_R &= ~ADC_ACTSS_ASEN3; //disable sequencer3
    //Step 2: Config trigger event
    ADC1_EMUX_R |= ADC_EMUX_EM3_PROCESSOR|ADC_EMUX_EM3_TIMER; // Enable ADC triggered by Processor and Timer
    //Step 3: PWM trigger source (if use)
    //Step 4: Config input source for each sequencer.
    ADC1_SSMUX3_R |= 2; // AIN2
    //Step 5: Config the sample control bits
    ADC1_SSCTL3_R |= ADC_SSCTL3_END0|ADC_SSCTL3_IE0;
    //Step 6: Enable Interrupt
    ADC1_IM_R |= ADC_IM_MASK3; // Enable ADC SS3 Interrupts
    //Step 7: Enable sequencer
    ADC1_ACTSS_R |= ADC_ACTSS_ASEN3; // Enable SS3.
    ADC1_ISC_R |= ADC_ISC_IN3; //clear SS3 Interrupt
}

bool Is_ADC1_Interrupt(void){
    if((ADC1_RIS_R&(ADC_RIS_INRDC|ADC_RIS_INR3))) // //Check SS3 interrupt
        return 1;
    return 0;
}

void Clear_ADC1_Interrupt(void){
    ADC1_ISC_R |= ADC_ISC_IN3; //clear SS3 Interrupt
}

void ADC1_Start_Sample(void){
    ADC1_PSSI_R |= ADC_PSSI_SS3; //Start Sampling on SS3
}

uint32_t Read_ADC(void){
    return ADC1_SSFIFO3_R;
}

// ADC interrupt handler
void ADC1_ISR(void){
    // Clear the interrupt flag.
    Clear_ADC1_Interrupt();
    // Read ADC value
    if(scount == 99) {
        scount = 0;
        UARTCharPut(UART0_BASE, 0x0A); // Line Feed (New Line)
        UARTCharPut(UART0_BASE, 0x0D); // Carriage Return (Move Cursor to the beginning of new line)
        for(i=0;i<20;i++){
            UARTCharPut(UART0_BASE, String1[i]);
        }
        for(i=0;i<100;i++){
            Send_DEC(Buff[i]);
        }
        UARTCharPut(UART0_BASE, 0x0A); // Line Feed (New Line)
        UARTCharPut(UART0_BASE, 0x0D);
        for(i=0;i<18;i++){
            UARTCharPut(UART0_BASE, String2[i]);
        }
    }
    else scount++;
    Buff[scount] = Read_ADC();
    // Check if ADC should Work as expect
    // Toggle Blue LED.
//    GPIO_PORTF_DATA_R ^= GPIO_PIN_3;
}

// Config Timer0, 32-bit configuration.
void Init_timer0(void){
    // Enable Timer0 Module.
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
    // Based on datasheet page 722.
    //Step 1: disable timer 0A.
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //Step 2: Selects 32-bit configuration.
    TIMER0_CFG_R = 0x0;
    //Step 3: Selects Preodic mode.
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;
    //Step 4: Selects scount direction.
    TIMER0_TAMR_R |= TIMER_TAMR_TACDIR; // scount up;
    TIMER0_CTL_R |= TIMER_CTL_TAOTE; // Enable Timer trigger ADC sample
    //Step 5: Load TOP value for timer0.
    TIMER0_TAILR_R = SysCtlClockGet()/1000; // 1mS
    //Step 6: Config Interrupt
    TIMER0_IMR_R |= TIMER_IMR_TATOIM; // Timer A time-out.
    //Step 7
    TIMER0_CTL_R |= TIMER_CTL_TAEN; // enable timer 0A.
}
// Timer0 interrupt handler
void Timer0_ISR(void){
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Clear Timer0 Interrupt
    // Toggle count direction
    TIMER0_TAMR_R ^= TIMER_TAMR_TACDIR;

//    ADC1_Start_Sample();
//    while(!Is_ADC1_Interrupt());
//    Clear_ADC1_Interrupt();
//    Buff[0] = Read_ADC();
    // Check if ADC should Work as expect
    // Toggle Blue LED.
//    GPIO_PORTF_DATA_R ^= GPIO_PIN_2;
}

int main(void)
{
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

//-----  USE BUILTIN FUNCTION                -----//
//-----  USE ADC0 Module, Channel AIN2       -----//

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
//    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
//    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
//    ADCSequenceEnable(ADC0_BASE, 3);
//    ADCIntClear(ADC0_BASE, 3);

//-----  USE BUILTIN FUNCTION                -----//

    Init_timer0();
    Init_ADC1_PE1();
    IntEnable(INT_TIMER0A);
    IntEnable(INT_ADC1SS3);
    IntMasterEnable();

	while(1){
//-----  USE BUILTIN FUNCTION   -----//
//	    ADCProcessorTrigger(ADC0_BASE, 3);
//	    while(!ADCIntStatus(ADC0_BASE, 3, false));
//	    ADCSequenceDataGet(ADC0_BASE, 3, Buff);
//	    ADCIntClear(ADC0_BASE, 3);
//-----  USE BUILTIN FUNCTION   -----//

//        ADC1_Start_Sample();
//        while(!Is_ADC1_Interrupt());
//        Clear_ADC1_Interrupt();
//        Buff[0] = Read_ADC();
//	    SysCtlDelay(SysCtlClockGet() / 12);
	}
}

