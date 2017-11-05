import pygame
from time import sleep
pygame.init()
from oled.serial import i2c
from oled.device import ssd1306, sh1106
from oled.render import canvas
from PIL import Image, ImageFont
import smbus2
bus = smbus2.SMBus(1)

class const:
    OLEDADDR  = 0x3c
    DSPICADDR = 0x50
    ANGLEADDR = 0x01
    SPEEDADDR = 0x02

serial = i2c(port=1, address=const.OLEDADDR)
oled = ssd1306(serial)


# Initialize the joysticks
pygame.joystick.init()
joystick_count = pygame.joystick.get_count()
for i in range(joystick_count):
    joystick = pygame.joystick.Joystick(i)
    joystick.init()


def send_i2c(address,cmd,vals):
    try:
        bus.write_i2c_block_data(address,cmd,vals)
        test=bus.read_i2c_block_data(address,0,1)
        print test  
    except IOError as e:
        print "I2C Error",e

def direction(raw_value):
    direction = int(raw_value*100)
    print("Direction: {}%".format(direction))
# --Print value on oled screen
    with canvas(oled) as draw:
        font2 = ImageFont.truetype('/etc/stratux-screen/CnC_Red_Alert.ttf', 60)
        draw.text((0,0),str(direction)+"%",fill=255)
# --Send to dspic
    send_i2c(const.DSPICADDR, const.ANGLEADDR, [direction]) 

def speed(raw_value):
    speed = int(-raw_value*100)
    print("Speed: {}%".format(speed))
    send_i2c(const.DSPICADDR, const.SPEEDADDR, [speed]) 

# -------- Main Program Loop -----------

while True:
    event = pygame.event.wait()
    if event.type == pygame.JOYAXISMOTION:
        if event.axis == 0:
            direction(event.value)
        elif event.axis == 2:
            speed(event.value)
    elif event.type == pygame.QUIT:
        break
    elif event.type == pygame.JOYBUTTONDOWN:
        break

# Close the window and quit.
# If you forget this line, the program will 'hang'
# on exit if running from IDLE.

direction(0)
speed(0)

bus.close()
pygame.quit()
