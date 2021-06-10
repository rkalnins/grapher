import logging
import struct

import PyQt6.QtCore
import paho.mqtt.client as mqtt

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataProvider, DataPacket

logger = gl.get_logger(__name__, logging.DEBUG)


class MqttSink(DataProvider):
    post_signal = PyQt6.QtCore.pyqtSignal(DataPacket)

    def __init__(self, host, port, topics):
        super().__init__()

        self.name = 'grapher'
        self.host = host
        self.port = port
        self.client = None
        self.topics = topics

    def connect(self):
        self.client = mqtt.Client(self.name)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_data
        self.client.connect(self.host, port=self.port)  # port = 1883
        for topic in self.topics:
            self.client.subscribe(topic)

    def start(self):
        self.client.loop_start()

    def on_disconnect(self, usrdata=None, rc=None):
        self.client.loop_stop()

    def close(self):
        self.on_disconnect()

    def on_data(self, mosq, userdata, msg: mqtt.MQTTMessage):
        unpacked_msg = struct.unpack('ilf', msg.payload)
        packet = DataPacket()
        packet.source_id = unpacked_msg[0]
        packet.timestamp = unpacked_msg[1] / 1000.0
        packet.data = unpacked_msg[2]

        logger.debug('%s %s %s %s', msg.payload, packet.source_id, packet.timestamp, packet.data)

        self.post_signal.emit(packet)

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            logger.debug('Connected to MQTT broker at %s', self.host)
