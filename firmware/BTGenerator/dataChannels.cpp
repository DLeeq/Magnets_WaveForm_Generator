#include "dataChannels.h"

GenChannel channels[2];

void channelsInit()
{
  for(uint8_t i = 0; i < 2; i++)
  {
    channels[i].form = OFF;

    channels[i].amp = 0;
    channels[i].freq = 0;
    channels[i].phase = 0;
    
    channels[i].coef_amp = 0;
    channels[i].step_phase = 0;
    channels[i].shift_phase = 0;

    channels[i].current_phase = 0;
  }
}

void channelsSync()
{
  for(uint8_t i = 0; i < 2; i++)
    channels[i].current_phase = 0;
}