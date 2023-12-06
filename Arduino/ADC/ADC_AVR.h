/*
 * ADC_AVR.h
 *
 * Created: 7/20/2023 1:08:20 AM
 *  Author: Chuong
 *  SEE datasheet for more configuration
 */ 


#ifndef __ADC_AVR_H__
#define __ADC_AVR_H__

// Prescaler
/*
                 F_system
  F =    ------------------      = 1kHz
       2*Presaler*(1+OCR1A)
*/
#define CLK_DIV_2 1
#define CLK_DIV_4 2
#define CLK_DIV_8 3
#define CLK_DIV_16  4
#define CLK_DIV_32  5
#define CLK_DIV_64  6
#define CLK_DIV_128 7


// Auto trigger mode 
// Has effect when ADATE in ADCSRA is set to 1
#define FREE_RUNNING  0
#define ANA_COMP    1
#define EXT_INT_0   2
#define T0_COMP_A   3
#define T0_OVF      4
#define T1_COMP_B   5
#define T1_OVF      6
#define T1_CAP_E    7

//Vref
#define AREF      0
#define AVCC      0x40
#define INTERNAL    0xC0  

class ADC_AVR
{ 
public:
  void Init(uint8_t prescaler,uint8_t vref);
  void Auto_trig(uint8_t source);
  void Set_channel(uint8_t chan);
  void Start_conversion();
  uint16_t read_adc();
protected:
private:
};


#endif /* ADC_AVR */
