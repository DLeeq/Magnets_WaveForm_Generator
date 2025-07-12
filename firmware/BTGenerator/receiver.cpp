#include "receiver.h"

BluetoothSerial SerialBT;

static uint8_t CRC8(const uint8_t *data, uint8_t length) 
{
   uint8_t crc = 0x00;
   uint8_t extract;
   uint8_t sum;

    for(uint8_t i = 0; i < length; i++)
    {
      extract = *data;

      for (uint8_t tempI = 8; tempI; tempI--) 
      {
         sum = (crc ^ extract) & 0x01;
         crc >>= 1;
         if (sum)
            crc ^= 0x8C;
         extract >>= 1;
      }

      data++;
    }

    if(crc == 0xFF)
      crc = 0xA7;

   return crc;
}

void rvrInit(String id)
{
  SerialBT.begin(id);
}

void rvrSDC8Reading(uint8_t start_byte, uint8_t len_data, uint8_t *buf, uint8_t buf_size, void (*bufHandler)(uint8_t *buf, uint8_t buf_size))
{
  static bool data_reading = false;
  static uint8_t data_counter = 0;

  while(SerialBT.available()) 
  {
    uint8_t data_byte = SerialBT.read();

    if(!data_reading)
    {
      if(data_byte == start_byte)
        data_reading = true;
    }
    else
    {
      if(data_byte == start_byte)
        data_counter = 0;
      else
      {
        if(data_counter < len_data)
        {
          buf[data_counter] = data_byte;
          data_counter++;
        }
        else
        {
          data_reading = false;
          data_counter = 0;

          if(data_byte == CRC8(buf, len_data))
            bufHandler(buf, buf_size);
        }
      }
    }
  }
}
