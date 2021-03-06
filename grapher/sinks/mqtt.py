import logging
import struct

import PyQt6.QtCore
import paho.mqtt.client as mqtt

from grapher.util import get_logger
from grapher.sinks import DataProvider, DataPacket

logger = get_logger(__name__, logging.DEBUG)


class MqttSink(DataProvider):
    post_signal = PyQt6.QtCore.pyqtSignal(DataPacket)

    def __init__(self):
        super().__init__()

        self.name = "grapher"
        self.host = ""
        self.port = 0
        self.client = mqtt.Client(self.name)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_data
        self.topics = []

    def update_host(self, hostname, port):
        self.host = hostname
        self.port = port
        self.close()
        self.connect_client()

    def update_topics(self, topics):
        self.topics = topics
        if self.client.is_connected():
            for topic in self.topics:
                self.client.subscribe(topic)

    def connect_client(self):
        self.client.connect(self.host, port=self.port)  # port = 1883
        for topic in self.topics:
            self.client.subscribe(topic)

    def start(self):
        self.client.loop_start()

    def on_disconnect(self, usrdata=None, rc=None):
        self.client.loop_stop()

    def close(self):
        self.on_disconnect()
        logger.info("Closed MQTT sink")

    def on_data(self, mosq, userdata, msg: mqtt.MQTTMessage):
        unpacked_msg = struct.unpack("ilf", msg.payload)
        packet = DataPacket()
        packet.source_id = unpacked_msg[0]
        packet.timestamp = unpacked_msg[1] / 1000.0
        packet.data = unpacked_msg[2]

        self.post_signal.emit(packet)

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            logger.debug("Connected to MQTT broker at %s", self.host)
