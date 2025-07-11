#ifndef GENERATOR_H
#define GENERATOR_H

#include <Arduino.h>
#include "driver/dac.h"

#define TABLE_SIZE 32
#define CHANNELS_COUNT 2

extern const uint8_t SIN_TABLE[TABLE_SIZE];
extern const uint8_t TRG_TABLE[TABLE_SIZE];
extern const uint8_t SAW_TABLE[TABLE_SIZE];
extern const uint8_t SQR_TABLE[TABLE_SIZE];

enum FormChannel {OFF, SIN, TRG, SAW, SQR};

struct GenChannel
{
  FormChannel form;

  //Обработанные значения амплитуды, частоты и фазы (0...100, 0...1000.00, 0...360)
  uint16_t amp;
  float freq;
  uint16_t phase;

  //Предварительные вычисленные удобные значения
  //для ускорения работы прерываний
  uint32_t coef_amp;
  uint32_t step_phase;
  uint32_t shift_phase;

  //Фазовый накопитель
  uint32_t current_phase;
};

extern GenChannel channels[CHANNELS_COUNT];

void genInit(); //Инициализация каналов нулями
void genSync(); //Функция принудительной синхронизации всех каналов
void genChSet(uint8_t channel, FormChannel form, uint16_t amp, float freq, uint16_t phase);//Функция установки параметров канала

#endif