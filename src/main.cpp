#include <Arduino.h>
#include <avr/eeprom.h>


#include "ATtinySerialOut.h"

uint8_t EEP_4WD_ON = 0;
uint8_t EEP_4WD_LOOK = 1;

#define PIN_4WD_ON 0
#define PIN_4WD_LOOK 1
#define PIN_4WD_ON_LED 2

#define PIN_BTN_4WD A3
#define PIN_RUCHNIK A2

#define HOLD_BTN 1000
#define CLICK_BTN 50

int LastBtn = 0;

long LastDownBtn = 0, LastUpBtn = 0;

bool WD_On = false;
bool WD_Look = false;
bool RUCHNIK_On = false;
bool Btn_Press = false;
bool Btn_4WD_Hold = false;

long TimeLoop;

void setup()
{

  pinMode(PIN_4WD_ON, OUTPUT);
  pinMode(PIN_4WD_LOOK, OUTPUT);
  pinMode(PIN_4WD_ON_LED, OUTPUT);

  
  WD_On = eeprom_read_byte(&EEP_4WD_ON);
  WD_Look = eeprom_read_byte(&EEP_4WD_LOOK);
}

void loop()
{

  long ThisMillis = millis();

  if (!digitalRead(PIN_RUCHNIK))
  {
    RUCHNIK_On = true;
  }
  else
  {
    RUCHNIK_On = false;
  }

  int Btn_4WD = digitalRead(PIN_BTN_4WD);
  //int Btn_4WD = analogRead(PIN_BTN_4WD);
  if (LastBtn > Btn_4WD)
  {
    LastDownBtn = ThisMillis;
    LastUpBtn = ThisMillis;
    Btn_Press = true;
  }
  if (LastBtn < Btn_4WD)
  {
    LastUpBtn = ThisMillis;
    Btn_Press = false;
    if (Btn_4WD_Hold)
    {
      Btn_4WD_Hold = false;
      LastDownBtn = ThisMillis;
      LastUpBtn = ThisMillis;
    }
  }

  // Подсчет времени удержания кнопки
  //if (Btn_4WD < 900 && !Btn_4WD_Hold)
  if (!Btn_4WD && !Btn_4WD_Hold)
  {
    LastUpBtn = ThisMillis;
  }

  LastBtn = Btn_4WD;

  // Включение/отключение 4WD
  if (LastUpBtn - LastDownBtn > HOLD_BTN)
  {
    LastDownBtn = ThisMillis;
    WD_On = !WD_On;
    if (!WD_On)
    {
      WD_Look = false;
      eeprom_write_byte(&EEP_4WD_LOOK, WD_Look);
    }
    eeprom_write_byte(&EEP_4WD_ON, WD_On);
    Btn_4WD_Hold = true;
    //Btn_Press= false;
  }

  // Включение/отключение 4WD Look
  if (LastUpBtn - LastDownBtn > CLICK_BTN && !Btn_Press)
  {
    LastDownBtn = LastUpBtn;
    if (WD_On)
    {
      WD_Look = !WD_Look;
      eeprom_write_byte(&EEP_4WD_LOOK, WD_Look);
    }
    Btn_Press = false;
  }

  if (RUCHNIK_On)
  {
    // Отключение 4WD если ручник поднят
    digitalWrite(PIN_4WD_ON, LOW);
    digitalWrite(PIN_4WD_LOOK, LOW);
  }
  else
  {
    // Запись состояния 4WD
    if (WD_On)
      digitalWrite(PIN_4WD_ON, HIGH);
    else
      digitalWrite(PIN_4WD_ON, LOW);
    if (WD_Look)
      digitalWrite(PIN_4WD_LOOK, HIGH);
    else
      digitalWrite(PIN_4WD_LOOK, LOW);
  }

  // Включение подсветки если 4WD включен
  /*if (WD_On)
  {
    digitalWrite(PIN_4WD_ON_LED, HIGH);
  }
  else
  {
    digitalWrite(PIN_4WD_ON_LED, LOW);
  }*/
}
