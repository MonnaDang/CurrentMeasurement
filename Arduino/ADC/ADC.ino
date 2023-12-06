
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ADC_AVR.h"

uint16_t count = 0, temp = 99, scount = 0;
uint16_t Buff[100];
ADC_AVR adc;

void T1_init() {
  TCCR1A = (1 << WGM11) | (1 << WGM10);
  TCCR1B = (1 << WGM13) | (1 << WGM12); // CTC mode TOP = OCR1A
  TCCR1B |= (1 << CS11) | (1 << CS10); // set prescaler 64
  OCR1A  = 249;
  TIMSK1 = (1 << TOIE1);
  sei();
}

int main(void)
{
  /* Replace with your application code */
  T1_init();
  adc.Init(CLK_DIV_2, AVCC);
  adc.Auto_trig(T1_OVF);
  adc.Set_channel(3);
  Serial.begin(9600);
  while (1)
  {

  }
}

ISR(ADC_vect) {
  Buff[scount] = ADC;
  if (scount == 100) {
    scount = 0;
    Serial.println("BEGIN 100 Character:");
    for (int i = 0; i < 100; i++)
      Serial.println(Buff[i]);
    Serial.println("END 100 Character.");
  } else scount++;
}

ISR(TIMER1_OVF_vect) {
  // this for ADC trigger
}
