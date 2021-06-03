import logging
import selectors
import socket
import types

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataProvider

logger = gl.get_logger('tcpsink', logging.DEBUG)


class TCPSink(DataProvider):
    def __init__(self, host: str, port: int):
        super().__init__()
        self.host = host
        self.port = port
        self.sel = selectors.DefaultSelector()

    def accept_wrapper(self, sock):
        conn, addr = sock.accept()  # Should be ready to read
        logger.debug("accepted connection from %s", addr)
        conn.setblocking(False)
        data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
        events = selectors.EVENT_READ
        self.sel.register(conn, events, data=data)

    def service_connection(self, key, mask):
        sock = key.fileobj
        data = key.data
        if mask & selectors.EVENT_READ:
            recv_data = sock.recv(16)  # TODO: configure protocol
            if recv_data:
                self.data = float(recv_data)
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
        while True:
            events = self.sel.select(timeout=None)
            for key, mask in events:
                if key.data is None:
                    self.accept_wrapper(key.fileobj)
                else:
                    self.service_connection(key, mask)
