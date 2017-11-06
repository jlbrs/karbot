from wifi_server import WifiServer
from i2c_communication import I2CCommunication, FakeI2CCommunication
from protocol import PROTOCOL
from time import sleep


def on_wifi_message_received(data):
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
    except KeyboardInterrupt:
        print ""
        print "[main] Exiting..."
