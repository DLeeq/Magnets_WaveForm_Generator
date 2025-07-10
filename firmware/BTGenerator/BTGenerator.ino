#include "driver/dac.h"

#include "receiver.h"
#include "generator.h"


hw_timer_t* timer = NULL;


void bufferHandler(uint8_t *buf, uint8_t buf_size);

void IRAM_ATTR onTimer() 
{
  genTicker();
}

void setup() 
{
  receiverInit("ESP32");
  genInit(); //Инициализация генератора на каналах цап и обнуление значений полей каналов генерации

  timer = timerBegin(32000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1, true, 0);
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

  genChannelsSync(); //Синхронизация каналов

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