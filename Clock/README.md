# Clock

Displays date, day of week, ISO week number, time and timer. Date, time, timer
and clock drift adjustment can be set using the buttons on the LCD shield or
via the serial interface.

The timer is stored as a preset timer so that it can be restarted with a single
click of a button. The LCD screen blinks for 10 seconds before the timer
expires.

Any clock drift can be eliminated with microsecond resolution adjustment.

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
  * `Year` = {18..99}
  * `Month` = {1..12}
  * `Day` = {1..31}
  * `Hour` = {0..23}
  * `Minute` = {0..59}
  * `Second` = {0..59}
  * `Timer hour` = {0..9}
  * `Timer minute` = {0..59}
  * `Timer second` = {0..59}
  * `Microseconds` = {990000..1010000}
* UP / DOWN
  Adjusts the field value up or down.
* RESET
  Resets the Arduino so you probably want to avoid pressing this.

## Serial interface

The serial interface can be enabled (#define SERIAL) to provide functions for
viewing and setting field values in the Arduino Serial Monitor or other serial
communication application. The serial interface is default enabled.

Set field values by entering command D(ate), C(lock), T(imer) or A(djust):
* `D Year Month Day` sets the date.
  * `Year` = {2018..2099}
  * `Month` = {1..12}
  * `Day` = {1..31} (depending on `Month`)
* `C Year Month Day Hour Minute Second` sets the clock.
  * `Hour` = {0..23}
  * `Minute` = {0..59}
  * `Second` = {0..59}
* `T Timer_hour Timer_minute Timer_second` sets the timer.
  * `Timer_hour` = {0..9}
  * `Timer_minute` = {0..59}
  * `Timer_second` = {0..59}
* `A Microseconds` adjusts the microseconds/second.
  * `Microseconds` = {990000..1010000}

## Setting date and time from PC

There is a Python script that utilizes the serial commands to set the Arduino
clock to the current date and time of the connected PC. It is also possible to
adjust microseconds/second:

`python SetClock.py [portname [baudrate [microseconds]]]`

Ex: `python SetClock.py COM4 19200 1000123`

Python 3 and `serial` package must be installed to run `SetClock.py`.

`SetClock.py` resets the Arduino when opening the serial port and waits 5
seconds to allow the Arduino to boot before sending field values on the serial
port.

`SetClock.py` does not perform range check on field values (but `Clock` does).
