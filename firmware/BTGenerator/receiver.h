#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

static uint8_t CRC8(const uint8_t *data, uint8_t length);

void receiverInit(String id);
void SDC8Reading(uint8_t start_byte, uint8_t len_data, uint8_t *buf, uint8_t buf_size, void (*bufHandler)(uint8_t *buf, uint8_t buf_size));

#endif