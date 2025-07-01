#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

#include "dataChannels.h"

#define BUFFER_SIZE 7

extern BluetoothSerial SerialBT;

extern uint8_t buf[BUFFER_SIZE];

uint8_t CRC8(const uint8_t *data, uint8_t length);
void bufHandler();
void dataReader();

#endif