import logging
import selectors
import socket
import struct
import types

import PyQt6.QtCore

import util.grapher_logging as gl
from sinks.DataProvider import DataProvider, DataPacket

logger = gl.get_logger(__name__, logging.DEBUG)


class TCPSink(DataProvider):
    post_signal = PyQt6.QtCore.pyqtSignal(DataPacket)

    def __init__(self, host: str, port: int, callback):
        super().__init__()
        self.host = host
        self.port = port
        self.sel = selectors.DefaultSelector()
        self.post_data_callback = callback
        self.running = True

    def close(self):
        self.running = False
        logger.info("Closed TCP sink")

    def accept_wrapper(self, sock):
        conn, addr = sock.accept()  # Should be ready to read
        logger.debug("Accepted connection from %s", addr)
        conn.setblocking(False)
        data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
        events = selectors.EVENT_READ
        self.sel.register(conn, events, data=data)

    def service_connection(self, key, mask):
        sock = key.fileobj
        data = key.data
        if mask & selectors.EVENT_READ:
            recv_data = sock.recv(32)
            if recv_data:
                # logger.debug(recv_data)

                try:
                    msg = struct.unpack("ilf", recv_data)
                except struct.error:
                    return

                packet = DataPacket()

                packet.source_id = msg[0]
                packet.timestamp = msg[1] / 1000.0
                packet.data = msg[2]

                # logger.debug('%s, %s, %s', msg[0], msg[1], msg[2])
                self.post_signal.emit(packet)
            else:
                logger.debug("closing connection to %s", data.addr)
                self.sel.unregister(sock)
                sock.close()

    def start(self):

        lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        lsock.bind((self.host, self.port))
        lsock.listen()
        logger.info("listening on %s:%s", self.host, self.port)
        lsock.setblocking(False)
        self.sel.register(lsock, selectors.EVENT_READ, data=None)

    def run(self):
        while self.running:
            events = self.sel.select(timeout=None)
            for key, mask in events:
                if key.data is None:
                    self.accept_wrapper(key.fileobj)
                else:
                    self.service_connection(key, mask)

        logger.debug("quitting")
