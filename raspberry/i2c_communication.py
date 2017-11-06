import smbus2
from protocol import PROTOCOL


class I2CCommunication:
    _RASPBERRY_I2C_PORT = 1
    _I2C_BUS_PIC_ADDRESS = 0x50

    def __init__(self):
        print "[i2c]  Initializing i2c..."
        self._bus = smbus2.SMBus(self._RASPBERRY_I2C_PORT)
        print "[i2c]  i2c ready!"

    def send_speed(self, speed):
        """
        Send the new speed to motor control.
        :param speed: integer between -100 and +100
        :return: void
        """
        try:
            self._bus.write_byte_data(self._I2C_BUS_PIC_ADDRESS, PROTOCOL.I2C.SPEED, speed)
        except IOError as e:
            print "[i2c]  I2C Error while sending speed: {}".format(e)

    def send_angle(self, angle):
        """
        Send the new angle to motor control.
        :param angle: integer between -100 and +100
        :return: void
        """
        try:
            self._bus.write_byte_data(self._I2C_BUS_PIC_ADDRESS, PROTOCOL.I2C.ANGLE, angle)
        except IOError as e:
            print "[i2c]  I2C Error while sending speed: {}".format(e)

    # def send_i2c(address, cmd, vals):
    #     try:
    #         bus.write_i2c_block_data(address, cmd, vals)
    #         test = bus.read_i2c_block_data(address, 0, 1)
    #         print test
    #     except IOError as e:
    #         print "I2C Error", e


class FakeI2CCommunication:
    def __init__(self):
        print "[i2cF] I2C is not available, setting up a fake interface for test run!"

    def send_speed(self, speed):
        """
        Send the new speed to motor control.
        :param speed: integer between -100 and +100
        :return: void
        """
        print "[i2cF] Send speed to i2c: {}".format(speed)

    def send_angle(self, angle):
        """
        Send the new angle to motor control.
        :param angle: integer between -100 and +100
        :return: void
        """
        print "[i2cF] Send angle to i2c: {}".format(angle)
