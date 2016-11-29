import time
import serial
import sys
import RPi.GPIO as GPIO
import math

ser = serial.Serial('/dev/ttyUSB0',
                baudrate=9600,
                rtscts=0)
ser.setDTR(False)
time.sleep(1)

ser.setDTR(True)
time.sleep(4)
print "Serial is open: " + str(ser.isOpen())
pin = 7
GPIO.setmode(GPIO.BOARD)
ser.flushInput()
tagID = ""
PIN = ""
x = ""
tries = 0
while True:

    print "ID" + tagID
    while x == tagID:
        x = ser.readline()
	
	print "X=" + x
	tagID = x[0:len(x)-2]
	
	print tagID
	
	if tagID == 'ID951BD328':
		print "Valid User!"
		ser.write('#')
        tries = 0
		temp = ser.read()
        
		if temp == '#':
            temp = ser.read()
            while temp != '#' and temp != '*':
                PIN = PIN + temp
                temp =  ser.read()
		
	else:
		print "Try again"
		time.sleep(tries)
        tries = tries + 1
		ser.write('*')

    print "PIN=" + PIN
    
    if PIN == "1234":
        print "Uncloking"
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, GPIO.LOW)
        time.sleep(1)
        print "Locking"
        GPIO.output(pin, GPIO.HIGH)
		print "Door Open"
        GPIO.cleanup(pin)
                
    ser.readline()
	ser.flushInput()
	PIN = ""
	tagID = ""
	x = ""
