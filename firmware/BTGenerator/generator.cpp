#include "generator.h"

#define TABLE_SIZE 32

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

struct GenChannel
{
  FormChannel form;

  //Амплитуда, частота и фаза (0...100.00, 0...1000.00, 0...360.00)
  float amp;
  float freq;
  float phase;

  //Предварительные вычисленные удобные значения
  //для ускорения работы прерываний
  uint32_t coef_amp;
  uint32_t step_phase;
  uint32_t shift_phase;

  //Фазовый накопитель
  uint32_t current_phase;
} arr_channels[CHANNELS_COUNT];

static void IRAM_ATTR genTick();

void IRAM_ATTR onTimer() 
{
  genTick();
}

void genInit()
{
  dac_output_enable(DAC_CHANNEL_1); 
  dac_output_enable(DAC_CHANNEL_2);

  for(uint8_t i = 0; i < CHANNELS_COUNT; i++)
  {
    arr_channels[i].form = OFF;

    arr_channels[i].amp = 0;
    arr_channels[i].freq = 0;
    arr_channels[i].phase = 0;
    
    arr_channels[i].coef_amp = 0;
    arr_channels[i].step_phase = 0;
    arr_channels[i].shift_phase = 0;

    arr_channels[i].current_phase = 0;
  }

  static hw_timer_t* timer = timerBegin(32000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1, true, 0);
}

void genSync()
{
  for(uint8_t i = 0; i < CHANNELS_COUNT; i++)
    arr_channels[i].current_phase = 0;
}

void genChSet(uint8_t channel, FormChannel form, float amp, float freq, float phase)
{
  //Форма сигнала на канале
  arr_channels[channel].form = form;

  //Амплитуда, частота и фаза (0...100.00, 0...1000.00, 0...360.00)
  arr_channels[channel].amp = amp;    //(0...100.00 %)
  arr_channels[channel].freq = freq;  //(0...1000.00 Hz)
  arr_channels[channel].phase = phase;//(0...360.00 deg);

  //Предварительные вычисления удобных значений
  //для ускорения работы прерываний
  arr_channels[channel].coef_amp = (uint32_t)(amp * 254 / 100.0); //(0...254)
  arr_channels[channel].step_phase = (uint32_t)(freq * 100000 / 1000.0); //(0...100000)
  arr_channels[channel].shift_phase = (uint32_t)(phase * 3200000 / 360.0); //(0...3200000)
}

//Геттеры
FormChannel genChGetForm(uint8_t channel)
{ 
  return arr_channels[channel].form;
} 
float genChGetAmp(uint8_t channel)
{ 
  return arr_channels[channel].amp;
}              
float genChGetFreq(uint8_t channel)
{ 
  return arr_channels[channel].freq;
}            
float genChGetPhase(uint8_t channel)
{ 
  return arr_channels[channel].phase;
}            
uint32_t genChGetCoefAmp(uint8_t channel)
{ 
  return arr_channels[channel].coef_amp;
}       
uint32_t genChGetStepPhase(uint8_t channel)
{ 
  return arr_channels[channel].step_phase;
}     
uint32_t genChGetShiftPhase(uint8_t channel)
{ 
  return arr_channels[channel].shift_phase;
}   

static void IRAM_ATTR genTick()
{
  for(uint8_t i = 0; i < CHANNELS_COUNT; i++)
  {
    uint32_t phase = 0;
    phase = (arr_channels[i].shift_phase + arr_channels[i].current_phase) * 32 / 3200000;

    if(phase >= 32) phase -= 32;

    int32_t val = 0;

    if(arr_channels[i].form != OFF)
    {
      const uint8_t* TABLE = NULL;

      switch (arr_channels[i].form)
      {
        case SIN : TABLE = SIN_TABLE; break;
        case TRG : TABLE = TRG_TABLE; break;
        case SAW : TABLE = SAW_TABLE; break;
        case SQR : TABLE = SQR_TABLE; break;
      }

      int64_t dy0 = 0, dy = 0, dx = 0;

      dy0 = ( (int32_t)TABLE[phase] - (int32_t)TABLE[phase == 0 ? 31 : phase - 1] );
      dx = (arr_channels[i].shift_phase + arr_channels[i].current_phase) % 100000;
      dy = dy0 * dx / 100000;

      val = ( (int32_t)TABLE[phase] + dy ) * arr_channels[i].coef_amp / 254; 

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

    arr_channels[i].current_phase += arr_channels[i].step_phase;

    if(arr_channels[i].current_phase >= 3200000) arr_channels[i].current_phase -= 3200000;
  }
}