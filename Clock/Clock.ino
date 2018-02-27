/*
 *  Clock - Using LCD 1602 shield and Timer1 interrupts (no RTC needed).
 *
 *  v1.0.0, 2018-02-27, Lars Lindehaven.
 *      Date, day of week, ISO week number, time and timer.
 *      Adjustment with microsecond resolution.
 *
 */

#include <LiquidCrystal.h>
#include <TimerOne.h>

#define APP_TITLE  "Clock v1.0.0    "
#define APP_AUTHOR "Lars Lindehaven "
#define MICRO_SECS 1000280L
#define DEBUG      1
#define LCDS_PIN   A0
#define TIMER_EXP  10

enum State {
  FSM_RUN,
  FSM_DATE_YEAR,
  FSM_DATE_MONTH,
  FSM_DATE_DAY,
  FSM_TIME_HOUR,
  FSM_TIME_MINUTE,
  FSM_TIME_SECOND,
  FSM_TIMER_HOUR,
  FSM_TIMER_MINUTE,
  FSM_TIMER_SECOND,
  FSM_ADJUST
};
#define STATES 10

enum {
  VAL_OK,
  VAL_TOO_HIGH,
  VAL_TOO_LOW
};

enum {
  BTN_NONE,
  BTN_SELECT,
  BTN_LEFT,
  BTN_UP,
  BTN_DOWN,
  BTN_RIGHT
};

typedef struct {
  int year;
  int month;
  int day;
} Date;

typedef struct {
  int hour;
  int minute;
  int second;
} Time;

typedef struct {
  int totSeconds;
  int expSeconds;
  Time time;
} Timer;

LiquidCrystal myLCD(8, 9, 4, 5, 6, 7);
Date myDate = {2018, 2, 27};
Time myTime = {1, 0, 0};
Timer myTimer = {0, TIMER_EXP, {0, 0, 0}};
Timer myTimerPreset = {0, TIMER_EXP, {0, 0, 0}};
int myState = FSM_RUN;
long int myMicroSecs = MICRO_SECS;

static int setYear(Date &d, int year)
{
  if (year < 2018)
  {
    d.year = 2018;
    return VAL_TOO_LOW;
  }
  else if (year > 2099)
  {
    d.year = 2099;
    return VAL_TOO_HIGH;
  }
  else
  {
    d.year = year;
    return VAL_OK;
  }
}

static int setMonth(Date &d, int month)
{
  if (month < 1)
  {
    d.month = 1;
    return VAL_TOO_LOW;
  }
  else if (month > 12)
  {
    d.month = 12;
    return VAL_TOO_HIGH;
  }
  else
  {
    d.month = month;
    return VAL_OK;
  }
}

static int setDay(Date &d, int day)
{
  if (day < 1)
  {
    d.day = 1;
    return VAL_TOO_LOW;
  }
  else
  {
    switch (d.month)
    {
      case 2:
        if (day > 28 && d.year % 4)
        {
          d.day = 28;
          return VAL_TOO_HIGH;
        }
        else if (day > 29)
        {
          d.day = 29;
          return VAL_TOO_HIGH;
        }
        break;
      case 4: case 6: case 9: case 11:
        if (day > 30)
        {
          d.day = 30;
          return VAL_TOO_HIGH;
        }
        break;
      default:
        if (day > 31)
        {
          d.day = 31;
          return VAL_TOO_HIGH;
        }
        break;
    }
    d.day = day;
    return VAL_OK;
  }
}

static int setHour(Time &t, int hour)
{
  if (hour < 0)
  {
    t.hour = 0;
    return VAL_TOO_LOW;
  }
  else if (hour > 23)
  {
    t.hour = 23;
    return VAL_TOO_HIGH;
  }
  else
  {
    t.hour = hour;
    return VAL_OK;
  }
}

static int setMinute(Time &t, int minute)
{
  if (minute < 0)
  {
    t.minute = 0;
    return VAL_TOO_LOW;
  }
  else if (minute > 59)
  {
    t.minute = 59;
    return VAL_TOO_HIGH;
  }
  else
  {
    t.minute = minute;
    return VAL_OK;
  }
}

static int setSecond(Time &t, int second)
{
  if (second < 0)
  {
    t.second = 0;
    return VAL_TOO_LOW;
  }
  else if (second > 59)
  {
    t.second = 59;
    return VAL_TOO_HIGH;
  }
  else
  {
    t.second = second;
    return VAL_OK;
  }
}

static int setTimerHour(Timer &t, int hour)
{
  if (hour < 0)
  {
    t.time.hour = 0;
    setTimerTotSeconds(t);
    return VAL_TOO_LOW;
  }
  else if (hour > 9)
  {
    t.time.hour = 9;
    setTimerTotSeconds(t);
    return VAL_TOO_HIGH;
  }
  else
  {
    t.time.hour = hour;
    setTimerTotSeconds(t);
    return VAL_OK;
  }
}

static int setTimerMinute(Timer &t, int minute)
{
  if (minute < 0)
  {
    t.time.minute = 0;
    setTimerTotSeconds(t);
    return VAL_TOO_LOW;
  }
  else if (minute > 59)
  {
    t.time.minute = 59;
    setTimerTotSeconds(t);
    return VAL_TOO_HIGH;
  }
  else
  {
    t.time.minute = minute;
    setTimerTotSeconds(t);
    return VAL_OK;
  }
}

static int setTimerSecond(Timer &t, int second)
{
  if (second < 0)
  {
    t.time.second = 0;
    setTimerTotSeconds(t);
    return VAL_TOO_LOW;
  }
  else if (second > 59)
  {
    t.time.second = 59;
    setTimerTotSeconds(t);
    return VAL_TOO_HIGH;
  }
  else
  {
    t.time.second = second;
    setTimerTotSeconds(t);
    return VAL_OK;
  }
}

static void setTimerTotSeconds(Timer &t)
{
  t.totSeconds = 3600 * t.time.hour + 60 * t.time.minute + t.time.second;
}

static void copyTimer(Timer &td, Timer ts)
{
  td.totSeconds = ts.totSeconds;
  td.expSeconds = ts.expSeconds;
  td.time.hour = ts.time.hour;
  td.time.minute = ts.time.minute;
  td.time.second = ts.time.second;
}

static void addSecondToDateTime(Date &d, Time &t)
{
  if (setSecond(t, ++t.second) == VAL_TOO_HIGH)
  {
    setSecond(t, 0);
    if (setMinute(t, ++t.minute) == VAL_TOO_HIGH)
    {
      setMinute(t, 0);
      if (setHour(t, ++t.hour) == VAL_TOO_HIGH)
      {
        setHour(t, 0);
        if (setDay(d, ++d.day) == VAL_TOO_HIGH)
        {
          setDay(myDate, 1);
          if (setMonth(d, ++d.month) == VAL_TOO_HIGH)
          {
            setMonth(d, 1);
            if (setYear(d, ++d.year) == VAL_TOO_HIGH)
            {
              d = {2099, 12, 31};
              t = {23, 59, 59};
            }
          }
        }
      }
    }
  }
}

static void subtractSecondFromTimer(Timer &t)
{
  if (t.totSeconds)
  {
    if (--t.totSeconds <= 0)
    {
      t.totSeconds = 0;
      t.time = {0, 0, 0};
    }
    else
    {
      t.time.hour = t.totSeconds / 3600;
      t.time.minute = (t.totSeconds - t.time.hour * 3600) / 60;
      t.time.second = t.totSeconds % 60;
    }
  }
}

static int getDayOfWeek(int y, int m, int d)
{
  static int tv[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  y -= m < 3;
  return (y + y/4 - y/100 + y/400 + tv[m-1] + d - 1) % 7;
}

static int getWeekNumber(int y, int m, int d)
{
  int t1 = getDayOfWeek(y, m, d);
  int t2 = getDayOfWeek(y, 1, 1);
  int t3 = getDayOfWeek(y+1, 1, 1);

  if (m == 1 && t2 > 3 && t2 < 7-(d-1))
    return getWeekNumber(y-1, 12, 31);
  else if (m == 12 && t3 > 30-(d-1) && t3 < 4)
    return 1;
  else
    return (t2 < 4) + 4*(m-1) + (2*(m-1) + (d-1) + t2 - t1 + 6) * 36/256;
}

static int readButton(int adcButtonIn)
{
  int adcKeyIn = analogRead(adcButtonIn);
  if (adcKeyIn > 1000) return BTN_NONE;
  if (adcKeyIn < 50)   return BTN_RIGHT; 
  if (adcKeyIn < 195)  return BTN_UP;
  if (adcKeyIn < 380)  return BTN_DOWN;
  if (adcKeyIn < 555)  return BTN_LEFT;
  if (adcKeyIn < 790)  return BTN_SELECT;  
  return BTN_NONE;
}

static void displayDate(Date d)
{
  myLCD.setCursor(0, 0);
  myLCD.print(d.year - 2000); myLCD.print("-");
  if (d.month < 10) myLCD.print("0");
  myLCD.print(d.month); myLCD.print("-");
  if (d.day < 10) myLCD.print("0");
  myLCD.print(d.day);
#ifdef DEBUG
  if (myState == FSM_RUN)
  {
    Serial.print("Date=");
    Serial.print(d.year); Serial.print("-");
    if (d.month < 10) Serial.print("0");
    Serial.print(d.month); Serial.print("-");
    if (d.day < 10) Serial.print("0");
    Serial.print(d.day); Serial.print(" ");
  }
#endif
}

static void displayDayOfWeek(Date d)
{
  int weekDay = getDayOfWeek(d.year, d.month, d.day);

  myLCD.setCursor(9, 0);
  switch (weekDay)
  {
    case 0: myLCD.print("Mon"); break;
    case 1: myLCD.print("Tue"); break;
    case 2: myLCD.print("Wed"); break;
    case 3: myLCD.print("Thu"); break;
    case 4: myLCD.print("Fri"); break;
    case 5: myLCD.print("Sat"); break;
    case 6: myLCD.print("Sun"); break;
    default: myLCD.print("???"); break;
  }
#ifdef DEBUG
  if (myState == FSM_RUN)
  {
    Serial.print("Day=");
    Serial.print(weekDay);
    Serial.print(" ");
  }
#endif
}

static void displayWeekNumber(Date d)
{
  int weekNumber = getWeekNumber(d.year, d.month, d.day);

  myLCD.setCursor(13, 0);
  myLCD.print("w");
  if (weekNumber < 10) myLCD.print("0");
  myLCD.print(weekNumber);
#ifdef DEBUG
  if (myState == FSM_RUN)
  {
    Serial.print("Week=");
    Serial.print(weekNumber);
    Serial.print(" ");
  }
#endif
}

static void displayTime(Time t)
{
  myLCD.setCursor(0, 1);
  if (t.hour < 10) myLCD.print("0");
  myLCD.print(t.hour); myLCD.print(":");
  if (t.minute < 10) myLCD.print("0");
  myLCD.print(t.minute); myLCD.print(":");
  if (t.second < 10) myLCD.print("0");
  myLCD.print(t.second);
#ifdef DEBUG
  if (myState == FSM_RUN)
  {
    Serial.print("Time=");
    if (t.hour < 10) Serial.print("0");
    Serial.print(t.hour); Serial.print(":");
    if (t.minute < 10) Serial.print("0");
    Serial.print(t.minute); Serial.print(":");
    if (t.second < 10) Serial.print("0");
    Serial.print(t.second); Serial.print(" ");
  }
#endif
}

static void displayTimer(Timer t)
{
  myLCD.setCursor(9, 1);
  myLCD.print(t.time.hour); myLCD.print(":");
  if (t.time.minute < 10) myLCD.print("0");
  myLCD.print(t.time.minute); myLCD.print(":");
  if (t.time.second < 10) myLCD.print("0");
  myLCD.print(t.time.second);
#ifdef DEBUG
  if (myState == FSM_RUN)
  {
    Serial.print("Timer=");
    Serial.print(t.time.hour); Serial.print(":");
    if (t.time.minute < 10) Serial.print("0");
    Serial.print(t.time.minute); Serial.print(":");
    if (t.time.second < 10) Serial.print("0");
    Serial.print(t.time.second); Serial.print(" ");
    Serial.print("uSecs=");
    Serial.print(myMicroSecs); Serial.print(" ");
  }
#endif
}

static void displayAll(Date d, Time t, Timer tr)
{
  displayDate(d);
  displayDayOfWeek(d);
  displayWeekNumber(d);
  displayTime(t);
  displayTimer(tr);
}

static void setTimer1(long int microSecs)
{
  noInterrupts();
  if (microSecs > 990000L && microSecs < 1010000L)
    myMicroSecs = microSecs;
  else
    myMicroSecs = MICRO_SECS;
  Timer1.initialize(myMicroSecs);
  Timer1.attachInterrupt(ISR_SecondPassed);
  interrupts(); 
}

static void displayTimer1()
{
  myLCD.setCursor(0, 0);
  myLCD.print("Set microsec/sec");
  myLCD.setCursor(0, 1);
  myLCD.print(myMicroSecs);
}

#ifdef DEBUG
void serialEvent(void)
{
  int byteRead;

  if (Serial.available() > 0)
  {
    byteRead = Serial.read();
    switch (byteRead)
    {
      case '1': myLCD.display(); break;
      case '0': myLCD.noDisplay(); break;
      default: Serial.print(byteRead); Serial.println(" is not a valid command!");
    }
  }
}
#endif

void ISR_SecondPassed(void)
{
  if (myState != FSM_TIME_HOUR &&
      myState != FSM_TIME_MINUTE &&
      myState != FSM_TIME_SECOND)
  {
    addSecondToDateTime(myDate, myTime);
  }
  if (myState != FSM_TIMER_HOUR &&
      myState != FSM_TIMER_MINUTE &&
      myState != FSM_TIMER_SECOND)
  {
    subtractSecondFromTimer(myTimer);
    if (myTimer.totSeconds <= myTimer.expSeconds && myTimer.totSeconds % 2)
      myLCD.noDisplay();
    else
      myLCD.display();
  }
  if (myState == FSM_RUN)
  {
    displayAll(myDate, myTime, myTimer);
  }
#ifdef DEBUG
  Serial.print("State=");
  Serial.print(myState);
  Serial.println();
#endif
}

void setup(void)
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println(APP_TITLE);
  Serial.println(APP_AUTHOR);
#endif
  myLCD.begin(16, 2);
  myLCD.clear();
  myLCD.setCursor(0, 0);
  myLCD.print(APP_TITLE);
  myLCD.setCursor(0, 1);
  myLCD.print(APP_AUTHOR);
  delay(2000);
  myLCD.clear();
  setTimer1(MICRO_SECS);
}

void loop(void)
{
  int button, change;

  delay(150);
  button = readButton(LCDS_PIN);
  switch (button)
  {
    case BTN_SELECT:
      myState = FSM_RUN;
      change = 0;
      myLCD.noCursor();
      myLCD.noBlink();
      copyTimer(myTimer, myTimerPreset);
      break;
    case BTN_LEFT:
      if (--myState < FSM_DATE_YEAR) myState = FSM_ADJUST; 
      change = 0;
      myLCD.clear();
      break;
    case BTN_RIGHT:
      if (++myState > FSM_ADJUST) myState = FSM_DATE_YEAR; 
      change = 0;
      myLCD.clear();
      break;
    case BTN_UP:
      if (myState != FSM_RUN) change = 1;
      break;
    case BTN_DOWN:
      if (myState != FSM_RUN) change = -1;
      break;
    default:
      ;
  }
  if (myState != FSM_RUN)
  {
    myLCD.cursor();
    myLCD.blink();
    switch (myState)
    {
      case FSM_DATE_YEAR:
        setYear(myDate, myDate.year + change);
        setMonth(myDate, myDate.month);
        setDay(myDate, myDate.day);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(1, 0);
        break;
      case FSM_DATE_MONTH:
        setMonth(myDate, myDate.month + change);
        setDay(myDate, myDate.day);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(4, 0);
        break;
      case FSM_DATE_DAY:
        setDay(myDate, myDate.day + change);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(7, 0);
        break;
      case FSM_TIME_HOUR:
        setHour(myTime, myTime.hour + change);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(1, 1);
        break;
      case FSM_TIME_MINUTE:
        setMinute(myTime, myTime.minute + change);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(4, 1);
        break;
      case FSM_TIME_SECOND:
        setSecond(myTime, myTime.second + change);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(7, 1);
        break;
      case FSM_TIMER_HOUR:
        setTimerHour(myTimer, myTimer.time.hour + change);
        copyTimer(myTimerPreset, myTimer);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(9, 1);
        break;
      case FSM_TIMER_MINUTE:
        setTimerMinute(myTimer, myTimer.time.minute + change);
        copyTimer(myTimerPreset, myTimer);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(12, 1);
        break;
      case FSM_TIMER_SECOND:
        setTimerSecond(myTimer, myTimer.time.second + change);
        copyTimer(myTimerPreset, myTimer);
        displayAll(myDate, myTime, myTimer);
        myLCD.setCursor(15, 1);
        break;
      case FSM_ADJUST:
        setTimer1(myMicroSecs + change);
        displayTimer1();
        myLCD.setCursor(7, 1);
        break;
      default:
        ;
    }
  }
}
