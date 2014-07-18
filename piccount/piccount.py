#!/usr/bin/env python

# Gets picture count from db, poops it out the serial line

import serial
import requests
import json
from time import sleep, time

PORT = "/dev/ttyACM0"
SPEED = 38400
URL = "xxx"
AUTH_FILE = "auth.json"


def main():
    with open(AUTH_FILE, "r") as f:
        auth = json.loads(f.read())
    print auth
    auth_tuple = (auth['user'], auth['pass'])

    ser = serial.Serial(PORT, SPEED)
    sleep(5)
    ser.write(" \n")
    last_count = 0
    sleeptime = 1
    while True:
        count = requests.get(URL,
                             auth=auth_tuple).json()['count']
        print time(), count, last_count, sleeptime
        if count != last_count:
            ser.write("SPACE CA$H\n")
            sleep(.25)
        ser.write('PICS: %i\n' % count)
        if count == last_count:
            sleeptime = 60 if sleeptime > 60 else sleeptime * 2
        else:
            sleeptime = 1
        sleep(sleeptime)

        last_count = count

if __name__ == "__main__":
    main()
