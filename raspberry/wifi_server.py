import threading
import SocketServer

from protocol import PROTOCOL


class WifiServer:
    HOST = ''  # Symbolic name meaning all available interfaces
    PORT = PROTOCOL.WIFI.TCP_PORT

    class _MessageHandler(SocketServer.BaseRequestHandler):
        def handle(self):
            data = self.request.recv(1024)

            # Test message
            if data == PROTOCOL.WIFI.HELLO:
                self.request.sendall(PROTOCOL.WIFI.HELLO)
                print "[wifi] {} is connected!".format(self.client_address[0])
            else:
                print "[wifi] {} is trying to connect, but using incorrect protocol.".format(self.client_address[0])
                return

            while self.server.running:
                data = self.request.recv(1024)
                if not data:
                    # EOF, client closed, just return
                    break

                # Call all data receivers
                for callback in self.server.callbacks:
                    try:
                        callback(data)
                    except Exception as e:
                        print "[wifi] Error while calling {}: {}".format(callback, e)

            print "[wifi] {} is disconnected.".format(self.client_address[0])

    class _ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
        callbacks = []
        running = True

    def __init__(self):
        print "[wifi] Initializing WiFi server on port {}".format(self.PORT)
        self._server = self._ThreadedTCPServer((self.HOST, self.PORT), self._MessageHandler)
        self._client_connection = None
        self._client_address = None
        print "[wifi] Server ready to go!"

    def __enter__(self):
        print "[wifi] Starting server..."
        self._server_thread = threading.Thread(target=self._server.serve_forever)
        self._server_thread.daemon = True
        self._server_thread.start()
        print "[wifi] Server loop running in thread:", self._server_thread.name
        return self

    def register_callback(self, callback):
        self._server.callbacks.append(callback)

    def __exit__(self, exc_type, exc_val, exc_tb):
        print "[wifi] Stopping server..."
        self._server.running = False
        self._server.shutdown()
        self._server.server_close()
        print "[wifi] Server stopped!"
