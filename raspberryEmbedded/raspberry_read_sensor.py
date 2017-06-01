"""
This goes in raspberry
"""

#!/usr/bin/env python3
#!/usr/bin/python uart.py

import struct
import time
import math
import glob
import uinput
import pyudev
import os
import serial
serialPort = '/dev/ttyACM0'
#Baud rate at 115200
ser = serial.Serial(serialPort,115200)

def get_bytes(integer):
	return divmod(integer, 0x100)

# Wait and find devices
def read_and_emulate_mouse(deviceFound):
    with open(deviceFound, 'rb') as f:
        print("Read buffer")

        device = uinput.Device([
            uinput.BTN_LEFT,
            uinput.BTN_RIGHT,
            uinput.ABS_X,
            uinput.ABS_Y,
        ])

        clicked = False
        rightClicked = False
        (lastX, lastY) = (0, 0)
        startTime = time.time()

        while True:
            try:
                b = f.read(25)
                (tag, btnLeft, x, y) = struct.unpack_from('>c?HH', b)
                # print(btnLeft, x, y)
                highx, lowx = get_bytes(x)
                highy, lowy = get_bytes(y)
                payload = bytearray([highx,lowx,highy,lowy])
                nbytes = ser.write(payload)
                if nbytes!=4:
                	print('ERROR')

            except:
                print('failed to read from deviceFound' + str(deviceFound))
                return
            
            time.sleep(0.01)

            if btnLeft:
                device.emit(uinput.ABS_X, x, True)
                device.emit(uinput.ABS_Y, y, True)

                if not clicked:
                    print("Left click")
                    device.emit(uinput.BTN_LEFT, 1)
                    clicked = True
                    startTime = time.time()
                    (lastX, lastY) = (x, y)

                duration = time.time() - startTime
                movement = math.sqrt(pow(x - lastX, 2) + pow(y - lastY, 2))

                if clicked and (not rightClicked) and (duration > 1) and (movement < 20):
                    print("Right click")
                    device.emit(uinput.BTN_RIGHT, 1)
                    device.emit(uinput.BTN_RIGHT, 0)
                    rightClicked = True
            else:
                print("Release")
                clicked = False
                rightClicked = False
                device.emit(uinput.BTN_LEFT, 0)


if __name__ == "__main__":
    os.system("modprobe uinput")
    os.system("chmod 666 /dev/hidraw*")
    os.system("chmod 666 /dev/uinput*")

    while True:
        # try:
        print("Waiting device")
        hidrawDevices = glob.glob("/dev/hidraw*")

        context = pyudev.Context()

        deviceFound = None
        for hid in hidrawDevices:
            device = pyudev.Device.from_device_file(context, hid)
            if "0EEF:0005" in device.device_path:
                deviceFound = hid

        if deviceFound:
            print("Device found", deviceFound)
            read_and_emulate_mouse(deviceFound)
            # except:
            #     print("Error:", sys.exc_info())
            #     pass
            # finally:
            #     time.sleep(1)

"""
#!usr/bin/python uart.py
import time
import serial
serialPort = '/dev/ttyACM0'

#Baud rate at 115200
ser = serial.Serial(serialPort,115200)
count = 0

#Loop to display a series of integers
while 1:
        for x in range(1,10):
                payload = bytearray([x,x])
                l=ser.write(payload)
                print l
                count +=1
        print 'All done!'

"""