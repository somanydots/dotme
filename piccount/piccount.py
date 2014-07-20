#!/usr/bin/env python

# Gets picture count from db, poops it out the serial line

import serial
import requests

from time import sleep, time

PORT = "/dev/ttyACM1"
SPEED = 38400
URL = "xxx"


def main():
    ser = serial.Serial(PORT, SPEED)

    last_count = 0
    while True:
        count = requests.get(URL,
                             auth=("xxx", "xxx")).json()['count']
        print time(), count, last_count
        ser.write('PICS: %i\n' % count)
        if count == last_count:
            sleep(60)
        else:
            sleep(5)

        last_count = count

if __name__ == "__main__":
    main()
