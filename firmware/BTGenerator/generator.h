#ifndef GENERATOR_H
#define GENERATOR_H

#include <Arduino.h>
#include "driver/dac.h"

#include "dataChannels.h"

#define TABLE_SIZE 32

extern const uint8_t SIN_TABLE[TABLE_SIZE];

extern const uint8_t TRG_TABLE[TABLE_SIZE];

extern const uint8_t SAW_TABLE[TABLE_SIZE];

extern const uint8_t SQR_TABLE[TABLE_SIZE];

void IRAM_ATTR signalGenerator();

#endif