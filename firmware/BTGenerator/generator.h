#ifndef GENERATOR_H
#define GENERATOR_H

#include <Arduino.h>
#include "driver/dac.h"

#define CHANNELS_COUNT 2

enum FormChannel {OFF, SIN, TRG, SAW, SQR};

void genInit(); //Инициализация каналов нулями

void genSync(); //Функция принудительной синхронизации всех каналов

void genChSet(uint8_t channel, FormChannel form, float amp, float freq, float phase);//Сеттер, Функция установки параметров канала

FormChannel genChGetForm(uint8_t channel);       //
float genChGetAmp(uint8_t channel);              //
float genChGetFreq(uint8_t channel);             //
float genChGetPhase(uint8_t channel);            //Геттеры
uint32_t genChGetCoefAmp(uint8_t channel);       //
uint32_t genChGetStepPhase(uint8_t channel);     //
uint32_t genChGetShiftPhase(uint8_t channel);    //

#endif