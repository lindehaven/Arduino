# Clock

Displays date, day of week, ISO week number, time and timer. Date, time and
timer can be set using the buttons on the LCD shield. The timer is stored as a
preset timer so that it can be restarted with a single click of a button. The
LCD screen blinks for 10 seconds before the timer expires. Any clock drift can
be eliminated with microsecond resolution adjustment.

## Equipment

* Arduino Uno
* Arduino LCD Shield 16x2

No RTC is needed. The clock runs as long as the power is ON.

## Installation

* Insert the Arduino LCD Shield on top of the Arduino Uno.
* Connect the power to the Arduino Uno.
* Connect USB cable between Arduino Uno and PC.
* Clone `Clock` to PC from GitHub:
  `git clone https://github.com/lindehaven/Arduino.git`
* Open Arduino IDE on PC.
  * Open `Clock`.
  * Upload `Clock` to Arduino Uno.

## Function

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
