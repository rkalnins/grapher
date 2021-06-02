from grapher.util.grapher_logging import logger
from sinks.tcp import TCPSink


def main():
    logger.info("Starting grapher app")

    tcp_sink = TCPSink('127.0.0.1', 8888)
    tcp_sink.start()


if __name__ == "__main__":
    main()
