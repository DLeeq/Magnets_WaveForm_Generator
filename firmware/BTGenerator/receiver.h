#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

void rvrInit(String id);
void rvrSDC8Reading(uint8_t start_byte, uint8_t len_data, uint8_t *buf, uint8_t buf_size, void (*bufHandler)(uint8_t *buf, uint8_t buf_size));

#endif