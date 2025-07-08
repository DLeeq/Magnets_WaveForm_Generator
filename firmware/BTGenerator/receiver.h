#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

#include "dataChannels.h"

#define BUFFER_SIZE 6

extern BluetoothSerial SerialBT;

extern uint8_t buf[BUFFER_SIZE];

uint8_t CRC8(const uint8_t *data, uint8_t length);
void bufHandler();
void SDC8Reading(uint8_t start_byte, uint8_t len_data);

#endif