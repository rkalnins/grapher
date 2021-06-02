import asyncio
import logging

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataProvider

logger = gl.get_logger('tcpsink', logging.DEBUG)


class TCPClient(DataProvider):
    def __init__(self, reader, writer):
        super().__init__()
        self.reader = reader
        self.writer = writer
        self.running = True

    def stop(self):
        self.running = False

    async def run(self):
        while self.running:
            tmp_data = (await self.reader.read(12))  # TODO: define protocol and configure msg size

            if tmp_data:
                logger.debug('%s', tmp_data.decode())

            await self.writer.drain()

        self.writer.close()


class TCPSink():
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.clients = list()
        self.client_index = 0

    def start(self):
        logger.debug('Starting TCP/IP Sink')
        asyncio.run(self.run_server())

    def close(self):
        [c.stop() for c in self.clients]

    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        logger.info('Connected client')

        c = TCPClient(reader, writer)
        self.clients.append(c)
        await c.run()

    async def run_server(self):
        server = await asyncio.start_server(self.handle_client, self.host, self.port)
        async with server:
            await server.serve_forever()
