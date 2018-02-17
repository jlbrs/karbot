import socket
import pygame
import sys

from protocol import PROTOCOL


class Robot:
    _TCP_PORT = PROTOCOL.WIFI.TCP_PORT

    def __init__(self, ip):
        self._current_angle = 0
        self._current_speed = 0
        self._ip_address = ip

        # Connect to the raspberry
        print "Connecting to raspberry ({}:{})...".format(self._ip_address, self._TCP_PORT)
        self._raspberry_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self._raspberry_socket.connect((self._ip_address, self._TCP_PORT))
            self._raspberry_socket.sendall(PROTOCOL.WIFI.HELLO)
            response = self._raspberry_socket.recv(1024)
            print "Test message: {}".format(response)
            if response != PROTOCOL.WIFI.HELLO:
                print "[ERROR] Did not receive hello from raspberry, are you sure the IP is {}?".format(
                    self._ip_address)
            print "Raspberry connected!"
        except Exception as e:
            self._raspberry_socket.close()
            print "[ERROR] Could not connect to raspberry ({}). Error was: {}".format(self._ip_address, e)

    def cleanup(self):
        self.set_direction(0)
        self.set_speed(0)
        self._raspberry_socket.close()

    def set_direction(self, raw_value):
        self._current_angle = int(raw_value * 100)
        print("Direction: {}%".format(self._current_angle))
        self._send_to_raspberry()

    def set_speed(self, raw_value):
        self._current_speed = int(-raw_value * 100)
        print("Speed: {}%".format(self._current_speed))
        self._send_to_raspberry()

    def get_direction(self):
        return self._current_angle

    def get_speed(self):
        return self._current_speed

    def _send_to_raspberry(self):
        data = PROTOCOL.WIFI.convert_data_to_wifi_message(
            speed=self._current_speed,
            angle=self._current_angle
        )

        try:
            self._raspberry_socket.sendall(data)
        except Exception as e:
            print "[ERROR] Could not send data to raspberry. Error was: {}".format(e)


# -------- Main Program Loop -----------

# Initialize robot
robot = None
default_ip = "192.168.111.137"

if len(sys.argv) == 2:
    robot = Robot(sys.argv[1])
else:
    print ""
    print "Usage: python {} [IP]".format(sys.argv[0].split("/")[-1])
    print "       IP: IP address of the Raspberry. Default value is {}".format(default_ip)
    print ""
    robot = Robot(default_ip)

# Initialize pyGame
print "Initializing joystick..."
pygame.init()
pygame.joystick.init()
joystick_count = pygame.joystick.get_count()
for i in range(joystick_count):
    joystick = pygame.joystick.Joystick(i)
    joystick.init()
print "Joystick ready!"

try:
    while True:
        event = pygame.event.wait()
        if event.type == pygame.JOYAXISMOTION:
            if event.axis == 0:
                robot.set_direction(event.value)
            elif event.axis == 2:
                robot.set_speed(event.value)
        elif event.type == pygame.QUIT:
            break
        elif event.type == pygame.JOYBUTTONDOWN:
            break
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a:
                currentDirection=robot.get_direction()
                robot.set_direction(min(1,max(-1,currentDirection/100.-0.1)))
            elif event.key == pygame.K_d:
                currentDirection=robot.get_direction()
                robot.set_direction(min(1,max(-1,currentDirection/100.+0.1)))
            elif event.key == pygame.K_w:
                currentSpeed=robot.get_speed()
                robot.set_speed(min(1,max(-1,-currentSpeed/100.-0.1)))
            elif event.key == pygame.K_s:
                currentSpeed=robot.get_speed()
                robot.set_speed(min(1,max(-1,-currentSpeed/100.+0.1)))
            elif event.key == pygame.K_SPACE:
                robot.set_direction(0)
                robot.set_speed(0)
            elif event.key == pygame.K_RETURN:
                break

finally:
    print "Exiting..."
    robot.cleanup()
    pygame.quit()
