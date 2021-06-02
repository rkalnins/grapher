import asyncio

from grapher.util.grapher_logging import logger


class TCPSink:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.running = False

    def start(self):
        logger.debug('Starting TCP/IP Sink')
        self.running = True
        asyncio.run(self.run_server())

    def close(self):
        self.running = False

    async def handle_client(self, reader, writer):
        logger.info('Connected client')
        while self.running:
            data = (await reader.read(12))  # TODO: define protocol and configure msg size

            if data:
                logger.debug('%s', data.decode())

            await writer.drain()

        writer.close()

    async def run_server(self):
        server = await asyncio.start_server(self.handle_client, self.host, self.port)
        async with server:
            await server.serve_forever()
