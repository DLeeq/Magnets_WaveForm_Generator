#include "generator.h"

const uint8_t SIN_TABLE[TABLE_SIZE] = {
  128, 153, 178, 200, 221, 237, 248, 254,
  255, 254, 248, 237, 221, 200, 178, 153,
  128, 102,  77,  55,  34,  18,   7,   1,
    0,   1,   7,  18,  34,  55,  77, 102
};

const uint8_t TRG_TABLE[TABLE_SIZE] = {
   0,  16,  32,  48,  64,  80,  96, 112,
 128, 144, 160, 176, 192, 208, 224, 240,
 240, 224, 208, 192, 176, 160, 144, 128,
 112,  96,  80,  64,  48,  32,  16,   0
};

const uint8_t SAW_TABLE[TABLE_SIZE] = {
   0,   8,  16,  24,  32,  40,  48,  56,
  64,  72,  80,  88,  96, 104, 112, 120,
 128, 136, 144, 152, 160, 168, 176, 184,
 192, 200, 208, 216, 224, 232, 240, 248
};

const uint8_t SQR_TABLE[TABLE_SIZE] = {
 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0
};

GenChannel channels[2];

void genInit()
{
  dac_output_enable(DAC_CHANNEL_1); 
  dac_output_enable(DAC_CHANNEL_2);

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

void genChannelsSync()
{
  for(uint8_t i = 0; i < 2; i++)
    channels[i].current_phase = 0;
}

void IRAM_ATTR genTicker()
{
  for(uint8_t i = 0; i < 2; i++)
  {
    uint32_t phase = 0;
    phase = (channels[i].shift_phase + channels[i].current_phase) * 32 / 3200000;

    if(phase >= 32) phase -= 32;

    int32_t val = 0;

    if(channels[i].form != OFF)
    {
      const uint8_t* TABLE = NULL;

      switch (channels[i].form)
      {
        case SIN : TABLE = SIN_TABLE; break;
        case TRG : TABLE = TRG_TABLE; break;
        case SAW : TABLE = SAW_TABLE; break;
        case SQR : TABLE = SQR_TABLE; break;
      }

      int64_t dy0 = 0, dy = 0, dx = 0;

      dy0 = ( (int32_t)TABLE[phase] - (int32_t)TABLE[phase == 0 ? 31 : phase - 1] );
      dx = (channels[i].shift_phase + channels[i].current_phase) % 100000;
      dy = dy0 * dx / 100000;

      val = ( (int32_t)TABLE[phase] + dy ) * channels[i].coef_amp / 254; 

      if(val > 255) 
        val = 255;
      else if(val < 0) 
        val = 0;
    }
    else
      val = 0;

    switch(i)
    {
      case 0 : dac_output_voltage(DAC_CHANNEL_1, val); break;
      case 1 : dac_output_voltage(DAC_CHANNEL_2, val); break;
    }

    channels[i].current_phase += channels[i].step_phase;

    if(channels[i].current_phase >= 3200000) channels[i].current_phase -= 3200000;
  }
}