#!/usr/bin/env python

# Gets picture count from db, poops it out the serial line

import serial
import requests
import json
from time import sleep, time

PORT = "/dev/ttyACM0"
SPEED = 38400
AUTH_FILE = "auth.json"


def main():
    with open(AUTH_FILE, "r") as f:
        auth = json.loads(f.read())
    print auth
    auth_tuple = (auth['user'], auth['pass'])
    url = auth['url']
    ser = serial.Serial(PORT, SPEED)
    sleep(1)
    ser.write("space cash \n")
    last_count = 0
    sleeptime = 1

    while True:
        response = requests.get(url, auth=auth_tuple)
        if response.ok:
            count = response.json()['count']
            ser.write('PICS: %i\n' % count)
        else:
            ser.write(':( %s\n' % response.status_code)
        print time(), count, last_count, sleeptime
        if count == last_count:
            sleeptime = 20 if sleeptime > 20 else sleeptime * 1.5
        else:
            sleeptime = 1.5
        sleep(sleeptime)

        last_count = count

if __name__ == "__main__":
    main()
