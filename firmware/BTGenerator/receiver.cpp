#include "receiver.h"

BluetoothSerial SerialBT;

uint8_t buf[BUFFER_SIZE];

void bufHandler()
{
  uint8_t ch_num = buf[0] >> 4;

  channels[ch_num].form = (FormChannel)(buf[0] & 0xF);

  //Сырые значения амплитуды, частоты и фазы (0...254, 0...100000, 0...254)
  uint8_t raw_amp = buf[1];
  uint32_t raw_freq = buf[2] * 255 * 255 + buf[3] * 255 + buf[4];
  uint8_t raw_phase = buf[5];

  //Обработанные значения амплитуды, частоты и фазы (0...100, 0...1000.00, 0...360)
  channels[ch_num].amp = raw_amp * 100 / 254;
  channels[ch_num].freq = raw_freq / 100.0;
  channels[ch_num].phase = raw_phase * 360 / 254;

  //Предварительные вычисления удобных значений
  //для ускорения работы прерываний
  channels[ch_num].coef_amp = raw_amp; //(0...254)
  channels[ch_num].step_phase = raw_freq * 100 * 32 / 3200; //(0...100000)
  channels[ch_num].shift_phase = raw_phase * 3200000 / 254; //(0...3200000)

  noInterrupts();

  channelsSync(); //Синхронизация каналов

  interrupts();

  //ВЫВОД ЗНАЧЕНИЙ КАНАЛОВ В ТЕРМИНАЛ
  SerialBT.println("                   -------CHANNELS INFO-------");
  for(uint8_t i = 0; i < 2; i++)
  {
    SerialBT.println("Channel: " + String(i));

    SerialBT.print("    Form: ");
    switch(channels[i].form)
    {
      case OFF : SerialBT.println("OFF"); break;
      case SIN : SerialBT.println("SIN"); break;
      case TRG : SerialBT.println("TRG"); break;
      case SAW : SerialBT.println("SAW"); break;
      case SQR : SerialBT.println("SQR"); break;
    }

    SerialBT.println("    Amplitude: " + String(channels[i].amp) + "% (" + String(3.3 * channels[i].amp / 100, 1) + "V | " + String(12.0 * channels[i].amp / 100, 1) + "V)");
    SerialBT.println("    Frequency: " + String(channels[i].freq) + "Hz");
    SerialBT.println("    Phase: " + String(channels[i].phase) + "°");
    SerialBT.println("    Coef Amp: " + String(channels[i].coef_amp));
    SerialBT.println("    Step Phase: " + String(channels[i].step_phase));
    SerialBT.println("    Shift Phase: " + String(channels[i].shift_phase));

    SerialBT.println("----------------------------");
  }
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
