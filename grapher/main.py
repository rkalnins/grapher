import asyncio
import logging

import grapher.util.grapher_logging as gl
from sinks.tcp import TCPSink

logger = gl.get_logger('main', logging.DEBUG)
tcp_sink = TCPSink('127.0.0.1', 8888)  # TODO: configuration


async def update():
    while True:
        data = [c.get() for c in tcp_sink.clients]
        for d in data:
            logger.debug("data: %s", d)

        await asyncio.sleep(1.0)


def main():
    logger.debug("Getting data")
    tcp_sink.start()

    loop = asyncio.get_event_loop()
    loop.run_until_complete(update())


if __name__ == '__main__':
    logger.info('Starting grapher app')
    main()

