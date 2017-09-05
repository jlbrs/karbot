import pygame
from time import sleep

pygame.init()

#Loop until the user clicks the close button.
done = False

# Initialize the joysticks
pygame.joystick.init()
joystick_count = pygame.joystick.get_count()
for i in range(joystick_count):
    joystick = pygame.joystick.Joystick(i)
    joystick.init()

def direction(raw_value):
    direction = int(raw_value*100)
    print("Direction: {}%".format(direction))

def speed(raw_value):
    speed = int(-raw_value*100)
    print("Speed: {}%".format(speed))

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

pygame.quit()
