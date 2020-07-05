#include <Arduino.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <dht.h>
#include <LiquidCrystal.h>
#include <Ds1302.h>
#include <string.h>

// Thermometer & Humidity PIN
#define THERMO_HUM_PIN PIN_A5

// Relay Definitions
#define PIN_ROLE1 PIN_A1
#define PIN_ROLE2 PIN_A2
#define PIN_ROLE3 PIN_A3
#define PIN_ROLE4 PIN_A4

dht DHT;

// Home Screen Toggle Time
#define HOME_SCREEN_TOGGLE_TIME 5 // Second

// Menu Indexes
#define MAIN_MENU 1000
#define ROLE_1 1001
#define ROLE_2 1002
#define ROLE_3 1003
#define ROLE_4 1004

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Ds1302 rtc(13, 11, 12);

int buttonValue;
int currentMenu = 1000;
int temperature, humidity = 0;
String day, month, year, hour, minute, second, dayOfWeek;
String currentDate, currentTime;
uint64_t HomeScreenTimer;
boolean ROLE1_ACTIVE, ROLE2_ACTIVE, ROLE3_ACTIVE, ROLE4_ACTIVE = false;

byte ROLE_DURUM_AKTIF[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
byte ROLE_DURUM_PASIF[8] = {B11111, B10001, B10001, B10001, B10001, B10001, B10001, B11111};

void SetDateTime(uint8_t dayValue, uint8_t monthValue, uint8_t yearValue, uint8_t hourValue, uint8_t minuteValue, uint8_t secondsValue, uint8_t dayOfWeekValue)
{
  Ds1302::DateTime dt = {
      .year = yearValue,
      .month = monthValue,
      .day = dayValue,
      .hour = hourValue,
      .minute = minuteValue,
      .second = secondsValue,
      .dow = dayOfWeekValue};
  rtc.setDateTime(&dt);
}

const static char *WeekDays[] =
    {
        "PT",
        "SA",
        "CA",
        "PE",
        "CU",
        "CT",
        "PZ"};
void Sleep()
{
  sleep_enable();
  // attachInterrupt(0, WakeUp, LOW);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  sleep_cpu();
}
void WakeUp()
{
  sleep_disable();
  // detachInterrupt(0);
}

void ButtonListener()
{
  if (millis() - HomeScreenTimer >= (HOME_SCREEN_TOGGLE_TIME * 1000 * 2))
  {
    HomeScreenTimer = millis();
  }
  buttonValue = analogRead(PIN_A0);
  // Serial.println(buttonValue);
  delay(150);
  if (1000 < buttonValue && buttonValue < 1040)
  {
    // Idle State
  }
  else if (620 < buttonValue && buttonValue < 660)
  {
    lcd.setCursor(0, 0);
    lcd.print("SELECT          ");
  }
  else if (80 < buttonValue && buttonValue < 120)
  {
    lcd.setCursor(0, 0);
    lcd.print("UP              ");
  }
  else if (237 < buttonValue && buttonValue < 277)
  {
    lcd.setCursor(0, 0);
    lcd.print("DOWN            ");
  }
  else if (380 < buttonValue && buttonValue < 420)
  {
    if (!(currentMenu <= MAIN_MENU))
    {
      currentMenu--;
      lcd.clear();
    }
  }
  else if (0 < buttonValue && buttonValue < 40)
  {
    if (!(currentMenu >= ROLE_4))
    {
      currentMenu++;
      lcd.clear();
      lcd.print(currentMenu);
    }
  }
}
void ConcatDateTime()
{
  // Get Date & Time
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  static uint8_t last_second = 0;
  if (last_second != now.second)
  {
    last_second = now.second;

    year = "20" + String(now.year);
    if (now.month < 10)
    {
      month = "0" + String(now.month);
    }
    else
    {
      month = String(now.month);
    }
    if (now.day < 10)
    {
      day = "0" + String(now.day);
    }
    else
    {
      day = String(now.day);
    }
    dayOfWeek = WeekDays[now.dow - 1];
    if (now.hour < 10)
    {
      hour = "0" + String(now.hour);
    }
    else
    {
      hour = String(now.hour);
    }
    if (now.minute < 10)
    {
      minute = "0" + String(now.minute);
    }
    else
    {
      minute = String(now.minute);
    }
    if (now.second < 10)
    {
      second = "0" + String(now.second);
    }
    else
    {
      second = String(now.second);
    }
    currentDate = day + "." + month + "." + year;
    currentTime = hour + ":" + minute + ":" + second;
  }
  delay(750);
  // End Date & Time
}
void MainMenu()
{
  ConcatDateTime();
  lcd.setCursor(0, 0);
  if (millis() - HomeScreenTimer >= (HOME_SCREEN_TOGGLE_TIME * 1000))
  {
    lcd.print(currentDate + "  " + String(temperature) + String((char)223) + "C");
  }
  else
  {
    lcd.print(currentDate + "   %" + String(humidity));
  }
  lcd.setCursor(0, 1);
  lcd.print(currentTime + " " + dayOfWeek + " ");
  lcd.print(char(0));
  lcd.print(char(1));
  lcd.print(char(1));
  lcd.print(char(0));
}

void SerialControl()
{
  if (Serial.available() > 0)
  {
    switch ((char)Serial.read())
    {
    case '1':
      if (ROLE1_ACTIVE)
      {
        digitalWrite(A1, HIGH);
      }
      else
      {
        digitalWrite(A1, LOW);
      }
      ROLE1_ACTIVE = !ROLE1_ACTIVE;
      break;
    case '2':
      if (ROLE2_ACTIVE)
      {
        digitalWrite(A2, HIGH);
      }
      else
      {
        digitalWrite(A2, LOW);
      }
      ROLE2_ACTIVE = !ROLE2_ACTIVE;
      break;
    case '3':
      if (ROLE3_ACTIVE)
      {
        digitalWrite(A3, HIGH);
      }
      else
      {
        digitalWrite(A3, LOW);
      }
      ROLE3_ACTIVE = !ROLE3_ACTIVE;
      break;
    case '4':
      if (ROLE4_ACTIVE)
      {
        digitalWrite(A4, HIGH);
      }
      else
      {
        digitalWrite(A4, LOW);
      }
      ROLE4_ACTIVE = !ROLE4_ACTIVE;
      break;
    case 'n':
      if (!(currentMenu >= ROLE_4))
      {
        currentMenu++;
        lcd.clear();
      }
      break;
    case 'p':
      if (!(currentMenu <= MAIN_MENU))
      {
        currentMenu--;
        lcd.clear();
      }
      break;
    }
  }
}

void RoleOneMenu()
{
  lcd.clear();
  lcd.print("ROLE 1");
}
void RoleTwoMenu()
{
  lcd.clear();
  lcd.print("ROLE 2");
}
void RoleThreeMenu()
{
  lcd.clear();
  lcd.print("ROLE 3");
}
void RoleFourMenu()
{
  lcd.clear();
  lcd.print("ROLE 4");
}

void ShowMenu(int MenuID)
{
  switch (MenuID)
  {
  case MAIN_MENU:
    MainMenu();
    break;
  case ROLE_1:
    RoleOneMenu();
    break;
  case ROLE_2:
    RoleTwoMenu();
    break;
  case ROLE_3:
    RoleThreeMenu();
    break;
  case ROLE_4:
    RoleFourMenu();
    break;
  default:
    break;
  }
}
void setup()
{
  rtc.init();
  HomeScreenTimer = millis();
  lcd.begin(16, 2);
  lcd.createChar(0, ROLE_DURUM_PASIF);
  lcd.createChar(1, ROLE_DURUM_AKTIF);
  lcd.print("SISTEM BASLIYOR");
  Serial.begin(9600);
  pinMode(PIN_A1, OUTPUT);
  pinMode(PIN_A2, OUTPUT);
  pinMode(PIN_A3, OUTPUT);
  pinMode(PIN_A4, OUTPUT);

  // Relay Power
  digitalWrite(PIN_A1, HIGH);
  digitalWrite(PIN_A2, HIGH);
  digitalWrite(PIN_A3, HIGH);
  digitalWrite(PIN_A4, HIGH);
  // SetDateTime(6, 7, 20, 1, 51, 30, 1);
}
void loop()
{
  if (currentMenu == 1000)
  {
    int chk = DHT.read11(PIN_A5);
    delay(100);
    temperature = int(DHT.temperature);
    humidity = int(DHT.humidity);
    Serial.println(String(temperature) + " " + String(humidity));
  }
  ButtonListener();
  ShowMenu(currentMenu);
  SerialControl();
}