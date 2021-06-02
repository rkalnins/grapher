import logging

import grapher.util.grapher_logging as gl
from sinks.tcp import TCPSink

logger = gl.get_logger('main', logging.DEBUG)


def main():
    logger.info('Starting grapher app')

    tcp_sink = TCPSink('127.0.0.1', 8888)  # TODO: configuration
    tcp_sink.start()


if __name__ == '__main__':
    main()
