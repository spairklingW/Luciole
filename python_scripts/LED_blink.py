'https://raspberrypihq.com/making-a-led-blink-using-the-raspberry-pi-and-python/'
import RPi.GPIO as GPIO    # Import Raspberry Pi GPIO library
from time import sleep     # Import the sleep function from the time module

GPIO.setwarnings(False)    # Ignore warning for now
GPIO.setmode(GPIO.BOARD)   # Use physical pin numbering
GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW)   # Set pin 8 to be an output pin and set initial value to low (off)
GPIO.setup(10, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(12, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(7, GPIO.OUT, initial=GPIO.LOW)


while True: # Run forever
    
    GPIO.output(8, GPIO.HIGH) # Turn on
    GPIO.output(10, GPIO.HIGH) # Turn on
    GPIO.output(12, GPIO.HIGH) # Turn on
    GPIO.output(7, GPIO.HIGH) # Turn on
    sleep(1)                  # Sleep for 1 second
    GPIO.output(8, GPIO.LOW)  # Turn off
    GPIO.output(10, GPIO.LOW) # Turn on
    GPIO.output(12, GPIO.LOW) # Turn on
    GPIO.output(7, GPIO.LOW) # Turn on
    sleep(1)                  # Sleep for 1 second
