#include "dataReceiver.h"
#include "generator.h"
#include "driver/dac.h"

hw_timer_t* timer = NULL;

void IRAM_ATTR onTimer() 
{
  signalGenerator();
}

void setup() 
{
  SerialBT.begin("ESP32 (30PIN)");

  channels[0].current_phase = 0; //Сброс фазовых накопителей для синхронизации до запуска прерываний
  channels[1].current_phase = 0; //Сброс фазовых накопителей для синхронизации до запуска прерываний

  timer = timerBegin(32000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1, true, 0);

  dac_output_enable(DAC_CHANNEL_1); 
  dac_output_enable(DAC_CHANNEL_2);
}

void loop() 
{
  dataReader();  
}