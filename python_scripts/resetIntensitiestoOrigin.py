'https://raspberrypihq.com/making-a-led-blink-using-the-raspberry-pi-and-python/'
import json

data = {
    "light 0": 0.2,
    "light 1": 0.2,
    "light 2": 0.2,
    "light 3": 0.2
} 

with open('/home/pi/git_repos/Luciole_Main/Luciole_raspi/Luciole/Config/lightIntensity.json','w') as outfile:
        json.dump(data, outfile, ensure_ascii=False)

