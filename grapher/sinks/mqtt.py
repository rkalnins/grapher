import logging
import struct

import PyQt6.QtCore
import paho.mqtt.client as mqtt

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataProvider, DataPacket

logger = gl.get_logger(__name__, logging.DEBUG)


class MqttSink(DataProvider):
    post_signal = PyQt6.QtCore.pyqtSignal(DataPacket)

    def __init__(self):
        super().__init__()

        self.name = 'grapher'
        self.host = '127.0.0.1'
        self.client = None
        self.topics = ['topic/test/a']

    def connect(self):
        self.client = mqtt.Client(self.name)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_data
        self.client.connect(self.host)  # port = 1883

    def start(self):
        for topic in self.topics:
            self.client.subscribe(topic)

    def on_data(self, mosq, userdata, msg):
        msg = struct.unpack('ilf', msg)
        packet = DataPacket()
        packet.source_id = msg[0]
        packet.timestamp = msg[1] / 1000.0
        packet.data = msg[2]

        logger.debug('%s %s %s', packet.source_id, packet.timestamp, packet.timestamp)

        self.post_signal.emit(packet)

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            logger.debug('Connected to MQTT broker at %s', self.host)
