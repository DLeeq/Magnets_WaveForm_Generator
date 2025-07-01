#ifndef DATACHANNELS_H
#define DATACHANNELS_H

#include <Arduino.h>

enum FormChannel {OFF, SIN, TRG, SAW, SQR};

struct GenChannel
{
  FormChannel form;

  //Сырые значения амплитуды, частоты и фазы  (0...254, 0...100000, 0...254)

  uint8_t raw_amp;
  uint32_t raw_freq;
  uint8_t raw_phase;

  //Предварительные вычисленные удобные значения
  //для ускорения работы прерываний

  uint32_t coef_amp;
  uint32_t step_phase;
  uint32_t shift_phase;

  //Фазовый накопитель
  uint32_t current_phase;
};

extern GenChannel channels[2];

void channelsInit(); //Инициализация каналов нулями
void channelsSync(); //Функция принудительной синхронизации всех каналов

#endif