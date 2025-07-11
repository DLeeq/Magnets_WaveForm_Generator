#include "driver/dac.h"

#include "receiver.h"
#include "generator.h"

void bufferHandler(uint8_t *buf, uint8_t buf_size);
void printChannelsInfo();

void setup() 
{
  recvInit("ESP32"); //Инициализация приемника блютуз
  genInit(); //Инициализация генератора на каналах цап и обнуление значений полей каналов генерации
}

void loop() 
{
  static uint8_t buffer[8];
  SDC8Reading(0xFF, 6, buffer, 8, bufferHandler);  
}

//USER FUNCS

void bufferHandler(uint8_t *buf, uint8_t buf_size)
{
  uint8_t ch_num = buf[0] >> 4;
  FormChannel form = (FormChannel)(buf[0] & 0xF);

  //Сырые значения амплитуды, частоты и фазы (0...254, 0...100000, 0...254)
  uint8_t raw_amp = buf[1];
  uint32_t raw_freq = buf[2] * 255 * 255 + buf[3] * 255 + buf[4];
  uint8_t raw_phase = buf[5];

  //Обработанные значения амплитуды, частоты и фазы (0...100.00, 0...1000.00, 0...360.00)
  float amp = raw_amp * 100.0 / 254;
  float freq = raw_freq * 1000.0 / 100000;
  float phase = raw_phase * 360.0 / 254;

  //Установка параметров канала
  genChSet(ch_num, form, amp, freq, phase);

  //Синхронизация каналов
  noInterrupts();
  genSync(); 
  interrupts();

  //Вывод информации о каналах в терминал
  printChannelsInfo();
}

void printChannelsInfo()
{
  SerialBT.println("                   -------CHANNELS INFO-------");
  for(uint8_t i = 0; i < CHANNELS_COUNT; i++)
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

    SerialBT.println("    Amplitude: " + String(channels[i].amp, 1) + "% (" + String(3.3 * channels[i].amp / 100.0, 1) + "V | " + String(12.0 * channels[i].amp / 100.0, 1) + "V)");
    SerialBT.println("    Frequency: " + String(channels[i].freq, 2) + "Hz");
    SerialBT.println("    Phase: " + String(channels[i].phase, 0) + "°");
    SerialBT.println("    Coef Amp: " + String(channels[i].coef_amp));
    SerialBT.println("    Step Phase: " + String(channels[i].step_phase));
    SerialBT.println("    Shift Phase: " + String(channels[i].shift_phase));

    SerialBT.println("----------------------------");
  }
}