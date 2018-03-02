"""
    SetClock
   
    Copyright (C) 2018 Lars Lindehaven. All rights reserved.
   
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
   
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
   
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
   
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""

import datetime
import time
import serial
import sys

portname = 'COM5'
baudrate = 9600

if len(sys.argv) >= 2 :
    if sys.argv[1] in ("?", "-h", "--help"):
        print("Usage: %s [portname [baudrate [microseconds]]]" % sys.argv[0])
        sys.exit(1)
    else:
        portname = sys.argv[1]

if len(sys.argv) == 3 :
    baudrate = int(sys.argv[2])

if len(sys.argv) == 4 :
    microseconds = int(sys.argv[3])

try:
    print("Opening port '%s' (%d baud) ..." % (portname, baudrate), end="")
    sys.stdout.flush()
    serial_instance = serial.serial_for_url(portname, baudrate)
    for delay in range(5):
       time.sleep(1)
       print(".", end="")
       sys.stdout.flush()
    txstr = time.strftime("D %Y %m %d C %H %M %S ", time.localtime());
    for txchr in txstr:
        serial_instance.write(txchr.encode('ascii', 'ignore'));
    print(" done.")
    print("Date and time have been set.")
    sys.stdout.flush()
    try:
        txstr = "A %d " % microseconds;
        for txchr in txstr:
            serial_instance.write(txchr.encode('ascii', 'ignore'));
        print("Microseconds have been set.")
        sys.stdout.flush()

    except NameError:
        txstr = None

except serial.SerialException:
    print(" failed!")
    print("Could not to open port '%s'." % portname)
    sys.exit(-2)

sys.exit(0)
