/*
 * T13_ESC1.c
 *
 * Created: 1/12/2015 7:59:08 PM
 *  Author: Will
 */ 

#define F_CPU 9600000 // internal clock @ 9.6MHz
#include <avr/io.h>
#include <util/delay.h>

void PWM_setup(void)
{
  
  TCCR0A |= ((1 << COM0A1) | (0 << COM0A0) // COM0A1 - COM0A0 (Clear OC0A on Compare Match when up-counting. Set OC0A on Compare Match when down-counting.)
  | (0 << WGM01) | (1 << WGM00)); // WGM01 - WGM00 (set Phase Correct PWM Mode)
  
  TCCR0B |= (1 << CS01) | (1 << CS00); //clk/8 prescaler 
}

void PWM_write(int val)
{
  OCR0A = val; //This value sets the duty cycle 0 to 255 (8-bit timer)
}

int main(void)
{ 
  DDRB = 0b00000001; //PortB0 = output
  int Pulslen = 0;
  int TimeOut = 0;
  PWM_setup();
  
  while(1)
  {
    while(PINB & 0b00001000)  // while the servoPWM input is high
    {
      Pulslen=Pulslen+1; // increment
      _delay_us(8); //resolution setting, may require further adjustment
    } 
  
    if((Pulslen > 114) * (Pulslen < 118)) //throttle must be low initially
    { 
    
      while(1)
      {
      
        while(PINB & 0b00001000)  // while the servoPWM input is high
        {
          Pulslen=Pulslen+1; // increment
          _delay_us(8); //resolution setting, may require further adjustment
        }
    
        if (TimeOut>20000) //this will shut off the motor if the RX signal is lost
        {
          Pulslen = 115;
        }
      
        if (Pulslen > 114)  // ignore false triggers and only set the PWM when a pulse is received 
        {
          Pulslen = ((Pulslen-115)*2);
      
          //if(Pulslen<0){Pulslen=0;} //just in case there is a negative 
      
          if (Pulslen<5) //lower threshold, produces a dead-band  
          {Pulslen = 0;}
      
          if (Pulslen>220) //Full power after crossing a threshold determined by trial and error
          {Pulslen = 255;}
        
          PWM_write(Pulslen); //sets the PWM duty cycle
    
          TimeOut = 0;  //resets the timeout counter
      
        }
    
        if (Pulslen == 0) //counts every time the loop completes and a pulse is not registered
        {
          TimeOut+=1;
        }
    
        Pulslen = 0; // resets the variable for the next pulse measurement and starts the timeout counter 
     
      }
    }
    else
    {Pulslen = 0;}
    
  }
}
