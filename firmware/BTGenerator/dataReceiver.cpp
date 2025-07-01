#include "dataReceiver.h"

BluetoothSerial SerialBT;

uint8_t buf[BUFFER_SIZE];

void bufHandler()
{
  for(int i = 0; i < BUFFER_SIZE; i++)
  {
    SerialBT.print(buf[i], HEX);
    SerialBT.print(" ");
  }
    
  SerialBT.println("\n");

  uint8_t ch_num = buf[0] >> 4;

  channels[ch_num].form = (FormChannel)(buf[0] & 0xF);

  //Сырые значения амплитуды, частоты и фазы (0...254, 0...100000, 0...254)

  channels[ch_num].raw_amp = buf[1];
  channels[ch_num].raw_freq = buf[2] * 255 * 255 + buf[3] * 255 + buf[4];
  channels[ch_num].raw_phase = buf[5];

  //Предварительные вычисления удобных значений
  //для ускорения работы прерываний

  channels[ch_num].coef_amp = channels[ch_num].raw_amp; //(0...254)
  channels[ch_num].step_phase = channels[ch_num].raw_freq * 100 * 32 / 3200; //(0...100000)
  channels[ch_num].shift_phase =  channels[ch_num].raw_phase * 3200000 / 254; //(0...3200000)

  noInterrupts();

  channels[0].current_phase = 0; //Сброс фазовых накопителей для синхронизации
  channels[1].current_phase = 0; //Сброс фазовых накопителей для синхронизации

  interrupts();

  SerialBT.println("Channel: 0");
  SerialBT.println("Form: " + String(channels[0].form));
  SerialBT.println("Amplitude: " + String(channels[0].raw_amp));
  SerialBT.println("Frequency: " + String(channels[0].raw_freq));
  SerialBT.println("Phase: " + String(channels[0].raw_phase));
  SerialBT.println("Coef Amp: " + String(channels[0].coef_amp));
  SerialBT.println("Step Phase: " + String(channels[0].step_phase));
  SerialBT.println("Shift Phase: " + String(channels[0].shift_phase));

  SerialBT.print("\n");

  SerialBT.println("Channel: 1");
  SerialBT.println("Form: " + String(channels[1].form));
  SerialBT.println("Amplitude: " + String(channels[1].raw_amp));
  SerialBT.println("Frequency: " + String(channels[1].raw_freq));
  SerialBT.println("Phase: " + String(channels[1].raw_phase));
  SerialBT.println("Coef Amp: " + String(channels[1].coef_amp));
  SerialBT.println("Step Phase: " + String(channels[1].step_phase));
  SerialBT.println("Shift Phase: " + String(channels[1].shift_phase));
}

uint8_t CRC8(const uint8_t *data, uint8_t length) 
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

void dataReader()
{
  static bool data_reading = false;
  static uint8_t data_counter = 0;

  while(SerialBT.available()) 
  {
    uint8_t data_byte = SerialBT.read();

    if(!data_reading)
    {
      if(data_byte == 0xFF)
        data_reading = true;
    }
    else
    {
      if(data_byte == 0xFF)
        data_counter = 0;
      else
      {
        buf[data_counter] = data_byte;

        data_counter++;

        if(data_counter >= BUFFER_SIZE)
        {
          data_reading = false;
          data_counter = 0;

          if(buf[BUFFER_SIZE - 1] == CRC8(buf, BUFFER_SIZE - 1))
            bufHandler();
        }
      }
    }
  }
}
