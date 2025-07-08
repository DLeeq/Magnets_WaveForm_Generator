#include "driver/dac.h"

#include "dataChannels.h"
#include "receiver.h"
#include "generator.h"

hw_timer_t* timer = NULL;

void IRAM_ATTR onTimer() 
{
  signalGenerator();
}

void setup() 
{
  SerialBT.begin("ESP32 (30PIN)");

  channelsInit(); //Инициализация нулями значений полей каналов генерации

  timer = timerBegin(32000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1, true, 0);

  dac_output_enable(DAC_CHANNEL_1); 
  dac_output_enable(DAC_CHANNEL_2);
}

void loop() 
{
  SDC8Reading(0xFF, 6);  
}