#!/usr/bin/env python

from wifi_server import WifiServer
from i2c_communication import I2CCommunication, FakeI2CCommunication
from protocol import PROTOCOL
from time import sleep
from threading import Event


command_received = Event()
robot_is_stopped = True


def on_wifi_message_received(data):
    command_received.set()
    print data
    try:
        robot_config = PROTOCOL.WIFI.convert_wifi_message_to_object(data)
    except Exception as e:
        print "[ERROR] Wifi message seems incorrect: {}".format(data)
        return
    try:
        i2c.send_angle(robot_config[PROTOCOL.WIFI.ANGLE])
    except IndexError:
        pass
    try:
        i2c.send_speed(robot_config[PROTOCOL.WIFI.SPEED])
    except IndexError:
        pass


def stop_the_robot():
    i2c.send_speed(0)


print "[main] Starting I2C"
i2c = None
try:
    i2c = I2CCommunication()
except OSError:
    i2c = FakeI2CCommunication()

print "[main] Starting Wifi"
with WifiServer() as wifi:
    wifi.register_callback(on_wifi_message_received)
    try:
        while True:
            sleep(1)
            if robot_is_stopped and command_received.is_set():
                print "Receiving orders!"
                robot_is_stopped = False

            elif not robot_is_stopped and not command_received.is_set():
                print 'Timeout while robot was moving! Stopping the robot!'
                stop_the_robot()
                robot_is_stopped = True

            command_received.clear()

    except KeyboardInterrupt:
        print ""
        print "[main] Exiting..."
