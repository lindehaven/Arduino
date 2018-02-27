# ARDUINO

## Background

Got ymyself an Arduino Uno to tinker with. A small one-board computer that is
easy to develop hardware and software for. I like that. Here are my projects.

## Clock

### Hardware

* Arduino Uno
* Arduino LCD Shield 16x2

No RTC is needed. The clock runs as long as the power is ON.

### Software

* Clock.ino

Displays date, day of week, ISO week number, time and timer. Date, time and
timer can be set using the buttons on the LCD shield. The timer is stored as a
preset timer so that it can be restarted with a single click of a button. The
LCD screen blinks for 10 seconds before the timer expires. Any clock drift can
be eliminated with microsecond resolution adjustment.

Function of the buttons on the LCD shield:
* SELECT
  Starts the timer (if set). Leaves the "set mode" (if entered).
* LEFT / RIGHT
  Enters "set mode". Steps forward or backward through the fields:
  * Year
  * Month
  * Day
  * Hour
  * Minute
  * Second
  * Timer hour
  * Timer minute
  * Timer second
  * Microsecond/second 
* UP / DOWN
  Adjusts the field value up or down.
