'https://raspberrypihq.com/making-a-led-blink-using-the-raspberry-pi-and-python/'
import RPi.GPIO as GPIO    # Import Raspberry Pi GPIO library
from time import sleep     # Import the sleep function from the time module
import json

GPIO.setwarnings(False)    # Ignore warning for now
GPIO.setmode(GPIO.BOARD)   # Use physical pin numbering
GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW)   # Set pin 8 to be an output pin and set initial value to low (off)
GPIO.setup(10, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(12, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(7, GPIO.OUT, initial=GPIO.LOW)


while True: # Run forever
    
    with open('/home/pi/git_repos/Luciole_Main/Luciole_raspi/Luciole/Config/lightIntensity.json') as f:
        lightIntensities = json.load(f)
    
    light0 = json.dumps(lightIntensities['light 0'])
    light1 = json.dumps(lightIntensities['light 1'])
    light2 = json.dumps(lightIntensities['light 2'])
    light3 = json.dumps(lightIntensities['light 3'])
    
    threshold = 0.75
    
    print(light0)
    print(light1)
    print(light2)
    print(light3)
    
    if float(light0) > threshold :
        GPIO.output(8, GPIO.HIGH) # Turn on
    else :
        GPIO.output(8, GPIO.LOW)
    
    if float(light1) > threshold :
        GPIO.output(10, GPIO.HIGH) # Turn on
    else :
        GPIO.output(10, GPIO.LOW)
    
    if float(light2) > threshold :
        GPIO.output(12, GPIO.HIGH) # Turn on
    else :
        GPIO.output(12, GPIO.LOW)
    
    if float(light3) > threshold :
        GPIO.output(7, GPIO.HIGH) # Turn on
    else :
        GPIO.output(7, GPIO.LOW)
    
    sleep(1)                  # Sleep for 1 second
