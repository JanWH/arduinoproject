from random import random
import serial, time, argparse, sys, random

# Find the name of the port from the Aduino Tools menu
# this is windows port naming, if you have MacOS it might be like this "/dev/cu.usbmodem4301"
port="COM5" 

print('connecting to port', port)

ser = serial.Serial(port, 9600, timeout=5,
            rtscts=False, xonxoff=False, dsrdtr=False)

# Wait until Arduino sends a hello message
going = True
while going:
  print("waiting")
  s = ser.read_until()
  print("read ", s)
  if s.startswith(b"hello"):
    going = False

# Generate random integers between 0-7 and send them over the serial interface to the Arduino
while True:
  x=random.randint(0,7)
  ser.write(str(x).encode())
  print(x)
  time.sleep(3)