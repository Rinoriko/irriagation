#include <Arduino.h>
 
// программа постоянно все пробегает, не использует delay
// дисплей отображает все время параметры в зависимости от mode
// единственное место остановки запись в EEPROM
 
//TO DO
//прикрутить: 
//- энкодер
//- полив
#include <TimerOne.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки дисплея
#include <iarduino_RTC.h>      // Подключение библиотеки RTC
 
#define TIME_LAG_BUTTON 100 //антидребезг кнопки в 100мс
#define PIN_FILL_SENSOR 3   // датчик наполнения на 3 контакт
#define PIN_BUTTON_1 4      //первая кнопка на 4 контакт
#define PIN_BUTTON_2 5      //первая кнопка на 5 контакт
#define PIN_RELEY_1 6       // реле клапан 1 парника на 6 контакт
#define PIN_RELEY_2 7       // реле клапан 2 парника на 7 контакт
#define PIN_RELEY_IN 8      // реле клапан впуска на 8 контакт
 
#define PIN_RST 11 // RST 11 пин
#define PIN_CLK 12 // CLK 12 пин
#define PIN_DAT 13 // DAT 13 пин
 
iarduino_RTC time(RTC_DS1302, PIN_RST, PIN_CLK, PIN_DAT); // для модуля DS1302 - RST, CLK, DAT
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой
 
uint8_t mode;
uint8_t type_error = 0;
// режимы:0-запись EERPROM,1-установка 1 парника,2-установка 2 парника, 6-полив 1 парника, 7-полив 2 парника, 8-ожидание, 9-установки времени, 10 - стоп ошибка 
uint8_t set_menu = 1;    //для проистывания меню при нажатии 1 кнопой
uint8_t change_menu = 0;
uint8_t click_1 = 0;     // клик по 1 кнопке
uint8_t click_2 = 0;     //клик по 2 кнопке
uint8_t double_clik = 0; // одновремнное нажатие двух кнопок
 
uint32_t btnTimer = 0;
 
uint8_t time_seconds;
uint8_t time_minutes;
uint8_t time_hours;
uint8_t time_day;
uint8_t time_month;
uint8_t time_year;
uint8_t time_week;
 
uint8_t set_min_1;
uint8_t set_hour_1;
uint8_t day_next_irrigation_1; // день след полива
uint8_t set_interval_1; //1 -раз в день,2 - раз в два дня, 3 - раз в три дня, 4 раз в четые дня, 5 раз в пять дней
uint8_t set_liters_1;
uint8_t day_last_irrigation_1; // день след полива
 
uint8_t set_min_2;
uint8_t set_hour_2;
uint8_t day_next_irrigation_2; // день  след полива
uint8_t set_interval_2; //1 -раз в день,2 - раз в два дня, 3 - раз в три дня, 4 раз в четые дня, 5 раз в пять дней
uint8_t set_liters_2;
uint8_t day_last_irrigation_2; // день след полива
 
int16_t value;
void setup()
{
  Serial.begin(9600);
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  initialization();
 
}
void loop()
{
  read_buttons();
  set_mode();
  set_value();
}
void set_value()
{
  //1-установка 1 парника,2-установка 2 парника, 9-установки времени,
  switch (mode)
  {
    case 1:
    break;
 
    case 2:
    break;
 
    case 9:
 
      switch (set_menu)
      {
      case 1:
      if (change_menu = 1)
      {
        change_menu = 0;
        value =  time_minutes;
      }
 
      break;
 
      case 2:
       if (change_menu = 1)
      {
        change_menu = 0;
        time_hours = time.Hours;
      }
      break;
 
      case 3:
       if (change_menu = 1)
      {
        change_menu = 0;
        time_day = time.day; 
      }
      break;
 
      case 4:
       if (change_menu = 1)
      {
        change_menu = 0;
        time_month = time.month;
      }
      break;
 
      case 5:
       if (change_menu = 1)
      {
        change_menu = 0;
        time_year = time.year;
      }
      break;
 
      case 6:
       if (change_menu = 1)
      {
        change_menu = 0;
        time_week = time.weekday;//(0-воскресенье, 6-суббота)
      }
      break;
    }
    break;
 
  }
}
void set_mode()
{
  
  if (double_clik == 1) //клик по двум кнопкам сразу
  {
    double_clik = 0;
    mode = 9;
  }
 
  if (click_1 == 1 && mode == 9)
  {
    click_1 = 0;
    if (set_menu >= 6)
    {
      set_menu = 1;
      set_time();
      mode = 8;
      change_menu = 1;
    }
    else
    {
      set_menu += 1;
      change_menu = 1;
    }
  }
 
  if (click_1 == 1 && mode != 1) //клик по 1 кнопке
  {
    click_1 = 0;
    mode = 1;
    set_menu = 1;
  }
  if (click_1 == 1 && mode == 1)
  {
    click_1 = 0;
    if (set_menu >= 3)
    {
      set_menu = 1;
      mode = 8;
      change_menu = 1;
    }
    else
    {
      set_menu += 1;
      change_menu = 1;
    }
 
    if (click_2 == 1 && mode != 2) //клик по 2 кнопке
    {
      click_2 = 0;
      mode = 2;
      set_menu = 1;
    }
    if (click_2 == 1 && mode == 2)
    {
      click_2 = 0;
      if (set_menu >= 3)
      {
        set_menu = 1;
        mode = 8;
        change_menu = 1;
      }
      else
      {
        set_menu += 1;
        change_menu = 1;
      }
    }
  }
}
 
void lcd_show() //вызываем по прерыванию 
{
  switch (mode)
  //0-запись EERPROM,1-установка 1 парника,2-установка 2 парника,
  //6-полив 1 парника, 7-полив 2 парника, 8-ожидание, 9-установки времени
  {
  case 0: //0-запись EERPROM
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SAVING DATA");
    lcd.setCursor(0, 1);
    lcd.print(" TO EEPROM");
  break;
 
  case 1: //1-установка 1 парника
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" PARNIK 1");
    lcd.setCursor(0, 1);
    if (set_hour_1 < 10) //время 5 знаков
    {
      lcd.print("0");
    }
    else
    {
      lcd.print(set_hour_1);
    }
    lcd.print(":");
 
    if (set_min_1 < 10)
    {
      lcd.print("0");
    }
    else
    {
      lcd.print(set_min_1);
    }
    lcd.print(" "); // пробел итого 6
 
    lcd.print(set_interval_1); //период 2 знака итого 8
 
    lcd.print(" "); //пробелитого 9
 
    if (set_liters_1 < 100)
    {
      lcd.print(" ");
    }
    if (set_liters_1 < 10)
    {
      lcd.print(" ");
    }
    lcd.print(set_liters_1); //литров 3 знака итого 12
  break;
 
  case 2: //2-установка 2 парника
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" PARNIK 2");
    lcd.setCursor(0, 1);
    if (set_hour_2 < 10) //время 5 знаков
    {
      lcd.print("0");
    }
    else
    {
      lcd.print(set_hour_2);
    }
    lcd.print(":");
 
    if (set_min_2 < 10)
    {
      lcd.print("0");
    }
    else
    {
      lcd.print(set_min_2);
    }
    lcd.print(" "); // пробел итого 6
 
    lcd.print(set_interval_2); //период 2 знака итого 8
 
    lcd.print(" "); //пробелитого 9
 
    if (set_liters_2 < 100)
    {
      lcd.print(" ");
    }
    if (set_liters_2 < 10)
    {
      lcd.print(" ");
    }
    lcd.print(set_liters_2); //литров 3 знака итого 12
  break;
 
  case 3:
  break;
 
  case 4:
  break;
 
  case 5:
  break;
 
  case 6: //6-полив 1 парника
  break;
 
  case 7: //7-полив 2 парника
  break;
 
  case 8: //8-ожидание
  if ((time.seconds/10)%2==0)
  {
    lcd.setCursor(0, 0);
    lcd.print("D");
    lcd.print(day_next_irrigation_1);
    lcd.print("T");
    lcd.print(set_hour_1);
    lcd.print(set_min_1);
    lcd.print("L");
    lcd.print(set_liters_1);
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("D");
    lcd.print(day_next_irrigation_2);
    lcd.print("T");
    lcd.print(set_hour_2);
    lcd.print(set_min_2);
    lcd.print("L");
    lcd.print(set_liters_2);
  }
  break;
 
  case 9: //9-установки времени
  break;
 
  case 10: //0-основка по ошибке
    lcd.setCursor(0, 0);
    lcd.print("STOP ERROR:");
    lcd.setCursor(0, 1);
    lcd.print(type_error);
  break;
  }
}
void get_time()
{
  time_seconds = time.seconds;
  time_minutes = time.minutes;
  time_hours = time.Hours;
  time_day = time.day; 
  time_month = time.month;
  time_year = time.year;
  time_week = time.weekday;//(0-воскресенье, 6-суббота)
}
void set_time()
{
 
  // Ниже у нас прописываются временные значения в виде секунд, минут, часов, даты, включая месяц и год, и также можете указать день недели
  //time.settime(10, 10, 10, 4, 10, 17, 3); // 10 сек, 10 мин, 10 час, 4 , октября, 2017 года, среда
}
void read_buttons() //TESTS PASSED
{
  uint8_t button_1 = 0;                                 //флаг 1 кнопки
  uint8_t button_2 = 0;                                 //флаг 2 кнопки
  uint8_t status_button_1 = !digitalRead(PIN_BUTTON_1); //инвертируем т.к. кнопка подтянута к 5в INPUT_PULLUP
  uint8_t status_button_2 = !digitalRead(PIN_BUTTON_2); //инвертируем т.к. кнопка подтянута к 5в INPUT_PULLUP
 
  if (status_button_1 && !button_1 && millis() - btnTimer > TIME_LAG_BUTTON)
  {
    button_1 = 1;
    btnTimer = millis();
  }
 
  if (!status_button_1 && button_1 && millis() - btnTimer > TIME_LAG_BUTTON)
  {
    button_1 = 0;
    btnTimer = millis();
    click_1 = 1;
  }
 
  if (status_button_2 && !button_2 && millis() - btnTimer > TIME_LAG_BUTTON)
  {
    button_2 = 1;
    btnTimer = millis();
  }
 
  if (!status_button_2 && button_2 && millis() - btnTimer > TIME_LAG_BUTTON)
  {
    button_2 = 0;
    btnTimer = millis();
    click_2 = 1;
  }
  if (button_1 == 1 && button_2 == 1)
  {
    button_1 = 0;
    button_2 = 0;
    double_clik = 1;
  }
}
void initialization()
{
  mode  = 9;
  lcd.init();      // Инициализация дисплея
  lcd.backlight(); // Подключение подсветки
  time.begin(); // функция служит для инициализации работы модуля
  Timer1.initialize(100000); // таймер 0.1 сек
  Timer1.attachInterrupt(lcd_show); //


 
// адреса первого парника:         адреса втого паника
// 11 - минут начала полива        21 - минут начала полива
// 12 - часов начала полива        22 - часов начала полива
// 13 - количество литров          23 - количество литров
// 14 - интервал полива            24 - - интервал полива (в днях)
// 15 - дата последнеого полива    25 - дата последнеого полива 
// 16 - дата след полива           26 - дата след полива
  EEPROM.get(11, set_min_1);
  EEPROM.get(12, set_hour_1);
  EEPROM.get(13, set_liters_1);
  EEPROM.get(14, set_interval_1);
  EEPROM.get(15, day_last_irrigation_1);
  EEPROM.get(16, day_next_irrigation_1);
 
  EEPROM.get(21, set_min_2);
  EEPROM.get(22, set_hour_2);
  EEPROM.get(23, set_liters_2);
  EEPROM.get(24, set_interval_2);
  EEPROM.get(25, day_last_irrigation_2);
  EEPROM.get(26, day_next_irrigation_2);
  
}
void seve_EEPROM()
// адреса первого парника:         адреса втого паника
// 11 - минут начала полива        21 - минут начала полива
// 12 - часов начала полива        22 - часов начала полива
// 13 - количество литров          23 - количество литров
// 14 - интервал полива            24 - - интервал полива (в днях)
// 15 - дата последнеого полива    25 - дата последнеого полива 
// 16 - дата след полива           26 - дата след полива
{
  
  EEPROM.update(11, set_min_1);
  EEPROM.update(12, set_hour_1);
  EEPROM.update(13, set_liters_1);
  EEPROM.update(14, set_interval_1);
  EEPROM.update(15, day_last_irrigation_1);
  EEPROM.update(16, day_next_irrigation_1);
 
  EEPROM.update(21, set_min_2);
  EEPROM.update(22, set_hour_2);
  EEPROM.update(23, set_liters_2);
  EEPROM.update(24, set_interval_2);
  EEPROM.update(25, day_last_irrigation_2);
  EEPROM.update(26, day_next_irrigation_2);
}
int8_t get_day(int day_now, int month_now, int year, int interval)
//возвращает дату следеющего полива , интервал не более 5 дней
{
   int day_offset[12];
  if ((year%4 == 0 && year%100 != 0) || (year%400 == 0))
  {
    int day_offset[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  }
  else
  {
    int day_offset[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  } 
 
  if (day_offset[month_now-1] - (day_now + interval) >= 0 ) // если следущий полив в текущем месяце
  {
    return (day_now + interval);
  }
  else
  {
    return ((day_now + interval)-day_offset[month_now-1]);
  }
}