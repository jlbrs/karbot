import json


class PROTOCOL:

    class I2C:
        HELLO = 0x00
        ANGLE = 0x01
        SPEED = 0x02

    class WIFI:
        HELLO = "HELLO"
        ANGLE = "ANGLE"
        SPEED = "SPEED"
        TCP_PORT = 56789

        @staticmethod
        def convert_wifi_message_to_object(message):
            # data is a json string
            return json.loads(message)

        @staticmethod
        def convert_data_to_wifi_message(speed=0, angle=0):
            to_send = {
                PROTOCOL.WIFI.ANGLE: angle,
                PROTOCOL.WIFI.SPEED: speed
            }
            return json.dumps(to_send)
