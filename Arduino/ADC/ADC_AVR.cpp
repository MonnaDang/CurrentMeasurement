/*
 * ADC.cpp
 *
 * Created: 7/20/2023 1:07:11 AM
 * Author : Chuong
 */ 

#include <avr/io.h>
#include "ADC_AVR.h"

/* Replace with your library code */
void ADC_AVR::Init(uint8_t prescaler, uint8_t vref){
  // ADC Control and Status Register A
  ADCSRA  = (1<<ADEN); // enable Adc module
  //clear older prescaler
  ADCSRA &= 0xF8; 
  ADCSRA |= prescaler; // set prescaler = 111(128) (125kHz)
  
  //ADC Multiplexer Selection Register
  //clear older vref
  ADMUX &= 0x3f;
  ADMUX |= vref; // set reference voltage AVCC
}

void ADC_AVR::Auto_trig(uint8_t source){
  ADCSRA |= (1<<ADATE); // enable auto triger source
  ADCSRA |= (1<<ADIE);  // enable ADC complete interrupt
  
  // clear older source
  ADCSRB  &= 0xf8;
  ADCSRB  |= source; 
}

void ADC_AVR::Set_channel(uint8_t chan){
  // clear older channel
  ADMUX &= 0xf0;
  ADMUX |= chan;
}

void ADC_AVR::Start_conversion(){
  ADCSRA |= (1<<ADSC);                      // Starts a new conversion
  while(!(ADCSRA & (1<<ADIF)));             // Wait until the conversion is done
}

uint16_t ADC_AVR::read_adc(){
  return ADC;   
}
